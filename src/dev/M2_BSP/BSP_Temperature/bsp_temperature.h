/* 
 * File:   bsp_temperature.h
 * Author: HTSANG
 *
 * Created on June 21, 2026, 10:05 AM
 */

#ifndef BSP_TEMPERATURE_H
#define	BSP_TEMPERATURE_H

#include <stdio.h>
#include <stdint.h>

void bsp_temperature_prepare_to_config_profile(uint8_t profile_index);
void bsp_temperature_config_main_ntc(uint8_t main_ntc);
void bsp_temperature_config_sec_ntc(uint8_t sec_ntc);
void bsp_temperature_config_tec_mask(uint8_t tec_mask);
void bsp_temperature_config_heater_mask(uint8_t heater_mask);
void bsp_temperature_config_setpoint(int16_t setpoint);
void bsp_temperature_config_main_sec_delta(int16_t main_sec_delta);
void bsp_temperature_config_step(uint8_t step_id, uint16_t start_temp, uint16_t stop_temp, uint16_t duration, uint8_t mode);

uint8_t bsp_temperature_config_save(void);
void bsp_temperature_config_cancel(void);

#endif	/* BSP_TEMPERATURE_H */

