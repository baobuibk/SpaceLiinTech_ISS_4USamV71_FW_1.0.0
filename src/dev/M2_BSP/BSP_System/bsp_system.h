/* 
 * File:   bsp_system.h
 * Author: HTSANG
 *
 * Created on June 4, 2026, 1:58 PM
 */

#ifndef BSP_SYSTEM_H
#define	BSP_SYSTEM_H

#include "BSP_Board/bsp_core.h"

void bsp_system_ifp_power_ena(void);
void bsp_system_ifp_power_dis(void);

void bsp_system_mpu_gpioA_set(void);
void bsp_system_mpu_gpioA_reset(void);
void bsp_system_mpu_gpioA_pulse(void);

void bsp_system_mpu_gpioB_set(void);
void bsp_system_mpu_gpioB_reset(void);
void bsp_system_mpu_gpioB_pulse(void);

#endif	/* BSP_SYSTEM_H */

