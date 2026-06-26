/*
 * cli_command.h
 *
 *  Created on: Feb 27, 2025
 *      Author:
 */

#ifndef M2_SYSTEM_CLI_TERMINAL_CLI_COMMAND_CLI_COMMAND_H_
#define M2_SYSTEM_CLI_TERMINAL_CLI_COMMAND_CLI_COMMAND_H_

#include "../CLI_Src/embedded_cli.h"

const CliCommandBinding *getCliStaticBindings(void);
uint16_t getCliStaticBindingCount(void);

void CMD_Photo_Turn_On_Channel(EmbeddedCli *cli, char *args, void *context);

void CMD_BootInfo_Read(EmbeddedCli *cli, char *args, void *context);
void CMD_FRAM_WriteRead(EmbeddedCli *cli, char *args, void *context);

void CMD_PSRAM_Switch_MCU(EmbeddedCli *cli, char *args, void *context);
void CMD_PSRAM_Switch_MPU(EmbeddedCli *cli, char *args, void *context);
void CMD_PSRAM_Read_ID(EmbeddedCli *cli, char *args, void *context);
void CMD_PSRAM_Write_Byte(EmbeddedCli *cli, char *args, void *context);
void CMD_PSRAM_Read_Byte(EmbeddedCli *cli, char *args, void *context);

void CMD_Flow_Sen_ReadAll(EmbeddedCli *cli, char *args, void *context);

void CMD_NTC_Read(EmbeddedCli *cli, char *args, void *context);
void CMD_NTC_Read_All(EmbeddedCli *cli, char *args, void *context);
void CMD_Check_NTC(EmbeddedCli *cli, char *args, void *context);

void CMD_Temp_wizard_set(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_profile_config(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_profile_config_main_ntc(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_profile_config_sec_ntc(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_profile_config_tec(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_profile_config_heater(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_profile_config_setpoint(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_profile_config_main_sec_delta(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_profile_config_step(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_profile_config_save(EmbeddedCli *cli, char *args, void *context);

void CMD_Temp_profile_config_cancel(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_auto_ena(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_auto_start(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_manu(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_log_toggle(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_PID_set(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_PID_get(EmbeddedCli *cli, char *args, void *context);

//void CMD_PowerSOM_ON(EmbeddedCli *cli, char *args, void *context);
//void CMD_PowerSOM_OFF(EmbeddedCli *cli, char *args, void *context);
//void CMD_PowerSOM_Get(EmbeddedCli *cli, char *args, void *context);
//void CMD_PowerBuckPeri_ON(EmbeddedCli *cli, char *args, void *context);
//void CMD_PowerBuckPeri_OFF(EmbeddedCli *cli, char *args, void *context);
//void CMD_PowerBuckPeri_Get(EmbeddedCli *cli, char *args, void *context);
void CMD_PowerTec_ON(EmbeddedCli *cli, char *args, void *context);
void CMD_PowerTec_OFF(EmbeddedCli *cli, char *args, void *context);
void CMD_PowerTec_Get(EmbeddedCli *cli, char *args, void *context);
void CMD_PowerSLN_HD_ON(EmbeddedCli *cli, char *args, void *context);
void CMD_PowerSLN_HD_OFF(EmbeddedCli *cli, char *args, void *context);
void CMD_PowerSLN_HD_Get(EmbeddedCli *cli, char *args, void *context);
//void CMD_PowerSolenoid_ON(EmbeddedCli *cli, char *args, void *context);
//void CMD_PowerSolenoid_OFF(EmbeddedCli *cli, char *args, void *context);
//void CMD_PowerSolenoid_Get(EmbeddedCli *cli, char *args, void *context);
void CMD_PowerLP_Heater_ON(EmbeddedCli *cli, char *args, void *context);
void CMD_PowerLP_Heater_OFF(EmbeddedCli *cli, char *args, void *context);
void CMD_PowerLP_Heater_Get(EmbeddedCli *cli, char *args, void *context);
//void CMD_PowerHeater_ON(EmbeddedCli *cli, char *args, void *context);
//void CMD_PowerHeater_OFF(EmbeddedCli *cli, char *args, void *context);
//void CMD_PowerHeater_Get(EmbeddedCli *cli, char *args, void *context);
void CMD_PowerAll_ON(EmbeddedCli *cli, char *args, void *context);
void CMD_PowerAll_OFF(EmbeddedCli *cli, char *args, void *context);
void CMD_PowerAll_Get(EmbeddedCli *cli, char *args, void *context);
//void CMD_PowerAI_ON(EmbeddedCli *cli, char *args, void *context);
//void CMD_PowerAI_OFF(EmbeddedCli *cli, char *args, void *context);
//void CMD_PowerAI_Get(EmbeddedCli *cli, char *args, void *context);

void CMD_TEC_Init(EmbeddedCli *cli, char *args, void *context);
void CMD_TEC_Set_Vol(EmbeddedCli *cli, char *args, void *context);
void CMD_TEC_En(EmbeddedCli *cli, char *args, void *context);
void CMD_TEC_SWen(EmbeddedCli *cli, char *args, void *context);
void CMD_TEC_Read_Reg(EmbeddedCli *cli, char *args, void *context);

void CMD_SOL_Single_On(EmbeddedCli *cli, char *args, void *context);
void CMD_SOL_Single_Off(EmbeddedCli *cli, char *args, void *context);
void CMD_SOL_Single_Get(EmbeddedCli *cli, char *args, void *context);

void CMD_SOL_Dual_Forward (EmbeddedCli *cli, char *args, void *context);
void CMD_SOL_Dual_Reverse(EmbeddedCli *cli, char *args, void *context);

void CMD_SOL_MAX_Init(EmbeddedCli *cli, char *args, void *context);
void CMD_SOL_MAX_Control(EmbeddedCli *cli, char *args, void *context);

void CMD_PAC1934_Read(EmbeddedCli *cli, char *args, void *context);

#endif /* M2_SYSTEM_CLI_TERMINAL_CLI_COMMAND_CLI_COMMAND_H_ */
