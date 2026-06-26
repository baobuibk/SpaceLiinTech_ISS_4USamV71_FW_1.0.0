#include "cli_command.h"
#include "BSP_Board/bsp_core.h"
#include "BSP_Solenoid_MAX22200/bsp_solenoid_max.h"
#include "stdio.h"
#include "stdlib.h"

void CMD_SOL_MAX_Init(EmbeddedCli *cli, char *args, void *context) {
    int status = bsp_solenoid_max_init();
    if (status == 0) {
        embeddedCliPrint(cli, "OK: Solenoid Driver Initialized Successfully.");
    } else {
        embeddedCliPrint(cli, "ERROR: Solenoid Driver Init Failed.");
    }
}

void CMD_SOL_MAX_Control(EmbeddedCli *cli, char *args, void *context) {
    char buf[128];

    const char *indStr = embeddedCliGetToken(args, 1);
    const char *dirStr = embeddedCliGetToken(args, 2);
    const char *timeMsStr = embeddedCliGetToken(args, 3);

    if (indStr == NULL || dirStr == NULL || timeMsStr == NULL) {
        embeddedCliPrint(cli,
                "Usage: sln_ctl <solenoid:1-20> <dir:0|1> <pulse_ms>");
        return;
    }

    uint8_t ind = (uint8_t) strtoul(indStr, NULL, 0);
    uint8_t dir = (uint8_t) strtoul(dirStr, NULL, 0);
    uint32_t timeMs = (uint32_t) strtoul(timeMsStr, NULL, 0);

    if (ind < 1 || ind > 20) {
        embeddedCliPrint(cli, "ERROR: Invalid solenoid index");
        return;
    }

    if (dir) {
        bsp_solenoid_set(ind - 1, 2);
        os_delay_ms(timeMs);
        bsp_solenoid_set(ind - 1, 0);
    } else {
        bsp_solenoid_set(ind - 1, 1);
        os_delay_ms(timeMs);
        bsp_solenoid_set(ind - 1, 0);
    }

    snprintf(buf, sizeof (buf),
            "OK: Solenoid %u, Direction %u, Pulse %lu ms",
            ind, dir, (unsigned long) timeMs);

    embeddedCliPrint(cli, buf);
}