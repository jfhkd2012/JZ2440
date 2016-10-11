#include	<linux/module.h>
#include	<linux/kernel.h>
#include	<linux/fs.h>
#include	<linux/init.h>
#include	<linux/delay.h>

#include	<asm/uaccess.h>
#include	<asm/irq.h>
#include	<asm/io.h>
#include	<asm/arch/regs-gpio.h>
#include	<asm/hardware.h>

//volatile unsigned long *myGPFCON = NULL;
//volatile unsigned long *myGPFDAT = NULL;
volatile unsigned long *gpfcon = NULL;
volatile unsigned long *gpfdat = NULL;

static int major;
static struct class *myled_drv_class;
static struct class_device *myled_drv_class_dev;

static int myled_drv_open(struct inode *inode, struct file *file)
{
	printk("myled_drv_open\n");

	//*myGPFCON &= ~((0x3 << (4 * 2)) | (0x3 << (5 * 2)) | (0x3 << (6 * 2)));
	//*myGPFDAT |= ((0x1 << (4 * 2)) | (0x1 << ( 5 * 2)) | (0X1 << (6 * 2)));
	*gpfcon &= ~((0x3<<(4*2)) | (0x3<<(5*2)) | (0x3<<(6*2)));
	*gpfcon |= ((0x1<<(4*2)) | (0x1<<(5*2)) | (0x1<<(6*2)));
	return 0;
}

static ssize_t myled_drv_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
{
	int val;
	
	printk("myled_drv_write\n");
	copy_from_user(&val, buf, count);
	if(1 == val)
	{
		//*myGPFDAT &= ~((1 << 4) | (1 << 5) | (1 << 6));
		*gpfdat &= ~((1<<4) | (1<<5) | (1<<6));
		printk("open leds\n");
	}
	else
	{
		//*myGPFDAT |= (1 << 4) | (1 << 5) | (1 << 6);
		*gpfdat |= (1<<4) | (1<<5) | (1<<6);
		printk("close leds\n");
	}
	return 0;
}

static struct file_operations myled_drv_fops ={
	.owner	=	THIS_MODULE,
	.open	=	myled_drv_open,
	.write	=	myled_drv_write,
};

static int myled_drv_init(void)
{

	printk("myled_drv_init, register char device led\n");
	major = register_chrdev(0,  "myled_drv", &myled_drv_fops);

	myled_drv_class = class_create(THIS_MODULE, "myled_drv");
	myled_drv_class_dev = class_device_create(myled_drv_class, NULL, MKDEV(major, 0), NULL, "myled");

	//myGPFCON = (volatile unsigned long *)ioremap(0x56000050, 16);
	gpfcon        = (volatile unsigned long *)ioremap(0x56000050, 16);
	//myGPFDAT = myGPFCON + 1;
	gpfdat = gpfcon + 1;

	
	return 0;
}

static void myled_drv_exit(void)
{
	printk("unregister_chrdev\n");
	unregister_chrdev(major, "myled_drv");

	class_device_unregister(myled_drv_class_dev);
	class_destroy(myled_drv_class);
	//iounmap(myGPFCON);
	iounmap(gpfcon);
}

module_init(myled_drv_init);
module_exit(myled_drv_exit);

MODULE_LICENSE("GPL v2");
