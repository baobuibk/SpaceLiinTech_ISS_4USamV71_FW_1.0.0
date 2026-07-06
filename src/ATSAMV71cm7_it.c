#include "ATSAMV71cm7_it.h"
#include "interrupts.h"
#include "peripheral/systick/plib_systick.h"
#include "peripheral/pio/plib_pio.h"
#include "peripheral/tc/plib_tc3.h"
#include "M5_Utils/DateTime/date_time.h"
#include "M5_Utils/Tick/tick.h"
#include "stdio.h"

#include "BSP_Board/bsp_core.h"
#include "BSP_Laser_Int/bsp_laser_int.h"

/******************************************************************************/
/*            Cortex Processor Interruption and Exception Handlers            */
/******************************************************************************/

/*****************************************************************************/
/*                              Sys-tick Handler                             */

/*****************************************************************************/
static void EX_Systick_Handler(uintptr_t context) {
    (void) context;
}

void EX_SysTick_HandlerRegister(void) {
    SYSTICK_TimerCallbackSet(EX_Systick_Handler, 0);
}

/*****************************************************************************/
/*                                Timer Handler                              */
/*****************************************************************************/
static uint32_t tick = 0;
static uint8_t toggleState = false;

static void EX_TC3_CH0_Handler(TC_TIMER_STATUS status, uintptr_t context) {
    (void) context;
    (void) status;

    tick++;
    if (tick >= 1000) {
        tick = 0;

        toggleState = 1 - toggleState;

        Utils_SoftTime_Update(); // FREERTOS
    }

    TickTimer_IRQHandler();

}

void EX_TC3_CH0_HandlerRegister(void) {
    TC3_CH0_TimerCallbackRegister(EX_TC3_CH0_Handler, 0);
}


/*****************************************************************************/
/*                                DMA SPI1                                   */
/*****************************************************************************/
dma_spi1_obj_t dma_spi1_obj = DMA_SPI1_NONE;
volatile bool dma_spi1_tx_done = false;
volatile bool dma_spi1_rx_done = false;
volatile bool dma_spi1_psram_busy = false;

void SPI1_DMA_TX_Callback(XDMAC_TRANSFER_EVENT event, uintptr_t context) {
    (void) context;
    if (event == XDMAC_TRANSFER_COMPLETE) {
        dma_spi1_tx_done = true;
    }
}

void SPI1_DMA_RX_Callback(XDMAC_TRANSFER_EVENT event, uintptr_t context) {
    (void) context;
    if (event == XDMAC_TRANSFER_COMPLETE) {
        dma_spi1_rx_done = true;

        if (dma_spi1_tx_done && dma_spi1_rx_done) {
            while (!(SPI1_REGS->SPI_SR & SPI_SR_TXEMPTY_Msk));
            if (dma_spi1_obj == DMA_SPI1_PSRAM) {
                PIOC_REGS->PIO_SODR = (1U << 28);
                dma_spi1_psram_busy = false;
            } else if (dma_spi1_obj == DMA_SPI1_INT_LASER) {
                PIOA_REGS->PIO_SODR = (1U << 7) | (1U << 22);
                bsp_laser_int_latch();
                SPI1_REGS->SPI_CSR[0] |= SPI_CSR_BITS_8_BIT;
            }
            dma_spi1_obj = DMA_SPI1_NONE;
        }
    }
}

/*****************************************************************************/
/*                                USART1 Handler                             */
/*****************************************************************************/


/******************************************************************************/
/*                                  USBHS Handler                             */

/******************************************************************************/
void __attribute__((used)) USBHS_Handler(void) {
}