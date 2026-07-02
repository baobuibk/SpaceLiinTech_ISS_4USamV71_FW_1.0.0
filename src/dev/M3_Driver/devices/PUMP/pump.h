#ifndef _PUMP_H_

#define _PUMP_H_

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "stdint.h"
#include "stdbool.h"

/* FreeRTOS includes. */
#include "os.h"

/* Component includes. */
#include "i2c_io.h"
#include "do.h"
#include "define.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* Bartels mp-Highdriver registers */
#define I2C_HIGHDRIVER_ADRESS (0x78) // Default adress for mp-Highdriver
#define I2C_PRODUCT_ID 0xB2
#define I2C_DEVICEID  0x00
#define I2C_POWERMODE 0x01
#define I2C_FREQUENCY 0x02
#define I2C_SHAPE     0x03
#define I2C_BOOST     0x04
#define I2C_AUDIO     0x05
#define I2C_PVOLTAGE  0x06
#define I2C_P1VOLTAGE 0x06
#define I2C_P2VOLTAGE 0x07
#define I2C_P3VOLTAGE 0x08
#define I2C_P4VOLTAGE 0x09
#define I2C_UPDATEVOLTAGE 0x0A

#ifndef constrain
#define constrain(amt, low, high) (( (amt) < (low) ) ? (low) : ( ((amt) > (high)) ? (high) : (amt) ))
#endif
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
typedef struct {
    i2c_io_t* i2c_bus;
    uint8_t slaveAdd;
    bool      init_status;
} pump_dev_t;
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
Std_ReturnType Highdriver_init(pump_dev_t* dev);
Std_ReturnType Highdriver_enable(pump_dev_t* dev, uint8_t is_enable);
Std_ReturnType Highdriver_setvoltage(pump_dev_t* dev, uint8_t _voltage);
Std_ReturnType Highdriver_setfrequency(pump_dev_t* dev, uint16_t _frequency);

Std_ReturnType _mp_i2c_write_block(pump_dev_t* dev, uint8_t reg, uint8_t val);
Std_ReturnType _mp_i2c_read_block(pump_dev_t* dev, uint8_t reg, uint8_t *buf, uint8_t len);

#endif //_PUMP_H_
