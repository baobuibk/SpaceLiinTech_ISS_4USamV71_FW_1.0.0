/*
 * cli_setup.c
 *
 *  Created on: December, 2025
 *      Author: CAO HIEU
 */
#include "cli_command.h"
/*************************************************
 *              More User Include                *
 *************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "cli_smoke_test.h"
#include "cli_expander_test.h"
#include "cli_laser.h"
#include "cli_experiment.h"
#include "cli_pump.h"

#include <param/table.h>
#include <param/internal/types.h>
#include <csp/csp.h>
#include "exp_rparam_table.h"

#include "M0_App/OS/os.h"
#include "M0_App/AppOS/App_3_CSP/app_csp.h"

#include "M1_SysApp/dmesg/dmesg.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Command/cli_switch_to_adc.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Auth/simple_shield.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Setup/cli_setup.h"

#include "BSP_Board/board.h"
#include "BSP_Board/bsp_core.h"
#include "UART/uart_irq.h"
#include "BSP_Power/bsp_power.h"
#include "BSP_Led/bsp_led.h"
#include "BSP_Heater/bsp_heater.h"
#include "BSP_TEC/bsp_tec.h"
#include "BSP_BMP390/bsp_bmp390.h"
#include "BSP_PAC1934/bsp_pac1934.h"
#include "BSP_Laser_Int/bsp_laser_int.h"
#include "BSP_Laser_Ext/bsp_laser_ext.h"
#include "BSP_FRAM/bsp_fram.h"
#include "BSP_BME688/bsp_bme688.h"
#include "BSP_LSM6DSOX/bsp_lsm6dsox.h"
#include "BSP_Solenoid_MAX22200/bsp_solenoid_max.h"
#include "BSP_System/bsp_system.h"

#include "cli_rtc.h"
#include "cli_lsm6dsox.h"
#include "cli_bme688.h"

#include "M3_Devices/FRAM_USART-SPI/fram_usart.h"

#include "M5_Utils/DateTime/date_time.h"
#include "cli_temperature.h"

#include "config/default/peripheral/tc/plib_tc3.h"

/*************************************************
 *                     Variable                    *
 *************************************************/
static char cli_buf[128];
/*************************************************
 *                Command Define                 *
 *************************************************/
static void CMD_ClearCLI(EmbeddedCli *cli, char *args, void *context);
static void CMD_RtcSet(EmbeddedCli *cli, char *args, void *context);
static void CMD_RtcGet(EmbeddedCli *cli, char *args, void *context);
static void CMD_RtcSetEpoch(EmbeddedCli *cli, char *args, void *context);
static void CMD_AutoComplete(EmbeddedCli *cli, char *args, void *context);

static void CMD_OsStat(EmbeddedCli *cli, char *args, void *context);
static void CMD_Dmesg(EmbeddedCli *cli, char *args, void *context);
static void CMD_UART_BaudToggle(EmbeddedCli *cli, char *args, void *context);

static void CMD_Reset(EmbeddedCli *cli, char *args, void *context);

static void CMD_CanSend(EmbeddedCli *cli, char *args, void *context);
static void CMD_CspPing(EmbeddedCli *cli, char *args, void *context);
static void CMD_CspSend(EmbeddedCli *cli, char *args, void *context);
static void CMD_CspInfo(EmbeddedCli *cli, char *args, void *context);

static void CMD_ParamShow(EmbeddedCli *cli, char *args, void *context);
static void CMD_ParamSet(EmbeddedCli *cli, char *args, void *context);

static void CMD_LED_Set(EmbeddedCli *cli, char *args, void *context);
static void CMD_LED_Reset(EmbeddedCli *cli, char *args, void *context);
static void CMD_HEATER_SetDuty(EmbeddedCli *cli, char *args, void *context);

static void CMD_State_monitor(EmbeddedCli *cli, char *args, void *context);

static void CMD_a(EmbeddedCli *cli, char *args, void *context) {
    return;
}

static void CMD_b(EmbeddedCli *cli, char *args, void *context) {
    return;
}

static void CMD_d(EmbeddedCli *cli, char *args, void *context) {
    return;
}

/*************************************************
 *                 Command  Array                *
 *************************************************/
// Guide: Command bindings are declared in the following order:
// { category, name, help, tokenizeArgs, context, binding }
// - category: Command group; set to NULL if grouping is not needed.
// - name: Command name (required)
// - help: Help string describing the command (required)
// - tokenizeArgs: Set to true to automatically split arguments when the command is called.
// - context: Pointer to a command-specific context; can be NULL.
// - binding: Callback function that handles the command.

