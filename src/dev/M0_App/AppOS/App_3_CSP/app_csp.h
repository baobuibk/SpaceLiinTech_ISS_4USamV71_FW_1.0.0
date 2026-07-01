/************************************************
 *  @file     : app_csp.h
 *  @date     : February 2026
 *  @author   : CAO HIEU
 *  @version  : 3.0.0
 ************************************************/

#ifndef APP_CSP_H
#define APP_CSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "os.h"
#include <stdbool.h>
#include <stdint.h>
#include <param/internal/types.h>

/* ---- CSP node configuration ---- */
#define CSP_ADDR_MASTER  8U
#define CSP_ADDR_EXP1    11U
#define CSP_ADDR_EXP2    12U
#define CSP_CAN_PROMISC  true

/* ---- Task ---- */
extern const osThreadAttr_t csp_attr;
void App_CSPTask(void *param);

/* ---- Parameter tables (index 4 = Telemetry, see exp_rparam_table.h) ---- */
extern gs_param_table_instance_t g_tables[5];

/* ---- CLI helper ---- */
bool CAN_RawSend(uint32_t canId, uint8_t *data, uint8_t dlc, bool extended);

#ifdef __cplusplus
}
#endif

#endif /* APP_CSP_H */