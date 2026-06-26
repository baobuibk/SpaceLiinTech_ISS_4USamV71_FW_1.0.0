#include "cli_command.h"
#include "BSP_Board/bsp_core.h"
#include "BSP_TEC/bsp_tec.h"
#include "stdio.h"
#include "stdlib.h"

void CMD_TEC_Init(EmbeddedCli *cli, char *args, void *context) {
    const char *indexStr = embeddedCliGetToken(args, 1);
    char *endptr;
    char buf[128];

    if (indexStr == NULL) {
        embeddedCliPrint(cli, "Usage: tec_init <index>");
        return;
    }

    uint32_t index = strtoul(indexStr, &endptr, 0);

    /* Check convert error */
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid index");
        return;
    }

    /* Check index range (v? d? TEC1..TECn) */
    if (index == 0 || index > TEC_MAX_NUM) {
        snprintf(buf, sizeof (buf), "Index out of range (1-%d)", TEC_MAX_NUM);
        embeddedCliPrint(cli, buf);
        return;
    }

    uint32_t ret = bsp_tec_init(p_tec[index - 1]);

    snprintf(buf, sizeof (buf), "TEC[%lu] %s", index, (ret == 0) ? "init success" : "init fail");

    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "");
}

void CMD_TEC_Set_Vol(EmbeddedCli *cli, char *args, void *context) {
    const char *indexStr = embeddedCliGetToken(args, 1);
    const char *valStr = embeddedCliGetToken(args, 2);
    char *endptr;
    char buf[128];

    if (indexStr == NULL || valStr == NULL) {
        embeddedCliPrint(cli, "Usage: tec_vol <index> <mVol>");
        return;
    }

    uint32_t index = strtoul(indexStr, &endptr, 0);
    if (*endptr != '\0' || index == 0 || index > TEC_MAX_NUM) {
        embeddedCliPrint(cli, "Invalid index");
        return;
    }

    uint32_t mVol = strtoul(valStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid voltage");
        return;
    }

    bsp_tec_set_output_voltage_channel(p_tec[index - 1], (int64_t) (mVol * 1000000));

    snprintf(buf, sizeof (buf), "TEC[%lu] output voltage = %lu mV", index, mVol);
    embeddedCliPrint(cli, buf);
}

void CMD_TEC_En(EmbeddedCli *cli, char *args, void *context) {
    const char *indexStr = embeddedCliGetToken(args, 1);
    char *endptr;
    char buf[64];

    if (indexStr == NULL) {
        embeddedCliPrint(cli, "Usage: tec_en <index>");
        return;
    }

    uint32_t index = strtoul(indexStr, &endptr, 0);
    if (*endptr != '\0' || index == 0 || index > TEC_MAX_NUM) {
        embeddedCliPrint(cli, "Invalid index");
        return;
    }

    bsp_tec_set_enable_req(p_tec[index - 1]);

    snprintf(buf, sizeof (buf), "TEC[%lu] enable request sent", index);
    embeddedCliPrint(cli, buf);
}

void CMD_TEC_SWen(EmbeddedCli *cli, char *args, void *context) {
    const char *indexStr = embeddedCliGetToken(args, 1);
    char *endptr;
    char buf[64];

    if (indexStr == NULL) {
        embeddedCliPrint(cli, "Usage: tec_swen <index>");
        return;
    }

    uint32_t index = strtoul(indexStr, &endptr, 0);
    if (*endptr != '\0' || index == 0 || index > TEC_MAX_NUM) {
        embeddedCliPrint(cli, "Invalid index");
        return;
    }

    bsp_tec_set_swen_req(p_tec[index - 1]);

    snprintf(buf, sizeof (buf), "TEC[%lu] SW enable request sent", index);
    embeddedCliPrint(cli, buf);
}

void CMD_TEC_Read_Reg(EmbeddedCli *cli, char *args, void *context) {
    const char *indexStr = embeddedCliGetToken(args, 1);
    const char *addStr = embeddedCliGetToken(args, 2);
    char *endptr;
    char buf[120];

    if (indexStr == NULL || addStr == NULL) {
        embeddedCliPrint(cli, "Usage: tec_read <index> <addr>");
        return;
    }

    uint32_t index = strtoul(indexStr, &endptr, 0);
    if (*endptr != '\0' || index == 0 || index > TEC_MAX_NUM) {
        embeddedCliPrint(cli, "Invalid index");
        return;
    }

    uint32_t add = strtoul(addStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid address");
        return;
    }

    uint32_t reg_val = 0;

    lt8722_reg_read(p_tec[index - 1], add, &reg_val);

    snprintf(buf, sizeof (buf),
            "TEC[%lu] REG ADD: 0x%02lX  VAL: 0x%08lX",
            index, add, reg_val);

    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "");
}