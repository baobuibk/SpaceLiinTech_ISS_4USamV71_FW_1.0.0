#include "cli_command.h"
#include "BSP_Board/bsp_core.h"
#include "BSP_POWER/bsp_power.h"
#include "stdio.h"

//void CMD_PowerSOM_ON(EmbeddedCli *cli, char *args, void *context) {
//    bsp_power_som_on();
//    embeddedCliPrint(cli, "Power ON Success");
//}
//
//void CMD_PowerSOM_OFF(EmbeddedCli *cli, char *args, void *context) {
//    bsp_power_som_off();
//    embeddedCliPrint(cli, "Power OFF Success");
//}
//
//void CMD_PowerSOM_Get(EmbeddedCli *cli, char *args, void *context) {
//    if (bsp_power_som_status())
//        embeddedCliPrint(cli, "SOM Power: ON");
//    else
//        embeddedCliPrint(cli, "SOM Power: OFF");
//}

//void CMD_PowerBuckPeri_ON(EmbeddedCli *cli, char *args, void *context) {
//    bsp_power_buck_peri_on();
//    embeddedCliPrint(cli, "Power ON Success");
//}
//
//void CMD_PowerBuckPeri_OFF(EmbeddedCli *cli, char *args, void *context) {
//    bsp_power_buck_peri_off();
//    embeddedCliPrint(cli, "Power OFF Success");
//}
//
//void CMD_PowerBuckPeri_Get(EmbeddedCli *cli, char *args, void *context) {
//    if (bsp_power_buck_peri_status())
//        embeddedCliPrint(cli, "BUCK_PERI Power: ON");
//    else
//        embeddedCliPrint(cli, "BUCK_PERI Power: OFF");
//}

void CMD_PowerTec_ON(EmbeddedCli *cli, char *args, void *context) {
    bsp_power_tec_on();
    embeddedCliPrint(cli, "Power ON Success");
}

void CMD_PowerTec_OFF(EmbeddedCli *cli, char *args, void *context) {
    bsp_power_tec_off();
    embeddedCliPrint(cli, "Power OFF Success");
}

void CMD_PowerTec_Get(EmbeddedCli *cli, char *args, void *context) {
    if (bsp_power_tec_status())
        embeddedCliPrint(cli, "TEC Power: ON");
    else
        embeddedCliPrint(cli, "TEC Power: OFF");
}

void CMD_PowerSLN_HD_ON(EmbeddedCli *cli, char *args, void *context) {
    bsp_power_sln_hd_on();
    embeddedCliPrint(cli, "Power ON Success");
}

void CMD_PowerSLN_HD_OFF(EmbeddedCli *cli, char *args, void *context) {
    bsp_power_sln_hd_off();
    embeddedCliPrint(cli, "Power OFF Success");
}

void CMD_PowerSLN_HD_Get(EmbeddedCli *cli, char *args, void *context) {
    if (bsp_power_sln_hd_status())
        embeddedCliPrint(cli, "SLN HD Power: ON");
    else
        embeddedCliPrint(cli, "SLN HD Power: OFF");
}

//void CMD_PowerSolenoid_ON(EmbeddedCli *cli, char *args, void *context) {
//    bsp_power_solenoid_on();
//    embeddedCliPrint(cli, "Power ON Success");
//}
//
//void CMD_PowerSolenoid_OFF(EmbeddedCli *cli, char *args, void *context) {
//    bsp_power_solenoid_off();
//    embeddedCliPrint(cli, "Power OFF Success");
//}
//
//void CMD_PowerSolenoid_Get(EmbeddedCli *cli, char *args, void *context) {
//    if (bsp_power_solenoid_status())
//        embeddedCliPrint(cli, "Solenoid Power: ON");
//    else
//        embeddedCliPrint(cli, "Solenoid Power: OFF");
//}

void CMD_PowerLP_Heater_ON(EmbeddedCli *cli, char *args, void *context) {
    bsp_power_lp_heater_on();
    embeddedCliPrint(cli, "Power ON Success");
}

void CMD_PowerLP_Heater_OFF(EmbeddedCli *cli, char *args, void *context) {
    bsp_power_lp_heater_off();
    embeddedCliPrint(cli, "Power OFF Success");
}

