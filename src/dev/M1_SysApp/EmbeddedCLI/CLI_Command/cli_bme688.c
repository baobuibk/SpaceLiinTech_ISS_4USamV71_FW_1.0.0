#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "samv71q21b.h"
#include "define.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Setup/cli_setup.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Command/cli_command.h"
#include "BSP_BME688/bsp_bme688.h"

extern bme688_dev_t bme688_int;
extern i2c_io_t i2c1;

bme688_data_t bme688_all_data = {0};
float bme688_temperature = 0.0f;
float bme688_pressure = 0.0f;
float bme688_humidity = 0.0f;

/*************************************************
 *             Command List Function             *
 *************************************************/
void CMD_BME688_Init(EmbeddedCli *cli, char *args, void *context) {
    if (bsp_bme688_init(&bme688_int) != ERROR_OK) {
        embeddedCliPrint(cli, "BME688 Init Fail");
    } else {
        if (bsp_bme688_read_all(&bme688_int, &bme688_all_data) != ERROR_OK) {
            return;
        }
        embeddedCliPrint(cli, "BME688 Init OK");
    }
}

void CMD_BME688_Read_Temperature(EmbeddedCli *cli, char *args, void *context) {
    char buf[32];

    if (bsp_bme688_read_temperature(&bme688_int, &bme688_temperature) != ERROR_OK) {
        embeddedCliPrint(cli, "[BME688] Read temperature FAIL");
        return;
    }

    snprintf(buf, sizeof (buf), "[BME688] Temp : %.2f C", bme688_temperature);
    embeddedCliPrint(cli, buf);
}

void CMD_BME688_Read_Pressure(EmbeddedCli *cli, char *args, void *context) {
    char buf[32];

    if (bsp_bme688_read_pressure(&bme688_int, &bme688_pressure) != ERROR_OK) {
        embeddedCliPrint(cli, "[BME688] Read pressure FAIL");
        return;
    }

    snprintf(buf, sizeof (buf), "[BME688] Press: %.2f Pa", bme688_pressure);
    embeddedCliPrint(cli, buf);
}

void CMD_BME688_Read_Humidity(EmbeddedCli *cli, char *args, void *context) {
    char buf[32];

    if (bsp_bme688_read_humidity(&bme688_int, &bme688_humidity) != ERROR_OK) {
        embeddedCliPrint(cli, "[BME688] Read humidity FAIL");
        return;
    }

    snprintf(buf, sizeof (buf), "[BME688] Hum  : %.2f %%", bme688_humidity);
    embeddedCliPrint(cli, buf);
}

void CMD_BME688_Read_All(EmbeddedCli *cli, char *args, void *context) {
    char buf[48];

    if (bsp_bme688_read_all(&bme688_int, &bme688_all_data) != ERROR_OK) {
        embeddedCliPrint(cli, "[BME688] Read all FAIL");
        return;
    }

    snprintf(buf, sizeof (buf), "[BME688] Temp : %.2f C", bme688_all_data.temperature);
    embeddedCliPrint(cli, buf);

    snprintf(buf, sizeof (buf), "[BME688] Press: %.2f Pa", bme688_all_data.pressure);
    embeddedCliPrint(cli, buf);

    snprintf(buf, sizeof (buf), "[BME688] Hum  : %.2f %%", bme688_all_data.humidity);
    embeddedCliPrint(cli, buf);
}