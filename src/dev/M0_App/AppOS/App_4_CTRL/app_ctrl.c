/* ============================================================
 * app_ctrl.c  — Periodically samples sensors and updates
 *               rparam Table 4 (Telemetry), no CSP callback.
 * ============================================================ */
#include "app_ctrl.h"
#include <stdbool.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "M1_SysApp/dmesg/dmesg.h"

#include "M0_App/AppOS/App_3_CSP/app_csp.h"
#include "M0_App/AppOS/App_4_NTC/app_ntc.h"
#include "exp_rparam_table.h"

#include "BSP_BME688/bsp_bme688.h"
#include "BSP_LSM6DSOX/bsp_lsm6dsox.h"
#include "BSP_PAC1934/bsp_pac1934.h"

extern lsm6d_dev_t  lsm6dsox;
extern lsm6d_data_t lsm6d_all_data;

#define CTRL_TICK_MS        1000U
#define NTC_INTERVAL_S       3U
#define BME_INTERVAL_S       4U
#define LSM_INTERVAL_S       5U
#define PAC_INTERVAL_S      10U

#define SENSOR_STAT_BME_BIT  0x01U
#define SENSOR_STAT_LSM_BIT  0x02U
#define SENSOR_STAT_PAC_BIT  0x04U

const osThreadAttr_t ctrl_attr = {
    .name       = "Ctrl",
    .stack_size = configMINIMAL_STACK_SIZE * 4,
    .priority   = configMAX_PRIORITIES - 4
};

static uint8_t s_sensor_stat = 0;

static void CTRL_UpdateSensorStat(void)
{
    gs_param_set(&g_tables[4], PARAM_T4_SENSOR_STAT, GS_PARAM_UINT8, &s_sensor_stat, sizeof(s_sensor_stat), 0);
}

static void CTRL_UpdateBme688(void)
{
    static bool give_up = false;
    if (give_up) return;

    if (!bme688_int.init_status) {
        if (bsp_bme688_init(&bme688_int) != ERROR_OK) {
            give_up = true;
            s_sensor_stat |= SENSOR_STAT_BME_BIT;
            CTRL_UpdateSensorStat();
            Dmesg_Write("[CTRL] BME688 init failed, giving up");
            return;
        }
    }

    bme688_data_t data;
    if (bsp_bme688_read_all(&bme688_int, &data) != ERROR_OK) return;

    int16_t  temp_x100  = (int16_t) (data.temperature * 100.0f);
    uint32_t press_x10  = (uint32_t)(data.pressure * 10.0f);
    uint16_t hum_x100   = (uint16_t)(data.humidity * 100.0f);

    gs_param_set(&g_tables[4], PARAM_T4_BME_TEMP,  GS_PARAM_INT16,  &temp_x100, sizeof(temp_x100), 0);
    gs_param_set(&g_tables[4], PARAM_T4_BME_PRESS, GS_PARAM_UINT32, &press_x10, sizeof(press_x10), 0);
    gs_param_set(&g_tables[4], PARAM_T4_BME_HUM,   GS_PARAM_UINT16, &hum_x100,  sizeof(hum_x100),  0);
}

