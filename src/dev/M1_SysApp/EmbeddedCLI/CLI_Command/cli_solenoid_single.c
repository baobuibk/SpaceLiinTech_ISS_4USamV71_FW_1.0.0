#include "cli_command.h"
#include "BSP_Board/bsp_core.h"
#include "BSP_Solenoid/bsp_solenoid.h"
#include "stdio.h"
#include "stdlib.h"

void CMD_SOL_Single_On(EmbeddedCli *cli, char *args, void *context) {
    const char *idexStr = embeddedCliGetToken(args, 1);
    char buf[128];

    if (idexStr == NULL) {
        embeddedCliPrint(cli, "Usage: sol_single_on <index>");
        return;
    }

    uint8_t index = (uint8_t) strtoul(idexStr, NULL, 0);

    if (index < 1 | index > 16) {
        embeddedCliPrint(cli, "Index 1 to 16");
        return;
    }

    Std_ReturnType rc = bsp_sol_single_on(index - 1);

    if (rc == ERROR_OK) {
        snprintf(buf, sizeof (buf), "Single solenoid on index: %u", index);
        embeddedCliPrint(cli, buf);
    } else {
        embeddedCliPrint(cli, "Single solenoid: Fail");
    }
}

void CMD_SOL_Single_Off(EmbeddedCli *cli, char *args, void *context) {
    const char *idexStr = embeddedCliGetToken(args, 1);
    char buf[128];

    if (idexStr == NULL) {
        embeddedCliPrint(cli, "Usage: sol_single_off <index>");
        return;
    }

    uint8_t index = (uint8_t) strtoul(idexStr, NULL, 0);

    if (index < 1 | index > 16) {
        embeddedCliPrint(cli, "Index 1 to 16");
        return;
    }

    Std_ReturnType rc = bsp_sol_single_off(index - 1);

    if (rc == ERROR_OK) {
        snprintf(buf, sizeof (buf), "Single solenoid off index: %u", index);
        embeddedCliPrint(cli, buf);
    } else {
        embeddedCliPrint(cli, "Single solenoid: Fail");
    }
}

void CMD_SOL_Single_Get(EmbeddedCli *cli, char *args, void *context) {
    const char *idexStr = embeddedCliGetToken(args, 1);
    char buf[128];

    if (idexStr == NULL) {
        embeddedCliPrint(cli, "Usage: sol_single_get <index>");
        return;
    }

    uint8_t index = (uint8_t) strtoul(idexStr, NULL, 0);

    if (index < 1 | index > 16) {
        embeddedCliPrint(cli, "Index 1 to 16");
        return;
    }

    uint32_t rc;
    rc = bsp_sol_single_status(index - 1);

    if (rc == 1) {
        snprintf(buf, sizeof (buf), "Status of single solenoid %u: ON", index);
        embeddedCliPrint(cli, buf);
    } else {
        snprintf(buf, sizeof (buf), "Status of single solenoid %u: OFF", index);
        embeddedCliPrint(cli, buf);
    }
}