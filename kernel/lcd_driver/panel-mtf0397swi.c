// SPDX-License-Identifier: GPL-2.0
/*
 * Driver based on sunxi sl698ph_720p and mainline panel-himax-hx8394
 * to suport the "MTF0397SWI-06" tft lcd panl using the ic OTM8019A.
 * Copyright (C) 2025 Patrick Areny
 * 
 * Based on the driver for panels based on Himax HX8394 controller, such as:
 * - HannStar HSD060BHW4 5.99" MIPI-DSI panel
 * Copyright (C) 2021 Kamil Trzciński
 *
 * Based on drivers/gpu/drm/panel/panel-sitronix-st7703.c
 * Copyright (C) Purism SPC 2019
 */

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/media-bus-format.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/regulator/consumer.h>

#include <video/mipi_display.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

/* Manufacturer specific commands sent via DSI, listed in HX8394-F datasheet */
#define HX8394_CMD_SETSEQUENCE	  0xb0
#define HX8394_CMD_SETPOWER	  0xb1
#define HX8394_CMD_SETDISP	  0xb2
#define HX8394_CMD_SETCYC	  0xb4
#define HX8394_CMD_SETVCOM	  0xb6
#define HX8394_CMD_SETTE	  0xb7
#define HX8394_CMD_SETSENSOR	  0xb8
#define HX8394_CMD_SETEXTC	  0xb9
#define HX8394_CMD_SETMIPI	  0xba
#define HX8394_CMD_SETOTP	  0xbb
#define HX8394_CMD_SETREGBANK	  0xbd
#define HX8394_CMD_UNKNOWN1	  0xc0
#define HX8394_CMD_SETDGCLUT	  0xc1
#define HX8394_CMD_SETID	  0xc3
#define HX8394_CMD_SETDDB	  0xc4
#define HX8394_CMD_UNKNOWN2	  0xc6
#define HX8394_CMD_SETCABC	  0xc9
#define HX8394_CMD_SETCABCGAIN	  0xca
#define HX8394_CMD_SETPANEL	  0xcc
#define HX8394_CMD_SETOFFSET	  0xd2
#define HX8394_CMD_SETGIP0	  0xd3
#define HX8394_CMD_UNKNOWN3	  0xd4
#define HX8394_CMD_SETGIP1	  0xd5
#define HX8394_CMD_SETGIP2	  0xd6
#define HX8394_CMD_SETGPO	  0xd6
#define HX8394_CMD_SETSCALING	  0xdd
#define HX8394_CMD_SETIDLE	  0xdf
#define HX8394_CMD_SETGAMMA	  0xe0
#define HX8394_CMD_SETCHEMODE_DYN 0xe4
#define HX8394_CMD_SETCHE	  0xe5
#define HX8394_CMD_SETCESEL	  0xe6
#define HX8394_CMD_SET_SP_CMD	  0xe9
#define HX8394_CMD_SETREADINDEX	  0xfe
#define HX8394_CMD_GETSPIREAD	  0xff

struct mtf0397swi {
	struct device *dev;
	struct drm_panel panel;
	struct gpio_desc *reset_gpio;
	struct regulator *vcc;
	struct regulator *iovcc;
	bool prepared;

	const struct mtf0397swi_panel_desc *desc;
};

struct mtf0397swi_panel_desc {
	const struct drm_display_mode *mode;
	unsigned int lanes;
	unsigned long mode_flags;
	enum mipi_dsi_pixel_format format;
	int (*init_sequence)(struct mtf0397swi *ctx);
};

static inline struct mtf0397swi *panel_to_mtf0397swi(struct drm_panel *ppanel)
{
	return container_of(ppanel, struct mtf0397swi, panel);
}