static const CliCommandBinding cliStaticBindings_internal[] = {
    { "Ultis", "dev", "Print list of developer-specific commands", false, NULL, CMD_Dev,},
    { "Ultis", "help", "Print list of all available commands [Firmware: 1]", false, NULL, CMD_Help,},
    { "Ultis", "cls", "Clear the terminal console output", false, NULL, CMD_ClearCLI,},
    { "Ultis", "autocomplete", "Toggle autocompletion: autocomplete [on|off]", true, NULL, CMD_AutoComplete,},

    { "Time", "rtc_set", "Set RTC time: rtc_set <h> <m> <s> <DD> <MM> <YY>", true, NULL, CMD_RtcSet,},
    { "Time", "rtc_get", "Get RTC time: rtc_get <hard|soft|work|epoch|all>", true, NULL, CMD_RtcGet,},
    { "Time", "epoch_set", "Set RTC time using Unix epoch: rtc_setepoch <epoch>", true, NULL, CMD_RtcSetEpoch,},

    { "System", "os_stat", "Display FreeRTOS task statistics and stack usage", false, NULL, CMD_OsStat,},
    { "System", "dmesg", "Print system log buffer: dmesg [N lines]", true, NULL, CMD_Dmesg,},
    { "System", "baud", "Toggle UART0: 250k>500k>921k>1M>115k->...", false, NULL, CMD_UART_BaudToggle},

    { "FRAM", "fram_write_read", "Run FRAM hardware integrity test (Write/Read)", false, NULL, CMD_FRAM_WriteRead,},
    { "FRAM", "bootinfo", "Read boot info (count + cause history) from FRAM", false, NULL, CMD_BootInfo_Read,},

    { "CAN", "can_send", "Send raw CAN frame: can_send <id> [ext] [b0 b1...]", true, NULL, CMD_CanSend,},
    { "CSP", "csp_ping", "Send CSP ping: csp_ping <dest_addr> [timeout_ms]", true, NULL, CMD_CspPing,},
    { "CSP", "csp_send", "Send CSP packet: csp_send <addr> <port> <data...>", true, NULL, CMD_CspSend,},
    { "CSP", "csp_info", "Display current CSP interface status and statistics", false, NULL, CMD_CspInfo,},

    { "Param", "param_show", "Show all fields in a parameter table: param_show <table_id 0-4>", true, NULL, CMD_ParamShow},
    { "Param", "param_set", "Update a parameter field: param_set <table_id> <addr> <type> <value>", true, NULL, CMD_ParamSet},

    { "LED", "led_set", "Turn on a specific LED indicator", true, NULL, CMD_LED_Set},
    { "LED", "led_reset", "Turn off a specific LED indicator", true, NULL, CMD_LED_Reset},

    { "SOL", "sln_hb_init", "Initialize MAX22200 Solenoid Driver", false, NULL, CMD_SOL_MAX_Init},
    { "SOL", "sln_ctl", "Check MAX22200 Solenoid Faults", true, NULL, CMD_SOL_MAX_Control},

    { "HTR", "heater_set", "Set heater duty cycle: heater_set <channel> <duty>", true, NULL, CMD_HEATER_SetDuty},

    { "POWER", "power_all_on", "Enable all power rails simultaneously", false, NULL, CMD_PowerAll_ON},
    { "POWER", "power_all_off", "Disable all power rails simultaneously", false, NULL, CMD_PowerAll_OFF},
    { "POWER", "power_all_get", "Get status of all power rails", false, NULL, CMD_PowerAll_Get},
    { "POWER", "power_tec_on", "Enable power to the TEC module", false, NULL, CMD_PowerTec_ON},
    { "POWER", "power_tec_off", "Disable power to the TEC module", false, NULL, CMD_PowerTec_OFF},
    { "POWER", "power_tec_get", "Get TEC module power status", false, NULL, CMD_PowerTec_Get},
    { "POWER", "power_sln_hd_on", "Enable power to HD4 interface", false, NULL, CMD_PowerSLN_HD_ON},
    { "POWER", "power_sln_hd_off", "Disable power to HD4 interface", false, NULL, CMD_PowerSLN_HD_OFF},
    { "POWER", "power_sln_hd_get", "Get HD4 interface power status", false, NULL, CMD_PowerSLN_HD_Get},
    { "POWER", "power_lp_heater_on", "Enable power to the LP rail", false, NULL, CMD_PowerLP_Heater_ON},
    { "POWER", "power_lp_heater_off", "Disable power to the LP rail", false, NULL, CMD_PowerLP_Heater_OFF},
    { "POWER", "power_lp_heater_get", "Get LP rail power status", false, NULL, CMD_PowerLP_Heater_Get},
    
    { "BME688", "bme688_init", "Init BME688 sensor", false, NULL, CMD_BME688_Init},
    { "BME688", "bme688_read_temp", "Read temp from BME688 sensor", false, NULL, CMD_BME688_Read_Temperature},
    { "BME688", "bme688_read_pressure", "Read pressure from BME688 sensor", false, NULL, CMD_BME688_Read_Pressure},
    { "BME688", "bme688_read_humidity", "Read humidity from  BME688 sensor", false, NULL, CMD_BME688_Read_Humidity},
    { "BME688", "bme688_read_all", "Read pressure/temp/humidity from BME688 sensor", false, NULL, CMD_BME688_Read_All},

    { "PAC1934", "pac1934_read", "Read voltage and current from all PAC1934 channels", false, NULL, CMD_PAC1934_Read},

    { "TEST", "echo", "Echo input text back to console: echo <text>", true, NULL, CMD_CLI_Echo},

    { "TEC", "tec_init", "Initialize TEC controller configurations", true, NULL, CMD_TEC_Init},
    { "TEC", "tec_vol", "Set target voltage for TEC module", true, NULL, CMD_TEC_Set_Vol},
    { "TEC", "tec_en", "Enable TEC hardware output", true, NULL, CMD_TEC_En},
    { "TEC", "tec_swen", "Toggle software enable for TEC control", true, NULL, CMD_TEC_SWen},
    { "TEC", "tec_read_reg", "Read specific register from TEC controller", true, NULL, CMD_TEC_Read_Reg},

    { "LASER", "laser_int_set_dac", "Set DAC value for internal laser intensity", true, NULL, CMD_Laser_Int_Set_DAC},
    { "LASER", "laser_int_sw_on", "Turn ON specific internal laser channel", true, NULL, CMD_Laser_Int_Turn_On_Channel},
    { "LASER", "laser_int_sw_off", "Turn OFF specific internal laser channel", true, NULL, CMD_Laser_Int_Turn_Off_Channel},
    { "LASER", "laser_int_sw_off_all", "___", true, NULL, CMD_Laser_Int_Turn_Off_All},
    { "LASER", "laser_int_current", "Read current monitor value for internal laser", true, NULL, CMD_Laser_Int_Current_Monitor},
    
    { "LASER", "laser_ext_set_dac", "Set DAC value for external laser intensity", true, NULL, CMD_Laser_Ext_Set_DAC},
    { "LASER", "laser_ext_sw_on", "Turn ON specific external laser channel", true, NULL, CMD_Laser_Ext_Turn_On_Channel},
    { "LASER", "laser_ext_sw_off", "Turn OFF specific external laser channel", true, NULL, CMD_Laser_Ext_Turn_Off_All},
    { "LASER", "laser_ext_current", "Get current external laser", true, NULL, CMD_Laser_Ext_Get_Current},

    { "PHOTO", "photo_sw_on", "___", true, NULL, CMD_Photo_Turn_On_Channel},

    { "NTC", "ntc_read", "Read temperature from specific NTC: ntc_read <index>", true, NULL, CMD_NTC_Read},
    { "NTC", "ntc_read_all", "Read temperatures from all NTC sensors", true, NULL, CMD_NTC_Read_All},
    { "NTC", "ntc_check", "Perform NTC sensor functionality test", true, NULL, CMD_Check_NTC},

    { "RTC", "rtc_read", "Read current time and date from RTC hardware", true, NULL, CMD_RTC_Read},
    { "RTC", "rtc_set_date", "Set RTC date: rtc_set_date <DD> <MM> <YY> <WD>", true, NULL, CMD_RTC_Set_Date},
    { "RTC", "rtc_set_time", "Set RTC time: rtc_set_time <h> <m> <s>", true, NULL, CMD_RTC_Set_Time},
    { "RTC", "rtc_ping", "Ping RTC hardware via I2C", true, NULL, CMD_RTC_Ping},
    { "RTC", "rtc_test", "Run comprehensive RTC hardware test suite", true, NULL, CMD_RTC_Test},
    
    { "FLOW", "flow_init", "Read data from all flow sensors", true, NULL, CMD_Flow_Sen_Init},
    { "FLOW", "flow_read_all", "Read data from all flow sensors", true, NULL, CMD_Flow_Sen_ReadAll},

    { "EXP", "dls_setup", "Configure experiment profile parameters", true, NULL, CMD_DLS_setup},
    { "EXP", "dls_start", "Start the scheduled experiment sequence", true, NULL, CMD_DLS_start},

    { "PUMP", "pump_enable", "Enable pump hardware", true, NULL, CMD_PUMP_Enable},
    { "PUMP", "pump_disable", "Disable pump hardware", true, NULL, CMD_PUMP_Disable},
    { "PUMP", "pump_init", "Init pump hardware", true, NULL, CMD_PUMP_Init},
    { "PUMP", "pump_set_volt", "Set operating voltage for the pump", true, NULL, CMD_PUMP_Set_Volt},
    { "PUMP", "pump_set_freq", "Set operating frequency for the pump", true, NULL, CMD_PUMP_Set_Freq},
    { "PUMP", "pump_status", "Read status for the pump", true, NULL, CMD_PUMP_Stat},

    { "TEMP", "temp_profile_cfg", "Display stored temperature profiles from database", true, NULL, CMD_Temp_profile_config},
    { "TEMP", "temp_profile_cfg_main_ntc", "Display stored temperature profiles from database", true, NULL, CMD_Temp_profile_config_main_ntc},
    { "TEMP", "temp_profile_cfg_sec_ntc", "Display stored temperature profiles from database", true, NULL, CMD_Temp_profile_config_sec_ntc},
    { "TEMP", "temp_profile_cfg_tec", "Display stored temperature profiles from database", true, NULL, CMD_Temp_profile_config_tec},
    { "TEMP", "temp_profile_cfg_heater", "Display stored temperature profiles from database", true, NULL, CMD_Temp_profile_config_heater},
    { "TEMP", "temp_profile_cfg_setpoint", "Display stored temperature profiles from database", true, NULL, CMD_Temp_profile_config_setpoint},
    { "TEMP", "temp_profile_cfg_main_sec_delta", "Display stored temperature profiles from database", true, NULL, CMD_Temp_profile_config_main_sec_delta},
    { "TEMP", "temp_profile_cfg_step", "Display stored temperature profiles from database", true, NULL, CMD_Temp_profile_config_step},
    { "TEMP", "temp_profile_cfg_save", "Display stored temperature profiles from database", true, NULL, CMD_Temp_profile_config_save},
    { "TEMP", "temp_profile_cfg_cancel", "Display stored temperature profiles from database", true, NULL, CMD_Temp_profile_config_cancel},

    { "TEMP", "temp_profile_dis", "Display stored temperature profiles from database", true, NULL, CMD_DB_TempProfile_display},
    { "TEMP", "temp_profile_val", "Validate temperature profile data integrity", true, NULL, CMD_DB_TempProfile_validation},
    { "TEMP", "temp_auto_ena", "Enable automatic temperature control mode", true, NULL, CMD_Temp_auto_ena},
    { "TEMP", "temp_auto_start", "Start the automatic temperature control process", true, NULL, CMD_Temp_auto_start},
    { "TEMP", "temp_auto_pid_set", "Set PID constants for temperature control", true, NULL, CMD_Temp_PID_set},
    { "TEMP", "temp_auto_pid_get", "Get current PID constants", true, NULL, CMD_Temp_PID_get},
    { "TEMP", "temp_manu", "Switch to manual temperature control mode", true, NULL, CMD_Temp_manu},
    { "TEMP", "c", "Toggle temperature data logging to console", true, NULL, CMD_Temp_log_toggle},

    { "PSRAM", "psram_sw_mcu", "Read manufacturer and device ID of PSRAM", true, NULL, CMD_PSRAM_Switch_MCU},
    { "PSRAM", "psram_sw_mpu", "Read manufacturer and device ID of PSRAM", true, NULL, CMD_PSRAM_Switch_MPU},
    { "PSRAM", "psram_id", "Read manufacturer and device ID of PSRAM", true, NULL, CMD_PSRAM_Read_ID},
    { "PSRAM", "psram_write", "Read manufacturer and device ID of PSRAM", true, NULL, CMD_PSRAM_Write_Byte},
    { "PSRAM", "psram_read", "Read manufacturer and device ID of PSRAM", true, NULL, CMD_PSRAM_Read_Byte},

    { "LSM6", "lsm6d_init", "Initialize LSM6DSOX IMU sensor", true, NULL, CMD_LSM6DSOX_Init},
    { "LSM6", "lsm6d_conf_accel", "Configure Accelerometer settings (ODR, Scale)", true, NULL, CMD_LSM6DSOX_Config_Accel},
    { "LSM6", "lsm6d_conf_gyro", "Configure Gyroscope settings (ODR, Scale)", true, NULL, CMD_LSM6DSOX_Config_Gyro},
    { "LSM6", "lsm6d_conf_int", "Configure hardware interrupt pins for IMU", true, NULL, CMD_LSM6DSOX_Config_Int},
    { "LSM6", "lsm6d_conf_ff", "Configure Free-Fall detection parameters", true, NULL, CMD_LSM6DSOX_Config_FF},
    { "LSM6", "lsm6d_conf_wu", "Configure Wake-Up event parameters", true, NULL, CMD_LSM6DSOX_Config_WU},
    { "LSM6", "lsm6d_dis_int", "Disable all IMU hardware interrupts", true, NULL, CMD_LSM6DSOX_Dis_Int},
    { "LSM6", "lsm6d_check", "Check IMU connection (WhoAmI register)", true, NULL, CMD_LSM6DSOX_Check},
    { "LSM6", "lsm6d_read_accel", "Read current Accelerometer data", true, NULL, CMD_LSM6DSOX_Read_Accel},
    { "LSM6", "lsm6d_read_gyro", "Read current Gyroscope data", true, NULL, CMD_LSM6DSOX_Read_Gyro},
    { "LSM6", "lsm6d_read_temp", "Read IMU internal temperature", true, NULL, CMD_LSM6DSOX_Read_Temp},
    { "LSM6", "lsm6d_read_all", "Read all IMU sensor data (Accel + Gyro + Temp)", true, NULL, CMD_LSM6DSOX_Read_All},
    
    { "STATE", "state_monitor", "Show initialization status of all devices", false, NULL, CMD_State_monitor },

    { NULL, "a", "Test command A", true, NULL, CMD_a},
    { NULL, "b", "Test command B", true, NULL, CMD_b},
    { NULL, "d", "Test command C", true, NULL, CMD_d},
    { NULL, "reset", "Perform a system software reset", false, NULL, CMD_Reset},
};

/*************************************************
 *                 External Declarations         *
 *************************************************/

/*************************************************
 *             Command List Function             *
 *************************************************/
static void CMD_ClearCLI(EmbeddedCli *cli, char *args, void *context) {
    snprintf(cli_buf, sizeof (cli_buf), "\33[2J\33[H");
    embeddedCliPrint(cli, cli_buf);
}

static void CMD_RtcSet(EmbeddedCli *cli, char *args, void *context) {
    const char *arg1 = embeddedCliGetToken(args, 1); // hour
    const char *arg2 = embeddedCliGetToken(args, 2); // minute
    const char *arg3 = embeddedCliGetToken(args, 3); // second
    const char *arg4 = embeddedCliGetToken(args, 4); // day
    const char *arg5 = embeddedCliGetToken(args, 5); // month
    const char *arg6 = embeddedCliGetToken(args, 6); // year

    if (arg1 == NULL || arg2 == NULL || arg3 == NULL ||
            arg4 == NULL || arg5 == NULL || arg6 == NULL) {
        snprintf(cli_buf, sizeof (cli_buf),
                "Usage: rtc_set <hour> <minute> <second> <day> <month> <year>");
        embeddedCliPrint(cli, cli_buf);
        return;
    }

    int hour = atoi(arg1);
    int minute = atoi(arg2);
    int second = atoi(arg3);
    int day = atoi(arg4);
    int month = atoi(arg5);
    int year = atoi(arg6);

    if (hour < 0 || hour > 23) {
        embeddedCliPrint(cli, "Invalid hour (must be 0-23). Please enter again.");
        return;
    }
    if (minute < 0 || minute > 59) {
        embeddedCliPrint(cli, "Invalid minute (must be 0-59). Please enter again.");
        return;
    }
    if (second < 0 || second > 59) {
        embeddedCliPrint(cli, "Invalid second (must be 0-59). Please enter again.");
        return;
    }
    if (day < 1 || day > 31) {
        embeddedCliPrint(cli, "Invalid day (must be 1-31). Please enter again.");
        return;
    }
    if (month < 1 || month > 12) {
        embeddedCliPrint(cli, "Invalid month (must be 1-12). Please enter again.");
        return;
    }
    if (year < 0 || year > 99) {
        embeddedCliPrint(cli, "Invalid year (must be 2 digits, e.g., 25 for 2025). Please enter again.");
        return;
    }

    s_DateTime dt;
    dt.hour = (uint8_t) hour;
    dt.minute = (uint8_t) minute;
    dt.second = (uint8_t) second;
    dt.day = (uint8_t) day;
    dt.month = (uint8_t) month;
    dt.year = (uint8_t) year;

    Utils_SetRTC(&dt);
    bsp_rtc_sync_from_soft();

    snprintf(cli_buf, sizeof (cli_buf),
            "--> RTC set to %02d:%02d:%02d, %02d/%02d/20%02d",
            dt.hour, dt.minute, dt.second, dt.day, dt.month, dt.year);
    embeddedCliPrint(cli, cli_buf);
    embeddedCliPrint(cli, "");
}

