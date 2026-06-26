/* 
 * File:   qspi_io.h
 * Author: HTSANG
 *
 * Created on June 12, 2026, 4:49 PM
 */

#ifndef QSPI_IO_H
#define	QSPI_IO_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
    uint32_t   ui32SpiPort;
}qspi_io_t;

void QSPI_SPI_Initialize(void);
uint32_t qspi_io_transfer_sync(qspi_io_t *dev, uint8_t *tx, uint8_t *rx, uint32_t len);
uint32_t qspi_io_write_sync(qspi_io_t *dev, uint8_t *tx, uint32_t len);
uint32_t qspi_io_read_sync(qspi_io_t *dev, uint8_t *rx, uint32_t len);
uint32_t qspi_io_set_mode(qspi_io_t *dev, uint8_t mode);
uint32_t qspi_io_set_prescale(qspi_io_t *dev, uint8_t scbr);
uint32_t qspi_io_set_frame_size(qspi_io_t *dev, uint8_t bits);


#endif	/* QSPI_IO_H */

