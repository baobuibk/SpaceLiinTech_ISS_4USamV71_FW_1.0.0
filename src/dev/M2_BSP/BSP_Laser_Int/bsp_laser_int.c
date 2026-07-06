/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "bsp_laser_int.h"
#include "board.h"
#include "bsp_board/bsp_core.h"

#include "M0_App/AppOS/App_dls_exp/app_dls_exp.h"
#include "M3_Driver/components/adc/adc.h"
#include "M3_Driver/devices/ADG1414BR/adg1414.h"
#include "M3_Driver/devices/MCP4902/mcp4902.h"
#include "BSP_Power/bsp_power.h"


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ---- Minimal bits we need ---- */
// #define SAR_ADC_MCR_NSTART_MASK      (1u << 24)   /* start normal conversion */
// #define SAR_ADC_ISR_ECH_MASK         (1u << 0)    /* end of chain */
// #define SAR_ADC_ISR_EOC_MASK         (1u << 1)    /* end of conversion */
#define SAR_ADC_ISR_ALL              (0xFFFFFFFFu)
#define SAR_ADC_CEOCFR_ALL           (0xFFFFFFFFu)

/* i.MX93 SAR ADC precision data are 12-bit right aligned in PCDR[x] */
#define SAR_ADC_PCDR_MASK_12B        (0x0FFFu)


#define LASER_INT_ADC                     AFEC_CH6  // Laser internal current monitor ADC channel

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
adg1414_dev_t laser_int_dev = {
    .cs = &laser_int_sw_cs,
    .num_of_sw = 3
};

mcp4902_dev_t laser_int_dac_dev = {
    .cs = &laser_ext_dac_cs,
    .latch = &laser_ext_dac_latch,
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static int8_t map_int_LD_position(int x);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
uint16_t laser_current[32 * 2] = {0};

uint32_t laser_adc_set_count;
uint32_t laser_adc_count;

uint8_t is_laser_tim_run = 0;

void bsp_laser_int_latch(void) {
    mcp4902_latch_pulse(&laser_int_dac_dev);
}
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
void bsp_laser_init(void) {
    bsp_power_lp_heater_on();
//    adg1414_chain_qspi_init(&laser_int_dev, &qspi, laser_int_dev.cs, laser_int_dev.num_of_sw);
//    mcp4902_dev_qspi_init(&laser_int_dac_dev, &qspi, laser_int_dac_dev.cs, laser_int_dac_dev.latch);
//    adg1414_chain_spi_init(&laser_int_dev, &spi1, laser_int_dev.cs, laser_int_dev.num_of_sw);
    
    
    
    mcp4902_dev_dma_spi_init(&laser_int_dac_dev, &spi1, laser_int_dac_dev.cs, laser_int_dac_dev.latch);
}

void bsp_laser_int_set_dac(uint8_t code) {    
    mcp4902_set_dac(&laser_int_dac_dev, MCP4902_CHA, code);
}

uint8_t bsp_laser_int_get_dac(void) {
    return laser_int_dac_dev.dac_channel[MCP4902_CHA];
}

void bsp_laser_int_sw_on(uint8_t channel) {
    uint8_t real_channel = map_int_LD_position(channel);
    adg1414_chain_sw_on(&laser_int_dev, real_channel);
}

void bsp_laser_int_sw_off(uint8_t channel) {
    uint8_t real_channel = map_int_LD_position(channel);
    adg1414_chain_sw_off(&laser_int_dev, real_channel);
}

void bsp_laser_int_all_sw_off(void) {
    adg1414_chain_all_sw_off(&laser_int_dev);
}

/* ============================================================
 * 1) TRIGGER: select channel and start a one-shot conversion
 *    - Channel 0 -> ADC1_IN0
 *    - Channel 1 -> ADC1_IN1
 * ============================================================ */
void bsp_laser_int_current_trigger_adc(void) {

}

/* ============================================================
 * 2) READ DATA (assumes conversion already finished)
 *    Reads PCDR[x] that corresponds to the selected channel.
 * ============================================================ */
uint16_t bsp_laser_int_current_read_adc_data(void) {
    return 0;
}

uint16_t bsp_laser_int_read_adc(void) {
    return AFEC1_ReadChannel(LASER_INT_ADC) + 10;
}

float bsp_laser_int_current_get(void) {
    float voltage = (float) bsp_laser_int_read_adc() * 3.3000f / 4095.0000f;
    return voltage / 280.0f;
}

int16_t bsp_laser_int_current_100mA_get(void) {
    float voltage = (float) bsp_laser_int_read_adc() * 3.3000f / 4095.0000f;
    float current_100mA = voltage * 10000 / 280.00f;
    return (int16_t)current_100mA;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static int8_t map_int_LD_position(int x) {
    static const uint8_t map[] = {
        0,
        1, 2, 3, 4, 8, 7, 6, 5,
        9, 10, 11, 12, 16, 15, 14, 13,
        17, 18, 19, 20, 24, 23, 22, 21
    };

    if (x < 1 || x > 24) {
        return -1;
    }

    return map[x];
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */