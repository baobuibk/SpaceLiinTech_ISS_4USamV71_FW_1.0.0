#include "bsp_bme688.h"
#include "stdbool.h"
#include "bsp_board/bsp_core.h"

bme688_dev_t bme688_int =
{
    .i2c_bus  = &i2c1,
    .slaveAdd = BME688_ADDRESS_0,
    .init_status = false,
};

Std_ReturnType bsp_bme688_init(bme688_dev_t* dev)
{
    if(dev == NULL) return ERROR_INVALID_PARAM;

    return bme688_init(dev);
}

Std_ReturnType bsp_bme688_read_temperature(bme688_dev_t* dev, float* temperature)
{
    if(dev == NULL) return ERROR_INVALID_PARAM;
    
    return bme688_read_temperature(dev, temperature);
}

Std_ReturnType bsp_bme688_read_pressure(bme688_dev_t* dev, float* pressure)
{
    if(dev == NULL) return ERROR_INVALID_PARAM;
    
    return bme688_read_pressure(dev, pressure);
}

Std_ReturnType bsp_bme688_read_humidity(bme688_dev_t* dev, float* humidity)
{
    if(dev == NULL) return ERROR_INVALID_PARAM;
    
    return bme688_read_humidity(dev, humidity);
}

Std_ReturnType bsp_bme688_read_all(bme688_dev_t* dev, bme688_data_t* p_data)
{
    if(dev == NULL) return ERROR_INVALID_PARAM;
   
    return bme688_read_forced(dev, p_data);
}