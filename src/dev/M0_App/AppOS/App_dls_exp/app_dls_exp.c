#include "app_dls_exp.h"
#include "board.h"
#include "samv71q21b.h"
#include "task.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "packs/ATSAMV71Q21B_DFP/component/spi.h"
#include "config/default/peripheral/tc/plib_tc0.h"
#include "config/default/peripheral/tc/plib_tc3.h"

#include "M0_App/AppOS/App_8_DataTranfer/app_data_tranfer.h"

#include "BSP_Board/bsp_core.h"
#include "BSP_Power/bsp_power.h"
#include "BSP_Photo/bsp_photo.h"
#include "BSP_PSRAM/bsp_psram.h"
#include "BSP_System/bsp_system.h"

uint32_t psram_addr_photo = PHOTO_PSRAM_ADD;
uint32_t psram_addr_laser = LASER_PSRAM_ADD;
uint32_t psram_addr_log = LOG_PSRAM_ADD;

__attribute__((section(".ram_nocache"), aligned(32)))
static photo_data_t photo_pool[BUF_COUNT][BUF_SIZE];

__attribute__((section(".ram_nocache"), aligned(32)))
static laser_data_t laser_pool[BUF_COUNT][BUF_SIZE];

static photo_data_t *photo_current_buf;
static uint16_t photo_buf_idx;

static laser_data_t *laser_current_buf;
static uint16_t laser_buf_idx;

QueueHandle_t psram_store_photo_queue;
QueueHandle_t psram_store_photo_free_queue;

QueueHandle_t psram_store_laser_queue;
QueueHandle_t psram_store_laser_free_queue;

QueueHandle_t psram_store_log_queue;
QueueHandle_t psram_store_log_free_queue;

typedef struct {
    uint8_t channel;
    uint8_t sub_state;
    uint32_t counter_10ms;
    uint32_t pre_10ms_time;
    uint32_t main_10ms_time;
    uint32_t post_10ms_time;
    uint8_t wait_spi_timeout;
    uint32_t dac_code;
} exp_runtime_t;

/*==================== STRUCT DATA ====================*/
exp_profile_t s_exp_profile = {
    .channel = 23,
    .laser_intensity = 100,

    .pre_time_ms = 1000,
    .main_time_ms = 2000,
    .post_time_ms = 1000,

    .photo_adc_rate_khz = 100,
    .laser_adc_rate_khz = 10,
};

static exp_runtime_t exp_rt;

/*==================== DATA ====================*/
uint32_t volatile photo_spi_count = 0;
uint32_t volatile current_channel = 0;
uint32_t volatile photo_spi_set_count = 0;
uint8_t volatile is_spi_counter_finish = 0;
uint8_t volatile sampling_enable = 0;

/*==================== DEFINE ENUM ====================*/
typedef enum {
    SUB_PRE_PHASE,
    SUB_MAIN_PHASE,
    SUB_POST_PHASE,
    SUB_DONE_CH
} exp_sub_state_t;

/*==================== QUEUE ====================*/
QueueHandle_t experiment_command_queue;

/*============= PROTOTYPE PRIVATE FUNCTIONS ================*/
static void exp_setup(void);
static void exp_start(void);
static void adc_flush_last_buffer(void);
void TC3_CH1_My_Handler(TC_TIMER_STATUS status, uintptr_t context);
void TC3_CH2_My_Handler(TC_TIMER_STATUS status, uintptr_t context);

