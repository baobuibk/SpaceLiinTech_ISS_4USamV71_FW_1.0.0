#include "cli_command.h"
#include "BSP_Board/bsp_core.h"
#include "BSP_Solenoid_dual/bsp_sol_dual.h"
#include "stdio.h"

void CMD_SOL_Dual_Forward(EmbeddedCli *cli, char *args, void *context) {
#if (HW_VERSION == HW_VERSION_V100)
    const char *idexStr = embeddedCliGetToken(args, 1);
    const char *pulseStr = embeddedCliGetToken(args, 2);

    if (idexStr == NULL) {
        embeddedCliPrint(cli, "Usage: sol_dual_forward <index> <pulse>");
        return;
    }

    uint8_t pulse = (uint8_t) strtoul(pulseStr, NULL, 0);
    uint8_t index = (uint8_t) strtoul(idexStr, NULL, 0);

    if (pulseStr == NULL) {
        pulse = 100;
    }

    if ((index < 1) || (index > 4)) {
        embeddedCliPrint(cli, "Index 1 to 4");
        return;
    }

    bsp_sol_dual_forward(&sol_dual[index - 1], pulse);
#else
    embeddedCliPrint(cli, "HW110 dont have SOL DUAL");
#endif
}

void CMD_SOL_Dual_Reverse(EmbeddedCli *cli, char *args, void *context) {
#if (HW_VERSION == HW_VERSION_V100)
    const char *idexStr = embeddedCliGetToken(args, 1);
    const char *pulseStr = embeddedCliGetToken(args, 2);

    if (idexStr == NULL) {
        embeddedCliPrint(cli, "Usage: sol_dual_reverse <index> <pulse>");
        return;
    }

    uint8_t index = (uint8_t) strtoul(idexStr, NULL, 0);
    uint8_t pulse = (uint8_t) strtoul(pulseStr, NULL, 0);

    if (pulseStr == NULL) {
        pulse = 100;
    }

    if (index < 1 || index > 4) {
        embeddedCliPrint(cli, "Index 1 to 4");
        return;
    }

    bsp_sol_dual_reverse(&sol_dual[index - 1], pulse);

#else
    embeddedCliPrint(cli, "HW110 dont have SOL DUAL");
#endif
}