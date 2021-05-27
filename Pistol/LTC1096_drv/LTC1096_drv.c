#include <linux/cdev.h>    // cdev_add, cdev_init
#include <linux/uaccess.h> // copy_to_user
#include <linux/module.h>  // module_init, GPL
#include <linux/spi/spi.h> // spi_sync,

#define MAXLEN 32
#define MODULE_DEBUG 0 // Enable/Disable Debug messages

/* Char Driver Globals */
static struct spi_driver LTC1096_drv_spi_driver;
struct file_operations LTC1096_drv_fops;
static struct class *LTC1096_drv_class;
static dev_t devno;
static struct cdev LTC1096_drv_cdev;

/* Definition of SPI devices */
struct Myspi
{
  struct spi_device *LTC1096; // Pointer to SPI device
  int channel;                // channel, ex. adc ch 0
};
/* Array of SPI devices */
/* Minor used to index array */
struct Myspi LTC1096_devs[1];
const int LTC1096_devs_len = 1;  // Max nbr of devices
static int LTC1096_devs_cnt = 0; // Nbr devices present

/* Macro to handle Errors */
#define ERRGOTO(label, ...) \
  {                         \
    printk(__VA_ARGS__);    \
    goto label;             \
  }                         \
  while (0)

/**********************************************************
 * CHARACTER DRIVER METHODS
 **********************************************************/

/*
 * Character Driver Module Init Method
 */
static int __init LTC1096_drv_init(void)
{
  int err = 0;

  printk("spi_drv driver initializing\n");

  /* Allocate major number and register fops*/
  err = alloc_chrdev_region(&devno, 0, 255, "LTC1096_drv driver");
  if (MAJOR(devno) <= 0)
    ERRGOTO(err_no_cleanup, "Failed to register chardev\n");
  printk(KERN_ALERT "Assigned major no: %i\n", MAJOR(devno));

  cdev_init(&LTC1096_drv_cdev, &LTC1096_drv_fops);
  err = cdev_add(&LTC1096_drv_cdev, devno, 255);
  if (err)
    ERRGOTO(err_cleanup_chrdev, "Failed to create class");

  /* Polulate sysfs entries */
  LTC1096_drv_class = class_create(THIS_MODULE, "LTC1096_drv_class");
  if (IS_ERR(LTC1096_drv_class))
    ERRGOTO(err_cleanup_cdev, "Failed to create class");

  /* Register SPI Driver */
  /* THIS WILL INVOKE PROBE, IF DEVICE IS PRESENT!!! */
  err = spi_register_driver(&LTC1096_drv_spi_driver);
  if (err)
    ERRGOTO(err_cleanup_class, "Failed SPI Registration\n");

  /* Success */
  return 0;

  /* Errors during Initialization */
err_cleanup_class:
  class_destroy(LTC1096_drv_class);

err_cleanup_cdev:
  cdev_del(&LTC1096_drv_cdev);

err_cleanup_chrdev:
  unregister_chrdev_region(devno, 255);

err_no_cleanup:
  return err;
}

/*
 * Character Driver Module Exit Method
 */
static void __exit LTC1096_drv_exit(void)
{
  printk("LTC1096_drv driver Exit\n");

  spi_unregister_driver(&LTC1096_drv_spi_driver);
  class_destroy(LTC1096_drv_class);
  cdev_del(&LTC1096_drv_cdev);
  unregister_chrdev_region(devno, 255);
}

/*
 * Character Driver Write File Operations Method
 */
ssize_t LTC1096_drv_write(struct file *filep, const char __user *ubuf,
                          size_t count, loff_t *f_pos)
{
  return -EINVAL;
}

/*
 * Character Driver Read File Operations Method
 */
ssize_t LTC1096_drv_read(struct file *filep, char __user *ubuf,
                         size_t count, loff_t *f_pos)
{
  int minor, len;
  char resultBuf[MAXLEN];
  s16 result = 0;

  minor = iminor(filep->f_inode);

  /*
    Provide a result to write to user space
  */
  struct spi_transfer t[2];
  struct spi_message m;

  memset(t, 0, sizeof(t));
  spi_message_init(&m);

  m.spi = LTC1096_devs[minor].LTC1096;

  //Start bit
  t[0].tx_buf = NULL;
  t[0].rx_buf = &resultBuf[0];
  t[0].len = 1;
  spi_message_add_tail(&t[0], &m);

  //Addressevalg, MSB først og modtag
  t[1].tx_buf = NULL;
  t[1].rx_buf = &resultBuf[1];
  t[1].len = 1;
  spi_message_add_tail(&t[1], &m);

  spi_sync(m.spi, &m);

  /*Konverterer resultatet med resultat i mV
    - max input 5V = 5000mV
    - 8-bit ADC, resolution: 0 til 256-1, AKA 256 steps*/
  result = ((resultBuf[0] << 2) + (resultBuf[1] >> 6)) * 5000 / 256;

  if (MODULE_DEBUG)
    printk(KERN_ALERT "%s-%i read: %i\n",
           LTC1096_devs[minor].LTC1096->modalias, LTC1096_devs[minor].channel, result);

  /* Convert integer to string limited to "count" size. Returns
    * length excluding NULL termination */
  len = snprintf(resultBuf, count, "%d\n", result);
  /* Append Length of NULL termination */
  len++;

  /* Copy data to user space */
  if (copy_to_user(ubuf, resultBuf, len))
    return -EFAULT;

  /* Move fileptr */
  *f_pos += len;

  return len;
}

