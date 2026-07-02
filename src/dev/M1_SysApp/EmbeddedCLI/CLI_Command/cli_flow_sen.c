#include "cli_command.h"
#include "BSP_Board/bsp_core.h"
#include "BSP_Flow_Sen/bsp_flow_sen.h"

void CMD_Flow_Sen_Init(EmbeddedCli *cli, char *args, void *context) {
    char buf[128];
    const char *indStr = embeddedCliGetToken(args, 1);

    if (indStr == NULL) {
        embeddedCliPrint(cli, "Usage: flow_init <sensor:1-4>");
        return;
    }

    uint8_t ind = (uint8_t) strtoul(indStr, NULL, 0);

    int ret = bsp_flow_sen_init(ind - 1);
    if (ret == ERROR_OK) {
        snprintf(buf, sizeof (buf), "Flow %d init successful", ind);
    } else
        snprintf(buf, sizeof (buf), "Flow %d init failed", ind);

    embeddedCliPrint(cli, buf);
}

void CMD_Flow_Sen_ReadAll(EmbeddedCli *cli, char *args, void *context) {
    char buf[128];
    const char *indStr = embeddedCliGetToken(args, 1);

    if (indStr == NULL) {
        embeddedCliPrint(cli, "Usage: flow_read_all <sensor:1-4>");
        return;
    }

    uint8_t ind = (uint8_t) strtoul(indStr, NULL, 0);

    if ((ind < 1U) || (ind > 4U)) {
        embeddedCliPrint(cli, "Error: sensor index must be 1-4");
        return;
    }

    int ret = bsp_flow_sen_read_all(ind - 1);

    if (ret != ERROR_OK) {
        snprintf(buf, sizeof (buf), "Read sensor %u failed (%d)", ind, ret);
        embeddedCliPrint(cli, buf);
        return;
    }

    snprintf(buf, sizeof (buf),
            "Sensor %u: Flow=%.3f ml/min, Temp=%.2f C, Flags=0x%04X",
            ind,
            flow_dev[ind - 1].data.flow,
            flow_dev[ind - 1].data.temp,
            flow_dev[ind - 1].data.flags);

    embeddedCliPrint(cli, buf);
}