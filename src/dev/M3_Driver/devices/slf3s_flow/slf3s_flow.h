/*
 * slf3s_flow.h
 *
 * Driver for Sensirion SLF3S-0600F Liquid Flow Sensor
 * Created on: Oct 21, 2025
 * Author: Grok
 */

#ifndef SLF3S_FLOW_H_
#define SLF3S_FLOW_H_

#include "stdint.h"
#include "stdbool.h"
#include "define.h"
//#include "peripheral/twihs/master/plib_twihs0_master.h"

#include "M3_Driver/components/dio/do.h"
#include "M3_Driver/components/i2c/i2c_io.h"

// I2C 7-bit address
#define SLF3S_I2C_DEFAULT_ADDR     (0x08)
#define SLF3S_I2C_GENERAL_ADDR     (0x00)

// Commands (16-bit)
#define SLF3S_CMD_START_H2O        (0x3608)
#define SLF3S_CMD_START_IPA        (0x3615)
#define SLF3S_CMD_STOP             (0x3FF9)
#define SLF3S_CMD_READ_ID_1        (0x367C)
#define SLF3S_CMD_READ_ID_2        (0xE102)
#define SLF3S_CMD_SOFT_RESET       (0x0006)  // 8-bit, general call addr 0x00
#define SLF3S_CMD_CHANGE_ADD       (0x3661)

// Scale factors
#define SLF3S_FLOW_SCALE           (10.0f)   // ul/min
#define SLF3S_TEMP_SCALE           (200.0f)  // °C

// Error codes
#define SLF3S_OK                   ( 0)
#define SLF3S_ERROR                (-1)
#define SLF3S_CRC_ERROR            (-2)
#define SLF3S_I2C_ERROR            (-3)

// Product ID for SLF3S-0600F
#define SLF3S_PRODUCT_ID           (0x07030302UL)

typedef enum {
    WATER_MODE = 0,
    IPA_MODE,
} e_slf3s_mode_t;

typedef struct {
    float flow;
    float temp;
    uint16_t flags;
} slf3s_data_t;

int32_t slf3s_flow_soft_reset(i2c_io_t *i2c);
int32_t slf3s_flow_change_address(i2c_io_t *i2c, do_t *nIRQ_pin, uint16_t new_address);
int32_t slf3s_flow_init(i2c_io_t *i2c, uint8_t address, e_slf3s_mode_t mode);
int32_t slf3s_flow_stop(i2c_io_t *i2c, uint8_t address);
int32_t slf3s_flow_get_product_id(i2c_io_t *i2c, uint8_t address, int32_t* ID);
int32_t slf3s_flow_read_all(i2c_io_t *i2c, uint8_t address, slf3s_data_t *slf3s_data);

#endif /* SLF3S_FLOW_H_ */
