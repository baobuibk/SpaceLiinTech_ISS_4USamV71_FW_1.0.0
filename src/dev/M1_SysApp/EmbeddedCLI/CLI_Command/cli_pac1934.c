#include "cli_command.h"
#include "stdio.h"

#include "BSP_PAC1934/bsp_pac1934.h"

void CMD_PAC1934_Read(EmbeddedCli *cli, char *args, void *context)
{
    (void)args;
    (void)context;

    uint32_t status;
    pac1934_data_t data;
    char msg[64];

    static const char *ch_hi[PAC1934_CHANNELS]  = { "12V_AI", "EXP_12V", "12V_Heater", "LP_12V" };
    static const char *ch_lo[PAC1934_CHANNELS]   = { "5V_HD4", "5V_TEC",  "5V_SAMA7",   "5V_solenoid" };

    embeddedCliPrint(cli, "--- PAC1934 Power Monitor ---");

    if (pac1934_hi.init_status == false) {
        status = bsp_pac1934_init(&pac1934_hi);
        os_delay_ms(5);
        if (status != ERROR_OK) {
            embeddedCliPrint(cli, "PAC1934 0x1F: init fail");
            goto read_lo;
        }
    }

    status = bsp_pac1934_read_all(&pac1934_hi, &data);
    if (status != ERROR_OK) {
        embeddedCliPrint(cli, "PAC1934 0x1F: read fail");
    } else {
        for (uint8_t i = 0U; i < PAC1934_CHANNELS; i++) {
            snprintf(msg, sizeof(msg), "%-14s %6.3fV  %6.3fA",
                     ch_hi[i], data.vbus_v[i], data.current_a[i]);
            embeddedCliPrint(cli, msg);
        }
    }

read_lo:
    if (pac1934_lo.init_status == false) {
        status = bsp_pac1934_init(&pac1934_lo);
        os_delay_ms(5);
        if (status != ERROR_OK) {
            embeddedCliPrint(cli, "PAC1934 0x10: init fail");
            return;
        }
    }

    status = bsp_pac1934_read_all(&pac1934_lo, &data);
    if (status != ERROR_OK) {
        embeddedCliPrint(cli, "PAC1934 0x10: read fail");
        return;
    }

    for (uint8_t i = 0U; i < PAC1934_CHANNELS; i++) {
        snprintf(msg, sizeof(msg), "%-14s %6.3fV  %6.3fA",
                 ch_lo[i], data.vbus_v[i], data.current_a[i]);
        embeddedCliPrint(cli, msg);
    }
}
