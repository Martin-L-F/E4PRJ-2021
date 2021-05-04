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
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/timer.h>

#define DEV_NAME "targetbeam_powersave_timeout"
#define BUFFER_SZ 512

static DECLARE_WAIT_QUEUE_HEAD(wq);
static int flag = 0;
static long delay = 10000;

struct timer_list timer;

void timer_isr(struct timer_list *timer_arg)
{
    flag = 1;
    wake_up_interruptible(&wq);
    printk(KERN_INFO "[targetbeam_powersave_timeout] CB function triggered");
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
    del_timer_sync(&timer); /* in case a signal woke us up */

    char buffer_[count];

    if (count <= 0)
        return 0;

    /* read data from user buffer to buffer_ */
    if (copy_from_user(buffer_, buf, count))
        return -EFAULT;

    printk(KERN_INFO "[targetbeam_powersave_timeout] Message recieved(%i): '%s'", count, buffer_);

    if (buffer_[0] == 'S')
    {
        return count;
    }
    if (buffer_[0] == 'I')
    {
        printk(KERN_INFO "[targetbeam_powersave_timeout] Updating delay");
        char str[count];
        int i = 0;
        long val;

        for (i = 0; i < (count - 2); i++)
        {
            str[i] = buffer_[i + 1];
        }
        str[count - 1] = '\0';

        if (kstrtol(str, 10, &val))
        {
            printk(KERN_INFO "[targetbeam_powersave_timeout] An error occurred updating delay. Param: '%s'", str);
            return count;
        }

        printk(KERN_INFO "[targetbeam_powersave_timeout] Updated delay length to %i", val);
        delay = val;
    }

    timer.expires = delay + (jiffies + HZ); /* one second */
    add_timer(&timer);
    printk(KERN_INFO "[targetbeam_powersave_timeout] Starting timer");

    *pos += count;
    return count;
}

static ssize_t read(struct file *file, char __user *buf, size_t count, loff_t *pos)
{
    wait_event_interruptible(wq, flag == 1);

    del_timer_sync(&timer); /* in case a signal woke us up */

    if (count <= 1)
        return -1;

    char buffer_[2];
    flag = 0;

    sprintf(buffer_, "%i", 1);

    int len = strlen(buffer_) + 1;

    int len_copy = copy_to_user(buf, buffer_, len);

    len = len > len_copy ? len : len_copy;

    printk(KERN_INFO "[targetbeam_powersave_timeout] triggered");

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
    printk(KERN_INFO "[targetbeam_powersave_timeout] Initiating");

    // Register a character device for communication with user space
    misc_register(&misc_device);

    init_timer_key(&timer, timer_isr, NULL, DEV_NAME, NULL); /* init the timer structure */
    timer.expires = delay + (jiffies + HZ);                  /* one second */

    return 0;
}

static void __exit exit(void)
{
    printk(KERN_INFO "[targetbeam_powersave_timeout] Exiting");

    misc_deregister(&misc_device);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Martin");
MODULE_DESCRIPTION("Linux Kernel Module for timeout Interrupt");

module_init(init);
module_exit(exit);