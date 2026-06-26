#include "cli_temperature.h"
#include "cli_command.h"
#include "stdio.h"
#include "stdlib.h"
#include "os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "M0_App/AppOS/Database/DB_Temperature_Profile/db_temperature_profile.h"
#include "M0_App/AppOS/App_6_Temperature_Control/app_temperature.h"
#include "M0_App/AppOS/App_6_Temperature_Control/profile_state.h"
#include "M0_App/AppOS/App_3_CLI/app_cli.h"

#include "UART/uart_irq.h"
#include "BSP_Temperature/bsp_temperature.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////
//=======================================HUYNH THANH SANG============================================//
///////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t profile_config_index = 0;
static uint8_t current_step_idx = 0;
static profileData_t profileDataTemp;

void CMD_Temp_wizard_set(EmbeddedCli *cli, char *args, void *context) {
    app_cli_wizard_start(MAX_STEP_GETTING);
    UART2_SendString("Do you want to continue? (Y/N)\r\n");
}

void TempProfile_InputHandler(EmbeddedCli *cli, char *input, uint8_t step) {
    switch (step) {
        case GET_CONFIRM:
        {
            UART2_SendString("\r\nStart getting profile...\r\n");
            UART2_SendString(" | -> profile index: ");
            break;
        }

        case GET_PROFILE_INDEX:
        {
            profile_config_index = (uint8_t) atoi(input);
            UART2_SendString("\r\n | Notes: 0=NTC1 1=NTC2 ... 7=NTC8");
            UART2_SendString("\r\n | -> main ntc: ");
            break;
        }

        case GET_MAIN_NTC:
        {
            profileDataTemp.main_ntc = (uint8_t) atoi(input);
            UART2_SendString("\r\n | -> sec ntc: ");
            break;
        }

        case GET_SEC_NTC:
        {
            profileDataTemp.sec_ntc = (uint8_t) atoi(input);
            UART2_SendString("\r\n | Notes: eg:0x01 mean tec1 is ena");
            UART2_SendString("\r\n | -> tec mask: ");
            break;
        }

        case GET_TEC_MASK:
        {
            profileDataTemp.tec_mask = (uint8_t) atoi(input);
            UART2_SendString("\r\n | Notes: eg:0x10 mean heater5 is ena");
            UART2_SendString("\r\n | -> heater mask: ");
            break;
        }

        case GET_HEATER_MASK:
        {
            profileDataTemp.heater_mask = (uint8_t) atoi(input);
            UART2_SendString("\r\n | -> setpoint (0.01*C): ");
            break;
        }

        case GET_SETPOINT:
        {
            profileDataTemp.set_point = (uint16_t) atoi(input);
            UART2_SendString("\r\n | -> main-sec delta (0.01*C): ");
            break;
        }

        case GET_MAIN_SEC_DELTA:
        {
            profileDataTemp.main_sec_delta = (uint16_t) atoi(input);
            UART2_SendString("\r\n | -> step count: ");
            break;
        }

        case GET_STEP_COUNT:
        {
            profileDataTemp.step_count = (uint8_t) atoi(input);

            if (profileDataTemp.step_count == 0 || profileDataTemp.step_count > 8) {
                UART2_SendString("\r\n | Invalid step count: ");
                return;
            }

            current_step_idx = 0;
            UART2_SendString("\r\n | Notes: Format: start(0.01*C) stop(0.01*C) duration(s) mode[0=SOAK 1=HEAT 2=COOL]");
            UART2_SendString("\r\n | -> step[0]: ");
            break;
        }

        case GET_STEP:
        {
            int start, set, duration, mode;

            // parse input
            if (sscanf(input, "%d %d %d %d", &start, &set, &duration, &mode) != 4) {
                UART2_SendString("\r\n | Invalid format! Use: start set duration mode");
                UART2_SendString("\r\n | -> step[");
                char buf[4];
                sprintf(buf, "%d", current_step_idx);
                UART2_SendString(buf);
                UART2_SendString("]: ");
                return;
            }

            // asign input value into profileDataTemp
            profileDataTemp.steps[current_step_idx].start_point = start;
            profileDataTemp.steps[current_step_idx].set_point = set;
            profileDataTemp.steps[current_step_idx].duration = duration;
            profileDataTemp.steps[current_step_idx].control_mode = mode;


            // next step
            current_step_idx++;
            if (current_step_idx < profileDataTemp.step_count) {
                char buf[32];
                sprintf(buf, "\r\n | -> step[%d]: ", current_step_idx);
                UART2_SendString(buf);
            } else {
                // xong ? sang SAVE
                UART2_SendString("\r\n Save? (Y/N): ");
            }
            break;
        }

            // ...
        case GET_SAVE:
            DB_temp_profile_write(profile_config_index, &profileDataTemp);
            break;

        default:
            break;
    }
}

