/* ============================================================
 * app_ctrl.c  — Tables 2-4 removed; task retained as stub so
 *               app_root.c does not need changes.
 * ============================================================ */
#include "app_ctrl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "M1_SysApp/dmesg/dmesg.h"

const osThreadAttr_t ctrl_attr = {
    .name       = "Ctrl",
    .stack_size = configMINIMAL_STACK_SIZE * 2,
    .priority   = configMAX_PRIORITIES - 4
};

void App_CtrlTask(void *param)
{
    (void)param;
    Dmesg_Write("[CTRL] task idle (T2-T4 removed)");
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(30000));
    }
}
