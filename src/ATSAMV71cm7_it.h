/************************************************
 *  @file     : ATSAMV71cm7_it.h
 *  @date     : November 2025
 *  @author   : CAO HIEU
 *  @version  : 1.0.0
 *-----------------------------------------------
 *  Description :
 *    [-]
 ************************************************/

#ifndef ATSAMV71cm7_it_H
#define ATSAMV71cm7_it_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================*/
/*                        Includes                            */
/*============================================================*/
#include <stdint.h>
#include "config/default/peripheral/xdmac/plib_xdmac.h"
#include "config/default/peripheral/spi/spi_master/plib_spi1_master.h"

void EX_SysTick_HandlerRegister(void);
void EX_TC3_CH0_HandlerRegister(void);


typedef enum {
    DMA_SPI1_NONE = 0,
    DMA_SPI1_PSRAM,
    DMA_SPI1_INT_LASER,
} dma_spi1_obj_t;

void SPI1_DMA_TX_Callback(XDMAC_TRANSFER_EVENT event, uintptr_t context);
void SPI1_DMA_RX_Callback(XDMAC_TRANSFER_EVENT event, uintptr_t context);

#ifdef __cplusplus
}
#endif

#endif /* ATSAMV71cm7_it_H */