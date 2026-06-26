/************************************************
 *  @file     : sh_handler.h
 *  @brief    : Shell-tunnel via RPARAM Table 1 (MCU side)
 *
 *  Workflow:
 *    SET table1.ex = "help sensors"   → executes via embedded_cli
 *    GET table1.status               → poll until 2 (done) or 3 (error)
 *    GET table1.pages, outlen        → how many 128-byte pages total
 *    GET table1.out                  → page 0 pre-loaded automatically
 *    SET table1.page = N             → loads page N into table1.out
 *    GET table1.out                  → read page N
 *
 *    SET table1.ex = "stop"          → abort / reset to idle
 ************************************************/

#ifndef SH_HANDLER_H
#define SH_HANDLER_H

#include <param/internal/types.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------ Status values for PARAM_T1_STATUS ------ */
#define SH_STATUS_IDLE      0
#define SH_STATUS_RUNNING   1
#define SH_STATUS_DONE      2
#define SH_STATUS_ERROR     3
#define SH_STATUS_STOPPED   4

/* ------ Page size (bytes per page in PARAM_T1_OUT) ------ */
#define SH_PAGE_SIZE        128

/* ------ Max output buffered before truncation (bytes) ------ */
#define SH_MAX_OUTPUT       4096

/**
 * Link sh_handler to the Table 1 instance BEFORE starting the worker.
 * Call once, after data_table1_init(), before sh_handler_start().
 */
void sh_handler_set_table(gs_param_table_instance_t *table1);

/**
 * Create the FreeRTOS worker task and queue.
 * Call once, after sh_handler_set_table() and after the RTOS scheduler
 * has started (from inside a task context).
 */
void sh_handler_start(void);

/**
 * RPARAM callback for Table 1.
 * Assign to table1->callback after data_table1_init().
 *
 * Fires on SET of any Table 1 address:
 *   PARAM_T1_EX   → queue command for worker task
 *   PARAM_T1_PAGE → immediately copy requested page into PARAM_T1_OUT
 */
void sh_table1_callback(uint16_t addr, gs_param_table_instance_t *tinst);

#ifdef __cplusplus
}
#endif

#endif /* SH_HANDLER_H */
