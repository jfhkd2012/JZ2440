#include <linux/module.h>
#include <linux/version.h>

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/types.h>

#include <asm/uaccess.h>
#include <asm/io.h>

static struct class *cls;
static volatile unsigned long * gpio_con;
static volatile unsigned long * gpio_dat;
static int pin;
static unsigned int major;

static int led_drv_open (struct inode *inode, struct file *file)
{
	printk("led_drv_open\n");
	/* 配置为输出 */
	*gpio_con &= ~(0x3 << (pin * 2));	
	*gpio_con |= (0x1 << (pin * 2));

	return 0;
}
static ssize_t led_drv_write (struct file *file, const char __user *buf, size_t cnt, loff_t *ppos)
{
	int val;
	copy_from_user(&val, buf, cnt);
	if(val == 1)
	{
		*gpio_dat &= ~(1 << pin);		// led on
	}
	else
	{
		*gpio_dat |= (1 << pin);		// led off
	}

	return 0;
}

static struct file_operations led_fops = {
	.owner	=	THIS_MODULE,
	.open	=	led_drv_open,
	.write	=	led_drv_write,
};
static int led_drv_probe(struct platform_device * pdev)
{
	struct resource *res;

	/* 根据platform_device的资源进行ioremap */
	res = platform_get_resource(pdev, IORESOURCE_IO,  0);
	gpio_con = ioremap(res->start, res->end - res->start + 1);
	gpio_dat = gpio_con + 1;

	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	pin = res->start;

	printk("led_probe, found led\n");

	/* 注册字符设备驱动程序 */
	major = register_chrdev(0, "myled", &led_fops);
	cls = class_create(THIS_MODULE, "myled");
	class_device_create(cls, NULL, MKDEV(major, 0), NULL, "led");

	return 0;
}
static int led_drv_remove(struct platform_device * pdev)
{
	printk("led_drv_remove, remove led\n");

	/* 卸载字符设备驱动程序 */
	/* iounmap */
	class_device_destroy(cls, MKDEV(major, 0));
	class_destroy(cls);
	unregister_chrdev(major, "myled");
	iounmap(gpio_con);
	
	return 0;
}

struct platform_driver  led_drv ={
	.probe	=	led_drv_probe,
	.remove	=	led_drv_remove,
	.driver	=	{
		.name	=	"myled",
	}
};

static int led_drv_init(void)
{
	platform_driver_register(&led_drv);
	return 0;
}

static void led_drv_exit(void)
{
	platform_driver_unregister(&led_drv);
}

module_init(led_drv_init);
module_exit(led_drv_exit);
MODULE_LICENSE("GPL");