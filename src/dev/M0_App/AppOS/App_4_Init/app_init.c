#include "app_init.h"

#include "M0_App/AppOS/App_7_Syslog/app_system_log.h"

#include "M1_SysApp/xlog/xlog.h"
#include "M1_SysApp/dmesg/dmesg.h"

#include "BSP_Solenoid/bsp_solenoid.h"
#include "BSP_RTC/bsp_rtc.h"
#include "BSP_Pump/bsp_pump.h"
#include "BSP_Flow_Sen/bsp_flow_sen.h"
#include "BSP_TEC/bsp_tec.h"
#include "BSP_BME688/bsp_bme688.h"
#include "BSP_LSM6DSOX/bsp_lsm6dsox.h"
#include "BSP_Solenoid_MAX22200/bsp_solenoid_max.h"

#include "stdio.h"
#include "BSP_Power/bsp_power.h"

const osThreadAttr_t init_attr = {
    .name = "Init",
    .stack_size = 256,
    .priority = configMAX_PRIORITIES - 2
};

void App_InitTask(void *param) {
    (void) param;

    syslog_init();
    
    bsp_rtc_init();
    bsp_rtc_sync_to_soft();
    
    bsp_bme688_init(&bme688_int);
    bsp_lsm6dsox_init();
    
    bsp_tec_init(p_tec[0]);
    bsp_tec_init(p_tec[1]);
    
    bsp_power_sln_hd_on();
    bsp_pump_init();
    bsp_solenoid_max_init();
    
    vTaskDelete(NULL);
}