static void CMD_RtcGet(EmbeddedCli *cli, char *args, void *context) {
    const char *mode = embeddedCliGetToken(args, 1);

    if (mode == NULL) {
        snprintf(cli_buf, sizeof (cli_buf), "Usage: rtc_get <hard|soft|work|epoch|all>");
        embeddedCliPrint(cli, cli_buf);
        return;
    }

    if (strcmp(mode, "hard") == 0) {
        bsp_rtc_datetime_t dt;
        bsp_rtc_get(&dt);
        snprintf(cli_buf, sizeof (cli_buf),
                "--> Hard RTC: %02d:%02d:%02d, %02d/%02d/%04d",
                dt.hour, dt.min, dt.sec, dt.date, dt.month, (int) dt.year);
        embeddedCliPrint(cli, cli_buf);
    } else if (strcmp(mode, "soft") == 0) {
        s_DateTime rtc;
        Utils_GetRTC(&rtc);
        snprintf(cli_buf, sizeof (cli_buf),
                "--> Soft RTC: Time: %02d:%02d:%02d, Date: %02d/%02d/20%02d",
                rtc.hour, rtc.minute, rtc.second,
                rtc.day, rtc.month, rtc.year);
        embeddedCliPrint(cli, cli_buf);
    } else if (strcmp(mode, "work") == 0) {
        uint32_t days = 0;
        uint8_t hours = 0, minutes = 0, seconds = 0;
        Utils_GetWorkingTime(&days, &hours, &minutes, &seconds);
        snprintf(cli_buf, sizeof (cli_buf),
                "--> Working Uptime: Time: %02d:%02d:%02d, Days: %d",
                hours, minutes, seconds, (uint8_t) days);
        embeddedCliPrint(cli, cli_buf);
    } else if (strcmp(mode, "epoch") == 0) {
        uint32_t epoch = Utils_GetEpoch();
        snprintf(cli_buf, sizeof (cli_buf), "--> Epoch: %lu", (unsigned long) epoch);
        embeddedCliPrint(cli, cli_buf);
    } else if (strcmp(mode, "all") == 0) {
        bsp_rtc_datetime_t hdt;
        bsp_rtc_get(&hdt);
        snprintf(cli_buf, sizeof (cli_buf),
                "--> Hard RTC: %02d:%02d:%02d, %02d/%02d/%04d",
                hdt.hour, hdt.min, hdt.sec, hdt.date, hdt.month, (int) hdt.year);
        embeddedCliPrint(cli, cli_buf);
        // Soft RTC
        s_DateTime rtc;
        Utils_GetRTC(&rtc);
        snprintf(cli_buf, sizeof (cli_buf),
                "--> Soft RTC: Time: %02d:%02d:%02d, Date: %02d/%02d/20%02d",
                rtc.hour, rtc.minute, rtc.second,
                rtc.day, rtc.month, rtc.year);
        embeddedCliPrint(cli, cli_buf);
        // Working uptime:
        uint32_t days = 0;
        uint8_t hours = 0, minutes = 0, seconds = 0;
        Utils_GetWorkingTime(&days, &hours, &minutes, &seconds);
        snprintf(cli_buf, sizeof (cli_buf),
                "--> Working Uptime: Time: %02d:%02d:%02d, Days: %d",
                hours, minutes, seconds, (uint8_t) days);
        embeddedCliPrint(cli, cli_buf);
        // Epoch
        uint32_t epoch = Utils_GetEpoch();
        snprintf(cli_buf, sizeof (cli_buf), "--> Epoch: %lu", (unsigned long) epoch);
        embeddedCliPrint(cli, cli_buf);
    } else {
        snprintf(cli_buf, sizeof (cli_buf), "Unknown mode. Use: rtc_get <hard|soft|work|epoch|all>");
        embeddedCliPrint(cli, cli_buf);
    }
    embeddedCliPrint(cli, "");
}

