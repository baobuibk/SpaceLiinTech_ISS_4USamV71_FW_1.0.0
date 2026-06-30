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

    static const char *ch_hi[PAC1934_CHANNELS]  = { "5V_SLN_HD", "EXP_12V", "5V_TEC", "12V_LP_HEATER" };

    embeddedCliPrint(cli, "--- PAC1934 Power Monitor ---");

    if (pac1934_hi.init_status == false) {
        status = bsp_pac1934_init(&pac1934_hi);
        os_delay_ms(5);
        if (status != ERROR_OK) {
            embeddedCliPrint(cli, "PAC1934 0x1F: init fail");
            return;
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
}