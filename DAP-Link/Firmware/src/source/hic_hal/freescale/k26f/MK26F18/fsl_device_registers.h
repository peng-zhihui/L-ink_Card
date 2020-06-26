/*
 * Copyright 2014-2016 Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __FSL_DEVICE_REGISTERS_H__
#define __FSL_DEVICE_REGISTERS_H__

/*
 * Include the cpu specific register header files.
 *
 * The CPU macro should be declared in the project or makefile.
 */
#if (defined(CPU_MK26FN2M0CAC18) || defined(CPU_MK26FN2M0VLQ18) || defined(CPU_MK26FN2M0VMD18) || \
    defined(CPU_MK26FN2M0VMI18))

#define K26F18_SERIES

/* CMSIS-style register definitions */
#include "MK26F18.h"
/* CPU specific feature definitions */
#include "MK26F18_features.h"

#else
    #error "No valid CPU defined!"
#endif

#endif /* __FSL_DEVICE_REGISTERS_H__ */

/*******************************************************************************
 * EOF
 ******************************************************************************/