static void CMD_RtcSetEpoch(EmbeddedCli *cli, char *args, void *context) {
    const char *arg1 = embeddedCliGetToken(args, 1);

    if (arg1 == NULL) {
        snprintf(cli_buf, sizeof (cli_buf), "Usage: rtc_setepoch <epoch>");
        embeddedCliPrint(cli, cli_buf);
        return;
    }

    uint32_t epoch = (uint32_t) strtoul(arg1, NULL, 0);

    if (epoch < EPOCH_OFFSET_UNIX) {
        snprintf(cli_buf, sizeof (cli_buf), "Invalid epoch. Must be >= %lu", (unsigned long) EPOCH_OFFSET_UNIX);
        embeddedCliPrint(cli, cli_buf);
        return;
    }

    Utils_SetEpoch(epoch);
    bsp_rtc_sync_from_soft();

    s_DateTime dt;
    EpochToDateTime(epoch - EPOCH_OFFSET_UNIX, &dt);

    snprintf(cli_buf, sizeof (cli_buf),
            "--> RTC set to %02d:%02d:%02d, %02d/%02d/20%02d",
            dt.hour, dt.minute, dt.second, dt.day, dt.month, dt.year);
    embeddedCliPrint(cli, cli_buf);
    embeddedCliPrint(cli, "");
}

static void CMD_OsStat(EmbeddedCli *cli, char *args, void *context) {
    TaskStatus_t taskStatusArray[22];
    UBaseType_t arraySize = 22;
    UBaseType_t totalTasks;

    UBaseType_t numTasks = uxTaskGetNumberOfTasks();
    snprintf(cli_buf, sizeof (cli_buf), "Number of tasks running: %lu", (unsigned long) numTasks);
    embeddedCliPrint(cli, cli_buf);

    totalTasks = uxTaskGetSystemState(taskStatusArray, arraySize, NULL);

    if (totalTasks == 0) {
        embeddedCliPrint(cli, "No tasks found or error occurred.");
        embeddedCliPrint(cli, "");
        return;
    }

    snprintf(cli_buf, sizeof (cli_buf), "Total Tasks: %lu", (unsigned long) totalTasks);
    embeddedCliPrint(cli, cli_buf);
    embeddedCliPrint(cli, "Task Name          State     Prio  Stack Left");
    embeddedCliPrint(cli, "----------------------------------------");

    for (UBaseType_t i = 0; i < totalTasks; i++) {
        const char *stateStr;
        switch (taskStatusArray[i].eCurrentState) {
            case eRunning:
                stateStr = "Running";
                break;
            case eReady:
                stateStr = "Ready";
                break;
            case eBlocked:
                stateStr = "Blocked";
                break;
            case eSuspended:
                stateStr = "Suspend";
                break;
            case eDeleted:
                stateStr = "Deleted";
                break;
            default:
                stateStr = "Unknown";
                break;
        }

        snprintf(cli_buf, sizeof (cli_buf), "%-18s %-10s %-4lu %10lu",
                taskStatusArray[i].pcTaskName, stateStr,
                taskStatusArray[i].uxCurrentPriority,
                taskStatusArray[i].usStackHighWaterMark);
        embeddedCliPrint(cli, cli_buf);
    }

    embeddedCliPrint(cli, "----------------------------------------");
    embeddedCliPrint(cli, "");
}

static void CMD_Dmesg(EmbeddedCli *cli, char *args, void *context) {
    const char *arg1 = embeddedCliGetToken(args, 1);
    embeddedCliPrint(cli, "Dmesg - Logger Message:");

    if (arg1 == NULL) {
        embeddedCliPrint(cli, "--> Oldest >>");
        Dmesg_GetLogs();
        embeddedCliPrint(cli, "--> Latest <<");
    } else {
        size_t N = (size_t) strtoul(arg1, NULL, 10);
        snprintf(cli_buf, sizeof (cli_buf), "Latest %lu Logs:", (unsigned long) N);
        embeddedCliPrint(cli, cli_buf);
        Dmesg_GetLatestN(N);
    }

    embeddedCliPrint(cli, "");
}

void CMD_AutoComplete(EmbeddedCli *cli, char *args, void *context) {
    (void) context;
    embeddedCliTokenizeArgs(args);
    const char *arg1 = embeddedCliGetToken(args, 1);

    if (arg1 == NULL) {
        bool cur = embeddedCliGetAutoComplete(cli);
        embeddedCliPrint(cli, cur ? "autocomplete: ON" : "autocomplete: OFF");
        return;
    }

    embeddedCliSetAutoComplete(cli, true);
    if (strcmp(arg1, "on") == 0) {
        embeddedCliPrint(cli, "autocomplete: ON");
    } else if (strcmp(arg1, "off") == 0) {
        embeddedCliSetAutoComplete(cli, false);
        embeddedCliPrint(cli, "autocomplete: OFF");
    } else {
        embeddedCliPrint(cli, "Usage: autocomplete [on|off]");
    }
}

static const uint32_t s_baud_table[] = {250000U, 500000U, 1000000U, 921600U, 115200U};
#define BAUD_TABLE_LEN  (sizeof(s_baud_table) / sizeof(s_baud_table[0]))
static uint8_t s_baud_idx = 0U;

void CMD_UART_BaudToggle(EmbeddedCli *cli, char *args, void *context) {
    s_baud_idx = (s_baud_idx + 1U) % BAUD_TABLE_LEN;
    uint32_t new_baud = s_baud_table[s_baud_idx];

    UART_SERIAL_SETUP setup = {
        .baudRate = new_baud,
        .parity = UART_PARITY_NONE,
    };

    while (UART0_WriteIsBusy()) {
        ;
    }

    char buf[48];
    if (UART0_SerialSetup(&setup, 0U)) {
        snprintf(buf, sizeof (buf), "[BAUD] UART0 -> %lu bps", (unsigned long) new_baud);
    } else {
        snprintf(buf, sizeof (buf), "[BAUD] FAILED for %lu bps", (unsigned long) new_baud);
    }
    embeddedCliPrint(cli, buf);
}

/**
 * @brief Raw CAN send (bypasses CSP, for hardware testing)
 * Usage: can_send <id> [ext] [b0 b1 ... b7]
 * Example: can_send 0x100 0xAA 0xBB          (standard 11-bit)
 *          can_send 0x12345 ext 0xAA 0xBB    (extended 29-bit)
 */
