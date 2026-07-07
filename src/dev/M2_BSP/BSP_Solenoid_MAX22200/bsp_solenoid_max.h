/* 
 * File:   bsp_solenoid_max.h
 * Author: HTSANG
 *
 * Created on June 14, 2026, 6:06 PM
 */

#ifndef BSP_SOLENOID_MAX_H
#define	BSP_SOLENOID_MAX_H
#include <stdint.h>
#include <stdbool.h>
#include "M3_Driver/devices/MAX22200/max22200.h"

#define MAX22200_NUM_DEVICES        2u
#define MAX22200_PAIRS_PER_DEVICE   4u   /* 8 ch / 2 = 4 c?p half-bridge */
#define MAX22200_TOTAL_PAIRS        (MAX22200_NUM_DEVICES * MAX22200_PAIRS_PER_DEVICE)

extern max22200_dev_t *g_max22200_dev[MAX22200_NUM_DEVICES];

typedef enum {
    SOL_DIR_OFF     = 0,   /* c? hai OFF                          */
    SOL_DIR_FORWARD = 1,   /* high_ch ON  / low_ch OFF  ? A?B    */
    SOL_DIR_REVERSE = 2,   /* high_ch OFF / low_ch ON   ? B?A    */
} sol_direction_t;

int  bsp_solenoid_max_init(void);
int  bsp_solenoid_set(uint8_t pair, sol_direction_t dir);

#endif	/* BSP_SOLENOID_MAX_H */
