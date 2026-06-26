#include "bsp_tec.h"


tec_dev_t tec_1 = 
{
    .hspi = &spi2,
    .spi_cs_pin = &tec_1_cs,
    .sw_pin = &tec_1_sw,
};

tec_dev_t tec_2 = 
{
    .hspi = &spi2,
    .spi_cs_pin = &tec_2_cs,
    .sw_pin = &tec_2_sw,
};

//tec_dev_t tec_3 = 
//{
//    .hspi = &spi2,
//    .spi_cs_pin = &tec_3_cs,
//    .sw_pin = &tec_3_sw,
//};
//
//tec_dev_t tec_4 = 
//{
//    .hspi = &spi2,
//    .spi_cs_pin = &tec_4_cs,
//    .sw_pin = &tec_4_sw,
//};

//tec_dev_t* p_tec[4] = {&tec_1, &tec_2, &tec_3, &tec_4};

tec_dev_t* p_tec[2] = {&tec_1, &tec_2};

uint32_t bsp_tec_init(tec_dev_t* dev)
{
    return lt8722_init(dev);
}

uint32_t bsp_tec_clear_status(tec_dev_t* dev)
{
    return lt8722_clear_status(dev);
}

uint32_t bsp_tec_get_status(tec_dev_t* dev, uint16_t *status)
{
    return lt8722_get_status(dev, status);
}

uint32_t bsp_tec_set_output_voltage_channel(tec_dev_t* dev, int64_t nVol)
{
    return lt8722_set_output_voltage_channel(dev, TEC_COOL, nVol);
}

uint32_t bsp_tec_set_enable_req(tec_dev_t* dev)
{
    return lt8722_set_enable_req(dev, LT8722_ENABLE_REQ_ENABLED);
}

uint32_t bsp_tec_set_swen_req(tec_dev_t* dev)
{
    return lt8722_set_swen_req(dev, LT8722_SWEN_REQ_ENABLED);
}

uint32_t bsp_tec_clear_swen_req(tec_dev_t* dev)
{
    return lt8722_set_swen_req(dev, LT8722_SWEN_REQ_DISABLED);
}
