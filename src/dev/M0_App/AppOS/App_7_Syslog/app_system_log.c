#include "app_system_log.h"

const osThreadAttr_t syslog_attr = {
    .name = "syslog",
    .stack_size = configMINIMAL_STACK_SIZE * 4,
    .priority = configMAX_PRIORITIES - 4
};

void syslog_init(void) {
    lwl_start();
}

void App_SysLog(void *param) {
    (void) param;
    for (;;) {
        LWL(LWL_EXP_TEMP_SINGLE_NTC, LWL_1(0), LWL_2(0x1234));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}