uint8_t TempProfile_CheckRemainStep(void) {
    return profileDataTemp.step_count - current_step_idx;
}

void CMD_Temp_profile_config(EmbeddedCli *cli, char *args, void *context) {
    char buf[128];
    const char *profileIndexStr = embeddedCliGetToken(args, 1);

    if (profileIndexStr == NULL) {
        embeddedCliPrint(cli, "=> Usage: temp_profile_cfg <profile_index:0-7>");
        return;
    }

    uint8_t profile_id = (uint8_t) strtoul(profileIndexStr, NULL, 0);

    bsp_temperature_prepare_to_config_profile(profile_id);

    snprintf(buf, sizeof (buf), "Ready setting for profile[%d]", profile_id);
    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "=> Continue with cmd: temp_profile_cfg_main_ntc");
    embeddedCliPrint(cli, "=> Cancel with cmd: temp_profile_cfg_cancel");
}

void CMD_Temp_profile_config_main_ntc(EmbeddedCli *cli, char *args, void *context) {
    const char *mainNTCStr = embeddedCliGetToken(args, 1);

    if (mainNTCStr == NULL) {
        embeddedCliPrint(cli, "=> Usage: temp_profile_cfg_main_ntc <ntc_index:0-7>");
        return;
    }

    uint8_t main_ntc = (uint8_t) strtoul(mainNTCStr, NULL, 0);
    bsp_temperature_config_main_ntc(main_ntc);

    embeddedCliPrint(cli, "=> Continue with cmd: temp_profile_cfg_sec_ntc");
    embeddedCliPrint(cli, "=> Cancel with cmd: temp_profile_cfg_cancel");
}

void CMD_Temp_profile_config_sec_ntc(EmbeddedCli *cli, char *args, void *context) {
    const char *secNTCStr = embeddedCliGetToken(args, 1);

    if (secNTCStr == NULL) {
        embeddedCliPrint(cli, "=> Usage: temp_profile_cfg_sec_ntc <ntc_index:0-7>");
        return;
    }

    uint8_t sec_ntc = (uint8_t) strtoul(secNTCStr, NULL, 0);
    bsp_temperature_config_sec_ntc(sec_ntc);

    embeddedCliPrint(cli, "=> Continue with cmd: temp_profile_cfg_tec");
    embeddedCliPrint(cli, "=> Cancel with cmd: temp_profile_cfg_cancel");
}

void CMD_Temp_profile_config_tec(EmbeddedCli *cli, char *args, void *context) {
    const char *tec1Str = embeddedCliGetToken(args, 1);
    const char *tec2Str = embeddedCliGetToken(args, 2);
    const char *tec3Str = embeddedCliGetToken(args, 3);
    const char *tec4Str = embeddedCliGetToken(args, 4);

    if (tec1Str == NULL || tec2Str == NULL || tec3Str == NULL || tec4Str == NULL) {
        embeddedCliPrint(cli, "=> Usage: temp_profile_cfg_tec <TEC1:0(UNUSE)/1(USE)> <> <> <TEC4>");
        return;
    }

    uint8_t tec1 = (uint8_t) strtoul(tec1Str, NULL, 0);
    uint8_t tec2 = (uint8_t) strtoul(tec2Str, NULL, 0);
    uint8_t tec3 = (uint8_t) strtoul(tec3Str, NULL, 0);
    uint8_t tec4 = (uint8_t) strtoul(tec4Str, NULL, 0);

    uint8_t tec_mask = (tec1) | (tec2 << 1) | (tec3 << 2) | (tec4 << 3);
    bsp_temperature_config_tec_mask(tec_mask);

    embeddedCliPrint(cli, "=> Continue with cmd: temp_profile_cfg_heater");
    embeddedCliPrint(cli, "=> Cancel with cmd: temp_profile_cfg_cancel");
}

