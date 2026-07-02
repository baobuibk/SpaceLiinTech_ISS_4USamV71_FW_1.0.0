#ifndef EXP_RPARAM_TABLE_H
#define EXP_RPARAM_TABLE_H

#include <param/types.h>
#include <param/table.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Table 0: Board Parameter
 * ============================================================================ */
#define PARAM_T0_UID                          0  // 0x00
#define PARAM_T0_TYPE                        16  // 0x10
#define PARAM_T0_REV                         24  // 0x18
#define PARAM_T0_CSP_ADDR                    27  // 0x1B
#define PARAM_T0_CSP_PORT                    28  // 0x1C
#define PARAM_T0_CSP_RTABLE                  44  // 0x2C
#define PARAM_T0_CAN_SPEED                  140  // 0x8C
#define PARAM_T0_REBOOT_IN                  142  // 0x8E
#define PARAM_T0_DEFAULT_IN                 144  // 0x90

extern const gs_param_table_row_t data_table0_rows[];
extern const size_t data_table0_row_count;

/* ============================================================================
 * Table 1: Shell-tunnel Parameter (reserved)
 *   cmd    : write command string to execute
 *   status : 0=idle 1=running 2=done 3=error 4=stopped
 *   retcode: last CLI retcode (always 0 on MCU)
 *   pages  : total output page count
 *   page   : write page index to load that page into 'out'
 *   outlen : total output byte count
 *   out    : current output page (128 B)
 * ============================================================================ */
#define PARAM_T1_EX                           0  // 0x00  string[64]
#define PARAM_T1_STATUS                      64  // 0x40  uint8
#define PARAM_T1_RETCODE                     65  // 0x41  int8
#define PARAM_T1_PAGES                       66  // 0x42  uint8
#define PARAM_T1_PAGE                        67  // 0x43  uint8
#define PARAM_T1_OUTLEN                      68  // 0x44  uint16
#define PARAM_T1_OUT                         70  // 0x46  string[128]

extern const gs_param_table_row_t data_table1_rows[];
extern const size_t data_table1_row_count;

/* ============================================================================
 * Table 4: Telemetry (sensor snapshot, updated periodically by App_CtrlTask)
 *   sensor_stat: bit0=BME688 bit1=LSM6DSOX bit2=PAC1934 (1=init failed, gave up)
 * ============================================================================ */
#define PARAM_T4_SYS_UPTIME                   0  // 0x00  uint32, seconds
#define PARAM_T4_SENSOR_STAT                  4  // 0x04  uint8
#define PARAM_T4_BME_TEMP                     5  // 0x05  int16,  x100 degC
#define PARAM_T4_BME_PRESS                    7  // 0x07  uint32, x10 Pa
#define PARAM_T4_BME_HUM                     11  // 0x0B  uint16, x100 %RH
#define PARAM_T4_LSM_ACCEL                   13  // 0x0D  int16[3], mg (X,Y,Z)
#define PARAM_T4_LSM_GYRO                    19  // 0x13  int16[3], x10 dps (X,Y,Z)
#define PARAM_T4_LSM_TEMP                    25  // 0x19  int16,  x100 degC
#define PARAM_T4_NTC_TEMP                    27  // 0x1B  int16[8], x100 degC
#define PARAM_T4_BOARD_TEMP                  43  // 0x2B  int16,  x100 degC
#define PARAM_T4_PWR_VOLT                    45  // 0x2D  uint16[4], mV
#define PARAM_T4_PWR_CURR                    53  // 0x35  uint16[4], mA

extern const gs_param_table_row_t data_table4_rows[];
extern const size_t data_table4_row_count;

/* ============================================================================
 * Initialization
 * ============================================================================ */

gs_error_t data_tables_init_all(gs_param_table_instance_t *tables);
void       data_tables_free_all(gs_param_table_instance_t *tables);

gs_error_t data_table0_init(gs_param_table_instance_t *table);
gs_error_t data_table1_init(gs_param_table_instance_t *table);
gs_error_t data_table4_init(gs_param_table_instance_t *table);

const char *data_table_get_name(uint8_t table_id);

#ifdef __cplusplus
}
#endif

#endif /* EXP_RPARAM_TABLE_H */
