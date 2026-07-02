/* Board Support includes. */
#include "bsp_pump.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

pump_dev_t pump = {
    .i2c_bus = &i2c2,
    .slaveAdd = I2C_HIGHDRIVER_ADRESS,
    .init_status = false
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
Std_ReturnType bsp_pump_init(void) {
//    do_set(&power_buck_peri);

    do_set(&power_sln_hd);

    do_set(&hd4_en);

    os_delay_ms(200);

    return Highdriver_init(&pump);
}

Std_ReturnType bsp_pump_enable(bool is_enable) {
    return Highdriver_enable(&pump, is_enable);
}

Std_ReturnType bsp_pump_set_voltage(uint8_t volt) {
    return Highdriver_setvoltage(&pump, volt);
}

Std_ReturnType bsp_pump_set_freq(uint16_t freq) {
    return Highdriver_setfrequency(&pump, freq);
}

Std_ReturnType bsp_pump_read_reg(uint8_t reg ,uint8_t* val) {
    return _mp_i2c_read_block(&pump, reg, val, 1);
}