#include "is66wvs8m8.h"
#include "ATSAMV71cm7_it.h"

#define IS66_BURST      (1024 + 5)

__attribute__((section(".ram_nocache"), aligned(32)))
static uint8_t spi_work_tx[IS66_BURST];

__attribute__((section(".ram_nocache"), aligned(32)))
static uint8_t spi_work_rx[IS66_BURST];

extern dma_spi1_obj_t dma_spi1_obj;
extern volatile bool dma_spi1_tx_done;
extern volatile bool dma_spi1_rx_done;
extern volatile bool dma_spi1_psram_busy;

static inline void is66_select(is66_dev_t *dev) {
    PIOC_REGS->PIO_CODR = (1U << dev->cs->pin);
}

static inline void is66_deselect(is66_dev_t *dev) {
    PIOC_REGS->PIO_SODR = (1U << dev->cs->pin);
}

static int is66_transfer(is66_dev_t *dev, uint8_t *tx, uint8_t *rx, uint16_t len) {
    if (len == 0) return ERROR_OK;
    if (len > IS66_BURST) return ERROR_FAIL;

    /* Wait previous transfer */
    while (dma_spi1_psram_busy);

    dma_spi1_obj = DMA_SPI1_PSRAM;
    dma_spi1_psram_busy = true;
    dma_spi1_tx_done = false;
    dma_spi1_rx_done = false;

    uint8_t *p_tx = spi_work_tx;
    uint8_t *p_rx = spi_work_rx;

    /*================ PREPARE =================*/
    if (tx && tx != p_tx)
        memcpy(p_tx, tx, len);
    else if (!tx)
        memset(p_tx, 0xFF, len);

    /*================ CLEAR RX FIFO =================*/
    while (USART2_REGS->US_CSR & US_CSR_SPI_RXRDY_Msk) {
        volatile uint32_t dummy = USART2_REGS->US_RHR;
        (void) dummy;
    }

    /*================ START =================*/
    is66_select(dev);

    XDMAC_ChannelTransfer(XDMAC_CHANNEL_1,
            (const void *) &SPI1_REGS->SPI_RDR,
            (void *) p_rx,
            len);

    __DSB();

    XDMAC_ChannelTransfer(XDMAC_CHANNEL_0,
            (const void *) p_tx,
            (const void *) &SPI1_REGS->SPI_TDR,
            len);

    /*================ WAIT =================*/
    while (dma_spi1_psram_busy);

    /*================ COPY BACK =================*/
    if (rx)
        memcpy(rx, p_rx, len);

    return ERROR_OK;
}

int is66_write(is66_dev_t *dev, uint32_t address, uint8_t *buffer, uint16_t length) {
    if (!dev || !buffer || length == 0) return ERROR_INVALID_PARAM;

    spi_work_tx[0] = IS66WVS8M8_WRITE_CMD;
    spi_work_tx[1] = (uint8_t) (address >> 16);
    spi_work_tx[2] = (uint8_t) (address >> 8);
    spi_work_tx[3] = (uint8_t) (address);
    memcpy(&spi_work_tx[4], buffer, length);

    return is66_transfer(dev, spi_work_tx, spi_work_rx, 4 + length);
}

int is66_read(is66_dev_t *dev, uint32_t address, uint8_t *buffer, uint16_t length) {
    if (!dev || !buffer || length == 0) return ERROR_INVALID_PARAM;

    spi_work_tx[0] = IS66WVS8M8_READ_CMD;
    spi_work_tx[1] = (uint8_t) (address >> 16);
    spi_work_tx[2] = (uint8_t) (address >> 8);
    spi_work_tx[3] = (uint8_t) (address);

    //Wait Cycles
    spi_work_tx[4] = 0xAA;

    memset(&spi_work_tx[5], 0, length);

    uint16_t total_len = 5 + length;
    int st = is66_transfer(dev, spi_work_tx, spi_work_rx, total_len);

    if (st == ERROR_OK) {
        memcpy(buffer, &spi_work_rx[5], length);
    }
    return st;
}

Std_ReturnType is66_read_id(is66_dev_t *dev) {
    uint8_t cmd = IS66WVS8M8_READ_ID_CMD;

    is66_select(dev);

    SPI1_WriteRead(&cmd, 1, NULL, 0);
    SPI1_WriteRead(NULL, 0, &dev->id, 11);

    is66_deselect(dev);

    return ERROR_OK;
}

Std_ReturnType is66_dma_read_id(is66_dev_t *dev) {
    spi_work_tx[0] = IS66WVS8M8_READ_ID_CMD;

    uint16_t total_len = 12;
    int st = is66_transfer(dev, spi_work_tx, spi_work_rx, total_len);

    if (st == ERROR_OK)
        memcpy(dev->id, &spi_work_rx[4], 8);
    return st;
}