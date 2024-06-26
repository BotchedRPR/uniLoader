/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2022, Ivaylo Ivanov <ivo.ivanov.ivanov1@gmail.com>
 */

OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
TARGET(binary)

INPUT(KERNEL_PATH)
INPUT(CUSTBIN_PATH)
INPUT(DTB_PATH)

SECTIONS
{
	.boot : {
		arch/aarch64/Start.o
	}

	.dtb ALIGN(0x1000) : {
		dtb = .;
		DTB_PATH
	}

	.kernel ALIGN(0x1000) : {
		kernel = .;
		KERNEL_PATH
	}

	kernel_size = SIZEOF(.kernel);

	.kernel2 ALIGN(0x1000) : {
		kernel2 = .;
		CUSTBIN_PATH
	}

	kernel2_size = SIZEOF(.kernel2);
}
