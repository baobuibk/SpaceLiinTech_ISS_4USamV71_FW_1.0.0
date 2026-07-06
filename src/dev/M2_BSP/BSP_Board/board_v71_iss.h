#ifndef _BOARD_V71_ISS_H_
#define _BOARD_V71_ISS_H_


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "samv71q21b.h"

#define PB12_PORT        2   // PB12
#define PB12_PIN         12

/* ===================== POWER CONTROL ===================== */

#define POWER_TEC_PORT        4   // PD4
#define POWER_TEC_PIN         4

#define POWER_SLN_HD_PORT        4   // PD2
#define POWER_SLN_HD_PIN         2

#define POWER_LP_HEATER_PORT         4   // PD6
#define POWER_LP_HEATER_PIN          6

/* ===================== LASER ===================== */

#define LASER_INT_DAC_LATCH_PORT  3   // PC16
#define LASER_INT_DAC_LATCH_PIN   16

#define LASER_INT_DAC_CS_PORT     1   // PA31
#define LASER_INT_DAC_CS_PIN      31

#define LASER_INT_SW_CS_PORT  3   // PC07
#define LASER_INT_SW_CS_PIN   7

#define LASER_EXT_DAC_LATCH_PORT  3   // PC01
#define LASER_EXT_DAC_LATCH_PIN   1

#define LASER_EXT_DAC_CS_PORT  1   // PA22
#define LASER_EXT_DAC_CS_PIN   22

#define LASER_EXT_SW_CS_PORT     1   // PA07
#define LASER_EXT_SW_CS_PIN      7

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

/* ===================== FLOW_SENSOR ===================== */
#define FLOW_1_EN_PORT        3   // PC09
#define FLOW_1_EN_PIN         9

#define EXT_SEN_EN_PORT        3   // PC10
#define EXT_SEN_EN_PIN         10

#define MCU_SENSOR1_nIRQ_PORT           4  // PD23
#define MCU_SENSOR1_nIRQ_PIN            23

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

/* ===================== GPIO MCU-PMU ==================*/
#define MCU_MPU_GPIO_A_PORT     4   // PD09
#define MCU_MPU_GPIO_A_PIN      9

#define MCU_MPU_GPIO_B_PORT     4   // PD10
#define MCU_MPU_GPIO_B_PIN      10

/* ============== ADC LASER CURRENT MONITOR ============*/
#define LASER_INT_ADC_PORT      2   // PB00
#define LASER_INT_ADC_PIN       0

#define LASER_EXT_ADC_PORT      2   // PB02
#define LASER_EXT_ADC_PIN       2

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

#endif