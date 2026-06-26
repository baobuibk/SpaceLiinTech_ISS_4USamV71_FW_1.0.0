#ifndef _BOARD_V71_ISS_H_
#define _BOARD_V71_ISS_H_


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "samv71q21b.h"

#define MPU_nSTART_PORT        3   // PC30
#define MPU_nSTART_PIN         30

#define PB12_PORT        2   // PB12
#define PB12_PIN         12

/* ===================== POWER CONTROL ===================== */
//#define POWER_SOM_PORT        4   // PD5
//#define POWER_SOM_PIN         5
//
//#define POWER_PERI_PORT       5   // PE4
//#define POWER_PERI_PIN        4

#define POWER_TEC_PORT        4   // PD4
#define POWER_TEC_PIN         4

//#define POWER_HD4_PORT        4   // PD3
//#define POWER_HD4_PIN         3

#define POWER_SLN_HD_PORT        4   // PD2
#define POWER_SLN_HD_PIN         2

#define POWER_LP_HEATER_PORT         4   // PD6
#define POWER_LP_HEATER_PIN          6

//#define POWER_HEATER_PORT     1   // PA7
//#define POWER_HEATER_PIN      7

//#define POWER_AI_PORT         1   // PA2
//#define POWER_AI_PIN          2

/* ===================== LASER ===================== */

#define LASER_DAC_LATCH_PORT  3   // PC16
#define LASER_DAC_LATCH_PIN   16

#define LASER_DAC_CS_PORT     1   // PA31
#define LASER_DAC_CS_PIN      31

#define LASER_SW_INT_CS_PORT  3   // PC07
#define LASER_SW_INT_CS_PIN   7

#define LASER_SW_EXT_CS_PORT  1   // PA16
#define LASER_SW_EXT_CS_PIN   16

#define LASER_EXT_SPI1_nCS_PORT  1   // PA22
#define LASER_EXT_SPI1_nCS_PIN   22

#define LASER_EXT_DAC_LATCH_PORT  3   // PC01
#define LASER_EXT_DAC_LATCH_PIN   1

#define LASER_EXT_DAC_CS_PORT     1   // PA07
#define LASER_EXT_DAC_CS_PIN      7

/* ===================== PHOTO ===================== */

#define PHOTO_SW_CS_PORT      4   // PD12
#define PHOTO_SW_CS_PIN       12

#define PHOTO_ADC_CS_PORT     4   // PD0
#define PHOTO_ADC_CS_PIN      0

#define PHOTO_ADC_CONV_PORT   1   // PA15
#define PHOTO_ADC_CONV_PIN    15

#define PHOTO_ADC_EOC_PORT    1   // PA18
#define PHOTO_ADC_EOC_PIN     18

/* ===================== SPI0 ===================== */

#define SPI0_SCK_PORT         4   // PD22
#define SPI0_SCK_PIN          22

#define SPI0_MOSI_PORT        4   // PD21
#define SPI0_MOSI_PIN         21

#define SPI0_MISO_PORT        4   // PD20
#define SPI0_MISO_PIN         20


/* ===================== STATUS ===================== */

#define STATUS_LED_PORT       1   // PA05
#define STATUS_LED_PIN        5


/* ===================== HEATER PWM ===================== */

#define HTR_PWM_1_PORT        1   // PA27
#define HTR_PWM_1_PIN         27

#define HTR_PWM_2_PORT        1   // PA00
#define HTR_PWM_2_PIN         0

//#define HTR_PWM_3_PORT        1   // PA01
//#define HTR_PWM_3_PIN         1
//
//#define HTR_PWM_4_PORT        1   // PA26
//#define HTR_PWM_4_PIN         26
//
//#define HTR_PWM_5_PORT        3   // PC08
//#define HTR_PWM_5_PIN         8
//
//#define HTR_PWM_6_PORT        3   // PC23
//#define HTR_PWM_6_PIN         23
//
//#define HTR_PWM_7_PORT        3   // PC05
//#define HTR_PWM_7_PIN         5
//
//#define HTR_PWM_8_PORT        3   // PC06
//#define HTR_PWM_8_PIN         6


/* ===================== TEC ===================== */

#define TEC_1_CS_PORT         3   // PC18
#define TEC_1_CS_PIN          18

#define TEC_1_SW_PORT         1   // PA30
#define TEC_1_SW_PIN          30


#define TEC_2_CS_PORT         3   // PC20
#define TEC_2_CS_PIN          20

#define TEC_2_SW_PORT         4   // PD13
#define TEC_2_SW_PIN          13