static int mtf0397swi_init_sequence(struct mtf0397swi *ctx)
{
	dev_info(ctx->dev, "mtf0397swi init_sequence start\n");
	struct mipi_dsi_device *dsi = to_mipi_dsi_device(ctx->dev);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xFF, 0x80, 0x19, 0x01);
	//dev_info(ctx->dev, "sent 0xFF, 0x80, 0x19, 0x01\n");
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x80);
	//dev_info(ctx->dev, "sent 0x00, 0x80\n");
	mipi_dsi_dcs_write_seq(dsi, 0xFF, 0x80, 0x19);
	//dev_info(ctx->dev, "sent  0xFF, 0x80, 0x19\n");
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x8A);
	mipi_dsi_dcs_write_seq(dsi, 0xC4, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xA6);
	mipi_dsi_dcs_write_seq(dsi, 0xB3, 0x20, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x90);
	mipi_dsi_dcs_write_seq(dsi, 0xC0, 0x00, 0x15, 0x00, 0x00, 0x00, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xB4);
	mipi_dsi_dcs_write_seq(dsi, 0xC0, 0x00, 0x48);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x81);
	mipi_dsi_dcs_write_seq(dsi, 0xC1, 0x33);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x81);
	mipi_dsi_dcs_write_seq(dsi, 0xC4, 0x81);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x87);
	mipi_dsi_dcs_write_seq(dsi, 0xC4, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x89);
	mipi_dsi_dcs_write_seq(dsi, 0xC4, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x82);
	mipi_dsi_dcs_write_seq(dsi, 0xC5, 0xB0);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x90);
	mipi_dsi_dcs_write_seq(dsi, 0xC5, 0x4E, 0x79, 0x06, 0x91, 0x33, 0x34, 0x23);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xB1);
	mipi_dsi_dcs_write_seq(dsi, 0xC5, 0xA8);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xD8, 0x68, 0x68);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xD9, 0x44);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0xCE, 0x86, 0x01, 0x00, 0x85, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xA0);
	mipi_dsi_dcs_write_seq(dsi, 0xCE, 0x18, 0x05, 0x83, 0x39, 0x00, 0x00, 0x00, 0x18, 0x04, 0x83, 0x3A, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xB0);
	mipi_dsi_dcs_write_seq(dsi, 0xCE, 0x18, 0x03, 0x83, 0x3B, 0x86, 0x00, 0x00, 0x18, 0x02, 0x83, 0x3C, 0x88, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xC0);
	mipi_dsi_dcs_write_seq(dsi, 0xCF, 0x01, 0x01, 0x20, 0x20, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xD0);
	mipi_dsi_dcs_write_seq(dsi, 0xCF, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xC0);
	mipi_dsi_dcs_write_seq(dsi, 0xCB, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xD0);
	mipi_dsi_dcs_write_seq(dsi, 0xCB, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xD5);
	mipi_dsi_dcs_write_seq(dsi, 0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xE0);
	mipi_dsi_dcs_write_seq(dsi, 0xCB, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0xCC, 0x00, 0x26, 0x09, 0x0B, 0x01, 0x25, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x90);
	mipi_dsi_dcs_write_seq(dsi, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x9A);
	mipi_dsi_dcs_write_seq(dsi, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xA0);
	mipi_dsi_dcs_write_seq(dsi, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x25, 0x02, 0x0C, 0x0A, 0x26, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xB0);
	mipi_dsi_dcs_write_seq(dsi, 0xCC, 0x00, 0x25, 0x0C, 0x0A, 0x02, 0x26, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xC0);
	mipi_dsi_dcs_write_seq(dsi, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xCA);
	mipi_dsi_dcs_write_seq(dsi, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xD0);
	mipi_dsi_dcs_write_seq(dsi, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x26, 0x01, 0x09, 0x0B, 0x25, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xE1, 0x00, 0x16, 0x24, 0x30, 0x41, 0x4E, 0x4F, 0x79, 0x6A, 0x83, 0x7F, 0x6A, 0x7D, 0x5D, 0x5B, 0x51, 0x44, 0x3C, 0x38, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xE2, 0x00, 0x15, 0x25, 0x30, 0x41, 0x4D, 0x50, 0x79, 0x6A, 0x84, 0x7F, 0x6B, 0x7C, 0x5C, 0x5B, 0x51, 0x44, 0x3C, 0x38, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0xC4, 0x30);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x98);
	mipi_dsi_dcs_write_seq(dsi, 0xC0, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xA9);
	mipi_dsi_dcs_write_seq(dsi, 0xC0, 0x0A);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xB0);
	mipi_dsi_dcs_write_seq(dsi, 0xC1, 0x20, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xE1);
	mipi_dsi_dcs_write_seq(dsi, 0xC0, 0x40, 0x30);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0xC1, 0x03, 0x33);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xA0);
	mipi_dsi_dcs_write_seq(dsi, 0xC1, 0xE8);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x90);
	mipi_dsi_dcs_write_seq(dsi, 0xB6, 0xB4);
	// The following lines seem to be delays or NOPs, not DSI commands
	msleep(10);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xFB, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xFF, 0xFF, 0xFF, 0xFF);
	msleep(20);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x3A, 0x77);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x11, 0x00);
	msleep(120);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x29, 0x00);
	msleep(100);
	return 0;
}

