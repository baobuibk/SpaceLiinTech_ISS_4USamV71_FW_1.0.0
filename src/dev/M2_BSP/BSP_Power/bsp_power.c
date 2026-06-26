/*
 * bsp_power.c
 *
 *  Created on: Feb 27, 2026
 *      Author: Admin
 */

#include "bsp_power.h"
#include "do.h"
#include "bsp_board/bsp_core.h"

//void bsp_power_som_on(void) {
//    do_set(&power_som);
//}
//
//void bsp_power_som_off(void) {
//    do_reset(&power_som);
//}
//
//bool bsp_power_som_status(void) {
//    return power_som.bStatus;
//}

//void bsp_power_buck_peri_on(void) {
//    do_set(&power_buck_peri);
//}
//
//void bsp_power_buck_peri_off(void) {
//    do_reset(&power_buck_peri);
//}
//
//bool bsp_power_buck_peri_status(void) {
//    return power_buck_peri.bStatus;
//}

void bsp_power_tec_on(void) {
//    do_set(&power_buck_peri);
    do_set(&power_tec);
}

void bsp_power_tec_off(void) {
    do_reset(&power_tec);
}

bool bsp_power_tec_status(void) {
    return power_tec.bStatus;
}

void bsp_power_sln_hd_on(void) {
    do_set(&power_sln_hd);
}

void bsp_power_sln_hd_off(void) {
    do_reset(&power_sln_hd);
}

bool bsp_power_sln_hd_status(void) {
    return power_sln_hd.bStatus;
}

//bsp_power_solenoid

//void bsp_power_solenoid_on(void) {
//    do_set(&power_solenoid);
//}
//
//void bsp_power_solenoid_off(void) {
//    do_reset(&power_solenoid);
//}
//
//bool bsp_power_solenoid_status(void) {
//    return power_solenoid.bStatus;
//}

void bsp_power_lp_heater_on(void) {
    do_set(&power_lp_heater);
}

void bsp_power_lp_heater_off(void) {
    do_reset(&power_lp_heater);
}

bool bsp_power_lp_heater_status(void) {
    return power_lp_heater.bStatus;
}

//void bsp_power_heater_on(void) {
//    do_set(&power_heater);
//}
//
//void bsp_power_heater_off(void) {
//    do_reset(&power_heater);
//}
//
//bool bsp_power_heater_status(void) {
//    return power_heater.bStatus;
//}

void power_on_all(void) {
//    bsp_power_som_on();
//    bsp_power_buck_peri_on();
    bsp_power_tec_on();
    bsp_power_sln_hd_on();
//    bsp_power_solenoid_on();
    bsp_power_lp_heater_on();
//    bsp_power_heater_on();
//#if (HW_VERSION == HW_VERSION_V110)
//    bsp_power_AI_on();
//#endif
}

void power_off_all(void) {
//    bsp_power_som_off();
//    bsp_power_buck_peri_off();
    bsp_power_tec_off();
    bsp_power_sln_hd_off();
//    bsp_power_solenoid_off();
    bsp_power_lp_heater_off();
//#if (HW_VERSION == HW_VERSION_V110)
//    bsp_power_AI_off();
//#endif
}

void power_status_all(void) {
//    bsp_power_som_status();
//    bsp_power_buck_peri_status();
    bsp_power_tec_status();
    bsp_power_sln_hd_status();
//    bsp_power_solenoid_status();
    bsp_power_lp_heater_status();
//    bsp_power_heater_status();
//#if (HW_VERSION == HW_VERSION_V110)
//    bsp_power_AI_status();
//#endif
}

//void bsp_power_AI_on(void) {
//    do_set(&power_ai);
//}
//
//void bsp_power_AI_off(void) {
//    do_reset(&power_ai);
//}
//
//bool bsp_power_AI_status(void) {
//    return power_ai.bStatus;
//}