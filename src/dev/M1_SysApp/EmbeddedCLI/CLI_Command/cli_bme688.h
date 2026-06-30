#include "M1_SysApp/EmbeddedCLI/CLI_Command/cli_command.h"

#include "samv71q21b.h"
#include "define.h"

/*************************************************
 *                Command Define                 *
 *************************************************/
void CMD_BME688_Init (EmbeddedCli *cli, char *args, void *context);

void CMD_BME688_Read_Temperature (EmbeddedCli *cli, char *args, void *context);
void CMD_BME688_Read_Pressure (EmbeddedCli *cli, char *args, void *context);
void CMD_BME688_Read_Humidity (EmbeddedCli *cli, char *args, void *context);
void CMD_BME688_Read_All (EmbeddedCli *cli, char *args, void *context);