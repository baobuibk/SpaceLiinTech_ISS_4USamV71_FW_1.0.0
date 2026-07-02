#include "exp_rparam_table.h"
#include "param/internal/types.h"
#include <string.h>

/* ============================================================================
 * Table 0: Board Parameter
 * ============================================================================ */

typedef struct __attribute__((packed)) {
    char     uid[16];       // 0
    char     type[8];       // 16
    uint8_t  rev[3];        // 24
    uint8_t  csp_addr;      // 27
    char     csp_port[16];  // 28
    char     csp_rtable[96];// 44
    uint16_t can_speed;     // 140
    uint16_t reboot_in;     // 142
    uint16_t default_in;    // 144
} table_data_t0;

static table_data_t0 g_data_t0 = {
    .uid       = "CM-EXP",
    .type      = "C",
    .rev       = {1, 1, 0},
    .csp_addr  = 12,
    .csp_port  = "7,8,12",
    .csp_rtable= "12/5 LOOP, 0/0 CAN",
    .can_speed  = 1000,
    .reboot_in  = 0,
    .default_in = 0,
};

const gs_param_table_row_t data_table0_rows[] = {
    {.addr = PARAM_T0_UID,        .type = GS_PARAM_STRING, .size = 16,              .array_size = 1, .flags = 0, .name = "uid"},
    {.addr = PARAM_T0_TYPE,       .type = GS_PARAM_STRING, .size = 8,               .array_size = 1, .flags = 0, .name = "type"},
    {.addr = PARAM_T0_REV,        .type = GS_PARAM_UINT8,  .size = sizeof(uint8_t), .array_size = 3, .flags = 0, .name = "rev"},
    {.addr = PARAM_T0_CSP_ADDR,   .type = GS_PARAM_UINT8,  .size = sizeof(uint8_t), .array_size = 1, .flags = 0, .name = "csp_addr"},
    {.addr = PARAM_T0_CSP_PORT,   .type = GS_PARAM_STRING, .size = 16,              .array_size = 1, .flags = 0, .name = "csp_port"},
    {.addr = PARAM_T0_CSP_RTABLE, .type = GS_PARAM_STRING, .size = 96,              .array_size = 1, .flags = 0, .name = "csp_rtable"},
    {.addr = PARAM_T0_CAN_SPEED,  .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),.array_size = 1, .flags = 0, .name = "can_speed"},
    {.addr = PARAM_T0_REBOOT_IN,  .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),.array_size = 1, .flags = 0, .name = "reboot_in"},
    {.addr = PARAM_T0_DEFAULT_IN, .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),.array_size = 1, .flags = 0, .name = "default_in"},
};

const size_t data_table0_row_count = sizeof(data_table0_rows) / sizeof(data_table0_rows[0]);

/* ============================================================================
 * Table 1: Shell-tunnel Parameter
 * ============================================================================ */

typedef struct __attribute__((packed)) {
    char     ex[64];    // 0x00  PARAM_T1_EX
    uint8_t  status;    // 0x40  PARAM_T1_STATUS
    int8_t   retcode;   // 0x41  PARAM_T1_RETCODE
    uint8_t  pages;     // 0x42  PARAM_T1_PAGES
    uint8_t  page;      // 0x43  PARAM_T1_PAGE
    uint16_t outlen;    // 0x44  PARAM_T1_OUTLEN
    char     out[128];  // 0x46  PARAM_T1_OUT
} table_data_t1;

static table_data_t1 g_data_t1 = {
    .ex      = "",
    .status  = 0,
    .retcode = 0,
    .pages   = 0,
    .page    = 0,
    .outlen  = 0,
    .out     = "",
};

const gs_param_table_row_t data_table1_rows[] = {
    {.addr = PARAM_T1_EX,      .type = GS_PARAM_STRING, .size = 64,              .array_size = 1, .flags = 0, .name = "ex"},
    {.addr = PARAM_T1_STATUS,  .type = GS_PARAM_UINT8,  .size = sizeof(uint8_t), .array_size = 1, .flags = 0, .name = "status"},
    {.addr = PARAM_T1_RETCODE, .type = GS_PARAM_INT8,   .size = sizeof(int8_t),  .array_size = 1, .flags = 0, .name = "retcode"},
    {.addr = PARAM_T1_PAGES,   .type = GS_PARAM_UINT8,  .size = sizeof(uint8_t), .array_size = 1, .flags = 0, .name = "pages"},
    {.addr = PARAM_T1_PAGE,    .type = GS_PARAM_UINT8,  .size = sizeof(uint8_t), .array_size = 1, .flags = 0, .name = "page"},
    {.addr = PARAM_T1_OUTLEN,  .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),.array_size = 1, .flags = 0, .name = "outlen"},
    {.addr = PARAM_T1_OUT,     .type = GS_PARAM_STRING, .size = 128,             .array_size = 1, .flags = 0, .name = "out"},
};

const size_t data_table1_row_count = sizeof(data_table1_rows) / sizeof(data_table1_rows[0]);

/* ============================================================================
 * Table 4: Telemetry
 * ============================================================================ */

typedef struct __attribute__((packed)) {
    uint32_t sys_uptime;      // 0
    uint8_t  sensor_stat;     // 4
    int16_t  bme_temp;        // 5
    uint32_t bme_press;       // 7
    uint16_t bme_hum;         // 11
    int16_t  lsm_accel[3];    // 13
    int16_t  lsm_gyro[3];     // 19
    int16_t  lsm_temp;        // 25
    int16_t  ntc_temp[8];     // 27
    int16_t  board_temp;      // 43
    uint16_t pwr_volt[4];     // 45
    uint16_t pwr_curr[4];     // 53
} table_data_t4;

