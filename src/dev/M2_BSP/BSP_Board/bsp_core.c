#include "bsp_board/bsp_core.h"
#include "board.h"
#include "M3_Driver/components/qspi/qspi_io.h"

////////////////////////////////////////
/* Protocol */
////////////////////////////////////////

spi_io_t spi0 ={
    .ui32SpiPort = 1,
};

spi_io_t spi1 = {
    .ui32SpiPort = 2,
};

spi_io_t spi2 ={
    .ui32SpiPort = 2,
};

i2c_io_t i2c0 ={
    .ui32I2cPort = 0,
};

i2c_io_t i2c1 ={
    .ui32I2cPort = 1,
};

i2c_io_t i2c2 ={
    .ui32I2cPort = 2
};

qspi_io_t qspi = {
    .ui32SpiPort = 0
};




////////////////////////////////////////
/* Device */
////////////////////////////////////////

is66_dev_t g_psram = {
    .spi = &spi1,
    .cs = &psram_cs,
};

mb85rs2mt_dev_t g_fram = {
    .spi = &spi2,
    .cs = &fram_cs,
};






////////////////////////////////////////
/* IO */
////////////////////////////////////////

do_t laser_dac_cs ={
    .port = LASER_DAC_CS_PORT,
    .pin = LASER_DAC_CS_PIN,
    .bStatus = false,
};

do_t laser_dac_latch ={
    .port = LASER_DAC_LATCH_PORT,
    .pin = LASER_DAC_LATCH_PIN,
    .bStatus = false,
};

do_t laser_sw_int_cs ={
    .port = LASER_SW_INT_CS_PORT,
    .pin = LASER_SW_INT_CS_PIN,
    .bStatus = true,
};

do_t laser_sw_ext_cs ={
    .port = LASER_SW_EXT_CS_PORT,
    .pin = LASER_SW_EXT_CS_PIN,
    .bStatus = true,
};

do_t laser_ext_spi1_cs ={
    .port = LASER_EXT_SPI1_nCS_PORT,
    .pin = LASER_EXT_SPI1_nCS_PIN,
    .bStatus = false,
};

do_t laser_ext_dac_cs ={
    .port = LASER_EXT_DAC_CS_PORT,
    .pin = LASER_EXT_DAC_CS_PIN,
    .bStatus = true,
};

do_t laser_ext_dac_latch ={
    .port = LASER_EXT_DAC_LATCH_PORT,
    .pin = LASER_EXT_DAC_LATCH_PIN,
    .bStatus = false,
};

do_t photo_sw_cs ={
    .port = PHOTO_SW_CS_PORT,
    .pin = PHOTO_SW_CS_PIN,
    .bStatus = true,
};

do_t photo_adc_cs ={
    .port = PHOTO_ADC_CS_PORT,
    .pin = PHOTO_ADC_CS_PIN,
    .bStatus = true,
};

do_t photo_adc_conv ={
    .port = PHOTO_ADC_CONV_PORT,
    .pin = PHOTO_ADC_CONV_PIN,
    .bStatus = true,
};

do_t photo_adc_eoc ={
    .port = PHOTO_ADC_EOC_PORT,
    .pin = PHOTO_ADC_EOC_PIN,
    .bStatus = true,
};

//do_t power_som ={
//    .port = POWER_SOM_PORT,
//    .pin = POWER_SOM_PIN,
//    .bStatus = true,
//};

//do_t power_buck_peri ={
//    .port = POWER_PERI_PORT,
//    .pin = POWER_PERI_PIN,
//    .bStatus = false
//};

do_t power_tec ={
    .port = POWER_TEC_PORT,
    .pin = POWER_TEC_PIN,
    .bStatus = false
};

do_t power_sln_hd ={
    .port = POWER_SLN_HD_PORT,
    .pin = POWER_SLN_HD_PIN,
    .bStatus = false
};

//do_t power_solenoid ={
//    .port = POWER_SLN_PORT,
//    .pin = POWER_SLN_PIN,
//    .bStatus = false
//};

do_t power_lp_heater ={
    .port = POWER_LP_HEATER_PORT,
    .pin = POWER_LP_HEATER_PIN,
    .bStatus = false
};

//do_t power_heater ={
//    .port = POWER_HEATER_PORT,
//    .pin = POWER_HEATER_PIN,
//    .bStatus = false
//};


do_t status_led ={
    .port = STATUS_LED_PORT,
    .pin = STATUS_LED_PIN,
    .bStatus = false,
};

//do_t bmp390_ext_sensor5 ={
//    .port = SENSOR5_EN_PORT,
//    .pin = SENSOR5_EN_PIN,
//    .bStatus = false,
//};

do_t tec_1_cs ={
    .port = TEC_1_CS_PORT,
    .pin = TEC_1_CS_PIN,
    .bStatus = true,
};

do_t tec_1_sw ={
    .port = TEC_1_SW_PORT,
    .pin = TEC_1_SW_PIN,
    .bStatus = false,
};

do_t tec_2_cs ={
    .port = TEC_2_CS_PORT,
    .pin = TEC_2_CS_PIN,
    .bStatus = true,
};

do_t tec_2_sw ={
    .port = TEC_2_SW_PORT,
    .pin = TEC_2_SW_PIN,
    .bStatus = false,
};

