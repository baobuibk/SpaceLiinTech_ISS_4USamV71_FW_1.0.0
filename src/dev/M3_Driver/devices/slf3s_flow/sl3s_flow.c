/*
 * slf3s_flow.c
 *
 * Implementation for Sensirion SLF3S-0600F
 * Created on: Oct 21, 2025
 * Author: HTSang
 */

#include "slf3s_flow.h"
#include "do.h"
#include <string.h>

// CRC-8 table (precomputed for polynomial 0x31, init 0xFF)
static const uint8_t crc8_table[256] = {
    0x00, 0x31, 0x62, 0x53, 0xC4, 0xF5, 0xA6, 0x97, 0xB9, 0x88, 0xDB, 0xEA, 0x7D, 0x4C, 0x1F, 0x2E,
    0x43, 0x72, 0x21, 0x10, 0x87, 0xB6, 0xE5, 0xD4, 0xFA, 0xCB, 0x98, 0xA9, 0x3E, 0x0F, 0x5C, 0x6D,
    0x86, 0xB7, 0xE4, 0xD5, 0x42, 0x73, 0x20, 0x11, 0x3F, 0x0E, 0x5D, 0x6C, 0xFB, 0xCA, 0x99, 0xA8,
    0xC5, 0xF4, 0xA7, 0x96, 0x01, 0x30, 0x63, 0x52, 0x7C, 0x4D, 0x1E, 0x2F, 0xB8, 0x89, 0xDA, 0xEB,
    0x3D, 0x0C, 0x5F, 0x6E, 0xF9, 0xC8, 0x9B, 0xAA, 0x84, 0xB5, 0xE6, 0xD7, 0x40, 0x71, 0x22, 0x13,
    0x7E, 0x4F, 0x1C, 0x2D, 0xBA, 0x8B, 0xD8, 0xE9, 0xC7, 0xF6, 0xA5, 0x94, 0x03, 0x32, 0x61, 0x50,
    0xBB, 0x8A, 0xD9, 0xE8, 0x7F, 0x4E, 0x1D, 0x2C, 0x02, 0x33, 0x60, 0x51, 0xC6, 0xF7, 0xA4, 0x95,
    0xF8, 0xC9, 0x9A, 0xAB, 0x3C, 0x0D, 0x5E, 0x6F, 0x41, 0x70, 0x23, 0x12, 0x85, 0xB4, 0xE7, 0xD6,
    0x7A, 0x4B, 0x18, 0x29, 0xBE, 0x8F, 0xDC, 0xED, 0xC3, 0xF2, 0xA1, 0x90, 0x07, 0x36, 0x65, 0x54,
    0x39, 0x08, 0x5B, 0x6A, 0xFD, 0xCC, 0x9F, 0xAE, 0x80, 0xB1, 0xE2, 0xD3, 0x44, 0x75, 0x26, 0x17,
    0xFC, 0xCD, 0x9E, 0xAF, 0x38, 0x09, 0x5A, 0x6B, 0x45, 0x74, 0x27, 0x16, 0x81, 0xB0, 0xE3, 0xD2,
    0xBF, 0x8E, 0xDD, 0xEC, 0x7B, 0x4A, 0x19, 0x28, 0x06, 0x37, 0x64, 0x55, 0xC2, 0xF3, 0xA0, 0x91,
    0x47, 0x76, 0x25, 0x14, 0x83, 0xB2, 0xE1, 0xD0, 0xFE, 0xCF, 0x9C, 0xAD, 0x3A, 0x0B, 0x58, 0x69,
    0x04, 0x35, 0x66, 0x57, 0xC0, 0xF1, 0xA2, 0x93, 0xBD, 0x8C, 0xDF, 0xEE, 0x79, 0x48, 0x1B, 0x2A,
    0xC1, 0xF0, 0xA3, 0x92, 0x05, 0x34, 0x67, 0x56, 0x78, 0x49, 0x1A, 0x2B, 0xBC, 0x8D, 0xDE, 0xEF,
    0x82, 0xB3, 0xE0, 0xD1, 0x46, 0x77, 0x24, 0x15, 0x3B, 0x0A, 0x59, 0x68, 0xFF, 0xCE, 0x9D, 0xAC
};