static table_data_t4 g_data_t4 = {0};

const gs_param_table_row_t data_table4_rows[] = {
    {.addr = PARAM_T4_SYS_UPTIME, .type = GS_PARAM_UINT32, .size = sizeof(uint32_t), .array_size = 1, .flags = 0, .name = "sys_uptime"},
    {.addr = PARAM_T4_SENSOR_STAT,.type = GS_PARAM_UINT8,  .size = sizeof(uint8_t),  .array_size = 1, .flags = 0, .name = "sensor_stat"},
    {.addr = PARAM_T4_BME_TEMP,   .type = GS_PARAM_INT16,  .size = sizeof(int16_t),  .array_size = 1, .flags = 0, .name = "bme_temp"},
    {.addr = PARAM_T4_BME_PRESS,  .type = GS_PARAM_UINT32, .size = sizeof(uint32_t), .array_size = 1, .flags = 0, .name = "bme_press"},
    {.addr = PARAM_T4_BME_HUM,    .type = GS_PARAM_UINT16, .size = sizeof(uint16_t), .array_size = 1, .flags = 0, .name = "bme_hum"},
    {.addr = PARAM_T4_LSM_ACCEL,  .type = GS_PARAM_INT16,  .size = sizeof(int16_t),  .array_size = 3, .flags = 0, .name = "lsm_accel"},
    {.addr = PARAM_T4_LSM_GYRO,   .type = GS_PARAM_INT16,  .size = sizeof(int16_t),  .array_size = 3, .flags = 0, .name = "lsm_gyro"},
    {.addr = PARAM_T4_LSM_TEMP,   .type = GS_PARAM_INT16,  .size = sizeof(int16_t),  .array_size = 1, .flags = 0, .name = "lsm_temp"},
    {.addr = PARAM_T4_NTC_TEMP,   .type = GS_PARAM_INT16,  .size = sizeof(int16_t),  .array_size = 8, .flags = 0, .name = "ntc_temp"},
    {.addr = PARAM_T4_BOARD_TEMP, .type = GS_PARAM_INT16,  .size = sizeof(int16_t),  .array_size = 1, .flags = 0, .name = "board_temp"},
    {.addr = PARAM_T4_PWR_VOLT,   .type = GS_PARAM_UINT16, .size = sizeof(uint16_t), .array_size = 4, .flags = 0, .name = "pwr_volt"},
    {.addr = PARAM_T4_PWR_CURR,   .type = GS_PARAM_UINT16, .size = sizeof(uint16_t), .array_size = 4, .flags = 0, .name = "pwr_curr"},
};

const size_t data_table4_row_count = sizeof(data_table4_rows) / sizeof(data_table4_rows[0]);

/* ============================================================================
 * Init / free
 * ============================================================================ */

gs_error_t data_table0_init(gs_param_table_instance_t *table)
{
    if (!table) return GS_ERROR_ARG;

    memset(table, 0, sizeof(*table));
    table->rows        = data_table0_rows;
    table->row_count   = data_table0_row_count;
    table->memory      = &g_data_t0;
    table->memory_size = sizeof(g_data_t0);
    table->flags       = 0;

    gs_param_table_checksum_le(table);

    printf("[TABLE 0] Board Parameter: %u rows, %u bytes, checksum BE=0x%04X\n",
           table->row_count, table->memory_size, table->checksum_be);

    return GS_OK;
}

gs_error_t data_table1_init(gs_param_table_instance_t *table)
{
    if (!table) return GS_ERROR_ARG;

    memset(table, 0, sizeof(*table));
    table->rows        = data_table1_rows;
    table->row_count   = data_table1_row_count;
    table->memory      = &g_data_t1;
    table->memory_size = sizeof(g_data_t1);
    table->flags       = 0;

    gs_param_table_checksum_le(table);

    printf("[TABLE 1] Shell-tunnel: %u rows, %u bytes, checksum BE=0x%04X\n",
           table->row_count, table->memory_size, table->checksum_be);

    return GS_OK;
}

gs_error_t data_table4_init(gs_param_table_instance_t *table)
{
    if (!table) return GS_ERROR_ARG;

    memset(table, 0, sizeof(*table));
    table->rows        = data_table4_rows;
    table->row_count   = data_table4_row_count;
    table->memory      = &g_data_t4;
    table->memory_size = sizeof(g_data_t4);
    table->flags       = 0;

    gs_param_table_checksum_le(table);

    printf("[TABLE 4] Telemetry: %u rows, %u bytes, checksum BE=0x%04X\n",
           table->row_count, table->memory_size, table->checksum_be);

    return GS_OK;
}

gs_error_t data_tables_init_all(gs_param_table_instance_t *tables)
{
    if (!tables) return GS_ERROR_ARG;

    gs_error_t err;

    err = data_table0_init(&tables[0]);
    if (err != GS_OK) return err;

    err = data_table1_init(&tables[1]);
    if (err != GS_OK) return err;

    err = data_table4_init(&tables[4]);
    if (err != GS_OK) return err;

    printf("All 3 tables initialized successfully\n");
    return GS_OK;
}

void data_tables_free_all(gs_param_table_instance_t *tables)
{
    if (tables) {
        gs_param_table_free(&tables[0]);
        gs_param_table_free(&tables[1]);
        gs_param_table_free(&tables[4]);
    }
}

const char *data_table_get_name(uint8_t table_id)
{
    switch (table_id) {
        case 0:  return "Board Parameter";
        case 1:  return "Shell-tunnel";
        case 4:  return "Telemetry";
        default: return "Unknown";
    }
}