/*
Not the same as in lcd.ko
static int mtf0397swi_init_sequence(struct mtf0397swi *ctx)
{
	struct mipi_dsi_device *dsi = to_mipi_dsi_device(ctx->dev);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xFF, 0x12,0x83,0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0xFF, 0x12,0x83);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0xC0, 0x00,0x64,0x00,0x10,0x10,0x00,0x64,0x10,0x10);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x90);
	mipi_dsi_dcs_write_seq(dsi, 0xC0, 0x00,0x56,0x00,0x01,0x00,0x04);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xA4);
	mipi_dsi_dcs_write_seq(dsi, 0xC0, 0x16);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xB3);
	mipi_dsi_dcs_write_seq(dsi, 0xC0, 0x00,0x50);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x81);
	mipi_dsi_dcs_write_seq(dsi, 0xC1, 0x66);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x90);
	mipi_dsi_dcs_write_seq(dsi, 0xC4, 0x49);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xA0);
	mipi_dsi_dcs_write_seq(dsi, 0xC4, 0x05,0x10,0x06,0x02,0x05,0x15,0x10,0x05,0x10,0x07,0x02,0x05,0x15,0x10);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xB0);
	mipi_dsi_dcs_write_seq(dsi, 0xC4, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x91);
	mipi_dsi_dcs_write_seq(dsi, 0xC5, 0x46, 0x40, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xD8, 0xCF, 0xCF);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xD9, 0x47);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x81);
	mipi_dsi_dcs_write_seq(dsi, 0xC4, 0x82);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xB0);
	mipi_dsi_dcs_write_seq(dsi, 0xC5, 0x04, 0xB8);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xBB);
	mipi_dsi_dcs_write_seq(dsi, 0xC5, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x82);
	mipi_dsi_dcs_write_seq(dsi, 0xC4, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xC6);
	mipi_dsi_dcs_write_seq(dsi, 0xB0, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xD0, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xD1, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0xCB, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x90);
	mipi_dsi_dcs_write_seq(dsi, 0xCB, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xA0);
	mipi_dsi_dcs_write_seq(dsi, 0xCB, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xB0);
	mipi_dsi_dcs_write_seq(dsi, 0xCB, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xC0);
	mipi_dsi_dcs_write_seq(dsi, 0xCB, 0x05,0x05,0x05,0x05,0x05,0x05,0x00,0x00,0x00,0x00,0x05,0x05,0x00,0x05,0x05);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xD0);
	mipi_dsi_dcs_write_seq(dsi, 0xCB, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x05,0x05,0x05,0x05,0x05,0x00,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xE0);
	mipi_dsi_dcs_write_seq(dsi, 0xCB, 0x00,0x00,0x05,0x05,0x00,0x05,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xF0);
	mipi_dsi_dcs_write_seq(dsi, 0xCB, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0xCC, 0x0E,0x10,0x0A,0x0C,0x02,0x04,0x00,0x00,0x00,0x00,0x2E,0x2D,0x00,0x29,0x2A);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x90);
	mipi_dsi_dcs_write_seq(dsi, 0xCC, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0D,0x0F,0x09,0x0B,0x01,0x03,0x00,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xA0);
	mipi_dsi_dcs_write_seq(dsi, 0xCC, 0x00,0x00,0x2E,0x2D,0x00,0x29,0x2A,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xB0);
	mipi_dsi_dcs_write_seq(dsi, 0xCC, 0x0B,0x09,0x0F,0x0D,0x03,0x01,0x00,0x00,0x00,0x00,0x2D,0x2E,0x00,0x29,0x2A);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xC0);
	mipi_dsi_dcs_write_seq(dsi, 0xCC, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x0A,0x10,0x0E,0x04,0x02,0x00,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xD0);
	mipi_dsi_dcs_write_seq(dsi, 0xCC, 0x00,0x00,0x2D,0x2E,0x00,0x29,0x2A,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0xCE, 0x8B,0x03,0x18,0x8A,0x03,0x18,0x89,0x03,0x18,0x88,0x03,0x18);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x90);
	mipi_dsi_dcs_write_seq(dsi, 0xCE, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xA0);
	mipi_dsi_dcs_write_seq(dsi, 0xCE, 0x38,0x07,0x05,0x00,0x00,0x18,0x00,0x38,0x06,0x05,0x01,0x00,0x18,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xB0);
	mipi_dsi_dcs_write_seq(dsi, 0xCE, 0x38,0x05,0x05,0x02,0x00,0x18,0x00,0x38,0x04,0x05,0x03,0x00,0x18,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xC0);
	mipi_dsi_dcs_write_seq(dsi, 0xCE, 0x38,0x03,0x05,0x04,0x00,0x18,0x00,0x38,0x02,0x05,0x05,0x00,0x18,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xD0);
	mipi_dsi_dcs_write_seq(dsi, 0xCE, 0x38,0x01,0x05,0x06,0x00,0x18,0x00,0x38,0x00,0x05,0x07,0x00,0x18,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0xCF, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x90);
	mipi_dsi_dcs_write_seq(dsi, 0xCF, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xA0);
	mipi_dsi_dcs_write_seq(dsi, 0xCF, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xB0);
	mipi_dsi_dcs_write_seq(dsi, 0xCF, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xC0);
	mipi_dsi_dcs_write_seq(dsi, 0xCF, 0x3D,0x02,0x15,0x20,0x00,0x00,0x01,0x81,0x00,0x03,0x08);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xB5);
	mipi_dsi_dcs_write_seq(dsi, 0xC5, 0x00,0x6F,0xFF,0x00,0x6F,0xFF);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x90);
	mipi_dsi_dcs_write_seq(dsi, 0xF5, 0x02,0x11,0x02,0x11);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x90);
	mipi_dsi_dcs_write_seq(dsi, 0xC5, 0x50);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x94);
	mipi_dsi_dcs_write_seq(dsi, 0xC5, 0x66);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xB2);
	mipi_dsi_dcs_write_seq(dsi, 0xF5, 0x00,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xB4);
	mipi_dsi_dcs_write_seq(dsi, 0xF5, 0x00,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xB6);
	mipi_dsi_dcs_write_seq(dsi, 0xF5, 0x00,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xB8);
	mipi_dsi_dcs_write_seq(dsi, 0xF5, 0x00,0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x94);
	mipi_dsi_dcs_write_seq(dsi, 0xF5, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xBA);
	mipi_dsi_dcs_write_seq(dsi, 0xF5, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xB4);
	mipi_dsi_dcs_write_seq(dsi, 0xC5, 0xC0);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xE1, 0x00,0x0C,0x12,0x0F,0x08,0x10,0x0b,0x0A,0x04,0x07,0x0D,0x07,0x0E,0x15,0x10,0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xE2, 0x00,0x0C,0x12,0x0E,0x07,0x10,0x0b,0x09,0x04,0x07,0x0D,0x07,0x0E,0x15,0x10,0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xB4);
	mipi_dsi_dcs_write_seq(dsi, 0xC0, 0x55);
	// necessary start
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0xC4, 0x30);
	msleep(10);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x8b);
	mipi_dsi_dcs_write_seq(dsi, 0xC4, 0x00);
	msleep(10);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x90);
	mipi_dsi_dcs_write_seq(dsi, 0xC4, 0x52);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xb9);
	mipi_dsi_dcs_write_seq(dsi, 0xb0, 0x51);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x82);
	mipi_dsi_dcs_write_seq(dsi, 0xC4, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xA2);
	mipi_dsi_dcs_write_seq(dsi, 0xC1, 0x08);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xA4);
	mipi_dsi_dcs_write_seq(dsi, 0xC1, 0xF0);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xC3);
	mipi_dsi_dcs_write_seq(dsi, 0xF5, 0x81);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xB1);
	mipi_dsi_dcs_write_seq(dsi, 0xC6, 0x05);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xA4);
	mipi_dsi_dcs_write_seq(dsi, 0xC0, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x87);
	mipi_dsi_dcs_write_seq(dsi, 0xC4, 0x18);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xC2);
	mipi_dsi_dcs_write_seq(dsi, 0xF5, 0xC0);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0xC4, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x88);
	mipi_dsi_dcs_write_seq(dsi, 0xC4, 0x80);
	// necessary end
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xff, 0xff,0xff,0xff);
	msleep(20);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x11, 0x00);
	msleep(120);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x29, 0x00);
	msleep(100);
	return 0;
}*/

