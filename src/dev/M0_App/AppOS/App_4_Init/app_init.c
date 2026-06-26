#include "app_init.h"

#include "M0_App/AppOS/App_7_Syslog/app_system_log.h"

#include "M1_SysApp/xlog/xlog.h"
#include "M1_SysApp/dmesg/dmesg.h"

#include "BSP_Solenoid/bsp_solenoid.h"
#include "BSP_RTC/bsp_rtc.h"
#include "BSP_Pump/bsp_pump.h"
#include "BSP_Flow_Sen/bsp_flow_sen.h"
#include "BSP_TEC/bsp_tec.h"

#include "stdio.h"
#include "BSP_Power/bsp_power.h"

const osThreadAttr_t init_attr = {
    .name = "Init",
    .stack_size = 256,
    .priority = configMAX_PRIORITIES - 2
};

void App_InitTask(void *param) {
    (void) param;

    bsp_rtc_init();
    bsp_rtc_sync_to_soft();
    bsp_flow_sen_init();
//    bsp_pump_init();

    bsp_tec_init(p_tec[0]);
    bsp_tec_init(p_tec[1]);
    
    syslog_init();
    
    bsp_power_sln_hd_on();
    
    vTaskDelete(NULL);
}