/*==================== PUBLIC FUNCTIONS ====================*/
void experiment_init(void) {
    TC3_CH1_TimerCallbackRegister(TC3_CH1_My_Handler, 0);
    TC3_CH2_TimerCallbackRegister(TC3_CH2_My_Handler, 0);

    psram_store_photo_queue = xQueueCreate(BUF_COUNT, sizeof (photo_data_t *));
    psram_store_photo_free_queue = xQueueCreate(BUF_COUNT, sizeof (photo_data_t *));

    psram_store_laser_queue = xQueueCreate(BUF_COUNT, sizeof (laser_data_t *));
    psram_store_laser_free_queue = xQueueCreate(BUF_COUNT, sizeof (laser_data_t *));

    psram_store_log_queue = xQueueCreate(BUF_COUNT, sizeof (log_data_t *));
    psram_store_log_free_queue = xQueueCreate(BUF_COUNT, sizeof (log_data_t *));

    experiment_command_queue = xQueueCreate(5, sizeof (exp_cmd_t));

    for (int i = 0; i < BUF_COUNT; i++) {
        photo_data_t *p_buf = photo_pool[i];
        xQueueSend(psram_store_photo_free_queue, &p_buf, 0);

        laser_data_t *l_buf = laser_pool[i];
        xQueueSend(psram_store_laser_free_queue, &l_buf, 0);
    }

    xQueueReceive(psram_store_photo_free_queue, &photo_current_buf, portMAX_DELAY);
    photo_buf_idx = 0;

    xQueueReceive(psram_store_laser_free_queue, &laser_current_buf, portMAX_DELAY);
    laser_buf_idx = 0;
}

void experiment_set_profile(exp_profile_t *exp_profile) {
    s_exp_profile.channel = exp_profile->channel;
    s_exp_profile.pre_time_ms = exp_profile->pre_time_ms;
    s_exp_profile.main_time_ms = exp_profile->main_time_ms;
    s_exp_profile.post_time_ms = exp_profile->post_time_ms;
    s_exp_profile.photo_adc_rate_khz = exp_profile->photo_adc_rate_khz;
    s_exp_profile.laser_adc_rate_khz = exp_profile->laser_adc_rate_khz;
    s_exp_profile.laser_intensity = exp_profile->laser_intensity;
}

/* ============================================================
 *  Experiment_Task
 * ============================================================ */
const osThreadAttr_t exp_attr = {
    .name = "exp",
    .stack_size = configMINIMAL_STACK_SIZE * 4,
    .priority = configMAX_PRIORITIES - 3
};

