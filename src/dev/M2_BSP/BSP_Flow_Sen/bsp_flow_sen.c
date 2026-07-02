#include "bsp_flow_sen.h"
#include "bsp_board/bsp_core.h"
#include "os_hal.h"
#include "stdbool.h"

flow_dev_t flow_dev[FLOW_SENSOR_NUMBER] = {
    {.i2c = &i2c0, .nIRQ_pin = &flow_1_nIRQ, .ena_pin = &flow_1_en, .init_state = INIT_NONE},
};

static uint8_t flow_address[4] = {0x0A, 0x0B, 0x0C, 0x0D};

Std_ReturnType bsp_flow_sen_init(uint8_t index) {
    if (index >= FLOW_SENSOR_NUMBER)
        return ERROR_INVALID_PARAM;

    if (flow_dev[index].i2c == NULL)
        return ERROR_INVALID_PARAM;

    /* Enable sensor */
    do_set(flow_dev[index].ena_pin);
    os_delay_ms(2);

    /* Reset sensor (broadcast) */
    slf3s_flow_soft_reset(flow_dev[index].i2c);

    os_delay_ms(2);

    /* Change I2C address */
    if (slf3s_flow_change_address(flow_dev[index].i2c, flow_dev[index].nIRQ_pin, flow_address[index]) != SLF3S_OK) {
        return ERROR_FAIL;
    }

    flow_dev[index].address = flow_address[index];
    os_delay_ms(2);

    /* Initialize measurement mode */
    if (slf3s_flow_init(flow_dev[index].i2c, flow_dev[index].address, WATER_MODE) != SLF3S_OK) {
        flow_dev[index].init_state = INIT_FAILED;
        return ERROR_FAIL;
    }
    
    flow_dev[index].init_state = INIT_DONE;
    os_delay_ms(2);

    return ERROR_OK;
}

Std_ReturnType bsp_flow_sen_read_all(uint8_t index) {
    if (flow_dev[index].init_state != INIT_DONE)
        if (bsp_flow_sen_init(index) != ERROR_OK)
            return ERROR_FAIL;
    
    int32_t ret1 = slf3s_flow_read_all(flow_dev[index].i2c, flow_dev[index].address, &flow_dev[index].data);
    if (ret1 == SLF3S_OK)
        return ERROR_OK;
    else
        return ERROR_FAIL;
}
