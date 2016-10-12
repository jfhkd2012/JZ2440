/* �ο�:
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
	/* 1�� ����һ��gendisk�ṹ��*/
	ramblock_disk = alloc_disk(16);  /* ���豸�Ÿ���:  ��������+1 */
	/* 2�� ����*/
	/* 2.1������/����һ������: �ṩ��д����*/

	/* 2.2�������������ԣ�����������*/

	/* 3��ע�� */
	return 0;
}

static void ramdisk_exit(void)
{

}

module_init(ramdisk_init);
module_exit(ramdisk_exit);
MODULE_LICENSE("GPL");