static const struct drm_display_mode mtf0397swi_mode = {
	.hdisplay    = 480,
	.hsync_start = 480 + 44,
	.hsync_end   = 480 + 44 + 6,
	.htotal	     = 480 + 44 + 6 + 45,
	.vdisplay    = 800,
	.vsync_start = 800 + 16,
	.vsync_end   = 800 + 16 + 1,
	.vtotal	     = 800 + 16 + 1 + 79,
	.clock	     = 31000,
	.flags	     = DRM_MODE_FLAG_NHSYNC | DRM_MODE_FLAG_NVSYNC,
	.width_mm    = 51,
	.height_mm   = 86,
};

// MIPI_DSI_FMT_RGB888 for 16.7M colors
static const struct mtf0397swi_panel_desc mtf0397swi_desc = {
	.mode = &mtf0397swi_mode,
	.lanes = 2,
	.mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST,
	.format = MIPI_DSI_FMT_RGB888,
	.init_sequence = mtf0397swi_init_sequence,
};

static int mtf0397swi_enable(struct drm_panel *panel)
{
	struct mtf0397swi *ctx = panel_to_mtf0397swi(panel);
	struct mipi_dsi_device *dsi = to_mipi_dsi_device(ctx->dev);
	int ret;

	ret = ctx->desc->init_sequence(ctx);
	if (ret) {
		dev_err(ctx->dev, "Panel init sequence failed: %d\n", ret);
		return ret;
	}

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret) {
		dev_err(ctx->dev, "Failed to exit sleep mode: %d\n", ret);
		return ret;
	}

	/* Panel is operational 120 msec after reset */
	msleep(120);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret) {
		dev_err(ctx->dev, "Failed to turn on the display: %d\n", ret);
		goto sleep_in;
	}
	dev_info(ctx->dev, "Panel initialized and display enabled\n");
	return 0;

