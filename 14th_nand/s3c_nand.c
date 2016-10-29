/******************  ²Î¿¼******************************
** drivers\mtd\nand\s3c2410.c
** drivers\mtd\nand\at91_nand.c
*******************************************************/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/clk.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>
#include <asm/arch/regs-nand.h>
#include <asm/arch/nand.h>

static struct nand_chip *s3c_nand;
static struct mtd_info *s3c_mtd;