void CMD_PowerLP_Heater_Get(EmbeddedCli *cli, char *args, void *context) {
    if (bsp_power_lp_heater_status())
        embeddedCliPrint(cli, "LP Power: ON");
    else
        embeddedCliPrint(cli, "LP Power: OFF");
}

//void CMD_PowerHeater_ON(EmbeddedCli *cli, char *args, void *context) {
//    bsp_power_heater_on();
//    embeddedCliPrint(cli, "Power ON Success");
//}
//
//void CMD_PowerHeater_OFF(EmbeddedCli *cli, char *args, void *context) {
//    bsp_power_heater_off();
//    embeddedCliPrint(cli, "Power OFF Success");
//}
//
//void CMD_PowerHeater_Get(EmbeddedCli *cli, char *args, void *context) {
//    if (bsp_power_heater_status())
//        embeddedCliPrint(cli, "Heater Power: ON");
//    else
//        embeddedCliPrint(cli, "Heater Power: OFF");
//}

//#if (HW_VERSION == HW_VERSION_V110)
//void CMD_PowerAI_ON(EmbeddedCli *cli, char *args, void *context) {
//    bsp_power_AI_on();
//    embeddedCliPrint(cli, "Power ON Success");
//    embeddedCliPrint(cli, "");
//}
//
//void CMD_PowerAI_OFF(EmbeddedCli *cli, char *args, void *context) {
//    bsp_power_AI_off();
//    embeddedCliPrint(cli, "Power OFF Success");
//    embeddedCliPrint(cli, "");
//}
//
//void CMD_PowerAI_Get(EmbeddedCli *cli, char *args, void *context) {
//    if (bsp_power_AI_status())
//        embeddedCliPrint(cli, "AI Power: ON");
//    else
//        embeddedCliPrint(cli, "AI Power: OFF");
//    embeddedCliPrint(cli, "");
//}
//#endif

void CMD_PowerAll_ON(EmbeddedCli *cli, char *args, void *context) {
    power_on_all();
    embeddedCliPrint(cli, "Power ON Success");
    embeddedCliPrint(cli, "");
}

void CMD_PowerAll_OFF(EmbeddedCli *cli, char *args, void *context) {
    power_off_all();
    embeddedCliPrint(cli, "Power OFF Success");
}

void CMD_PowerAll_Get(EmbeddedCli *cli, char *args, void *context) {
    char str[64];

    embeddedCliPrint(cli, "========== Power Tree ==========");

//    snprintf(str, sizeof (str), "SOM                : %s",
//            bsp_power_som_status() ? "ON" : "OFF");
//    embeddedCliPrint(cli, str);

//    snprintf(str, sizeof (str), "Buck Peripherals   : %s",
//            bsp_power_buck_peri_status() ? "ON" : "OFF");
//    embeddedCliPrint(cli, str);

    snprintf(str, sizeof (str), "|-- TEC            : %s",
            bsp_power_tec_status() ? "ON" : "OFF");
    embeddedCliPrint(cli, str);

    snprintf(str, sizeof (str), "|-- SLN HD         : %s",
            bsp_power_sln_hd_status() ? "ON" : "OFF");
    embeddedCliPrint(cli, str);

//    snprintf(str, sizeof (str), "|-- Solenoid       : %s",
//            bsp_power_solenoid_status() ? "ON" : "OFF");
//    embeddedCliPrint(cli, str);

    snprintf(str, sizeof (str), "LP Heater          : %s",
            bsp_power_lp_heater_status() ? "ON" : "OFF");
    embeddedCliPrint(cli, str);

//    snprintf(str, sizeof (str), "Heater             : %s",
//            bsp_power_heater_status() ? "ON" : "OFF");
//    embeddedCliPrint(cli, str);

//#if (HW_VERSION == HW_VERSION_V110)
//    snprintf(str, sizeof (str), "AI Board           : %s",
//            bsp_power_AI_status() ? "ON" : "OFF");
//    embeddedCliPrint(cli, str);
//#endif

    embeddedCliPrint(cli, "================================");
}