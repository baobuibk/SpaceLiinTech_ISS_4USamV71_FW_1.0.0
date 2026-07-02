#include "stdio.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Setup/cli_setup.h"
#include "stdlib.h"
#include "string.h"
#include "os.h"
#include "M2_BSP/BSP_Pump/bsp_pump.h"

void CMD_PUMP_Enable(EmbeddedCli *cli, char *args, void *context) {
    char buf[128];
    Std_ReturnType r;

    r = bsp_pump_enable(true);

    if (r != ERROR_OK) {
        snprintf(buf, sizeof (buf), "HighDriver PUMP Enable Fail: %u", r);
        embeddedCliPrint(cli, buf);
    } else embeddedCliPrint(cli, "Enable HighDriver PUMP");
}

void CMD_PUMP_Disable(EmbeddedCli *cli, char *args, void *context) {
    char buf[128];
    Std_ReturnType r;

    r = bsp_pump_enable(false);

    if (r != ERROR_OK) {
        snprintf(buf, sizeof (buf), "HighDriver PUMP Disable Fail: %u", r);
        embeddedCliPrint(cli, buf);
    } else embeddedCliPrint(cli, "Disable HighDriver PUMP");
}

void CMD_PUMP_Init(EmbeddedCli *cli, char *args, void *context) {
    char buf[128];

    Std_ReturnType r = bsp_pump_init();
    if (r != ERROR_OK) {
        snprintf(buf, sizeof (buf), "HighDriver PUMP Init Fail: %u", r);
        embeddedCliPrint(cli, buf);
    } else {
        embeddedCliPrint(cli, "Init HighDriver PUMP");
    }
}

void CMD_PUMP_Set_Volt(EmbeddedCli *cli, char *args, void *context) {
    char buf[128];

    const char *VoltStr = embeddedCliGetToken(args, 1);

    if (VoltStr == NULL) {
        snprintf(buf, sizeof (buf), "Usage: pump_set_volt <Vpp>");
        embeddedCliPrint(cli, buf);
        return;
    }

    uint8_t volt = (uint8_t) strtoul(VoltStr, NULL, 0);

    Std_ReturnType r;

    r = bsp_pump_set_voltage(volt);

    if (r != ERROR_OK) {
        snprintf(buf, sizeof (buf), "Volt set Fail: %u", r);
        embeddedCliPrint(cli, buf);
    } else {
        snprintf(buf, sizeof (buf), "Volt set channel %u: %u Vpp", volt);
        embeddedCliPrint(cli, buf);
    }
}

void CMD_PUMP_Set_Freq(EmbeddedCli *cli, char *args, void *context) {
    char buf[128];

    const char *FreqStr = embeddedCliGetToken(args, 1);

    if (FreqStr == NULL) {
        snprintf(buf, sizeof (buf), "Usage: pump_set_freq <Hz>");
        embeddedCliPrint(cli, buf);
        return;
    }

    uint16_t freq = (uint16_t) strtoul(FreqStr, NULL, 0);

    Std_ReturnType r;

    r = bsp_pump_set_freq(freq);

    if (r != ERROR_OK) {
        snprintf(buf, sizeof (buf), "Freq set Fail: %u", r);
        embeddedCliPrint(cli, buf);
    } else {
        snprintf(buf, sizeof (buf), "Freq set: %u Hz", freq);
        embeddedCliPrint(cli, buf);
    }
}

void CMD_PUMP_Stat(EmbeddedCli *cli, char *args, void *context) {
    char buf[160];
    uint8_t val;
    uint32_t r;

    static const struct {
        uint8_t reg;
        const char *name;
    } regs[] = {
        { I2C_DEVICEID, "DEVICEID    "},
        { I2C_POWERMODE, "POWERMODE   "},
        { I2C_FREQUENCY, "FREQUENCY   "},
        { I2C_SHAPE, "SHAPE       "},
        { I2C_BOOST, "BOOST       "},
        { I2C_AUDIO, "AUDIO       "},
        { I2C_P1VOLTAGE, "P1VOLTAGE   "},
        { I2C_P2VOLTAGE, "P2VOLTAGE   "},
        { I2C_P3VOLTAGE, "P3VOLTAGE   "},
        { I2C_P4VOLTAGE, "P4VOLTAGE   "},
    };

    for (uint8_t i = 0; i < sizeof (regs) / sizeof (regs[0]); i++) {
        val = 0xFFU;
        r = bsp_pump_read_reg(regs[i].reg, &val);

        if (r != ERROR_OK) {
            snprintf(buf, sizeof (buf), "0x%02X %s: READ FAIL (r=%lu)",
                    regs[i].reg, regs[i].name, (unsigned long) r);
        } else {
            snprintf(buf, sizeof (buf), "0x%02X %s: 0x%02X", regs[i].reg, regs[i].name, val);
        }
        embeddedCliPrint(cli, buf);

        if (regs[i].reg == I2C_POWERMODE && r == ERROR_OK) {
            snprintf(buf, sizeof (buf), "      -> OVRTEMP=%u  EN=%u",
                    (val >> 7) & 0x01U, val & 0x01U);
            embeddedCliPrint(cli, buf);
        }

        if (regs[i].reg == I2C_DEVICEID && r == ERROR_OK) {
            snprintf(buf, sizeof (buf), "      -> ID 0x%02X", I2C_PRODUCT_ID);
            embeddedCliPrint(cli, buf);
        }
    }

    embeddedCliPrint(cli, "---------------------------------------");
}