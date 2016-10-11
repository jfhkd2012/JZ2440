#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>

static int major;
static struct class * key_int_drv_class;
static struct class_device * key_int_drv_class_dev;

volatile unsigned long *gpfcon;
volatile unsigned long *gpfdat;
volatile unsigned long *gpgcon;
volatile unsigned long *gpgdat;

struct pin_desc{
unsigned int pin;
unsigned int key_val;
};

static unsigned char key_val;

// interrupt flag, interrupt server function set it 1, key_int_drv_read clear it
static volatile int ev_press = 0;

static struct fasync_struct *button_async;


static DECLARE_WAIT_QUEUE_HEAD(button_waitq);

static DECLARE_MUTEX(button_lock);

#ifdef __ATOMIC__
static atomic_t  canopen = ATOMIC_INIT(1);	//定义原子变量，并初始化为1
#endif


struct pin_desc pins_desc[4] = {
		{S3C2410_GPF0, 0x01},
			{S3C2410_GPF2, 0x02},
			{S3C2410_GPG3, 0x03},
			{S3C2410_GPG11, 0x04},
};
static irqreturn_t key_int_irq(int irq, void *dev_id)
{
	struct pin_desc *pindesc = (struct pin_desc *)dev_id;
	unsigned int pinval;

	pinval = s3c2410_gpio_getpin(pindesc->pin);
	if(pinval)
	{
		key_val = 0x80 | pindesc->key_val;
	}
	else
	{
		key_val = pindesc->key_val;
	}

	ev_press = 1;
	wake_up_interruptible(&button_waitq);

	kill_fasync(&button_async, SIGIO, POLL_IN);
	
	return IRQ_RETVAL(IRQ_HANDLED);
}
static int key_int_drv_open(struct inode * inode, struct file * file)
{
	printk("key_int_drv_open\n");

	#ifdef __ATOMIC__
	if(!atomic_dec_and_test(&canopen))
	{
		atomic_inc(&canopen);
		return -EBUSY;
	}
	#endif
	
	down(&button_lock);
	
	request_irq(IRQ_EINT0,   key_int_irq, IRQT_BOTHEDGE, "KEY1", &pins_desc[0]);
	request_irq(IRQ_EINT2,   key_int_irq, IRQT_BOTHEDGE, "KEY2", &pins_desc[1]);
	request_irq(IRQ_EINT11, key_int_irq, IRQT_BOTHEDGE, "KEY3", &pins_desc[2]);
	request_irq(IRQ_EINT19, key_int_irq, IRQT_BOTHEDGE, "KEY4", &pins_desc[3]);
	return 0;
}

static ssize_t key_int_drv_write(struct tty_struct * tty, const char __user * buf, int count)
{
	printk("key_int_drv_write\n");

	return 0;
}

static ssize_t key_int_drv_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	if(size != 1)
		return -EINVAL;
	// if no key pressed, sleep;
	wait_event_interruptible(button_waitq, ev_press);
	// if have key pressed, return key_value
	copy_to_user(buf, &key_val, 1);
	ev_press = 0;
	
	return 1;
}


static int key_int_drv_release(struct inode *inode, struct file *file)
{

	#ifdef __ATOMIC__
	atomic_inc(&canopen);
	#endif
	
	free_irq(IRQ_EINT0, &pins_desc[0]);
	free_irq(IRQ_EINT2, &pins_desc[1]);
	free_irq(IRQ_EINT11, &pins_desc[2]);
	free_irq(IRQ_EINT19, &pins_desc[3]);

	up(&button_lock);
	
	return 0;
}

static unsigned int key_int_drv_poll(struct file *file, struct poll_table_struct *wait)
{
	unsigned int mask = 0;

	poll_wait(file, &button_waitq, wait);

	if(ev_press)
		mask |= POLLIN | POLLRDNORM;

	return mask;
}

static int key_int_drv_fasync(int fd, struct file *filp, int on)
{
	printk("driver : key_int_drv_fasync\n");
	return fasync_helper(fd, filp, on, &button_async);
}

static struct file_operations key_int_drv_fops = {
	.owner	=	THIS_MODULE,
	.open	=	key_int_drv_open,
	.read	=	key_int_drv_read,
	.write	=	key_int_drv_write,
	.release	=	key_int_drv_release,
	.poll		=	key_int_drv_poll,
	.fasync	=	key_int_drv_fasync,
};

static int key_int_drv_init(void)
{
	major = register_chrdev(0, "key_int_drv", &key_int_drv_fops);

	key_int_drv_class = class_create(THIS_MODULE, "key");
	key_int_drv_class_dev = class_device_create(key_int_drv_class, NULL, MKDEV(major, 0), NULL, "key");

	gpfcon = (volatile unsigned long *)ioremap(0x56000050, 16);
	gpfdat = gpfcon + 1;
	gpgcon = (volatile unsigned long *)ioremap(0x56000060, 16);
	gpgdat = gpgcon + 1;
	return 0;
}

static void key_int_drv_exit(void)
{
	unregister_chrdev(major, "key");
	class_device_unregister(key_int_drv_class_dev);
	class_destroy(key_int_drv_class);
	iounmap(gpfcon);
	iounmap(gpgcon);
}


module_init(key_int_drv_init);
module_exit(key_int_drv_exit);

MODULE_LICENSE("GPL");


