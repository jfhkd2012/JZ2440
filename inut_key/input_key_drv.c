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
#include <linux/irq.h>

#include <asm/gpio.h>
#include	<asm/io.h>
#include	<asm/arch/regs-gpio.h>

struct pin_desc{
	int irq;
	char *name;
	unsigned int pin;
	unsigned int key_val;
};

struct pin_desc pins_desc[4] = {
		{IRQ_EINT0,	"S2",	S3C2410_GPF0,	KEY_L},
		{IRQ_EINT2,	"S3",	S3C2410_GPF2,	KEY_S},
		{IRQ_EINT11,	"S4",	S3C2410_GPG3,	KEY_ENTER},
		{IRQ_EINT19,	"S5",	S3C2410_GPG11,	KEY_LEFTSHIFT},
};
static struct input_dev * input_key_dev;
static struct pin_desc *irq_pd;
static struct timer_list input_key_timer;

static irqreturn_t input_key_irq(int irq, void *dev_id)
{
	irq_pd = (struct pin_desc*)dev_id;
	mod_timer(&input_key_timer, jiffies + HZ / 100);	// 定时10ms

	return IRQ_RETVAL(IRQ_HANDLED);
}

static void input_key_timer_function(unsigned long data)
{
	struct pin_desc *pindesc = irq_pd;
	unsigned int pinval;

	if(!pindesc)
		return;

	pinval = s3c2410_gpio_getpin(pindesc->pin);
	if(pinval)
	{
		// 松开，最后一个参数: 0 ---松开，1---按下
		input_event(input_key_dev, EV_KEY, pindesc->key_val, 0);
		input_sync(input_key_dev);
	}
	else
	{
		input_event(input_key_dev, EV_KEY, pindesc->key_val, 1);
		input_sync(input_key_dev);
	}
}

static int input_key_drv_init(void)
{
	int i;
	// 1、分配一个input_dev结构体
	input_key_dev = input_allocate_device();
	
	// 2、设置
	// 2.1、设置能产生哪一类事件
	set_bit(EV_KEY, input_key_dev->evbit);	
	set_bit(EV_REP, input_key_dev->evbit);	


	// 2.2、能产生这类操作里的哪些事件: L, S, ENTER, LEFTSHIFT
	set_bit(KEY_L,		 input_key_dev->keybit);
	set_bit(KEY_S,		 input_key_dev->keybit);
	set_bit(KEY_ENTER,	 input_key_dev->keybit);
	set_bit(KEY_LEFTSHIFT,	 input_key_dev->keybit);
	
	// 3、注册
	input_register_device(input_key_dev);

	// 4、硬件相关操作
	// 4.1、定时器初始化
	init_timer(&input_key_timer);
	input_key_timer.function = input_key_timer_function;
	add_timer(&input_key_timer);

	// 注册四个按键的外部中断
	for(i = 0; i < 4; i++)
		request_irq(pins_desc[i].irq, input_key_irq, IRQT_BOTHEDGE, pins_desc[i].name,  &pins_desc[i]);
	
	return 0;
}

static void input_key_drv_exit(void)
{
	int i;

	for(i = 0; i < 4; i++)
		free_irq(pins_desc[i].irq, &pins_desc[i]);
	del_timer(&input_key_timer);
	input_unregister_device(input_key_dev);
	input_free_device(input_key_dev);
}

module_init(input_key_drv_init);
module_exit(input_key_drv_exit);
MODULE_LICENSE("GPL");