void App_EXPTask(void *param) {
    (void) param;
    exp_cmd_t command = EXP_CMD_NULL;
    for (;;) {
        if (xQueueReceive(experiment_command_queue, &command, 0) == pdPASS) {
            if (command == EXP_CMD_RUN) {
                psram_addr_photo = PHOTO_PSRAM_ADD;
                psram_addr_laser = LASER_PSRAM_ADD;
                exp_setup();
                exp_start();
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/*==================== SETUP ====================*/
static void exp_setup(void) {
    exp_rt.channel = s_exp_profile.channel;
    exp_rt.counter_10ms = 0;
    exp_rt.pre_10ms_time = s_exp_profile.pre_time_ms / 10;
    exp_rt.main_10ms_time = s_exp_profile.main_time_ms / 10;
    exp_rt.post_10ms_time = s_exp_profile.post_time_ms / 10;
    exp_rt.sub_state = SUB_PRE_PHASE;
    exp_rt.dac_code = (uint8_t) ((((float) s_exp_profile.laser_intensity * 256.0) / 100.0) - 1.0);

    uint16_t exp_rt_period = (120000000 / s_exp_profile.photo_adc_rate_khz) / 1000.0;
    TC0_CH1_ComparePeriodSet(exp_rt_period);
    TC0_CH1_CompareASet(exp_rt_period * 995 / 1000);

    exp_rt_period = (937500 / s_exp_profile.laser_adc_rate_khz) / 1000.0;
    TC3_CH2_TimerPeriodSet(exp_rt_period);

    bsp_power_lp_heater_on();
    photo_spi_count = 0;
    is_spi_counter_finish = 0;
    bsp_laser_int_all_sw_off();
    bsp_photo_all_sw_off();
    bsp_laser_int_set_dac(exp_rt.dac_code);

    photo_buf_idx = 0;
    laser_buf_idx = 0;
}

static void exp_start(void) {
    bsp_photo_sw_on(exp_rt.channel);
    sampling_enable = 1;
    TC0_CH1_CompareStart();
    TC3_CH1_TimerStart();
    TC3_CH2_TimerStart();
}

/*============================================================================
 *This is callback function when program is trigger timer to ADC collection
 *============================================================================*/
void TC0_CH1_Handler(void) {
    uint32_t status = TC0_REGS->TC_CHANNEL[1].TC_SR;
    if (status & TC_SR_CPAS_Msk) {
        ads8329_spi_read_val(&photo_adc_dev);
    }
}

void TC3_CH2_My_Handler(TC_TIMER_STATUS status, uintptr_t context) {
    (void) status;
    (void) context;

    if (!sampling_enable) return;

    uint16_t laser_val = bsp_laser_int_read_adc();

    laser_current_buf[laser_buf_idx++] = laser_val;

    if (laser_buf_idx >= BUF_SIZE) {
        laser_data_t *full_buf = laser_current_buf;
        laser_data_t *new_buf = NULL;
        BaseType_t hpw = pdFALSE;

        if (xQueueReceiveFromISR(psram_store_laser_free_queue, &new_buf, &hpw) == pdTRUE &&
                xQueueSendFromISR(psram_store_laser_queue, &full_buf, &hpw) == pdTRUE) {
            laser_current_buf = new_buf;
            laser_buf_idx = 0;
        } else {
            laser_buf_idx = 0;
        }
        portYIELD_FROM_ISR(hpw);
    }
}

void TC3_CH1_My_Handler(TC_TIMER_STATUS status, uintptr_t context) {
    (void) status;
    (void) context;
    exp_rt.counter_10ms++;

    switch (exp_rt.sub_state) {
        case SUB_PRE_PHASE:
            if (exp_rt.counter_10ms >= exp_rt.pre_10ms_time) {
                bsp_laser_int_sw_on(exp_rt.channel);
                exp_rt.counter_10ms = 0;
                exp_rt.sub_state = SUB_MAIN_PHASE;
            }
            break;
        case SUB_MAIN_PHASE:
            if (exp_rt.counter_10ms >= exp_rt.main_10ms_time) {
                bsp_laser_int_all_sw_off();
                exp_rt.counter_10ms = 0;
                exp_rt.sub_state = SUB_POST_PHASE;
            }
            break;
        case SUB_POST_PHASE:
            if (exp_rt.counter_10ms >= exp_rt.post_10ms_time) {
                sampling_enable = 0;
                TC0_CH1_CompareStop();
                TC3_CH2_TimerStop();
                adc_flush_last_buffer();
                exp_rt.counter_10ms = 0;
                exp_rt.sub_state = SUB_DONE_CH;
            }
            break;
        case SUB_DONE_CH:
            TC3_CH1_TimerStop();
            
            BaseType_t hpw = pdFALSE;
            psram_full_object_t object = FULL_DLS_DATA;
            xQueueSendFromISR(psram_notify_full_queue, &object, &hpw);
            
            DEBUG_SendString("EXP DONE!");
            break;
        default:
            break;
    }
}

/*============================================================================
 * This is callback function when program is receive data from Photo board
 *============================================================================*/
void SPI0_Handler(void) {
    uint32_t sr = SPI0_REGS->SPI_SR;
    BaseType_t hpw = pdFALSE;

    /* RX ready */
    while (SPI0_REGS->SPI_SR & SPI_SR_RDRF_Msk) {
        if (!sampling_enable) {
            volatile uint32_t dummy = SPI0_REGS->SPI_RDR;
            (void) dummy;
            continue;
        }

        ads8329_spi_irq_callback(&photo_adc_dev);
        photo_current_buf[photo_buf_idx++] = photo_adc_dev.raw;

        if (photo_buf_idx >= BUF_SIZE) {
            photo_data_t *full_buf = photo_current_buf;
            photo_data_t *new_buf = NULL;

            if (xQueueReceiveFromISR(psram_store_photo_free_queue, &new_buf, &hpw) == pdTRUE &&
                    xQueueSendFromISR(psram_store_photo_queue, &full_buf, &hpw) == pdTRUE) {
                photo_current_buf = new_buf;
                photo_buf_idx = 0;
            } else {
                photo_buf_idx = 0;
            }
        }
    }

    portYIELD_FROM_ISR(hpw);

    /* Overrun */
    if (sr & SPI_SR_OVRES_Msk) {
        volatile uint32_t dummy = SPI0_REGS->SPI_RDR;
        (void) dummy;
    }

    /* Mode fault */
    if (sr & SPI_SR_MODF_Msk) {
        volatile uint32_t dummy = SPI0_REGS->SPI_SR;
        (void) dummy;

        SPI0_REGS->SPI_CR = SPI_CR_SPIEN_Msk;
    }

    /* Underrun */
    if (sr & SPI_SR_UNDES_Msk) {
        volatile uint32_t dummy = SPI0_REGS->SPI_SR;
        (void) dummy;
    }
}

const osThreadAttr_t psram_attr = {
    .name = "psram",
    .stack_size = configMINIMAL_STACK_SIZE * 4,
    .priority = configMAX_PRIORITIES - 2
};

void APP_PSRAMTask(void *param) {
    (void) param;
    photo_data_t *recv_photo;
    laser_data_t *recv_laser;
    log_data_t *recv_log;

    while (1) {
        if (xQueueReceive(psram_store_photo_queue, &recv_photo, pdMS_TO_TICKS(5)) == pdPASS) {
            uint32_t size = BUF_SIZE * sizeof (photo_data_t);
            bsp_psram_write(&g_psram, psram_addr_photo, (uint8_t *) recv_photo, size);
            psram_addr_photo += size;
            xQueueSend(psram_store_photo_free_queue, &recv_photo, 0);
        }

        if (xQueueReceive(psram_store_laser_queue, &recv_laser, pdMS_TO_TICKS(5)) == pdPASS) {
            uint32_t size = BUF_SIZE * sizeof (laser_data_t);
            bsp_psram_write(&g_psram, psram_addr_laser, (uint8_t *) recv_laser, size);
            psram_addr_laser += size;
            xQueueSend(psram_store_laser_free_queue, &recv_laser, 0);
        }

        if (xQueueReceive(psram_store_log_queue, &recv_log, pdMS_TO_TICKS(5)) == pdPASS) {
            uint32_t size = BUF_SIZE * sizeof (log_data_t);
            bsp_psram_write(&g_psram, psram_addr_log, (uint8_t *) recv_log, size);
            psram_addr_log += size;
            xQueueSend(psram_store_log_free_queue, &recv_log, 0);
        }
    }
}

void adc_flush_last_buffer(void) {
    BaseType_t hpw = pdFALSE;

    if (photo_buf_idx > 0) {
        photo_data_t *last_buf = photo_current_buf;
        if (xQueueSendFromISR(psram_store_photo_queue, &last_buf, &hpw) == pdTRUE) {
            if (xQueueReceiveFromISR(psram_store_photo_free_queue, &photo_current_buf, &hpw) != pdTRUE) {
                photo_current_buf = NULL;
            }
            photo_buf_idx = 0;
        }
    }

    if (laser_buf_idx > 0) {
        laser_data_t *last_l_buf = laser_current_buf;
        if (xQueueSendFromISR(psram_store_laser_queue, &last_l_buf, &hpw) == pdTRUE) {
            if (xQueueReceiveFromISR(psram_store_laser_free_queue, &laser_current_buf, &hpw) != pdTRUE) {
                laser_current_buf = NULL;
            }
            laser_buf_idx = 0;
        }
    }

    portYIELD_FROM_ISR(hpw);
}