// Internal: Compute CRC-8 for 2 bytes data

static uint8_t compute_crc8(const uint8_t *data, uint8_t len) {
    uint8_t crc = 0xFF;
    for (int i = 0; i < len; i++) {
        crc = crc8_table[(crc ^ data[i]) & 0xFF];
    }
    return crc;
}

// Validate CRC for 2 data bytes + received CRC

static bool slf3s_validate_crc(const uint8_t *data, uint8_t crc_received) {
    uint8_t computed = compute_crc8(data, 2);
    return (computed == crc_received);
}

static slf3s_data_t s_slf3s_data;

static void slf3s_delay_ms(uint32_t ms) {
    os_delay_ms(ms);
}

static int32_t slf3s_flow_send(i2c_io_t *i2c, uint8_t address, uint8_t *tx_buf, uint8_t len) {
    if (!i2c) return SLF3S_ERROR;
    uint32_t ret = i2c_io_send(i2c, address, (const char *) tx_buf, len);
    return (ret == ERROR_OK) ? SLF3S_OK : SLF3S_I2C_ERROR;
}

static int32_t slf3s_flow_send_cmd(i2c_io_t *i2c, uint8_t address, uint16_t cmd) {
    uint8_t tx_buf[2];
    tx_buf[0] = (uint8_t) (cmd >> 8);
    tx_buf[1] = (uint8_t) cmd;
    return slf3s_flow_send(i2c, address, tx_buf, 2);
}

static int32_t slf3s_flow_read(i2c_io_t *i2c, uint8_t address, uint8_t *rx_buf, uint8_t len) {
    if (!i2c) return SLF3S_ERROR;
    uint32_t ret = i2c_io_recv(i2c, address, (char *) rx_buf, len);
    return (ret == ERROR_OK) ? SLF3S_OK : SLF3S_I2C_ERROR;
}

// Internal: Read 9 bytes (flow + temp + flags with CRCs)

static int32_t slf3s_flow_read_data(i2c_io_t *i2c, uint8_t address) {
    if (!i2c) return SLF3S_ERROR;
    
    uint8_t rx_buf[9];
    if (slf3s_flow_read(i2c, address, (char *) rx_buf, 9) != SLF3S_OK)
        return SLF3S_ERROR;

    uint8_t flow_data[2] = {rx_buf[0], rx_buf[1]};
    if (!slf3s_validate_crc(flow_data, rx_buf[2])) return SLF3S_CRC_ERROR;

    uint8_t temp_data[2] = {rx_buf[3], rx_buf[4]};
    if (!slf3s_validate_crc(temp_data, rx_buf[5])) return SLF3S_CRC_ERROR;

    uint8_t flags_data[2] = {rx_buf[6], rx_buf[7]};
    if (!slf3s_validate_crc(flags_data, rx_buf[8])) return SLF3S_CRC_ERROR;

    s_slf3s_data.flow = (float) ((flow_data[0] << 8) | flow_data[1]);
    s_slf3s_data.temp = (float) ((temp_data[0] << 8) | temp_data[1]);
    s_slf3s_data.flags = (uint16_t) ((flags_data[0] << 8) | flags_data[1]);
    
    os_delay_ms(60);
    
    return SLF3S_OK;
}

int32_t slf3s_flow_soft_reset(i2c_io_t *i2c) {
    if (!i2c) return SLF3S_ERROR;
    uint8_t tx_buf = SLF3S_CMD_SOFT_RESET & 0xFF;
    return slf3s_flow_send(i2c, SLF3S_I2C_GENERAL_ADDR, &tx_buf, 1);
}

int32_t slf3s_flow_change_address(i2c_io_t *i2c, do_t *nIRQ_pin, uint16_t new_address) {
    if (!i2c) return SLF3S_ERROR;
    uint8_t tx_buf[5];
    tx_buf[0] = (uint8_t) (SLF3S_CMD_CHANGE_ADD >> 8);
    tx_buf[1] = (uint8_t) SLF3S_CMD_CHANGE_ADD;
    tx_buf[2] = (uint8_t) (new_address >> 8);
    tx_buf[3] = (uint8_t) new_address;
    tx_buf[4] = compute_crc8(&tx_buf[2], 2);
    if (slf3s_flow_send(i2c, SLF3S_I2C_DEFAULT_ADDR, tx_buf, 5) == SLF3S_ERROR)
        return SLF3S_ERROR;

    dio_mode(nIRQ_pin, OUTPUT);
    do_set(nIRQ_pin);
    slf3s_delay_ms(1);
    do_reset(nIRQ_pin);
    dio_mode(nIRQ_pin, INPUT_PULLUP);

    return SLF3S_OK;
}