static void CMD_CanSend(EmbeddedCli *cli, char *args, void *context) {
    const char *idStr = embeddedCliGetToken(args, 1);

    if (idStr == NULL) {
        embeddedCliPrint(cli, "Usage: can_send <id> [ext] [b0 b1 ... b7]");
        embeddedCliPrint(cli, "  can_send 0x100 0xAA 0xBB       (standard)");
        embeddedCliPrint(cli, "  can_send 0x12345 ext 0xAA 0xBB (extended)");
        return;
    }

    uint32_t canId = (uint32_t) strtoul(idStr, NULL, 0);
    bool extended = false;
    uint8_t data[8] = {0};
    uint8_t dlc = 0;
    uint8_t dataStartToken = 2;

    const char *tok2 = embeddedCliGetToken(args, 2);
    if (tok2 != NULL && strcmp(tok2, "ext") == 0) {
        extended = true;
        dataStartToken = 3;
    }

    if (!extended && canId > 0x7FF) {
        embeddedCliPrint(cli, "Hint: ID > 0x7FF, add 'ext' for extended frame");
        embeddedCliPrint(cli, "  can_send 0x12345 ext 0xAA");
        return;
    }

    for (uint8_t i = dataStartToken; dlc < 8; i++) {
        const char *valStr = embeddedCliGetToken(args, i);
        if (valStr == NULL) break;
        data[dlc++] = (uint8_t) strtoul(valStr, NULL, 0);
    }

    if (CAN_RawSend(canId, data, dlc, extended)) {
        snprintf(cli_buf, sizeof (cli_buf),
                "--> TX OK [%s]: ID=0x%lX DLC=%u",
                extended ? "EXT" : "STD",
                (unsigned long) canId, dlc);
        embeddedCliPrint(cli, cli_buf);
    } else {
        embeddedCliPrint(cli, "--> TX FAILED");
    }
    embeddedCliPrint(cli, "");
}

/**
 * @brief CSP ping another node
 * Usage: csp_ping <dest_addr> [timeout_ms]
 * Example: csp_ping 2          (ping node 2, default 1000ms)
 *          csp_ping 2 500      (ping node 2, 500ms timeout)
 */
static void CMD_CspPing(EmbeddedCli *cli, char *args, void *context) {
    const char *addrStr = embeddedCliGetToken(args, 1);
    const char *timeStr = embeddedCliGetToken(args, 2);

    if (addrStr == NULL) {
        embeddedCliPrint(cli, "Usage: csp_ping <dest_addr> [timeout_ms]");
        return;
    }

    uint8_t dest = (uint8_t) strtoul(addrStr, NULL, 0);
    uint32_t timeout = 1000;
    if (timeStr != NULL) {
        timeout = (uint32_t) strtoul(timeStr, NULL, 0);
    }

    snprintf(cli_buf, sizeof (cli_buf), "--> Pinging CSP node %u (timeout %lums)...", dest, (unsigned long) timeout);
    embeddedCliPrint(cli, cli_buf);

    int rtt = csp_ping(dest, timeout, 100, CSP_O_NONE);

    if (rtt >= 0) {
        snprintf(cli_buf, sizeof (cli_buf), "--> Reply from %u: time=%d ms", dest, rtt);
        embeddedCliPrint(cli, cli_buf);
    } else {
        snprintf(cli_buf, sizeof (cli_buf), "--> No reply from %u (timeout)", dest);
        embeddedCliPrint(cli, cli_buf);
    }
    embeddedCliPrint(cli, "");
}

/**
 * @brief Send CSP message to another node
 * Usage: csp_send <addr> <port> <string_data>
 * Example: csp_send 2 10 Hello
 */
static void CMD_CspSend(EmbeddedCli *cli, char *args, void *context) {
    const char *addrStr = embeddedCliGetToken(args, 1);
    const char *portStr = embeddedCliGetToken(args, 2);
    const char *dataStr = embeddedCliGetToken(args, 3);

    if (addrStr == NULL || portStr == NULL || dataStr == NULL) {
        embeddedCliPrint(cli, "Usage: csp_send <addr> <port> <data>");
        embeddedCliPrint(cli, "Example: csp_send 2 10 Hello");
        return;
    }

    uint8_t dest = (uint8_t) strtoul(addrStr, NULL, 0);
    uint8_t port = (uint8_t) strtoul(portStr, NULL, 0);
    uint16_t len = strlen(dataStr);

    csp_conn_t *conn = csp_connect(CSP_PRIO_NORM, dest, port, 1000, CSP_O_NONE);
    if (conn == NULL) {
        embeddedCliPrint(cli, "--> Connection failed");
        return;
    }

    csp_packet_t *pkt = csp_buffer_get(len);
    if (pkt == NULL) {
        embeddedCliPrint(cli, "--> Buffer alloc failed");
        csp_close(conn);
        return;
    }

    memcpy(pkt->data, dataStr, len);
    pkt->length = len;

    if (csp_send(conn, pkt, 10000) == 0) {
        csp_buffer_free(pkt);
        embeddedCliPrint(cli, "--> Send failed");
    } else {
        snprintf(cli_buf, sizeof (cli_buf),
                "--> Sent %u bytes to %u:%u", len, dest, port);
        embeddedCliPrint(cli, cli_buf);
    }

    csp_close(conn);
    embeddedCliPrint(cli, "");
}

/**
 * @brief Show CSP interface info
 * Usage: csp_info
 */
static void CMD_CspInfo(EmbeddedCli *cli, char *args, void *context) {

    embeddedCliPrint(cli, "=== CSP Info ===");

    snprintf(cli_buf, sizeof (cli_buf), "  My Address: %u", csp_get_address());
    embeddedCliPrint(cli, cli_buf);

    snprintf(cli_buf, sizeof (cli_buf), "  Free cli_bufs: %u", csp_buffer_remaining());
    embeddedCliPrint(cli, cli_buf);

    /* Print interface list */
    embeddedCliPrint(cli, "  Interfaces:");
    csp_iflist_print();

    /* Print routing table */
    embeddedCliPrint(cli, "  Route table:");
    csp_rtable_print();

    embeddedCliPrint(cli, "================");
    embeddedCliPrint(cli, "");
}

