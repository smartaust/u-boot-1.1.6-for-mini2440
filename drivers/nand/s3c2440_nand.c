/*
 * (C) Copyright 2006 OpenMoko, Inc.
 * Author: Harald Welte <laforge@openmoko.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>

#include <nand.h>
#include <s3c2440.h>

#define S3C2410_NFSTAT_READY (1<<0)
#define S3C2410_NFCONF_nFCE (1<<11)

#define S3C2440_NFSTAT_READY (1<<0)
#define S3C2440_NFCONF_nFCE (1<<1)

static void s3c2410_nand_select_chip(struct mtd_info *mtd,int chip)
{
	S3C2410_NAND *nand = S3C2410_GetBase_NAND();
	if(chip == -1)
		nand->NFCONF |= S3C2440_NFCONF_nFCE;
	else 	
		nand->NFCONF &= ~S3C2440_NFCONF_nFCE;
}

static void s3c2410_nand_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct nand_chip *chip = mtd->priv;

	 
	S3C2410_NAND *nand = S3C2410_GetBase_NAND();


	printf("cmd is coming\r\n");
	return ;
	debugX(1, "hwcontrol(): 0x%02x 0x%02x\n", cmd, ctrl);

	switch(cmd) 
	{
			case NAND_CTL_SETNCE:
			case NAND_CTL_CLRNCE:
					printf("%s: called for NCE\n",__FUNCTION__);
					break;

			case NAND_CTL_SETCLE:
					chip->IO_ADDR_W = (void *)&nand->NFCMD;
					break;

			case NAND_CTL_SETALE:
					chip->IO_ADDR_W = (void *)&nand->NFADDR;
					break;
			default :
				    chip->IO_ADDR_W = (void *)&nand->NFDATA;
					break;

					
	}		
}

static int s3c2410_nand_dev_ready(struct mtd_info *mtd)
{
	S3C2410_NAND *nand = S3C2410_GetBase_NAND();
	debugX(1, "dev_ready\n");
	return (nand->NFSTAT) & S3C2440_NFSTAT_READY;
}

static void s3c24x0_nand_inithw(void)
{
		
	S3C2410_NAND *nand = S3C2410_GetBase_NAND();

	#define TACLS 0
	#define TWRPH0 4
	#define TWRPH1 2
	{
		nand->NFCONF = (TACLS<<12)|(TWRPH0 << 8)|(TWRPH1 <<4);
		nand->NFCONF = (1<<4)|(0<<1)|(1<<0);
	}

	puts("in nand init fuck\n");
}

int board_nand_init(struct nand_chip *chip)
{
	S3C24X0_CLOCK_POWER *clk_power =S3C24X0_GetBase_CLOCK_POWER();
	S3C2410_NAND *nand = S3C2410_GetBase_NAND();

	printf("this is my version\r\n");
	debugX(1, "board_nand_init()\n");

	s3c24x0_nand_inithw();
	
	chip->IO_ADDR_R = (void *)&nand->NFDATA;
	chip->IO_ADDR_W = (void *)&nand->NFDATA;
	chip->hwcontrol = s3c2410_nand_hwcontrol;
	chip->dev_ready = s3c2410_nand_dev_ready;
	chip->select_chip = s3c2410_nand_select_chip;
	chip->options= 0;

	chip->eccmode = NAND_ECC_SOFT;
	return 0;
}
