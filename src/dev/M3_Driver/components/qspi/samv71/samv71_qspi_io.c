#include "../qspi_io.h"
#include "samv71q21b.h"
#include <stddef.h>
#include "M0_App/AppOS/App_dls_exp/app_dls_exp.h"
#include "define.h"

#define QSPI_MAX_BUS_NUMBER  1U

static qspi_registers_t * const qspi_periph[QSPI_MAX_BUS_NUMBER + 1U] = {
    NULL,
    QSPI_REGS
};

/*==============================================================================
 * Private Functions
 *============================================================================*/
static qspi_registers_t *qspi_get_instance(uint32_t qspi_port) {
    if ((qspi_port == 0U) || (qspi_port > QSPI_MAX_BUS_NUMBER)) {
        return NULL;
    }

    return qspi_periph[qspi_port];
}

/*==============================================================================
 * Synchronous Transfer
 *============================================================================*/
uint32_t qspi_io_set_mode(qspi_io_t *dev, uint8_t mode) {
    uint32_t scr = QSPI_REGS->QSPI_SCR;

    scr &= ~(QSPI_SCR_CPOL_Msk | QSPI_SCR_CPHA_Msk);

    if (mode & 0x02) scr |= QSPI_SCR_CPOL_Msk;
    if (mode & 0x01) scr |= QSPI_SCR_CPHA_Msk;

    QSPI_REGS->QSPI_SCR = scr;
    return ERROR_OK;
}

uint32_t qspi_io_set_prescale(qspi_io_t *dev, uint8_t scbr) {
    if (dev == NULL) {
        return ERROR_INVALID_PARAM;
    }

    qspi_registers_t *qspi = qspi_get_instance(dev->ui32SpiPort);

    if (qspi == NULL) {
        return ERROR_INVALID_PARAM;
    }

    uint32_t scr = qspi->QSPI_SCR;
    scr &= ~QSPI_SCR_SCBR_Msk;
    scr |= QSPI_SCR_SCBR(scbr);

    qspi->QSPI_SCR = scr;
    return ERROR_OK;
}

uint32_t qspi_io_set_frame_size(qspi_io_t *dev, uint8_t bits) {
    if (dev == NULL) {
        return ERROR_INVALID_PARAM;
    }

    qspi_registers_t *qspi = qspi_get_instance(dev->ui32SpiPort);

    if (qspi == NULL) {
        return ERROR_INVALID_PARAM;
    }

    uint32_t mr = qspi->QSPI_MR;

    mr &= ~QSPI_MR_NBBITS_Msk;
    if (bits == 16) mr |= QSPI_MR_NBBITS_16_BIT;
    else mr |= QSPI_MR_NBBITS_8_BIT;

    qspi->QSPI_MR = mr;
    return ERROR_OK;
}

void QSPI_SPI_Initialize(void) {
    QSPI_REGS->QSPI_CR = QSPI_CR_QSPIDIS_Msk | QSPI_CR_SWRST_Msk;

    QSPI_REGS->QSPI_MR = QSPI_MR_SMM_SPI | QSPI_MR_NBBITS_8_BIT | QSPI_MR_LLB_DISABLED;

    QSPI_REGS->QSPI_SCR = QSPI_SCR_CPHA_Msk | QSPI_SCR_CPOL(0) | QSPI_SCR_SCBR(120);

    QSPI_REGS->QSPI_CR = QSPI_CR_QSPIEN_Msk;
}

uint32_t qspi_io_transfer_sync(qspi_io_t *dev, uint8_t *tx, uint8_t *rx, uint32_t len) {
    for (size_t i = 0; i < len; i++) {
        while (!(QSPI_REGS->QSPI_SR & QSPI_SR_TDRE_Msk));
        QSPI_REGS->QSPI_TDR = tx[i];

        while (!(QSPI_REGS->QSPI_SR & QSPI_SR_RDRF_Msk));
        rx[i] = QSPI_REGS->QSPI_RDR;
    }

    return ERROR_OK;
}

uint32_t qspi_io_write_sync(qspi_io_t *dev, uint8_t *tx, uint32_t len) {
    return qspi_io_transfer_sync(dev, tx, NULL, len);
}

uint32_t qspi_io_read_sync(qspi_io_t *dev, uint8_t *rx, uint32_t len) {
    return qspi_io_transfer_sync(dev, NULL, rx, len);
}