#ifndef BSP_BSP_POWER_H_
#define BSP_BSP_POWER_H_

#include "stdbool.h"

//void bsp_power_som_on(void);
//void bsp_power_som_off(void);
//bool bsp_power_som_status(void);
//
//void bsp_power_buck_peri_on(void);
//void bsp_power_buck_peri_off(void);
//bool bsp_power_buck_peri_status(void);

void bsp_power_tec_on(void);
void bsp_power_tec_off(void);
bool bsp_power_tec_status(void);

void bsp_power_sln_hd_on(void);
void bsp_power_sln_hd_off(void);
bool bsp_power_sln_hd_status(void);

//void bsp_power_solenoid_on(void);
//void bsp_power_solenoid_off(void);
//bool bsp_power_solenoid_status(void);

void bsp_power_lp_heater_on(void);
void bsp_power_lp_heater_off(void);
bool bsp_power_lp_heater_status(void);

//void bsp_power_heater_on(void);
//void bsp_power_heater_off(void);
//bool bsp_power_heater_status(void);

void power_on_all(void);
void power_off_all(void);
void power_status_all(void);

//void bsp_power_AI_on(void);
//void bsp_power_AI_off(void);
//bool bsp_power_AI_status(void);

#endif /* BSP_BSP_POWER_H_ */