void CMD_Temp_profile_config_heater(EmbeddedCli *cli, char *args, void *context) {
    const char *heater1Str = embeddedCliGetToken(args, 1);
    const char *heater2Str = embeddedCliGetToken(args, 2);
    const char *heater3Str = embeddedCliGetToken(args, 3);
    const char *heater4Str = embeddedCliGetToken(args, 4);
    const char *heater5Str = embeddedCliGetToken(args, 5);
    const char *heater6Str = embeddedCliGetToken(args, 6);
    const char *heater7Str = embeddedCliGetToken(args, 7);
    const char *heater8Str = embeddedCliGetToken(args, 8);


    if (heater1Str == NULL || heater2Str == NULL || heater3Str == NULL || heater4Str == NULL
            || heater5Str == NULL || heater6Str == NULL || heater7Str == NULL || heater8Str == NULL) {
        embeddedCliPrint(cli, "=> Usage: temp_profile_cfg_heater <HEATER1:0(UNUSE)/1(USE)> <> <> <> <> <> <> <HEATER8>");
        return;
    }

    uint8_t heater1 = (uint8_t) strtoul(heater1Str, NULL, 0);
    uint8_t heater2 = (uint8_t) strtoul(heater2Str, NULL, 0);
    uint8_t heater3 = (uint8_t) strtoul(heater3Str, NULL, 0);
    uint8_t heater4 = (uint8_t) strtoul(heater4Str, NULL, 0);
    uint8_t heater5 = (uint8_t) strtoul(heater5Str, NULL, 0);
    uint8_t heater6 = (uint8_t) strtoul(heater6Str, NULL, 0);
    uint8_t heater7 = (uint8_t) strtoul(heater7Str, NULL, 0);
    uint8_t heater8 = (uint8_t) strtoul(heater8Str, NULL, 0);

    uint8_t heater_mask = (heater1) | (heater2 << 1) | (heater3 << 2) | (heater4 << 3) 
                        | (heater5 << 4) | (heater6 << 5) | (heater7 << 6) | (heater8 << 7);
    bsp_temperature_config_heater_mask(heater_mask);

    embeddedCliPrint(cli, "=> Continue with cmd: temp_profile_cfg_setpoint <setpoint(unit: milli-degree Celsius)>");
    embeddedCliPrint(cli, "=> Cancel with cmd: temp_profile_cfg_cancel");
}

void CMD_Temp_profile_config_setpoint(EmbeddedCli *cli, char *args, void *context) {
    const char *setpointStr = embeddedCliGetToken(args, 1);

    if (setpointStr == NULL) {
        embeddedCliPrint(cli, "=> Usage: temp_profile_cfg_setpoint <setpoints>");
        return;
    }

    int32_t setpoint = (int32_t) strtoul(setpointStr, NULL, 0);
    setpoint = setpoint / 10;
    bsp_temperature_config_setpoint((int16_t)setpoint);
    
    embeddedCliPrint(cli, "=> Continue with cmd: temp_profile_cfg_main_sec_delta <delta(unit: milli-degree Celsius)>");
    embeddedCliPrint(cli, "=> Cancel with cmd: temp_profile_cfg_cancel");
}

void CMD_Temp_profile_config_main_sec_delta(EmbeddedCli *cli, char *args, void *context) {
    const char *main_sec_delta_Str = embeddedCliGetToken(args, 1);

    if (main_sec_delta_Str == NULL) {
        embeddedCliPrint(cli, "=> Usage: temp_profile_cfg_main_sec_delta <main_sec_delta>");
        return;
    }

    int16_t main_sec_delta = (int16_t) strtoul(main_sec_delta_Str, NULL, 0);
    main_sec_delta = main_sec_delta / 10;
    bsp_temperature_config_main_sec_delta(main_sec_delta);
    
    
    embeddedCliPrint(cli, "=> Continue with cmd: temp_profile_cfg_step <step_index> <start_temp> <stop_temp> <duration>");
    embeddedCliPrint(cli, "=> Cancel with cmd: temp_profile_cfg_cancel");
}

