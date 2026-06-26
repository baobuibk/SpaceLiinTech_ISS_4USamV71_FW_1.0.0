#include "adg1414.h"
#include <string.h>
#include "peripheral/qspi/plib_qspi_spi.h"

static int adg1414_chain_write(adg1414_dev_t *dev) {
    uint8_t tx[ADG1414_CHAIN_NUM_CHIPS_MAX];
    uint8_t rx[ADG1414_CHAIN_NUM_CHIPS_MAX];
    if (dev->num_of_sw > ADG1414_CHAIN_NUM_CHIPS_MAX) 
        return (int) ERROR_INVALID_PARAM;

    for (int i = 0; i < dev->num_of_sw; i++) {
        tx[i] = dev->switch_state[dev->num_of_sw - 1 - i];
    }

    do_reset(dev->cs);
    uint32_t st;
    if (dev->bus == SPI_BUS) {
        spi_io_set_mode(dev->spi, 1);
        st = spi_io_transfer_sync(dev->spi, tx, rx, dev->num_of_sw);
    } else if (dev->bus == QSPI_BUS) {
        qspi_io_set_mode(dev->qspi, 1);
        st = qspi_io_transfer_sync(dev->qspi, tx, rx, dev->num_of_sw);
    } else {
        return ERROR_FAIL;
    }
    do_set(dev->cs);

    return (st == ERROR_OK) ? (int) ERROR_OK : (int) st;
}

int adg1414_chain_spi_init(adg1414_dev_t *dev, spi_io_t *spi, do_t *cs, uint8_t num) {
    if (!dev || !spi || !cs) return (int) ERROR_INVALID_PARAM;
    if (num == 0 || num > ADG1414_CHAIN_NUM_CHIPS_MAX) return (int) ERROR_INVALID_PARAM;

    dev->spi = spi;
    dev->cs = cs;
    dev->num_of_sw = num;
    dev->bus = SPI_BUS;

    memset(dev->switch_state, 0x00, sizeof (dev->switch_state));
    do_set(dev->cs);

    return adg1414_chain_write(dev);
}

int adg1414_chain_qspi_init(adg1414_dev_t *dev, qspi_io_t *qspi, do_t *cs, uint8_t num) {
    if (!dev || !qspi || !cs) return (int) ERROR_INVALID_PARAM;
    if (num == 0 || num > ADG1414_CHAIN_NUM_CHIPS_MAX) return (int) ERROR_INVALID_PARAM;

    dev->qspi = qspi;
    dev->cs = cs;
    dev->num_of_sw = num;
    dev->bus = QSPI_BUS;

    memset(dev->switch_state, 0x00, sizeof (dev->switch_state));
    do_set(dev->cs);

    return adg1414_chain_write(dev);
}

int adg1414_chain_sw_on(adg1414_dev_t *dev, uint8_t channel_num) {
    if (!dev) return (int) ERROR_INVALID_PARAM;

    /* INTERNAL mode (1 byte => 8 kênh) */
    if (dev->num_of_sw == INTERNAL_CHAIN_SWITCH_NUM) {
        if (channel_num > INTERNAL_CHAIN_CHANNEL_NUM) return (int) ERROR_INVALID_PARAM;
        for (int i = 0; i < dev->num_of_sw; i++) dev->switch_state[i] = 0x00;
        if (channel_num) {
            uint8_t chip_idx = (uint8_t) ((channel_num - 1) / 8);
            uint8_t bit_idx = (uint8_t) ((channel_num - 1) % 8);
            dev->switch_state[chip_idx] = (uint8_t) (1u << bit_idx);
        }
    }/* EXTERNAL mode (1 byte => 8 kênh) */
    else if (dev->num_of_sw == EXTERNAL_CHAIN_SWITCH_NUM) {
        if (channel_num > EXTERNAL_CHAIN_CHANNEL_NUM) return (int) ERROR_INVALID_PARAM;
        if (channel_num) dev->switch_state[0] = (uint8_t) (1u << (channel_num - 1));
        else dev->switch_state[0] = 0x00;
    } else {
        /* Mặc �?�?nh bật �?úng 1 kênh (nếu channel_num > 0) */
        for (int i = 0; i < dev->num_of_sw; i++) dev->switch_state[i] = 0x00;
        if (channel_num) {
            uint8_t chip_idx = (uint8_t) ((channel_num - 1) / 8);
            uint8_t bit_idx = (uint8_t) ((channel_num - 1) % 8);
            if (chip_idx >= dev->num_of_sw) return (int) ERROR_INVALID_PARAM;
            dev->switch_state[chip_idx] = (uint8_t) (1u << bit_idx);
        }
    }
    return adg1414_chain_write(dev);
}

