#include "M1_SysApp/EmbeddedCLI/CLI_Command/cli_command.h"


/*************************************************
 *                Command Define                 *
 *************************************************/
void CMD_LSM6DSOX_Init (EmbeddedCli *cli, char *args, void *context);
void CMD_LSM6DSOX_Dis_Int (EmbeddedCli *cli, char *args, void *context);

void CMD_LSM6DSOX_Config_Accel (EmbeddedCli *cli, char *args, void *context);
void CMD_LSM6DSOX_Config_Gyro (EmbeddedCli *cli, char *args, void *context);
void CMD_LSM6DSOX_Config_Int (EmbeddedCli *cli, char *args, void *context);
void CMD_LSM6DSOX_Config_FF (EmbeddedCli *cli, char *args, void *context);
void CMD_LSM6DSOX_Config_WU (EmbeddedCli *cli, char *args, void *context);

void CMD_LSM6DSOX_Check (EmbeddedCli *cli, char *args, void *context);
void CMD_LSM6DSOX_Read_All (EmbeddedCli *cli, char *args, void *context);
void CMD_LSM6DSOX_Read_Accel (EmbeddedCli *cli, char *args, void *context);
void CMD_LSM6DSOX_Read_Gyro (EmbeddedCli *cli, char *args, void *context);
void CMD_LSM6DSOX_Read_Temp (EmbeddedCli *cli, char *args, void *context);