void CMD_Temp_profile_config_step(EmbeddedCli *cli, char *args, void *context) {
    const char *stepIDStr = embeddedCliGetToken(args, 1);
    const char *startTempStr = embeddedCliGetToken(args, 2);
    const char *stopTempStr = embeddedCliGetToken(args, 3);
    const char *durationStr = embeddedCliGetToken(args, 4);

    if (stepIDStr == NULL || startTempStr == NULL || stopTempStr == NULL || durationStr == NULL) {
        embeddedCliPrint(cli, "=> Usage: temp_profile_cfg_step <step_index:0-10> <start_temp> <stop_temp> <duration(unit: seconds)>");
        return;
    }
    
    uint8_t stepID = (uint8_t) strtoul(stepIDStr, NULL, 0);
    int32_t startTemp = (int32_t) strtoul(startTempStr, NULL, 0);
    startTemp = startTemp / 10;
    int32_t stopTemp = (int32_t) strtoul(stopTempStr, NULL, 0);
    stopTemp = stopTemp / 10;
    uint16_t duration = (uint16_t) strtoul(durationStr, NULL, 0);
    uint8_t mode = 0xFF;
    
    if (startTemp > stopTemp)
        mode = 2;
    else if (startTemp < stopTemp)
        mode = 1;
    else
        mode = 0;
    
    bsp_temperature_config_step(stepID, (int16_t)startTemp, (int16_t)stopTemp, duration, mode);
    
    embeddedCliPrint(cli, "=> Add another step with cmd: temp_profile_cfg_step <step_index> <start_temp> <stop_temp> <duration(unit: seconds)>");
    embeddedCliPrint(cli, "=> Save profile with cmd: temp_profile_cfg_save");
    embeddedCliPrint(cli, "=> Cancel with cmd: temp_profile_cfg_cancel");
}

void CMD_Temp_profile_config_save(EmbeddedCli *cli, char *args, void *context) {
    char buf[64];
    uint8_t profile_id_saved = bsp_temperature_config_save();
    sprintf(buf, "=> Profile[[%d] save successful", profile_id_saved);
    embeddedCliPrint(cli, buf);
}

void CMD_Temp_profile_config_cancel(EmbeddedCli *cli, char *args, void *context) {
    bsp_temperature_config_cancel();
    embeddedCliPrint(cli, "=> Temperature profile configuration cancelled");
}

void CMD_DB_TempProfile_display(EmbeddedCli *cli, char *args, void *context) {
    const char *token = embeddedCliGetToken(args, 1);
    if (token == NULL) {
        embeddedCliPrint(cli, "=> Usage: temp_profile_display <index>");

        return;
    }

    int profile_index = atoi(token);
    DB_temp_profile_display(cli, profile_index);
}

