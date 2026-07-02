/* 
 * File:   task_experiment.h
 * Author: HTSANG
 *
 * Created on March 20, 2026, 12:26 PM
 */

#ifndef TASK_EXPERIMENT_H
#define	TASK_EXPERIMENT_H

#include "M0_App/OS/os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "BSP_Laser_Int/bsp_laser.h"
#include "BSP_Photo/bsp_photo.h"

#define SPI0_COMMON_PRESCALE    120
#define PHOTO_ADC_PRESCALE      12

#define LP_TOTAL_CHANNEL  1
/* ===================== MEMORY DEFINE ===================== */
#define PHOTO_PSRAM_ADD     0x00
#define LASER_PSRAM_ADD     0x600000
#define LOG_PSRAM_ADD       0x700000

#define photo_data_t        uint16_t
#define laser_data_t        uint16_t
#define log_data_t          uint8_t

#define BUF_SIZE            512
#define BUF_COUNT           16

/*==================== DEFINE STRUCT ====================*/
typedef struct exp_profile {
    uint8_t channel;

    uint16_t pre_time_ms;
    uint16_t main_time_ms;
    uint16_t post_time_ms;

    uint16_t photo_adc_rate_khz;
    uint16_t laser_adc_rate_khz;
    
    uint16_t laser_intensity;
} exp_profile_t;

/*==================== DEFINE ENUM ====================*/
typedef enum {
    EXP_CMD_NULL = 0,
    EXP_CMD_RUN
} exp_cmd_t;

extern QueueHandle_t experiment_command_queue;

extern QueueHandle_t psram_store_log_queue;
extern QueueHandle_t psram_store_log_free_queue;

extern const osThreadAttr_t exp_attr;
extern const osThreadAttr_t psram_attr;

void experiment_init(void);
void experiment_set_profile(exp_profile_t *exp_profile);
void App_EXPTask(void *param);
void APP_PSRAMTask(void *param);

#endif	/* TASK_EXPERIMENT_H */

