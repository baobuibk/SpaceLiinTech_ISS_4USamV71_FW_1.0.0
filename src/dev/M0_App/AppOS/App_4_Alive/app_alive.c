/************************************************
 *  @file     : app_alive.c
 *  @date     : January 2026
 *  @author   : CAO HIEU
 *  @version  : 1.0.0
 *-----------------------------------------------
 *  Description :
 *    [-]
 ************************************************/

#include "app_alive.h"
#include "peripheral/pio/plib_pio.h"
#include "M1_SysApp/xlog/xlog.h"
#include "M1_SysApp/dmesg/dmesg.h"
#include "stdio.h"
#include "BSP_Led/bsp_led.h"
#include "BSP_Watchdog/bsp_watchdog.h"
#include "BSP_RTC/bsp_rtc.h"
#include "M5_Utils/Tick/tick.h"
#include "M5_Utils/DateTime/date_time.h"


const osThreadAttr_t alive_attr = {
        .name = "LED_Alive",
        .stack_size = 256,
        .priority = configMAX_PRIORITIES - 4
};

#define RTC_SYNC_INTERVAL_S  60U

void App_AliveTask(void *param)
{
    static uint8_t counter = 0;
    static uint32_t s_rtc_sync_epoch = 0U;
    char buf[32];
    uint32_t last = Utils_GetTick();

    for(;;)
    {
        bsp_watchdog_update(Utils_GetTick());
        if(Utils_GetTick() - last >= 2000)
        {
            last = Utils_GetTick();
            bsp_led_toggle();
            osThreadFeed();
            snprintf(buf, sizeof(buf), "Alive counter: %lu", counter++);
//            xlog("Hello World %d\r\n", counter++);
            Dmesg_Write(buf);
        }

        uint32_t now = Utils_GetEpoch();
        if ((now - s_rtc_sync_epoch) >= RTC_SYNC_INTERVAL_S) {
            bsp_rtc_sync_to_soft();
            s_rtc_sync_epoch = now;
        }

        osDelay(pdMS_TO_TICKS(200));
    }
}
