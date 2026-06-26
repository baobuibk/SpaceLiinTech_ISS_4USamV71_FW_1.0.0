/* 
 * File:   app_ram_access.h
 * Author: HTSANG
 *
 * Created on June 2, 2026, 3:28 PM
 */

#ifndef APP_RAM_ACCESS_H
#define	APP_RAM_ACCESS_H

#include "os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

extern QueueHandle_t psram_notify_full_queue;

typedef enum {
    FULL_DLS_DATA = 0,
    FULL_LOG_DATA
} psram_full_object_t;

extern const osThreadAttr_t DataTranfer_attr;

void DataTranfer_init(void);
void App_DataTranfer(void *param);

#endif	/* APP_RAM_ACCESS_H */
