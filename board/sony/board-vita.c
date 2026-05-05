/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Igor Belwon <igor.belwon@mentallysanemainliners.org>
 */

#include <board.h>
#include <drivers/framework.h>
#include <lib/debug.h>
#include <lib/simplefb.h>

#define dmb() asm("dmb")

#define UART_REG_BASE_ADDR		0xE2030000
#define UARTCLKGEN_REG_BASE_ADDR	0xE3105000

#define UART_REGS(i)			((void *)(UART_REG_BASE_ADDR + (i) * 0x10000))
#define UARTCLKGEN_REGS(i)		((void *)(UARTCLKGEN_REG_BASE_ADDR + (i) * 4))

void uart_wait_ready(int bus)
{
	volatile uint32_t *uart_regs = UART_REGS(bus);

	while (!(uart_regs[0xA] & 0x200))
		dmb();
}

int uart_init(int bus)
{
	volatile uint32_t *uart_regs = UART_REGS(bus);
	volatile uint32_t *uartclkgen_regs = UARTCLKGEN_REGS(bus);

	uart_regs[1] = 0; // disable device

	printk(KERN_ALERT, "Uart_init\n");

	*uartclkgen_regs = 0x1001A; // Baudrate = 115200

	printk(KERN_ALERT, "CLK Gen Baudrate Set\n");

	uart_regs[8] = 3;
	uart_regs[4] = 1;
	uart_regs[0xC] = 0;
	uart_regs[0x18] = 0x303;
	uart_regs[0x10] = 0;
	uart_regs[0x14] = 0;
	uart_regs[0x19] = 0x10001;

	printk(KERN_ALERT, "Register Initialized\n");

	uart_regs[1] = 1; // enable device

	printk(KERN_ALERT, "UART device Enabled.\n");

	uart_wait_ready(bus);

	printk(KERN_ALERT, "Enable ACK. Hello world.\n");

	return 0;
}

void uart_write(int bus, uint32_t data)
{
	volatile uint32_t *uart_regs = UART_REGS(bus);

	while (!(uart_regs[0xA] & 0x100))
		dmb();

	uart_regs[0x1C] = data;
}

uint32_t uart_read_fifo_data_available(int bus)
{
	return ((uint32_t *)UART_REGS(bus))[0x1A] & 0x3F;
}

void uart_putc(int bus, char c)
{
	uart_write(bus, c);

	if (c == '\n')
		uart_write(bus, '\r');
}

void uart_print(int bus, const char *str)
{
	while (*str)
		uart_putc(bus, *str++);
}

void uart_puts(const char *str)
{
	uart_print(0, str);
}

#ifdef CONFIG_SIMPLE_FB
static struct video_info vita12k_fb = {
	.format = FB_FORMAT_BGRA8888,
	.width = 960,
	.height = 544,
	.stride = 4,
	.address = (void *)0x20000000
};
#endif

int vita_drv(void)
{
#ifdef CONFIG_SIMPLE_FB
	// for now only vita1k/vita2k is supported, PSTV would need more config :/
	REGISTER_DRIVER("simplefb", simplefb_probe, &vita12k_fb);

	uart_init(0);
#endif
	return 0;
}

struct board_data board_ops = {
	.name = "sony-psvita",
	.ops = {
		.drivers_init = vita_drv,
	},
	.quirks = 0
};
