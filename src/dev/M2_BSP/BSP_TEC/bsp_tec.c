#include "bsp_tec.h"


tec_dev_t tec_1 = 
{
    .hspi = &spi2,
    .spi_cs_pin = &tec_1_cs,
    .sw_pin = &tec_1_sw,
    .init_state = INIT_NONE,
};

tec_dev_t tec_2 = 
{
    .hspi = &spi2,
    .spi_cs_pin = &tec_2_cs,
    .sw_pin = &tec_2_sw,
    .init_state = INIT_NONE,
};

tec_dev_t* p_tec[2] = {&tec_1, &tec_2};

Std_ReturnType bsp_tec_init(tec_dev_t* dev)
{
    if (lt8722_init(dev) == ERROR_OK) {
        dev->init_state = INIT_DONE;
        return ERROR_OK;
    } else {
        dev->init_state = INIT_FAILED;
        return ERROR_FAIL;
    }
}

Std_ReturnType bsp_tec_clear_status(tec_dev_t* dev)
{
    if (dev->init_state != INIT_DONE)
        bsp_tec_init(dev);
    return lt8722_clear_status(dev);
}

Std_ReturnType bsp_tec_get_status(tec_dev_t* dev, uint16_t *status)
{
    if (dev->init_state != INIT_DONE)
        bsp_tec_init(dev);
    return lt8722_get_status(dev, status);
}

Std_ReturnType bsp_tec_set_output_voltage_channel(tec_dev_t* dev, int64_t nVol)
{
    if (dev->init_state != INIT_DONE)
        bsp_tec_init(dev);
    return lt8722_set_output_voltage_channel(dev, TEC_COOL, nVol);
}

Std_ReturnType bsp_tec_set_enable_req(tec_dev_t* dev)
{
    if (dev->init_state != INIT_DONE)
        bsp_tec_init(dev);
    return lt8722_set_enable_req(dev, LT8722_ENABLE_REQ_ENABLED);
}

Std_ReturnType bsp_tec_set_swen_req(tec_dev_t* dev)
{
    if (dev->init_state != INIT_DONE)
        bsp_tec_init(dev);
    return lt8722_set_swen_req(dev, LT8722_SWEN_REQ_ENABLED);
}

Std_ReturnType bsp_tec_clear_swen_req(tec_dev_t* dev)
{
    if (dev->init_state != INIT_DONE)
        bsp_tec_init(dev);
    return lt8722_set_swen_req(dev, LT8722_SWEN_REQ_DISABLED);
}