int adg1414_chain_sw_off(adg1414_dev_t *dev, uint8_t channel_num) {
    if (!dev || channel_num == 0) return (int) ERROR_INVALID_PARAM;

    if (dev->num_of_sw == INTERNAL_CHAIN_SWITCH_NUM) {
        if (channel_num > INTERNAL_CHAIN_CHANNEL_NUM) return (int) ERROR_INVALID_PARAM;
        uint8_t chip_idx = (uint8_t) ((channel_num - 1) / 6);
        uint8_t bit_idx = (uint8_t) ((channel_num - 1) % 6);
        dev->switch_state[chip_idx] = (uint8_t) (dev->switch_state[chip_idx] & (uint8_t) ~(1u << bit_idx));
    } else if (dev->num_of_sw == EXTERNAL_CHAIN_SWITCH_NUM) {
        if (channel_num > EXTERNAL_CHAIN_CHANNEL_NUM) return (int) ERROR_INVALID_PARAM;
        dev->switch_state[0] = (uint8_t) (dev->switch_state[0] & (uint8_t) ~(1u << (channel_num - 1)));
    } else {
        uint8_t chip_idx = (uint8_t) ((channel_num - 1) / 8);
        uint8_t bit_idx = (uint8_t) ((channel_num - 1) % 8);
        if (chip_idx >= dev->num_of_sw) return (int) ERROR_INVALID_PARAM;
        dev->switch_state[chip_idx] = (uint8_t) (dev->switch_state[chip_idx] & (uint8_t) ~(1u << bit_idx));
    }

    return adg1414_chain_write(dev);
}

int adg1414_chain_all_sw_off(adg1414_dev_t *dev) {
    if (!dev) return (int) ERROR_INVALID_PARAM;
    for (int i = 0; i < dev->num_of_sw; i++) {
        dev->switch_state[i] = 0x00;
    }
    return adg1414_chain_write(dev);
}

int adg1414_manual_sw_on(adg1414_dev_t *dev, uint8_t channel_num) {
    if (!dev) return (int) ERROR_INVALID_PARAM;

    /* INTERNAL mode (1 byte => 8 kênh) */
    if (dev->num_of_sw == INTERNAL_CHAIN_SWITCH_NUM) {
        if (channel_num > INTERNAL_CHAIN_CHANNEL_NUM) return (int) ERROR_INVALID_PARAM;
        // for (int i = 0; i < dev->num_of_sw; i++) dev->switch_state[i] = 0x00;
        if (channel_num) {
            uint8_t chip_idx = (uint8_t) ((channel_num - 1) / 8);
            uint8_t bit_idx = (uint8_t) ((channel_num - 1) % 8);
            dev->switch_state[chip_idx] |= (uint8_t) (1u << bit_idx);
        }
    }/* EXTERNAL mode (1 byte => 8 kênh) */
    else if (dev->num_of_sw == EXTERNAL_CHAIN_SWITCH_NUM) {
        if (channel_num > EXTERNAL_CHAIN_CHANNEL_NUM) return (int) ERROR_INVALID_PARAM;
        if (channel_num) dev->switch_state[0] |= (uint8_t) (1u << (channel_num - 1));
        else dev->switch_state[0] = 0x00;
    } else {
        /* Mặc �?�?nh bật �?úng 1 kênh (nếu channel_num > 0) */
        // for (int i = 0; i < dev->num_of_sw; i++) dev->switch_state[i] = 0x00;
        if (channel_num) {
            uint8_t chip_idx = (uint8_t) ((channel_num - 1) / 8);
            uint8_t bit_idx = (uint8_t) ((channel_num - 1) % 8);
            if (chip_idx >= dev->num_of_sw) return (int) ERROR_INVALID_PARAM;
            dev->switch_state[chip_idx] |= (uint8_t) (1u << bit_idx);
        }
    }
    return adg1414_chain_write(dev);
}

int adg1414_manual_sw_off(adg1414_dev_t *dev, uint8_t channel_num) {
    if (!dev || channel_num == 0) return (int) ERROR_INVALID_PARAM;

    if (dev->num_of_sw == INTERNAL_CHAIN_SWITCH_NUM) {
        if (channel_num > INTERNAL_CHAIN_CHANNEL_NUM) return (int) ERROR_INVALID_PARAM;
        uint8_t chip_idx = (uint8_t) ((channel_num - 1) / 6);
        uint8_t bit_idx = (uint8_t) ((channel_num - 1) % 6);
        dev->switch_state[chip_idx] = (uint8_t) (dev->switch_state[chip_idx] & (uint8_t) ~(1u << bit_idx));
    } else if (dev->num_of_sw == EXTERNAL_CHAIN_SWITCH_NUM) {
        if (channel_num > EXTERNAL_CHAIN_CHANNEL_NUM) return (int) ERROR_INVALID_PARAM;
        dev->switch_state[0] = (uint8_t) (dev->switch_state[0] & (uint8_t) ~(1u << (channel_num - 1)));
    } else {
        uint8_t chip_idx = (uint8_t) ((channel_num - 1) / 8);
        uint8_t bit_idx = (uint8_t) ((channel_num - 1) % 8);
        if (chip_idx >= dev->num_of_sw) return (int) ERROR_INVALID_PARAM;
        dev->switch_state[chip_idx] = (uint8_t) (dev->switch_state[chip_idx] & (uint8_t) ~(1u << bit_idx));
    }

    return adg1414_chain_write(dev);
}