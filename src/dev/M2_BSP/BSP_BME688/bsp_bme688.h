#ifndef BSP_BSP_BME688_H_
#define BSP_BSP_BME688_H_

#include "define.h"
#include "../../M3_Driver/devices/BME688/bme688.h"
#include "i2c_io.h"

extern bme688_dev_t bme688_int;

Std_ReturnType bsp_bme688_init(bme688_dev_t* dev);

Std_ReturnType bsp_bme688_read_temperature(bme688_dev_t* dev, float* temperature);
Std_ReturnType bsp_bme688_read_pressure(bme688_dev_t* dev, float* pressure);
Std_ReturnType bsp_bme688_read_humidity(bme688_dev_t* dev, float* humidity);
Std_ReturnType bsp_bme688_read_all(bme688_dev_t* dev, bme688_data_t* p_data);

#endif