#define TEC_ADC_1_PORT        4   // PD30
#define TEC_ADC_1_PIN         30

#define TEC_ADC_2_PORT        5   // PE04
#define TEC_ADC_2_PIN         4


//#define TEC_3_CS_PORT         3   // PC22
//#define TEC_3_CS_PIN          22
//
//#define TEC_3_SW_PORT         3   // PC01
//#define TEC_3_SW_PIN          1
//
//
//#define TEC_4_CS_PORT         4   // PD11
//#define TEC_4_CS_PIN          11
//
//#define TEC_4_SW_PORT         3   // PC02
//#define TEC_4_SW_PIN          2


/* ===================== USART2 SPI ===================== */

#define USART2_SPI_SCK_PORT   4   // PD17
#define USART2_SPI_SCK_PIN    17

#define USART2_SPI_MOSI_PORT  4   // PD16
#define USART2_SPI_MOSI_PIN   16

#define USART2_SPI_MISO_PORT  4   // PD15
#define USART2_SPI_MISO_PIN   15


/* ===================== I2C ===================== */

#define I2C0_SDA_PORT         1   // PA03
#define I2C0_SDA_PIN          3

#define I2C0_SCL_PORT         1   // PA04
#define I2C0_SCL_PIN          4

#define I2C1_SDA_PORT         2   // PB04
#define I2C1_SDA_PIN          4

#define I2C1_SCL_PORT         2   // PB05
#define I2C1_SCL_PIN          5

#define I2C2_SDA_PORT         4   // PD27
#define I2C2_SDA_PIN          27

#define I2C2_SCL_PORT         4   // PD28
#define I2C2_SCL_PIN          28

/* ===================== SENSOR ===================== */

//#define SENSOR5_EN_PORT       3     // PC11
//#define SENSOR5_EN_PIN        11

#define FRAM_CS_PORT          4     // PD19
#define FRAM_CS_PIN           19

/* ===================== NTC ===================== */
#define ADC_TEMP1_PORT        1  //PA19
#define ADC_TEMP1_PIN         19

#define ADC_TEMP2_PORT        1  //PA20
#define ADC_TEMP2_PIN         20

#define ADC_TEMP3_PORT        3  //PC31
#define ADC_TEMP3_PIN         31

#define ADC_TEMP4_PORT        2  //PB1
#define ADC_TEMP4_PIN         1

#define ADC_TEMP5_PORT        3  //PC15
#define ADC_TEMP5_PIN         15

#define ADC_TEMP6_PORT        3  //PC13
#define ADC_TEMP6_PIN         13

#define ADC_TEMP7_PORT        3  //PC0
#define ADC_TEMP7_PIN         0

#define ADC_TEMP8_PORT        3  //PC29
#define ADC_TEMP8_PIN         29

/* ==============ADC SENSOR============*/
#define ADC_TEMP_SENSOR_PORT  5 // PE3
#define ADC_TEMP_SENSOR_PIN   3

/* ===================== PUMP ===================== */
#define PUMP_EN_PORT        3  //PC17
#define PUMP_EN_PIN         17 //PC17

/* ============= SW SPI FOR PSRAM ================= */
#define SW_PSRAM_SEL_PORT    1  // PA8
#define SW_PSRAM_SEL_PIN     8

/* ===================== SPI0 ===================== */
#define PSRAM_CS_PORT         3   // PC28
#define PSRAM_CS_PIN          28

#define SPI1_SCK_PORT         3   // PC24
#define SPI1_SCK_PIN          24

#define SPI1_MOSI_PORT        3   // PC27
#define SPI1_MOSI_PIN         27

#define SPI1_MISO_PORT        3   // PC26
#define SPI1_MISO_PIN         26

/* ===================== I2C FLOW_SENSOR ===================== */
#define FLOW_1_EN_PORT        3   // PC9
#define FLOW_1_EN_PIN         9

#define EXT_SEN_EN_PORT        3   // PC10
#define EXT_SEN_EN_PIN         10

//#define FLOW_3_EN_PORT        1   // PA28
//#define FLOW_3_EN_PIN         28
//
//#define FLOW_4_EN_PORT        1   // PA29
//#define FLOW_4_EN_PIN         29

/* ===================== LSM6DSOX ===================== */
#define LSM6_INT1_PORT        3   // PC21
#define LSM6_INT1_PIN         21

/* ==================== UART0 ===================== */
#define UART0_RX_PORT         1   // PA9
#define UART0_RX_PIN          9

#define UART0_TX_PORT         1   // PA10
#define UART0_TX_PIN          10