static void CTRL_UpdateLsm6dsox(void)
{
    static bool give_up = false;
    if (give_up) return;

    if (!lsm6dsox.init_status) {
        if (bsp_lsm6dsox_init() != ERROR_OK) {
            give_up = true;
            s_sensor_stat |= SENSOR_STAT_LSM_BIT;
            CTRL_UpdateSensorStat();
            Dmesg_Write("[CTRL] LSM6DSOX init failed, giving up");
            return;
        }
    }

    if (bsp_lsm6dsox_read_all() != ERROR_OK) return;

    int16_t accel[3] = {
        (int16_t) lsm6d_all_data.Accel.x,
        (int16_t) lsm6d_all_data.Accel.y,
        (int16_t) lsm6d_all_data.Accel.z,
    };
    int16_t gyro[3] = {
        (int16_t) (lsm6d_all_data.Gyro.x * 10.0f),
        (int16_t) (lsm6d_all_data.Gyro.y * 10.0f),
        (int16_t) (lsm6d_all_data.Gyro.z * 10.0f),
    };
    int16_t temp_x100 = (int16_t) (lsm6d_all_data.Temperature * 100.0f);

    gs_param_set(&g_tables[4], PARAM_T4_LSM_ACCEL, GS_PARAM_INT16, accel, sizeof(accel), 0);
    gs_param_set(&g_tables[4], PARAM_T4_LSM_GYRO,  GS_PARAM_INT16, gyro,  sizeof(gyro),  0);
    gs_param_set(&g_tables[4], PARAM_T4_LSM_TEMP,  GS_PARAM_INT16, &temp_x100, sizeof(temp_x100), 0);
}

static void CTRL_UpdateNtc(void)
{
    if (xSemaphoreTake(ntc_mutex, pdMS_TO_TICKS(50)) != pdTRUE) return;

    int16_t ntc_x100[8];
    for (int i = 0; i < 8; i++) {
        ntc_x100[i] = (int16_t) (db_temperature_read((e_db_temperature_t) i) * 100.0f);
    }
    int16_t board_x100 = (int16_t) (db_temperature_read(BOARD_TEMP) * 100.0f);

    xSemaphoreGive(ntc_mutex);

    gs_param_set(&g_tables[4], PARAM_T4_NTC_TEMP,   GS_PARAM_INT16, ntc_x100, sizeof(ntc_x100), 0);
    gs_param_set(&g_tables[4], PARAM_T4_BOARD_TEMP, GS_PARAM_INT16, &board_x100, sizeof(board_x100), 0);
}

static void CTRL_UpdatePac1934(void)
{
    static bool give_up = false;
    if (give_up) return;

    if (!pac1934_hi.init_status) {
        if (bsp_pac1934_init(&pac1934_hi) != ERROR_OK) {
            give_up = true;
            s_sensor_stat |= SENSOR_STAT_PAC_BIT;
            CTRL_UpdateSensorStat();
            Dmesg_Write("[CTRL] PAC1934 init failed, giving up");
            return;
        }
    }

    pac1934_data_t data;
    if (bsp_pac1934_read_all(&pac1934_hi, &data) != ERROR_OK) return;

    uint16_t volt_mv[PAC1934_CHANNELS];
    uint16_t curr_ma[PAC1934_CHANNELS];
    for (uint8_t i = 0; i < PAC1934_CHANNELS; i++) {
        volt_mv[i] = (uint16_t) (data.vbus_v[i] * 1000.0f);
        curr_ma[i] = (uint16_t) (data.current_a[i] * 1000.0f);
    }

    gs_param_set(&g_tables[4], PARAM_T4_PWR_VOLT, GS_PARAM_UINT16, volt_mv, sizeof(volt_mv), 0);
    gs_param_set(&g_tables[4], PARAM_T4_PWR_CURR, GS_PARAM_UINT16, curr_ma, sizeof(curr_ma), 0);
}

void App_CtrlTask(void *param)
{
    (void)param;
    Dmesg_Write("[CTRL] telemetry task started");

    uint32_t uptime_s = 0;

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(CTRL_TICK_MS));
        uptime_s++;

        gs_param_set(&g_tables[4], PARAM_T4_SYS_UPTIME, GS_PARAM_UINT32, &uptime_s, sizeof(uptime_s), 0);

        if ((uptime_s % NTC_INTERVAL_S) == 0) CTRL_UpdateNtc();
        if ((uptime_s % BME_INTERVAL_S) == 0) CTRL_UpdateBme688();
        if ((uptime_s % LSM_INTERVAL_S) == 0) CTRL_UpdateLsm6dsox();
        if ((uptime_s % PAC_INTERVAL_S) == 0) CTRL_UpdatePac1934();
    }
}