static void print_param_row(EmbeddedCli *cli, gs_param_table_instance_t *tinst,
        const gs_param_table_row_t *row) {
    char buf[128];
    uint8_t cnt = (row->array_size > 1) ? row->array_size : 1;

    for (uint8_t i = 0; i < cnt; i++) {
        uint16_t addr = row->addr + (uint16_t) (i * row->size);
        char suffix[8] = "";
        if (cnt > 1) snprintf(suffix, sizeof (suffix), "[%u]", i);

        switch (row->type) {
            case GS_PARAM_UINT8:
            {
                uint8_t v = 0;
                gs_param_get(tinst, addr, GS_PARAM_UINT8, &v, sizeof (v), 0);
                snprintf(buf, sizeof (buf), "  %-13s%-6s = %u", row->name, suffix, v);
                break;
            }
            case GS_PARAM_UINT16:
            {
                uint16_t v = 0;
                gs_param_get(tinst, addr, GS_PARAM_UINT16, &v, sizeof (v), 0);
                snprintf(buf, sizeof (buf), "  %-13s%-6s = %u", row->name, suffix, v);
                break;
            }
            case GS_PARAM_UINT32:
            {
                uint32_t v = 0;
                gs_param_get(tinst, addr, GS_PARAM_UINT32, &v, sizeof (v), 0);
                snprintf(buf, sizeof (buf), "  %-13s%-6s = %lu", row->name, suffix, (unsigned long) v);
                break;
            }
            case GS_PARAM_INT8:
            {
                int8_t v = 0;
                gs_param_get(tinst, addr, GS_PARAM_INT8, &v, sizeof (v), 0);
                snprintf(buf, sizeof (buf), "  %-13s%-6s = %d", row->name, suffix, (int) v);
                break;
            }
            case GS_PARAM_INT16:
            {
                int16_t v = 0;
                gs_param_get(tinst, addr, GS_PARAM_INT16, &v, sizeof (v), 0);
                snprintf(buf, sizeof (buf), "  %-13s%-6s = %d", row->name, suffix, (int) v);
                break;
            }
            case GS_PARAM_INT32:
            {
                int32_t v = 0;
                gs_param_get(tinst, addr, GS_PARAM_INT32, &v, sizeof (v), 0);
                snprintf(buf, sizeof (buf), "  %-13s%-6s = %ld", row->name, suffix, (long) v);
                break;
            }
            case GS_PARAM_FLOAT:
            {
                float v = 0.0f;
                gs_param_get(tinst, addr, GS_PARAM_FLOAT, &v, sizeof (v), 0);
                snprintf(buf, sizeof (buf), "  %-13s%-6s = %.4f", row->name, suffix, (double) v);
                break;
            }
            case GS_PARAM_BOOL:
            {
                uint8_t v = 0;
                gs_param_get(tinst, addr, GS_PARAM_BOOL, &v, sizeof (v), 0);
                snprintf(buf, sizeof (buf), "  %-13s%-6s = %s", row->name, suffix, v ? "true" : "false");
                break;
            }
            case GS_PARAM_STRING:
            {
                char strbuf[32] = {0};
                gs_param_get_string(tinst, addr, strbuf, sizeof (strbuf), 0);
                snprintf(buf, sizeof (buf), "  %-13s%-6s = \"%s\"", row->name, suffix, strbuf);
                break;
            }
            default:
                snprintf(buf, sizeof (buf), "  %-13s%-6s = (type %u @ 0x%04X)",
                        row->name, suffix, (unsigned) row->type, addr);
                break;
        }
        embeddedCliPrint(cli, buf);
    }
}

static void CMD_ParamShow(EmbeddedCli *cli, char *args, void *context) {
    const char *tidStr = embeddedCliGetToken(args, 1);
    char buf[80];

    if (tidStr == NULL) {
        embeddedCliPrint(cli, "Usage: param_show <table_id>");
        embeddedCliPrint(cli, "  0=Board  1=Shell-tunnel  4=Telemetry");
        return;
    }

    uint8_t tid = (uint8_t) strtoul(tidStr, NULL, 0);
    if (tid > 4 || tid == 2 || tid == 3) {
        embeddedCliPrint(cli, "Error: table_id must be 0, 1 or 4");
        return;
    }

    gs_param_table_instance_t *tinst = &g_tables[tid];

    snprintf(buf, sizeof (buf), "=== Table %u: %s ===", tid, data_table_get_name(tid));
    embeddedCliPrint(cli, buf);
    snprintf(buf, sizeof (buf), "    Rows: %u | Mem: %u bytes | CRC: 0x%04X",
            tinst->row_count, tinst->memory_size,
            gs_param_table_checksum_be(tinst));
    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "---");

    for (unsigned int i = 0; i < tinst->row_count; i++) {
        print_param_row(cli, tinst, &tinst->rows[i]);
    }

    embeddedCliPrint(cli, "===================");
    embeddedCliPrint(cli, "");
}

/* ============================================================
 *  CMD_ParamSet
 *  Usage: param_set <table_id> <addr> <type> <value>
 *  type: u8 u16 u32 i8 i16 i32 f32 bool str
 *  eg:
 *    param_set 4 8   i16 250         -> BRD_TEMP = 250 (25.0?C)
 *    param_set 4 0   u32 1748000000  -> TIME_NOW = epoch
 *    param_set 1 121 u8  0           -> led_en = 0
 *    param_set 4 15  str NOMINAL     -> dev_status
 * ============================================================ */
