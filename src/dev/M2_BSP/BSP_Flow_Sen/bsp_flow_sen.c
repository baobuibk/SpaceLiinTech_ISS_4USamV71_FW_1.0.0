#include "bsp_flow_sen.h"
#include "stdbool.h"
#include "do.h"
#include "bsp_board/bsp_core.h"
#include "os_hal.h"

flow_sensor_data_t s_flow_sen_data[1];

//static do_t* const flow_en_pins[4] = {
//    &flow1_en, &flow2_en, &flow3_en, &flow4_en
//};

static do_t* const flow_en_pins[1] = {
    &flow1_en
};

int bsp_flow_sen_init (void)
{
    int success_count = 0;
    for (int i = 0; i < 1; i++) {
        do_set(flow_en_pins[i]);
        os_delay_ms(1);
        if (slf3s_init(&i2c0, true) == SLF3S_OK) {
            success_count++;
        }
        
        do_reset(flow_en_pins[i]);
    }
    return (success_count > 0);
}

int bsp_flow_sen_read_all (void)
{
    bool any_success = false;
    for (int i = 0; i < 1; i++) {
        do_set(flow_en_pins[i]);
        os_delay_ms(1);
        
        int32_t ret = slf3s_read_all(&i2c0, &s_flow_sen_data[i].slf3s_dat);
        s_flow_sen_data[i].read_oke = (ret == SLF3S_OK);
        
        if (s_flow_sen_data[i].read_oke) {
            any_success = true;
        }
        do_reset(flow_en_pins[i]);
    }
    
    return (int)any_success;
}
