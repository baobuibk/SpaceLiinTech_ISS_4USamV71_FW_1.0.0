#include "app_data_tranfer.h"
#include "BSP_PSRAM/bsp_psram.h"
#include "BSP_Board/bsp_core.h"
#include "BSP_System/bsp_system.h"

QueueHandle_t psram_notify_full_queue;


const osThreadAttr_t DataTranfer_attr = {
    .name = "RamAccess",
    .stack_size = configMINIMAL_STACK_SIZE * 4,
    .priority = configMAX_PRIORITIES - 4
};

void DataTranfer_init(void) {
    psram_notify_full_queue = xQueueCreate(10, sizeof(psram_full_object_t));
}

void App_DataTranfer(void *param) {
    (void) param;
    psram_full_object_t object;
    for (;;) {
        if (xQueueReceive(psram_notify_full_queue, &object, portMAX_DELAY) == pdPASS) {
            switch(object) {
                case FULL_DLS_DATA:
                    bsp_system_mpu_gpioA_pulse();
                    break;
                case FULL_LOG_DATA:
                    bsp_system_mpu_gpioB_pulse();
                    break;
                default:
                    break;
            }
        }
        
    }
}