int32_t slf3s_flow_init(i2c_io_t *i2c, uint8_t address, e_slf3s_mode_t mode) {
    if (!i2c)
        return SLF3S_ERROR;

    uint16_t cmd = (mode == WATER_MODE) ? SLF3S_CMD_START_H2O : SLF3S_CMD_START_IPA;

    int32_t ret = slf3s_flow_send_cmd(i2c, address, cmd);

    if (ret != SLF3S_OK)
        return ret;

    os_delay_ms(20);

    return SLF3S_OK;
}

int32_t slf3s_flow_stop(i2c_io_t *i2c, uint8_t address) {
    if (!i2c) return SLF3S_ERROR;
    return slf3s_flow_send_cmd(i2c, address, SLF3S_CMD_STOP);
}

int32_t slf3s_flow_get_product_id(i2c_io_t *i2c, uint8_t address, int32_t* ID) {
    int32_t err1 = slf3s_flow_send_cmd(i2c, address, SLF3S_CMD_READ_ID_1);
    if (err1 != SLF3S_OK) return err1;

    int32_t err2 = slf3s_flow_send_cmd(i2c, address, SLF3S_CMD_READ_ID_2);
    if (err2 != SLF3S_OK) return err2;

    uint8_t rx_buf[18];
    if (slf3s_flow_read(i2c, address, (char *) rx_buf, 18) != SLF3S_OK)
        return SLF3S_ERROR;

    // Validate first two CRCs for product ID (bytes 0-1 and 2-3)
    uint8_t prod_msb[2] = {rx_buf[0], rx_buf[1]};
    if (!slf3s_validate_crc(prod_msb, rx_buf[2])) return 0;
    uint8_t prod_lsb[2] = {rx_buf[3], rx_buf[4]};
    if (!slf3s_validate_crc(prod_lsb, rx_buf[5])) return 0;

    *ID = ((uint32_t) rx_buf[0] << 24) | ((uint32_t) rx_buf[1] << 16) | ((uint32_t) rx_buf[3] << 8) | rx_buf[4];
    return SLF3S_OK;
}

float slf3s_flow_read_flow(i2c_io_t *i2c, uint8_t address) {
    if (slf3s_flow_read_data(i2c, address) != SLF3S_OK)
        return SLF3S_ERROR;
    return (float) s_slf3s_data.flow / SLF3S_FLOW_SCALE;
}

float slf3s_flow_read_temperature(i2c_io_t *i2c, uint8_t address) {
    if (slf3s_flow_read_data(i2c, address) != SLF3S_OK)
        return SLF3S_ERROR;
    return (float) s_slf3s_data.temp / SLF3S_TEMP_SCALE;
}

uint16_t slf3s_flow_read_flags(i2c_io_t *i2c, uint8_t address) {
    if (slf3s_flow_read_data(i2c, address) != SLF3S_OK)
        return SLF3S_ERROR;
    return (uint16_t) s_slf3s_data.flags;
}

int32_t slf3s_flow_read_all(i2c_io_t *i2c, uint8_t address, slf3s_data_t *slf3s_data) {
    if (!i2c) return SLF3S_ERROR;
    if (slf3s_flow_read_data(i2c, address) != SLF3S_OK)
        return SLF3S_ERROR;
//    uint8_t tx_buf = SLF3S_CMD_SOFT_RESET & 0xFF;
//    slf3s_flow_send(i2c, SLF3S_I2C_GENERAL_ADDR, &tx_buf, 1);
    
    slf3s_data->flow = (float) s_slf3s_data.flow / SLF3S_FLOW_SCALE;
    slf3s_data->temp = (float) s_slf3s_data.temp / SLF3S_TEMP_SCALE;
    slf3s_data->flags = (float) s_slf3s_data.flags;
    return SLF3S_OK;
}