/* 参考:
 * drivers\block\xd.c
 * drivers\block\z2ram.c
 */

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/genhd.h>
#include <linux/hdreg.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/wait.h>
#include <linux/blkdev.h>
#include <linux/blkpg.h>
#include <linux/delay.h>
#include <linux/io.h>

#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/dma.h>

static struct gendisk *ramblock_disk;

static int ramdisk_init(void)
{
	/* 1、 分配一个gendisk结构体*/
	ramblock_disk = alloc_disk(16);  /* 次设备号个数:  分区个数+1 */
	/* 2、 设置*/
	/* 2.1、设置/分配一个队列: 提供读写能力*/

	/* 2.2、设置其他属性，比如容量等*/

	/* 3、注册 */
	return 0;
}

static void ramdisk_exit(void)
{

}

module_init(ramdisk_init);
module_exit(ramdisk_exit);
MODULE_LICENSE("GPL");

