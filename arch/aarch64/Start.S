/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2022, Ivaylo Ivanov <ivo.ivanov.ivanov1@gmail.com>
 */

    adrp	x0, dtb
    adrp	x1, kernel
    adrp	x2, kernel2
    b main

.global load_kernel
load_kernel:
    br	x4
