#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>



static int major;
static struct class *key_drv_class;
static struct class_device	*key_drv_class_dev;

volatile unsigned long *gpfcon = NULL;
volatile unsigned long *gpfdat = NULL;
volatile unsigned long *gpgcon = NULL;
volatile unsigned long *gpgdat = NULL;

static int key_drv_open(struct inode * inode, struct file * file)
{
	printk("key_drv_open\n");
	*gpfcon &= ~((0x03 << (0 * 2)) | (0x03 << (2 * 2)));
	*gpgcon &= ~((0x03 << (3 * 2)) | (0x03 << (11 * 2)));

	return 0;
}
static ssize_t key_drv_write(struct tty_struct * tty, const unsigned char * buf, int count)
{
	printk("key_drv_write\n");

	return 0;
}
static ssize_t key_drv_read( struct file *file,  char __user *buffer,   size_t len,  loff_t *offset )
{
	unsigned char key_value[4];
	int regval;
	
	//printk("key_drv_read\n");

	if(len != sizeof(key_value))
		return -EINVAL;

	regval = *gpfdat;
	key_value[0] = (regval & (1 << 0)) ? 1 : 0;
	key_value[1] = (regval & (1 << 2)) ? 1 : 0;

	regval = *gpgdat;
	key_value[2] = (regval & (1 << 3)) ? 1 : 0;
	key_value[3] = (regval & (1 << 11)) ? 1 : 0;

	copy_to_user(buffer, key_value, sizeof(key_value));

	return sizeof(key_value);
}


static struct file_operations	key_drv_fops = {
		.owner	=	THIS_MODULE,
		.open	=	key_drv_open,
		.write	=	key_drv_write,
		.read	=	key_drv_read,
};

static int key_drv_init(void)
{
	major = register_chrdev(0, "key_drv", &key_drv_fops);

	key_drv_class = class_create(THIS_MODULE, "key_drv");
	key_drv_class_dev = class_device_create(key_drv_class, NULL, MKDEV(major, 0), NULL, "key");

	gpfcon = (volatile unsigned long *)ioremap(0x56000050, 16);
	gpfdat = gpfcon + 1;
	gpgcon = (volatile unsigned long *)ioremap(0x56000060, 16);
	gpgdat = gpgcon + 1;
	
	return 0;
}

static void key_drv_exit(void)
{
	unregister_chrdev(major, "key_drv");

	class_device_unregister(key_drv_class_dev);
	class_destroy(key_drv_class);
	iounmap(gpfcon);
	iounmap(gpgcon);
}

module_init(key_drv_init);
module_exit(key_drv_exit);

MODULE_LICENSE("GPL v2");
