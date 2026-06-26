#include "cli_command.h"
#include "BSP_Board/bsp_core.h"
#include "BSP_FRAM/bsp_fram.h"
#include "stdio.h"

void CMD_FRAM_WriteRead(EmbeddedCli *cli, char *args, void *context)
{
    uint32_t addr = 0x00;
    uint8_t tx_buf[10] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA};
    uint8_t rx_buf[10];
    
    int st = bsp_fram_write(&g_fram, addr, tx_buf, 10);

    if (st != ERROR_OK) {
        embeddedCliPrint(cli, "Write FAIL\r\n");
        return;
    }

    st = bsp_fram_read(&g_fram, addr, rx_buf, 10);

    if (st != ERROR_OK) {
        embeddedCliPrint(cli, "Read FAIL\r\n");
        return;
    }

    for (uint8_t i = 0; i <10; i++)
    {
        if (tx_buf[i] != rx_buf[i])
        {
            embeddedCliPrint(cli, "Wrong data\r\n");
            return;
        }
    }
    embeddedCliPrint(cli, "PASS\r\n");
}

void CMD_BootInfo_Read(EmbeddedCli *cli, char *args, void *context)
{
    #define BOOTINFO_ROM_ADDR    0x000100UL
    #define BOOTINFO_RECORD_SIZE 12U
    #define BOOTINFO_HISTORY_LEN 8U

    uint8_t raw[BOOTINFO_RECORD_SIZE];

    int st = bsp_fram_read(&g_fram, BOOTINFO_ROM_ADDR, raw, BOOTINFO_RECORD_SIZE);
    if (st != ERROR_OK) {
        embeddedCliPrint(cli, "[BOOT] FRAM read FAIL");
        return;
    }

    uint16_t bcount = (uint16_t)(raw[0] | ((uint16_t)raw[1] << 8U));

    static const char *cause_str[] = {
        "POR/BOR", "Backup/WakeUp", "Watchdog", "Software", "User(NRST)"
    };

    uint8_t c = raw[2];  /* bhistory[0] = latest cause */
    const char *cstr = (c < 5U) ? cause_str[c] : "Unknown";

    char buf[64];

    embeddedCliPrint(cli, "[BOOT] ---- Boot Info ----");

    snprintf(buf, sizeof(buf), "[BOOT] Count   : %u (0x%04X)", (unsigned)bcount, (unsigned)bcount);
    embeddedCliPrint(cli, buf);

    snprintf(buf, sizeof(buf), "[BOOT] Cause   : 0x%02X (%s)", (unsigned)c, cstr);
    embeddedCliPrint(cli, buf);

    snprintf(buf, sizeof(buf),
             "[BOOT] History : [0]=%02X [1]=%02X [2]=%02X [3]=%02X",
             raw[2], raw[3], raw[4], raw[5]);
    embeddedCliPrint(cli, buf);
    
    snprintf(buf, sizeof(buf),
             "[BOOT]           [4]=%02X [5]=%02X [6]=%02X [7]=%02X",
             raw[6], raw[7], raw[8], raw[9]);
    embeddedCliPrint(cli, buf);

    embeddedCliPrint(cli, "[BOOT] --------------------");
}