/* ==================== UART2 ===================== */
#define UART2_RX_PORT         4   // PD25
#define UART2_RX_PIN          25

#define UART2_TX_PORT         4   // PD26
#define UART2_TX_PIN          26

/* ==================== WATCHDOG ===================== */
#define WDT_DONE_PORT      5   //  PE1
#define WDT_DONE_PIN       1

#define WDT_WAKE_PORT      5   //  PE2
#define WDT_WAKE_PIN       2

/* =====================GPIO MCU-PMU==================*/
#define MCU_MPU_GPIO_A_PORT     4   // PD09
#define MCU_MPU_GPIO_A_PIN      9

#define MCU_MPU_GPIO_B_PORT     4   // PD10
#define MCU_MPU_GPIO_B_PIN      10

/* ==============ADC LASER CURRENT MONITOR============*/
#define LASER_INT_ADC_PORT      2   // PB0
#define LASER_INT_ADC_PIN       0

#define LASER_EXT_ADC_PORT      2   // PB2
#define LASER_EXT_ADC_PIN       2

//#define SOLENOID_CTR_1_PORT         1   // PA22
//#define SOLENOID_CTR_1_PIN          22
//
//#define SOLENOID_CTR_2_PORT         1   // PA23
//#define SOLENOID_CTR_2_PIN          23
//
//#define SOLENOID_CTR_3_PORT         1   // PA24
//#define SOLENOID_CTR_3_PIN          24
//
//#define SOLENOID_CTR_4_PORT         1   // PA25
//#define SOLENOID_CTR_4_PIN          25

/* ============== QSPI ============*/
#define MCU_LASER_QSPI_MISO_PORT        1  // PA12
#define MCU_LASER_QSPI_MISO_PIN         12

#define MCU_LASER_QSPI_MOSI_PORT        1  // PA13
#define MCU_LASER_QSPI_MOSI_PIN         13

#define MCU_LASER_QSPI_SCK_PORT         1  // PA14
#define MCU_LASER_QSPI_SCK_PIN          14

#define MCU_MRAM_QSPI_IO1_PORT         1  // PA12
#define MCU_MRAM_QSPI_IO1_PIN          12

#define MCU_MRAM_QSPI_IO0_PORT         1  // PA13
#define MCU_MRAM_QSPI_IO0_PIN          13

#define MCU_MRAM_QSPI_SCK_PORT         1  // PA14
#define MCU_MRAM_QSPI_SCK_PIN          14

/* ============== MAX22200 ============*/
#define MCU_CMD_SOLE_A_PORT             3  // PC19
#define MCU_CMD_SOLE_A_PIN              19

#define MCU_SPI0_nCS_SOLE_A_PORT        2  // PB13
#define MCU_SPI0_nCS_SOLE_A_PIN         13

#define MCU_CMD_SOLE_B_PORT             5  // PE05
#define MCU_CMD_SOLE_B_PIN              5

#define MCU_SPI0_nCS_SOLE_B_PORT        5  // PE00
#define MCU_SPI0_nCS_SOLE_B_PIN         0

//#define MCU_CMD_SOLE_C_PORT             4  // PD14
//#define MCU_CMD_SOLE_C_PIN              14
//
//#define MCU_SPI0_nCS_SOLE_C_PORT        4  // PD01
//#define MCU_SPI0_nCS_SOLE_C_PIN         1
//
//#define MCU_CMD_SOLE_D_PORT             4  // PD07
//#define MCU_CMD_SOLE_D_PIN              7
//
//#define MCU_SPI0_nCS_SOLE_D_PORT        4  // PD18
//#define MCU_SPI0_nCS_SOLE_D_PIN         18
//
//#define MCU_CMD_SOLE_E_PORT             1  // PA06
//#define MCU_CMD_SOLE_E_PIN              6
//
//#define MCU_SPI0_nCS_SOLE_E_PORT        1  // PA5
//#define MCU_SPI0_nCS_SOLE_E_PIN         5


#define MCU_SENSOR1_nIRQ_PORT           4  // PD23
#define MCU_SENSOR1_nIRQ_PIN            23

//#define MCU_SENSOR2_nIRQ_PORT           4  // PD24
//#define MCU_SENSOR2_nIRQ_PIN            24
//
//#define MCU_SENSOR3_nIRQ_PORT           4  // PD29
//#define MCU_SENSOR3_nIRQ_PIN            29
//
//#define MCU_SENSOR4_nIRQ_PORT           4  // PD30
//#define MCU_SENSOR4_nIRQ_PIN            30
#endif