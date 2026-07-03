#include "cli_laser.h"
#include "BSP_Laser_Ext/bsp_laser_ext.h"

void CMD_Laser_Ext_Set_DAC(EmbeddedCli *cli, char *args, void *context) {
    const char *valStr = embeddedCliGetToken(args, 1);
    char *endptr;
    char buf[120];

    if (valStr == NULL) {
        embeddedCliPrint(cli, "Usage: laser_ext_set_dac <value>");
        return;
    }

    uint32_t val = strtoul(valStr, &endptr, 0);
    if (*endptr != '\0' || val > 150) {
        embeddedCliPrint(cli, "Invalid DAC value (0-150)");
        return;
    }

    bsp_laser_ext_set_dac((uint8_t) val);

    snprintf(buf, sizeof (buf),
            "Laser DAC set to: %lu /255", val);

    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "");
}

void CMD_Laser_Ext_Turn_On_Channel(EmbeddedCli *cli, char *args, void *context) {
    const char *chStr = embeddedCliGetToken(args, 1);
    char *endptr;
    char buf[120];

    if (chStr == NULL) {
        embeddedCliPrint(cli, "Usage: laser_ext_sw_on <channel>");
        return;
    }

    uint32_t ch = strtoul(chStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid channel");
        return;
    }
    
    if(ch < 1 || ch > 8) {
        embeddedCliPrint(cli, "Invalid channel (1-8)");
        return;
    }

    bsp_laser_ext_sw_on((uint8_t) ch);

    snprintf(buf, sizeof (buf),
            "Laser channel %lu ON", ch);

    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "");
}

void CMD_Laser_Ext_Turn_Off_All(EmbeddedCli *cli, char *args, void *context) {
    bsp_laser_ext_all_sw_off();
}