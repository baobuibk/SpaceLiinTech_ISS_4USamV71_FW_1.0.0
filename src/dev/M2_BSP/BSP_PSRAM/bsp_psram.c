#include "bsp_psram.h"
#include "BSP_Board/bsp_core.h"

int bsp_psram_read_id(is66_dev_t *dev) {
    return is66_dma_read_id(dev);
}

int bsp_psram_write(is66_dev_t *dev, uint32_t address, uint8_t *buffer, uint16_t length) {
    return is66_write(dev, address, buffer, length);
}

int bsp_psram_read(is66_dev_t *dev, uint32_t address, uint8_t *buffer, uint16_t length) {
    return is66_read(dev, address, buffer, length);
}

void bsp_psram_sw_2_mcu(void) {
    do_reset(&switch_spi_psram);
}

void bsp_psram_sw_2_mpu(void) {
    do_set(&switch_spi_psram);
}