sleep_in:
	/* This will probably fail, but let's try orderly power off anyway. */
	if (!mipi_dsi_dcs_enter_sleep_mode(dsi))
		msleep(50);

	return ret;
}

static int mtf0397swi_disable(struct drm_panel *panel)
{
	struct mtf0397swi *ctx = panel_to_mtf0397swi(panel);
	struct mipi_dsi_device *dsi = to_mipi_dsi_device(ctx->dev);
	int ret;

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret) {
		dev_err(ctx->dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}

	msleep(50); /* about 3 frames */

	return 0;
}

static int mtf0397swi_unprepare(struct drm_panel *panel)
{
	struct mtf0397swi *ctx = panel_to_mtf0397swi(panel);

	if (!ctx->prepared)
		return 0;

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);

	regulator_disable(ctx->iovcc);
	regulator_disable(ctx->vcc);

	ctx->prepared = false;

	return 0;
}

static int mtf0397swi_prepare(struct drm_panel *panel)
{
	struct mtf0397swi *ctx = panel_to_mtf0397swi(panel);
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_enable(ctx->vcc);
	if (ret) {
		dev_err(ctx->dev, "Failed to enable vcc supply: %d\n", ret);
		return ret;
	}

	ret = regulator_enable(ctx->iovcc);
	if (ret) {
		dev_err(ctx->dev, "Failed to enable iovcc supply: %d\n", ret);
		goto disable_vcc;
	}
	msleep(100);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	msleep(50);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(200);

	ctx->prepared = true;
	dev_info(ctx->dev, "Panel prepare succeeded\n");
	return 0;

disable_vcc:
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_disable(ctx->vcc);
	return ret;
}