static void CMD_ParamSet(EmbeddedCli *cli, char *args, void *context) {
    const char *tidStr = embeddedCliGetToken(args, 1);
    const char *addrStr = embeddedCliGetToken(args, 2);
    const char *typeStr = embeddedCliGetToken(args, 3);
    const char *valStr = embeddedCliGetToken(args, 4);
    char buf[128];

    if (!tidStr || !addrStr || !typeStr || !valStr) {
        embeddedCliPrint(cli, "Usage: param_set <table_id> <addr> <type> <value>");
        embeddedCliPrint(cli, "  type: u8 u16 u32 i8 i16 i32 f32 bool str");
        embeddedCliPrint(cli, "  Example: param_set 0 27 u8 11");
        return;
    }

    uint8_t tid = (uint8_t) strtoul(tidStr, NULL, 0);
    uint16_t addr = (uint16_t) strtoul(addrStr, NULL, 0);

    if (tid > 4 || tid == 2 || tid == 3) {
        embeddedCliPrint(cli, "Error: table_id must be 0, 1 or 4");
        return;
    }

    gs_param_table_instance_t *tinst = &g_tables[tid];
    gs_error_t err = GS_ERROR_ARG;

    if (strcmp(typeStr, "u8") == 0) {
        uint8_t v = (uint8_t) strtoul(valStr, NULL, 0);
        err = gs_param_set(tinst, addr, GS_PARAM_UINT8, &v, sizeof (v), 0);
    } else if (strcmp(typeStr, "u16") == 0) {
        uint16_t v = (uint16_t) strtoul(valStr, NULL, 0);
        err = gs_param_set(tinst, addr, GS_PARAM_UINT16, &v, sizeof (v), 0);
    } else if (strcmp(typeStr, "u32") == 0) {
        uint32_t v = (uint32_t) strtoul(valStr, NULL, 0);
        err = gs_param_set(tinst, addr, GS_PARAM_UINT32, &v, sizeof (v), 0);
    } else if (strcmp(typeStr, "i8") == 0) {
        int8_t v = (int8_t) atoi(valStr);
        err = gs_param_set(tinst, addr, GS_PARAM_INT8, &v, sizeof (v), 0);
    } else if (strcmp(typeStr, "i16") == 0) {
        int16_t v = (int16_t) atoi(valStr);
        err = gs_param_set(tinst, addr, GS_PARAM_INT16, &v, sizeof (v), 0);
    } else if (strcmp(typeStr, "i32") == 0) {
        int32_t v = (int32_t) atol(valStr);
        err = gs_param_set(tinst, addr, GS_PARAM_INT32, &v, sizeof (v), 0);
    } else if (strcmp(typeStr, "f32") == 0) {
        float v = (float) atof(valStr);
        err = gs_param_set(tinst, addr, GS_PARAM_FLOAT, &v, sizeof (v), 0);
    } else if (strcmp(typeStr, "bool") == 0) {
        uint8_t v = (atoi(valStr) != 0) ? 1 : 0;
        err = gs_param_set(tinst, addr, GS_PARAM_BOOL, &v, sizeof (v), 0);
    } else if (strcmp(typeStr, "str") == 0) {
        err = gs_param_set_string(tinst, addr, valStr, 0);
    } else {
        snprintf(buf, sizeof (buf), "Unknown type '%s'. Use: u8 u16 u32 i8 i16 i32 f32 bool str", typeStr);
        embeddedCliPrint(cli, buf);
        return;
    }

    if (err == GS_OK) {
        snprintf(buf, sizeof (buf), "--> OK: T%u[0x%04X] = %s (%s)", tid, addr, valStr, typeStr);
    } else {
        snprintf(buf, sizeof (buf), "--> FAIL: err=%d (addr out of range?)", (int) err);
    }
    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "");
}

static void CMD_LED_Set(EmbeddedCli *cli, char *args, void *context) {
    PIOB_REGS->PIO_SODR = (1U << 12U);

    //    char buf[128];
    //    sprintf(buf, "PB_PSR  = 0x%08lX", PIOB_REGS->PIO_PSR);
    //    embeddedCliPrint(cli, buf);
    //    sprintf(buf, "PB_OSR  = 0x%08lX", PIOB_REGS->PIO_OSR);
    //    embeddedCliPrint(cli, buf);
    //    sprintf(buf, "PB_ODSR = 0x%08lX", PIOB_REGS->PIO_ODSR);
    //    embeddedCliPrint(cli, buf);
    //    sprintf(buf, "PB_PDSR = 0x%08lX", PIOB_REGS->PIO_PDSR);
    //    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "LED_ON");
    embeddedCliPrint(cli, "");
}

static void CMD_LED_Reset(EmbeddedCli *cli, char *args, void *context) {
    PIOB_REGS->PIO_CODR = (1U << 12U);

    //    char buf[128];
    //    sprintf(buf, "PB_PSR  = 0x%08lX", PIOB_REGS->PIO_PSR);
    //    embeddedCliPrint(cli, buf);
    //    sprintf(buf, "PB_OSR  = 0x%08lX", PIOB_REGS->PIO_OSR);
    //    embeddedCliPrint(cli, buf);
    //    sprintf(buf, "PB_ODSR = 0x%08lX", PIOB_REGS->PIO_ODSR);
    //    embeddedCliPrint(cli, buf);
    //    sprintf(buf, "PB_PDSR = 0x%08lX", PIOB_REGS->PIO_PDSR);
    //    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "LED_OFF");
    embeddedCliPrint(cli, "");
}

static void CMD_HEATER_SetDuty(EmbeddedCli *cli, char *args, void *context) {
    (void) context;

    const char *HTR_CH = embeddedCliGetToken(args, 1);
    const char *HTR_Duty = embeddedCliGetToken(args, 2);
    char buf[128];

    if (HTR_CH == NULL || HTR_Duty == NULL) {
        embeddedCliPrint(cli, "Usage  : heater_set <channel 1-2> <duty 0-100>");
        embeddedCliPrint(cli, "Example: heater_set 1 80");
        return;
    }

    char *endptr;

    uint32_t ch = strtoul(HTR_CH, &endptr, 10);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Error: Channel must be a number (1-2)");
        return;
    }

    uint32_t duty = strtoul(HTR_Duty, &endptr, 10);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Error: Duty must be a number (0-100)");
        return;
    }

    if (ch < 1 || ch > 2) {
        embeddedCliPrint(cli, "Error: Channel out of range (1-2)");
        return;
    }

    if (duty > 100) {
        embeddedCliPrint(cli, "Error: Duty out of range (0-100%)");
        return;
    }

    bsp_heater_set_duty((uint8_t) (ch - 1), (uint8_t) duty);

    snprintf(buf, sizeof (buf), "--> OK: Heater[%lu] Duty = %lu%%", ch, duty);

    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "");
}

static void CMD_State_monitor(EmbeddedCli *cli, char *args, void *context) {
    char buf[256];

    sprintf(buf,
            "TEC      : %d %d\r\n"
            "BME688   : %d\r\n"
            "LSM6DSOX : %d\r\n"
            "MAX22200 : %d %d",
            
            (p_tec[0]->init_state == INIT_DONE) ? 1 : 0,
            (p_tec[1]->init_state == INIT_DONE) ? 1 : 0,

            (bme688_int.init_status == INIT_DONE) ? 1 : 0,

            (lsm6dsox.init_status == INIT_DONE) ? 1 : 0,
            
            (g_max22200_dev[0]->init_state == INIT_DONE) ? 1 : 0,
            (g_max22200_dev[1]->init_state == INIT_DONE) ? 1 : 0
            );

    embeddedCliPrint(cli, buf);
}

static void CMD_Reset(EmbeddedCli *cli, char *args, void *context) {
    NVIC_SystemReset();
    embeddedCliPrint(cli, "");
}

/*************************************************
 * Command Function "Dev"            *
 *************************************************/


/*************************************************
 *                  End CMD List                 *
 *************************************************/

/*************************************************
 *                Getter - Helper                *
 *************************************************/
const CliCommandBinding *getCliStaticBindings(void) {
    return cliStaticBindings_internal;
}

uint16_t getCliStaticBindingCount(void) {
    return sizeof (cliStaticBindings_internal) / sizeof (cliStaticBindings_internal[0]);
}
