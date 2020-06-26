/* -----------------------------------------------------------------------------
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 *
 * $Date:        27. June 2013
 * $Revision:    V1.00
 *
 * Project:      RTE Device Configuration for NXP Kinetis K20
 * -------------------------------------------------------------------------- */

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

#ifndef __RTE_DEVICE_H
#define __RTE_DEVICE_H


#define RTE_USART1 1
#define RTE_USART1_DMA_EN 0


#define RTE_USART1_DMA_TX_DMA_BASE (DMA0)
#define RTE_USART1_DMA_TX_CH (0)
#define RTE_USART1_DMA_TX_DMAMUX_BASE (DMAMUX0)
#define RTE_USART1_DMA_TX_PERI_SEL (5) // DMAMUX source 5 is UART1 TX

#define RTE_USART1_DMA_RX_DMA_BASE (DMA0)
#define RTE_USART1_DMA_RX_CH (1)
#define RTE_USART1_DMA_RX_DMAMUX_BASE (DMAMUX0)
#define RTE_USART1_DMA_RX_PERI_SEL (4) // DMAMUX source 4 is UART1 RX


#endif  /* __RTE_DEVICE_H */