/*
 * Character Driver File Operations Structure
 */
struct file_operations LTC1096_drv_fops =
    {
        .owner = THIS_MODULE,
        .write = LTC1096_drv_write,
        .read = LTC1096_drv_read,
};

/**********************************************************
 * LINUX DEVICE MODEL METHODS (spi)
 **********************************************************/

/*
 * LTC1096_drv Probe
 * Called when a device with the name "LTC1096_drv" is
 * registered.
 */
static int LTC1096_drv_probe(struct spi_device *sdev)
{
  int err = 0;
  struct device *LTC1096_drv_device;

  printk(KERN_DEBUG "New SPI device: %s using chip select: %i\n",
         sdev->modalias, sdev->chip_select);

  /* Check we are not creating more
     devices than we have space for */
  if (LTC1096_devs_cnt > LTC1096_devs_len)
  {
    printk(KERN_ERR "Too many SPI devices for driver\n");
    return -ENODEV;
  }

  /* Configure bits_per_word, always 8-bit for RPI!!! */
  sdev->bits_per_word = 8;
  spi_setup(sdev);

  /* Create devices, populate sysfs and
     active udev to create devices in /dev */

  /* We map spi_devs index to minor number here */
  LTC1096_drv_device = device_create(LTC1096_drv_class, NULL,
                                     MKDEV(MAJOR(devno), LTC1096_devs_cnt),
                                     NULL, "LTC1096-%d", LTC1096_devs_cnt);
  if (IS_ERR(LTC1096_drv_device))
    printk(KERN_ALERT "FAILED TO CREATE DEVICE\n");
  else
    printk(KERN_ALERT "Using LTC1096_devs%i on major:%i, minor:%i\n",
           LTC1096_devs_cnt, MAJOR(devno), LTC1096_devs_cnt);

  /* Update local array of SPI devices */
  LTC1096_devs[LTC1096_devs_cnt].LTC1096 = sdev;
  LTC1096_devs[LTC1096_devs_cnt].channel = 0x00; // channel address
  ++LTC1096_devs_cnt;

  return err;
}

/*
 * LTC1096_drv Remove
 * Called when the device is removed
 * Can deallocate data if needed
 */
static int LTC1096_drv_remove(struct spi_device *sdev)
{
  printk(KERN_ALERT "Removing spi device\n");

  //Sletter device.
  --LTC1096_devs_cnt;
  printk(KERN_ALERT "Removed LTC1096_devs%i on major:%i, minor:%i\n",
         LTC1096_devs_cnt, MAJOR(devno), LTC1096_devs_cnt);
  device_destroy(LTC1096_drv_class, MKDEV(MAJOR(devno), LTC1096_devs_cnt));

  return 0;
}

/*
 * spi Driver Struct
 * Holds function pointers to probe/release
 * methods and the name under which it is registered
 */
static const struct of_device_id matchtable[] = {
    {
        .compatible = "TargetBeam_Pistol, LTC1096",
    },
    {},
};

static struct spi_driver LTC1096_drv_spi_driver = {
    .probe = LTC1096_drv_probe,
    .remove = LTC1096_drv_remove,
    .driver = {
        .name = "LTC1096_drv",
        .bus = &spi_bus_type,
        .of_match_table = matchtable,
        .owner = THIS_MODULE,
    },
};

/**********************************************************
 * GENERIC LINUX DEVICE DRIVER STUFF
 **********************************************************/

/*
 * Assignment of module init/exit methods
 */
module_init(LTC1096_drv_init);
module_exit(LTC1096_drv_exit);

/*
 * Assignment of author and license
 */
MODULE_AUTHOR("Mikkel Reinholdt Hansen");
MODULE_LICENSE("GPL");
