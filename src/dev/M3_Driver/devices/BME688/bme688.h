#ifndef _BME_688_H
#define _BME_688_H

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "stdint.h"
#include "stdbool.h"
#include "define.h"
#include "i2c_io.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define BME688_ADDRESS_0          0x76  /* GND */
#define BME688_ADDRESS_1          0x77  /* VCC */

#define BME688_REG_CHIP_ID       0xD0u
#define BME688_REG_RESET         0xE0u
#define BME688_REG_CTRL_HUM      0x72u
#define BME688_REG_CTRL_MEAS     0x74u
#define BME688_REG_CONFIG        0x75u
#define BME688_REG_MEAS_STATUS0  0x1Du
#define BME688_REG_PRESS_MSB     0x1Fu  /* 0x1F..0x21: pres  adc[19:0] */
#define BME688_REG_TEMP_MSB      0x22u  /* 0x22..0x24: temp  adc[19:0] */
#define BME688_REG_HUM_MSB       0x25u  /* 0x25..0x26: hum   adc[15:0] */

/* NVM trim registers (temperature) */
#define BME688_REG_T1_LSB        0xE9u
#define BME688_REG_T2_LSB        0x8Au
#define BME688_REG_T3            0x8Cu
/* NVM trim registers (pressure) */
#define BME688_REG_P1_LSB        0x8Eu
/* ... (li?t kê t??ng t?; xem Section 5.2 datasheet) */
/* NVM trim registers (humidity) */
#define BME688_REG_H1_MSB        0xE3u
/* ... */

#define BME688_CHIP_ID           0x61u
#define BME688_RESET_CMD         0xB6u

/* Oversampling codes (osrs field, 3-bit) */
#define BME688_OS_SKIP           0x00u   /* measurement disabled */
#define BME688_OS_1X             0x01u
#define BME688_OS_2X             0x02u
#define BME688_OS_4X             0x03u
#define BME688_OS_8X             0x04u
#define BME688_OS_16X            0x05u

/* IIR filter coefficient codes (filter[2:0]) */
#define BME688_FILTER_OFF        0x00u
#define BME688_FILTER_COEFF_1    0x01u
#define BME688_FILTER_COEFF_3    0x02u
#define BME688_FILTER_COEFF_7    0x03u
#define BME688_FILTER_COEFF_15   0x04u
#define BME688_FILTER_COEFF_31   0x05u
#define BME688_FILTER_COEFF_63   0x06u
#define BME688_FILTER_COEFF_127  0x07u

/* mode[1:0] in ctrl_meas */
#define BME688_MODE_SLEEP        0x00u
#define BME688_MODE_FORCED       0x01u

/* new_data_0 flag in meas_status_0 (bit 5) */
#define BME688_NEW_DATA_MASK     0x20u
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
typedef struct
{
    /* Temperature */
    uint16_t T1;
    int16_t  T2;
    int8_t   T3;
    /* Pressure */
    uint16_t P1;
    int16_t  P2;
    int8_t   P3;
    int16_t  P4;
    int16_t  P5;
    int8_t   P6;
    int8_t   P7;
    int16_t  P8;
    int16_t  P9;
    uint8_t  P10;
    /* Humidity */
    uint16_t H1;
    uint16_t H2;
    int8_t   H3;
    int8_t   H4;
    int8_t   H5;
    uint8_t  H6;
    int8_t   H7;
} bme688_calib_t;

typedef struct
{
    i2c_io_t* i2c_bus;
    uint8_t slaveAdd;
    bool init_status;
    bme688_calib_t calib;
} bme688_dev_t;

typedef struct
{
	float temperature;   /* °C   */
    float pressure;      /* Pa   */
    float humidity;      /* %rH  */
} bme688_data_t;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
Std_ReturnType bme688_init(bme688_dev_t* dev);

Std_ReturnType bme688_read_temperature(bme688_dev_t* dev, float* temperature);
Std_ReturnType bme688_read_pressure(bme688_dev_t* dev, float* pressure);
Std_ReturnType bme688_read_humidity(bme688_dev_t* dev, float* humidity);

Std_ReturnType bme688_read_forced(bme688_dev_t* dev, bme688_data_t* out);


#endif //_BME_688_H