void CMD_DB_TempProfile_validation(EmbeddedCli *cli, char *args, void *context) {

    DB_temp_profile_validation(cli);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//=======================================HUYNH THANH SANG============================================//
///////////////////////////////////////////////////////////////////////////////////////////////////////

void CMD_Temp_auto_ena(EmbeddedCli *cli, char *args, void *context) {
    e_temp_cmd_t cmd = TEMP_CMD_MODE_AUTO;

    // Parse tham so
    const char *token = embeddedCliGetToken(args, 1);
    if (token == NULL) {
        embeddedCliPrint(cli, "=> Usage: temp_auto_ena <index>");
        return;
    }

    int state_index = atoi(token);

    // Check index hop ly (define max)
    if (state_index < 0 || state_index >= PROFILE_MAX_NUM) {
        embeddedCliPrint(cli, "Invalid index");
        return;
    }

    if (temperature_command_queue[state_index] == NULL) {
        embeddedCliPrint(cli, "Queue not init");
        return;
    }

    if (xQueueSend(temperature_command_queue[state_index], &cmd, 0) == pdPASS) {
        embeddedCliPrint(cli, "Auto mode enabled");
    } else {

        embeddedCliPrint(cli, "Queue full");
    }
}

void CMD_Temp_auto_start(EmbeddedCli *cli, char *args, void *context) {
    e_temp_cmd_t cmd = TEMP_CMD_EVENT_START;

    // Parse tham so
    const char *token = embeddedCliGetToken(args, 1);
    if (token == NULL) {
        embeddedCliPrint(cli, "=> Usage: temp_auto_start <index>");
        return;
    }

    int state_index = atoi(token);

    if (state_index < 0 || state_index >= PROFILE_MAX_NUM) {
        embeddedCliPrint(cli, "Invalid index");
        return;
    }

    if (temperature_command_queue[state_index] == NULL) {
        embeddedCliPrint(cli, "Queue not init");
        return;
    }

    if (xQueueSend(temperature_command_queue[state_index], &cmd, 0) == pdPASS) {
        embeddedCliPrint(cli, "Start OK");
    } else {

        embeddedCliPrint(cli, "Queue full");
    }
}

void CMD_Temp_manu(EmbeddedCli *cli, char *args, void *context) {
    e_temp_cmd_t cmd = TEMP_CMD_MODE_MANUAL;

    // Parse tham so
    const char *token = embeddedCliGetToken(args, 1);
    if (token == NULL) {
        embeddedCliPrint(cli, "=> Usage: temp_auto_stop <index>");
        return;
    }

    int state_index = atoi(token);

    if (state_index < 0 || state_index >= PROFILE_MAX_NUM) {
        embeddedCliPrint(cli, "Invalid index");
        return;
    }

    if (temperature_command_queue[state_index] == NULL) {
        embeddedCliPrint(cli, "Queue not init");
        return;
    }

    if (xQueueSend(temperature_command_queue[state_index], &cmd, 0) == pdPASS) {
        embeddedCliPrint(cli, "Stop OK");
    } else {

        embeddedCliPrint(cli, "Queue full");
    }
}

void CMD_Temp_log_toggle(EmbeddedCli *cli, char *args, void *context) {

    temp_pid_log_ena = !((bool) temp_pid_log_ena);
}

void CMD_Temp_PID_set(EmbeddedCli *cli, char *args, void *context) {
    const char *token;
    int state_id;
    float kP, kI, kD;

    // state
    token = embeddedCliGetToken(args, 1);
    if (token == NULL) {
        embeddedCliPrint(cli, "=> Usage: temp_pid_set <state> <kP> <kI> <kD>");
        return;
    }
    state_id = atoi(token);

    if (state_id >= 7 || state_id <= 0) {
        embeddedCliPrint(cli, "[ERROR] Invalid state");
        embeddedCliPrint(cli, "[NOTES] state = 1 : STATE_PREHEAT_HEAT");
        embeddedCliPrint(cli, "[NOTES] state = 2 : STATE_PREHEAT_COOL");
        embeddedCliPrint(cli, "[NOTES] state = 3 : STATE_HEATING");
        embeddedCliPrint(cli, "[NOTES] state = 4 : STATE_SOAK_HEAT");
        embeddedCliPrint(cli, "[NOTES] state = 5 : STATE_SOAK_COOL");
        embeddedCliPrint(cli, "[NOTES] state = 6 : STATE_COOLING");
        return;
    }

    // kP
    token = embeddedCliGetToken(args, 2);
    if (token == NULL) {
        embeddedCliPrint(cli, "[ERROR] Missing kP");
        return;
    }
    kP = atof(token);

    // kI
    token = embeddedCliGetToken(args, 3);
    if (token == NULL) {
        embeddedCliPrint(cli, "[ERROR] Missing kI");
        return;
    }
    kI = atof(token);

    // kD
    token = embeddedCliGetToken(args, 4);
    if (token == NULL) {
        embeddedCliPrint(cli, "[ERROR] Missing kD");
        return;
    }
    kD = atof(token);

    // validate
    if (kP < 0 || kI < 0 || kD < 0) {
        embeddedCliPrint(cli, "[ERROR] PID must be >= 0");

        return;
    }

    // apply
    pid_set((ProfileState_s) state_id, kP, kI, kD);

    char buf[128];
    snprintf(buf, sizeof (buf),
            "[OK] State %d PID set: Kp=%.3f Ki=%.3f Kd=%.3f",
            state_id, kP, kI, kD);
    embeddedCliPrint(cli, buf);
}

void CMD_Temp_PID_get(EmbeddedCli *cli, char *args, void *context) {
    const char *token;
    token = embeddedCliGetToken(args, 1);
    if (token == NULL) {
        embeddedCliPrint(cli, "=> Usage: temp_pid_get <state>");
        return;
    }
    int state_id = atoi(token);

    if (state_id >= 7 || state_id <= 0) {
        embeddedCliPrint(cli, "[ERROR] Invalid state");
        embeddedCliPrint(cli, "[NOTES] state = 1 : STATE_PREHEAT_HEAT");
        embeddedCliPrint(cli, "[NOTES] state = 2 : STATE_PREHEAT_COOL");
        embeddedCliPrint(cli, "[NOTES] state = 3 : STATE_HEATING");
        embeddedCliPrint(cli, "[NOTES] state = 4 : STATE_SOAK_HEAT");
        embeddedCliPrint(cli, "[NOTES] state = 5 : STATE_SOAK_COOL");
        embeddedCliPrint(cli, "[NOTES] state = 6 : STATE_COOLING");
        return;
    }

    PID_instance_t *cfg = pid_get((ProfileState_s) state_id);
    if (cfg == NULL) {
        embeddedCliPrint(cli, "[ERROR] No PID config for this state");
        return;
    }

    char buf[128];
    snprintf(buf, sizeof (buf), "State %d PID: Kp=%.3f Ki=%.3f Kd=%.3f", state_id, cfg->kp, cfg->ki, cfg->kd);
    embeddedCliPrint(cli, buf);
}