//do_t tec_3_cs ={
//    .port = TEC_3_CS_PORT,
//    .pin = TEC_3_CS_PIN,
//    .bStatus = true,
//};
//
//do_t tec_3_sw ={
//    .port = TEC_3_SW_PORT,
//    .pin = TEC_3_SW_PIN,
//    .bStatus = false,
//};
//
//do_t tec_4_cs ={
//    .port = TEC_4_CS_PORT,
//    .pin = TEC_4_CS_PIN,
//    .bStatus = true,
//};
//
//do_t tec_4_sw ={
//    .port = TEC_4_SW_PORT,
//    .pin = TEC_4_SW_PIN,
//    .bStatus = false,
//};

do_t fram_cs = {
    .port = FRAM_CS_PORT,
    .pin = FRAM_CS_PIN,
    .bStatus = true,
};

do_t hd4_en = {
    .port = PUMP_EN_PORT,
    .pin = PUMP_EN_PIN,
};

do_t psram_cs = {
    .port = PSRAM_CS_PORT,
    .pin = PSRAM_CS_PIN,
    .bStatus = true,
};

do_t flow1_en = {
    .port = FLOW_1_EN_PORT,
    .pin = FLOW_1_EN_PIN,
    .bStatus = false,
};

do_t ext_sen_en = {
    .port = EXT_SEN_EN_PORT,
    .pin = EXT_SEN_EN_PIN,
    .bStatus = false,
};
//do_t flow3_en = {
//    .port = FLOW_3_EN_PORT,
//    .pin = FLOW_3_EN_PIN,
//    .bStatus = false,
//};
//do_t flow4_en = {
//    .port = FLOW_4_EN_PORT,
//    .pin = FLOW_4_EN_PIN,
//    .bStatus = false,
//};

do_t lsm6_int1 = {
    .port = LSM6_INT1_PORT,
    .pin = LSM6_INT1_PIN,
    .bStatus = false,
};

do_t wdt_done = {
    .port = WDT_DONE_PORT,
    .pin = WDT_DONE_PIN,
    .bStatus = false,
};

do_t wdt_wake = {
    .port = WDT_WAKE_PORT,
    .pin = WDT_WAKE_PIN,
    .bStatus = false,
};

do_t switch_spi_psram = {
    .port = SW_PSRAM_SEL_PORT,
    .pin = SW_PSRAM_SEL_PIN,
    .bStatus = false,
};

do_t mcu_pmu_gpio_a = {
    .port = MCU_MPU_GPIO_A_PORT,
    .pin = MCU_MPU_GPIO_A_PIN,
    .bStatus = false,
};

do_t mcu_pmu_gpio_b = {
    .port = MCU_MPU_GPIO_B_PORT,
    .pin = MCU_MPU_GPIO_B_PIN,
    .bStatus = false,
};

//do_t ifp_enable = {
//    .port = IFP_ENABLE_PORT,
//    .pin = IFP_ENABLE_PIN,
//    .bStatus = false,
//};

//do_t power_ai = {
//    .port = POWER_AI_PORT,
//    .pin  = POWER_AI_PIN,
//    .bStatus = false,
//};

do_t sole_A_cmd = {
    .port = MCU_CMD_SOLE_A_PORT,
    .pin  = MCU_CMD_SOLE_A_PIN,
    .bStatus = false,
};

do_t sole_A_cs = {
    .port = MCU_SPI0_nCS_SOLE_A_PORT,
    .pin  = MCU_SPI0_nCS_SOLE_A_PIN,
    .bStatus = true,
};

do_t sole_B_cmd = {
    .port = MCU_CMD_SOLE_B_PORT,
    .pin  = MCU_CMD_SOLE_B_PIN,
    .bStatus = false,
};

do_t sole_B_cs = {
    .port = MCU_SPI0_nCS_SOLE_B_PORT,
    .pin  = MCU_SPI0_nCS_SOLE_B_PIN,
    .bStatus = true,
};

//do_t sole_C_cmd = {
//    .port = MCU_CMD_SOLE_C_PORT,
//    .pin  = MCU_CMD_SOLE_C_PIN,
//    .bStatus = false,
//};
//
//do_t sole_C_cs = {
//    .port = MCU_SPI0_nCS_SOLE_C_PORT,
//    .pin  = MCU_SPI0_nCS_SOLE_C_PIN,
//    .bStatus = true,
//};
//
//do_t sole_D_cmd = {
//    .port = MCU_CMD_SOLE_D_PORT,
//    .pin  = MCU_CMD_SOLE_D_PIN,
//    .bStatus = false,
//};
//
//do_t sole_D_cs = {
//    .port = MCU_SPI0_nCS_SOLE_D_PORT,
//    .pin  = MCU_SPI0_nCS_SOLE_D_PIN,
//    .bStatus = true,
//};
//
//do_t sole_E_cmd = {
//    .port = MCU_CMD_SOLE_E_PORT,
//    .pin  = MCU_CMD_SOLE_E_PIN,
//    .bStatus = false,
//};
//
//do_t sole_E_cs = {
//    .port = MCU_SPI0_nCS_SOLE_E_PORT,
//    .pin  = MCU_SPI0_nCS_SOLE_E_PIN,
//    .bStatus = true,
//};