static int mtf0397swi_get_modes(struct drm_panel *panel,
			    struct drm_connector *connector)
{
	struct mtf0397swi *ctx = panel_to_mtf0397swi(panel);
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, ctx->desc->mode);
	if (!mode) {
		dev_err(ctx->dev, "Failed to add mode %ux%u@%u\n",
			ctx->desc->mode->hdisplay, ctx->desc->mode->vdisplay,
			drm_mode_vrefresh(ctx->desc->mode));
		return -ENOMEM;
	}

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs mtf0397swi_drm_funcs = {
	.disable   = mtf0397swi_disable,
	.unprepare = mtf0397swi_unprepare,
	.prepare   = mtf0397swi_prepare,
	.enable	   = mtf0397swi_enable,
	.get_modes = mtf0397swi_get_modes,
};

static int mtf0397swi_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct mtf0397swi *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset gpio\n");

	mipi_dsi_set_drvdata(dsi, ctx);

	ctx->dev = dev;
	ctx->desc = of_device_get_match_data(dev);

	dsi->mode_flags = ctx->desc->mode_flags;
	dsi->format = ctx->desc->format;
	dsi->lanes = ctx->desc->lanes;
	dev_info(dev, "DSI format: 0x%x, lanes: %u, mode_flags: 0x%lx\n",
		 dsi->format, dsi->lanes, dsi->mode_flags);

	ctx->vcc = devm_regulator_get(dev, "vcc");
	if (IS_ERR(ctx->vcc))
		return dev_err_probe(dev, PTR_ERR(ctx->vcc),
				     "Failed to request vcc regulator\n");

	ctx->iovcc = devm_regulator_get(dev, "iovcc");
	if (IS_ERR(ctx->iovcc))
		return dev_err_probe(dev, PTR_ERR(ctx->iovcc),
				     "Failed to request iovcc regulator\n");

	drm_panel_init(&ctx->panel, dev, &mtf0397swi_drm_funcs,
		       DRM_MODE_CONNECTOR_DSI);

	ret = drm_panel_of_backlight(&ctx->panel);
	if (ret)
		return ret;

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		dev_err_probe(dev, ret, "mipi_dsi_attach failed\n");
		drm_panel_remove(&ctx->panel);
		return ret;
	}

	dev_dbg(dev, "%ux%u@%u %ubpp dsi %udl - ready\n",
		ctx->desc->mode->hdisplay, ctx->desc->mode->vdisplay,
		drm_mode_vrefresh(ctx->desc->mode),
		mipi_dsi_pixel_format_to_bpp(dsi->format), dsi->lanes);
	
	dev_info(dev, "%ux%u@%u %ubpp dsi %u lanes - ready\n",
		ctx->desc->mode->hdisplay, ctx->desc->mode->vdisplay,
		drm_mode_vrefresh(ctx->desc->mode),
		mipi_dsi_pixel_format_to_bpp(dsi->format), dsi->lanes);

	return 0;
}

static void mtf0397swi_shutdown(struct mipi_dsi_device *dsi)
{
	struct mtf0397swi *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = drm_panel_disable(&ctx->panel);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to disable panel: %d\n", ret);

	ret = drm_panel_unprepare(&ctx->panel);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to unprepare panel: %d\n", ret);
}

static void mtf0397swi_remove(struct mipi_dsi_device *dsi)
{
	struct mtf0397swi *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	mtf0397swi_shutdown(dsi);

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id mtf0397swi_of_match[] = {
	{ .compatible = "microtech,mtf0397swi-06", .data = &mtf0397swi_desc },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, mtf0397swi_of_match);

static struct mipi_dsi_driver mtf0397swi_driver = {
	.probe	= mtf0397swi_probe,
	.remove = mtf0397swi_remove,
	.shutdown = mtf0397swi_shutdown,
	.driver = {
		.name = "panel-mtf0397swi",
		.of_match_table = mtf0397swi_of_match,
	},
};
module_mipi_dsi_driver(mtf0397swi_driver);

MODULE_AUTHOR("Patrick Areny <pataraign@gmail.com>");
MODULE_DESCRIPTION("DRM driver for Microtech Technology MTF0397SWI-06 MIPI DSI panels");
MODULE_LICENSE("GPL");
