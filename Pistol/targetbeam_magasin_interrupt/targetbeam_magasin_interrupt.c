/*
 * Basic Linux Kernel module using GPIO interrupts.
 *
 * Author:
 * 	Martin
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>

#define GPIO_FOR_RX_SIGNAL_0 20
#define GPIO_FOR_RX_SIGNAL_1 16
#define DEV_NAME "targetbeam_magasin"
#define BUFFER_SZ 512

static DECLARE_WAIT_QUEUE_HEAD(wq);
static int flag = 0;
static int state = 0;

static struct gpio signals[] = {
    {GPIO_FOR_RX_SIGNAL_0, GPIOF_IN, "targetbeam_magasin_0"},
    {GPIO_FOR_RX_SIGNAL_1, GPIOF_IN, "targetbeam_magasin_1"},
};

static int rx_irqs[] = {-1, -1};

static irqreturn_t rx_isr(int irq, void *data)
{
    flag = 1;
    wake_up_interruptible(&wq);
    return IRQ_HANDLED;
}

static int open(struct inode *inode, struct file *file)
{
    return nonseekable_open(inode, file);
}

static int release(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t write(struct file *file, const char __user *buf, size_t count, loff_t *pos)
{
    return -EINVAL;
}

static ssize_t read(struct file *file, char __user *buf, size_t count, loff_t *pos)
{
    while (true)
    {
        wait_event_interruptible(wq, flag == 1);
        if (count <= 1)
            return -1;

        int val0 = gpio_get_value(signals[0].gpio);
        int val1 = gpio_get_value(signals[1].gpio);
        if ((val0 != val1) && (state != val1))
        {
            state = val1;
            break;
        }
    }

    char buffer_[2];
    flag = 0;
    sprintf(buffer_, "%i", state);

    int len = strlen(buffer_) + 1;

    int len_copy = copy_to_user(buf, buffer_, len);

    len = len > len_copy ? len : len_copy;

    printk(KERN_INFO "[targetbeam_magasin] triggered");

    *pos += len;

    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = open,
    .read = read,
    .write = write,
    .release = release,
};

static struct miscdevice misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEV_NAME,
    .fops = &fops,
};

static int __init init(void)
{
    int ret = 0;
    printk(KERN_INFO "[targetbeam_magasin] Initiating");

    // register GPIO PIN in use
    ret = gpio_request_array(signals, ARRAY_SIZE(signals));

    if (ret)
    {
        printk(KERN_ERR "[targetbeam_magasin] Unable to request GPIOs for RX Signals: %d\n", ret);
        goto fail2;
    }

    int i;
    for (i = 0; i < 2; i++)
    {

        // Register IRQ for this GPIO
        ret = gpio_to_irq(signals[i].gpio);
        if (ret < 0)
        {
            printk(KERN_ERR "[targetbeam_magasin] Unable to translate GPIO to IRQ: %d\n", ret);
            goto fail2;
        }
        rx_irqs[i] = ret;
        printk(KERN_INFO "[targetbeam_magasin] Successfully requested RX IRQ # %d\n", rx_irqs[i]);
        ret = request_irq(rx_irqs[i], rx_isr, IRQF_TRIGGER_RISING, DEV_NAME, NULL);
        if (ret)
        {
            printk(KERN_ERR "[targetbeam_magasin] Unable to request IRQ: %d\n", ret);
            goto fail3;
        }
    }
    printk(KERN_INFO "[targetbeam_magasin] Successfully requested all IRQs");

    // Register a character device for communication with user space
    misc_register(&misc_device);

    state = gpio_get_value(signals[1].gpio);

    return 0;

    // cleanup what has been setup so far
fail3:
    free_irq(rx_irqs[0], NULL);
    free_irq(rx_irqs[1], NULL);

fail2:
    gpio_free_array(signals, ARRAY_SIZE(signals));
    return ret;
}

static void __exit exit(void)
{
    printk(KERN_INFO "[targetbeam_magasin] Exiting");

    misc_deregister(&misc_device);

    free_irq(rx_irqs[0], NULL);
    free_irq(rx_irqs[1], NULL);

    gpio_free_array(signals, ARRAY_SIZE(signals));
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Martin");
MODULE_DESCRIPTION("Linux Kernel Module for dual-switch Interrupt");

module_init(init);
module_exit(exit);