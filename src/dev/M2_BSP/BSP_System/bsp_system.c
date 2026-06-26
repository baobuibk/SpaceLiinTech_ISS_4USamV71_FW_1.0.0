#include "bsp_system.h"
#include "os.h"

//void bsp_system_ifp_power_ena(void) {
//    do_set(&ifp_enable);
//}
//
//void bsp_system_ifp_power_dis(void) {
//    do_reset(&ifp_enable);
//}

void bsp_system_mpu_gpioA_set(void) {
    do_set(&mcu_pmu_gpio_a);
}

void bsp_system_mpu_gpioA_reset(void) {
    do_reset(&mcu_pmu_gpio_a);
}

void bsp_system_mpu_gpioA_pulse(void) {
    do_set(&mcu_pmu_gpio_a);
    osDelay(pdMS_TO_TICKS(100));
    do_reset(&mcu_pmu_gpio_a);
}

void bsp_system_mpu_gpioB_set(void) {
    do_set(&mcu_pmu_gpio_b);
}

void bsp_system_mpu_gpioB_reset(void) {
    do_reset(&mcu_pmu_gpio_b);
}

void bsp_system_mpu_gpioB_pulse(void) {
    do_set(&mcu_pmu_gpio_b);
    osDelay(pdMS_TO_TICKS(100));
    do_reset(&mcu_pmu_gpio_b);
}