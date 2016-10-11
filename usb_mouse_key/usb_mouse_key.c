#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb/input.h>
#include <linux/hid.h>

static struct input_dev *uk_dev;
static char *usb_buf;
static dma_addr_t usb_buf_phys;
static int len;
static struct urb *uk_urb;

static struct usb_device_id usb_mouse_key_id_table[] =
{
		{USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID, USB_INTERFACE_SUBCLASS_BOOT, 
			USB_INTERFACE_PROTOCOL_MOUSE)},
		{}
};



static void usb_mouse_key_irq(struct  urb* urb)
{
	#if 0
	int i;
	static int cnt = 0;
	printk("data cnt %d: ", ++cnt);
	for(i = 0; i < len; i++)
	{
		printk("%02x  ", usb_buf[i]);
	}
	printk("\n");

	
	#else
	//data cnt 2195: 02  01  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00
	//data cnt 2196: 02  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00
	//data cnt 2197: 02  02  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00
	//data cnt 2198: 02  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00
	//data cnt 2199: 02  04  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00
	//data cnt 2200: 02  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00
	/* USB����������ݺ���:
	 * data[1] : 	bit0--�����1-���£�0-�ɿ�
	 *			bit1--�Ҽ���1-���£�0-�ɿ�
	 *			bit2--�м���1-���£�0-�ɿ�
	 */
	
	static unsigned char pre_val;

	if((pre_val & (1 << 0) ) !=  (usb_buf[1] & (1 << 0)))  // pre_val & (1 << 0)      usb_buf[1] & (1 << 0)
	{
		/* ��������˱仯*/
		//printk("left key changed\n");
		input_event(uk_dev, EV_KEY, KEY_L, (usb_buf[1] & (1 << 0)) ? 1 : 0);
		input_sync(uk_dev);
	}

	if((pre_val & (1 << 1)) != (usb_buf[1] & (1 << 1)))
	{
		/* �Ҽ������˱仯*/
		//printk("right key changed\n");
		input_event(uk_dev, EV_KEY, KEY_S, (usb_buf[1] & (1 << 1)) ? 1 : 0);
		input_sync(uk_dev);
	}

	if((pre_val & (1 << 2)) != (usb_buf[1] & (1 << 2)))
	{
		/* �м������˱仯*/
		//printk("middle key changed\n");
		input_event(uk_dev, EV_KEY, KEY_ENTER, (usb_buf[1] & (1 << 2)) ? 1 : 0);
		input_sync(uk_dev);
	}

	pre_val = usb_buf[1];
	#endif

	/* �����ύurb */
	usb_submit_urb(uk_urb, GFP_KERNEL);
}

static int usb_mouse_key_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	struct usb_device *dev = interface_to_usbdev(intf);
	struct usb_host_interface *interface;
	struct usb_endpoint_descriptor *endpoint;
	int pipe;
	
	/*
	printk("found usb mouse!\n");

	printk("bcdUSB	= 0x%x\n", dev->descriptor.bcdUSB);
	printk("VID		= 0x%x\n", dev->descriptor.idVendor);
	printk("PID		= 0x%x\n", dev->descriptor.idProduct);
	*/

	interface = intf->cur_altsetting;
	endpoint = &interface->endpoint[0].desc;
	/*1������һ��input_dev�ṹ��*/
	uk_dev = input_allocate_device();
	
	/*2������*/
	/*2.1���ܲ��������¼�*/
	set_bit(EV_KEY, uk_dev->evbit);
	set_bit(EV_REP, uk_dev->evbit);

	/*2.2���ܲ�����Щ�¼�*/
	set_bit(KEY_L, uk_dev->keybit);
	set_bit(KEY_S, uk_dev->keybit);
	set_bit(KEY_ENTER, uk_dev->keybit);
	
	/*3��ע��*/
	input_register_device(uk_dev);

	/* 4��Ӳ����ز���*/
	/* ���ݴ�����Ҫ�أ�Դ��Ŀ�ġ�����*/
	/* Դ: USB�豸��ĳ���˵�*/
	pipe = usb_rcvintpipe(dev, endpoint->bEndpointAddress);
	
	/*   ����*/
	len = endpoint->wMaxPacketSize;
	
	/*   Ŀ��*/
	usb_buf = usb_buffer_alloc(dev, len, GFP_ATOMIC, &usb_buf_phys);

	/*  ʹ�����ݴ������Ҫ��*/
	/*  ����usb request block*/
	uk_urb = usb_alloc_urb(0, GFP_KERNEL);

	/*  ʹ�����ݴ������Ҫ�أ�����urb*/
	usb_fill_int_urb(uk_urb, dev, pipe, usb_buf, len, usb_mouse_key_irq, NULL, endpoint->bInterval);
	uk_urb->transfer_dma = usb_buf_phys;
	uk_urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

	/*  ʹ��URB*/
	usb_submit_urb(uk_urb, GFP_KERNEL);
	
	return 0;
}

static void usb_mouse_key_disconnect(struct usb_interface *intf)
{
	printk("disconnect usb mouse !\n");

	usb_kill_urb(uk_urb);
	usb_free_urb(uk_urb);

	usb_buffer_free(uk_dev, len, usb_buf, usb_buf_phys);
	input_unregister_device(uk_dev);
	input_free_device(uk_dev);
}

/*1�����䡢����usb_driver */
static struct usb_driver usb_mouse_key_driver =
{
	.name 		= "usb_mouse_key",
	.probe		= usb_mouse_key_probe,
	.disconnect	= usb_mouse_key_disconnect,
	.id_table		= usb_mouse_key_id_table,
};

static int usb_mouse_key_init(void)
{
	usb_register(&usb_mouse_key_driver);
	return 0;
}

static void usb_mouse_key_exit(void)
{
	usb_deregister(&usb_mouse_key_driver);
}

module_init(usb_mouse_key_init);
module_exit(usb_mouse_key_exit);
MODULE_LICENSE("GPL");






