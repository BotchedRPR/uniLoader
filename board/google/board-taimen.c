/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2025, Ivaylo Ivanov <ivo.ivanov.ivanov1@gmail.com>
 */
#include <board.h>
#include <drivers/framework.h>
#include <lib/simplefb.h>

#define UART_BASE		0xc1b0000

#define UARTDM_SR		0xa4
#define UARTDM_SR_TX_EMPTY	(1 << 3)
#define UARTDM_SR_RX_READY	(1 << 0)
#define UARTDM_SR_UART_OVERRUN	(1 << 4)

#define UARTDM_CR		0xa8
#define UARTDM_CR_CMD_RESET_TX_READY	(3 << 8)
#define UARTDM_CR_CMD_RESET_ERR		(3 << 4)
#define UARTDM_CR_CMD_FORCE_STALE	(4 << 8)
#define UARTDM_CR_CMD_RESET_STALE_INT	(8 << 4)

#define UARTDM_NCF_TX		0x40
#define UARTDM_TF		0x100
#define UARTDM_RF		0x140
#define UARTDM_RXFS		0x50
#define UARTDM_DMRX		0x34

#define UARTDM_RXFS_BUF_SHIFT	7
#define UARTDM_RXFS_BUF_MASK	0x7

void uart_putc(char ch)
{
	/* wait until TX is ready */
	while (!(readl((void *)(UART_BASE + UARTDM_SR)) & UARTDM_SR_TX_EMPTY))
		;

	writel(UARTDM_CR_CMD_RESET_TX_READY, (void *)(UART_BASE + UARTDM_CR));

	writel(1, (void *)(UART_BASE + UARTDM_NCF_TX));
	writel(ch, (void *)(UART_BASE + UARTDM_TF));
}

void uart_puts(const char *s)
{
	while (*s != '\0') {
		uart_putc(*s);
		s++;
	}
}

int taimen_init(void)
{
	return 0;
}

int taimen_late_init(void)
{
	return 0;
}

/* NOTE - does not work *yet* - stock bootloader messes up fb */
#ifdef CONFIG_SIMPLE_FB
static struct video_info taimen_fb = {
	.format = FB_FORMAT_ARGB8888,
	.width = CONFIG_FRAMEBUFFER_WIDTH,
	.height = CONFIG_FRAMEBUFFER_HEIGHT,
	.stride = CONFIG_FRAMEBUFFER_STRIDE,
	.address = (void *)CONFIG_FRAMEBUFFER_BASE
};
#endif

int taimen_drv(void)
{
#ifdef CONFIG_SIMPLE_FB
	REGISTER_DRIVER("simplefb", simplefb_probe, &taimen_fb);
#endif
	return 0;
}

struct board_data board_ops = {
	.name = "google-taimen",
	.ops = {
		.early_init = taimen_init,
		.drivers_init = taimen_drv,
		.late_init = taimen_late_init,
	},
	.quirks = 0
};
