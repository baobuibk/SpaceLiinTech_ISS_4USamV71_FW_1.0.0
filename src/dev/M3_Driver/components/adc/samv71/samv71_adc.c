#include "M3_Driver/components/adc/adc.h"
#include "define.h"

#include "samv71q21b.h"

uint16_t g_adc_raw[ADC_NTC_COUNT];
uint16_t g_adc_avg[ADC_NTC_COUNT];
uint16_t g_board_temp_raw = 0U;

static uint16_t g_adc_hist[ADC_NTC_COUNT][ADC_AVG_SAMPLE_COUNT];
static uint32_t g_adc_sum[ADC_NTC_COUNT];
static uint8_t g_adc_hist_index = 0U;
static bool g_adc_hist_full = false;

static void ADC_AvgInit(void) {
    g_adc_hist_index = 0U;
    g_adc_hist_full = false;

    for (uint8_t ch = 0U; ch < ADC_NTC_COUNT; ch++) {
        g_adc_raw[ch] = 0U;
        g_adc_sum[ch] = 0U;
        g_adc_avg[ch] = 0U;

        for (uint8_t i = 0U; i < ADC_AVG_SAMPLE_COUNT; i++) {
            g_adc_hist[ch][i] = 0U;
        }
    }
}

void AFEC0_Initialize(void) {
    /* Software reset */
    AFEC0_REGS->AFEC_CR = AFEC_CR_SWRST_Msk;

    /* Prescaler and different time settings as per CLOCK section */
    AFEC0_REGS->AFEC_MR = AFEC_MR_PRESCAL(9U) | AFEC_MR_STARTUP_SUT64 |
            AFEC_MR_TRANSFER(4U) | AFEC_MR_ONE_Msk;

    /* resolution and sign mode of result */
    AFEC0_REGS->AFEC_EMR = AFEC_EMR_RES_NO_AVERAGE
            | AFEC_EMR_SIGNMODE_SE_UNSG_DF_SIGN | AFEC_EMR_TAG_Msk;

    /* Enable gain amplifiers */
    AFEC0_REGS->AFEC_ACR = AFEC_ACR_PGA0EN_Msk | AFEC_ACR_PGA1EN_Msk | AFEC_ACR_IBCTL(0x3U);

    /* Gain */
    AFEC0_REGS->AFEC_CGR = AFEC_CGR_GAIN8(AFEC_CGR_GAIN_X1) |
            AFEC_CGR_GAIN9(AFEC_CGR_GAIN_X1);

    /* Offset */
//    AFEC0_REGS->AFEC_CSELR = (uint32_t) AFEC_CH2;   //SWITCH
//    AFEC0_REGS->AFEC_COCR = 512U;
    AFEC0_REGS->AFEC_CSELR = (uint32_t) AFEC_CH5;   // LASER 2
    AFEC0_REGS->AFEC_COCR = 512U;
    AFEC0_REGS->AFEC_CSELR = (uint32_t) AFEC_CH8;   // NTC 1
    AFEC0_REGS->AFEC_COCR = 512U;
    AFEC0_REGS->AFEC_CSELR = (uint32_t) AFEC_CH9;   // NTC 2
    AFEC0_REGS->AFEC_COCR = 512U;
    AFEC0_REGS->AFEC_CSELR = (uint32_t) AFEC_CH10;  // LASER 1
    AFEC0_REGS->AFEC_COCR = 512U;

    AFEC0_REGS->AFEC_MR |= AFEC_MR_USEQ_Msk;
    AFEC0_REGS->AFEC_SEQ1R =
            AFEC_SEQ1R_USCH0(AFEC_CH8)  |
            AFEC_SEQ1R_USCH1(AFEC_CH9)  |
            AFEC_SEQ1R_USCH2(AFEC_CH10) |
            AFEC_SEQ1R_USCH3(AFEC_CH5);
//            AFEC_SEQ1R_USCH4(AFEC_CH2);

    AFEC0_REGS->AFEC_CHER = (1U << 0U) | (1U << 1U) | (1U << 2U) | (1U << 3U);

    ADC_AvgInit();
}

void AFEC1_Initialize(void) {
    /* Software reset */
    AFEC1_REGS->AFEC_CR = AFEC_CR_SWRST_Msk;

    /* Prescaler and different time settings as per CLOCK section  */
    AFEC1_REGS->AFEC_MR = AFEC_MR_PRESCAL(9U) | AFEC_MR_STARTUP_SUT64 |
            AFEC_MR_TRANSFER(4U) | AFEC_MR_ONE_Msk;

    /* resolution and sign mode of result */
    AFEC1_REGS->AFEC_EMR = AFEC_EMR_RES_NO_AVERAGE
            | AFEC_EMR_SIGNMODE_SE_UNSG_DF_SIGN | AFEC_EMR_TAG_Msk;

    /* Enable gain amplifiers */
    AFEC1_REGS->AFEC_ACR = AFEC_ACR_PGA0EN_Msk | AFEC_ACR_PGA1EN_Msk | AFEC_ACR_IBCTL(0x3U);

    /* Gain */
    AFEC1_REGS->AFEC_CGR = AFEC_CGR_GAIN8(AFEC_CGR_GAIN_X1) |
            AFEC_CGR_GAIN9(AFEC_CGR_GAIN_X1);

    /* Offset */
    AFEC1_REGS->AFEC_CSELR = (uint32_t) AFEC_CH0;   //NTC 4
    AFEC1_REGS->AFEC_COCR = 512U;
    /* Offset */
    AFEC1_REGS->AFEC_CSELR = (uint32_t) AFEC_CH1;   //NTC 6
    AFEC1_REGS->AFEC_COCR = 512U;
    /* Offset */
    AFEC1_REGS->AFEC_CSELR = (uint32_t) AFEC_CH2;   //NTC 5
    AFEC1_REGS->AFEC_COCR = 512U;
    /* Offset */
    AFEC1_REGS->AFEC_CSELR = (uint32_t) AFEC_CH4;   //NTC 8
    AFEC1_REGS->AFEC_COCR = 512U;

    AFEC1_REGS->AFEC_CSELR = (uint32_t) AFEC_CH6;   //NTC 3
    AFEC1_REGS->AFEC_COCR = 512U;
    
    AFEC1_REGS->AFEC_CSELR = (uint32_t) AFEC_CH9;   //NTC 7
    AFEC1_REGS->AFEC_COCR = 512U;
    
#if (HW_VERSION == HW_VERSION_V110)
    AFEC1_REGS->AFEC_CSELR = (uint32_t) AFEC_CH10;  //TEMP_BOARD
    AFEC1_REGS->AFEC_COCR = 512U;
#endif

    AFEC1_REGS->AFEC_MR |= AFEC_MR_USEQ_Msk;
    AFEC1_REGS->AFEC_SEQ1R = 
            AFEC_SEQ1R_USCH0(AFEC_CH0) |
            AFEC_SEQ1R_USCH1(AFEC_CH1) |
            AFEC_SEQ1R_USCH2(AFEC_CH2) |
            AFEC_SEQ1R_USCH3(AFEC_CH4) |
            AFEC_SEQ1R_USCH4(AFEC_CH6) |
            AFEC_SEQ1R_USCH5(AFEC_CH9) |
            AFEC_SEQ1R_USCH6(AFEC_CH10);

    /* Enable channel */
    AFEC1_REGS->AFEC_CHER = (1U << 0U) | (1U << 1U) | (1U << 2U) | (1U << 3U) | (1U << 4U) | (1U << 5U) | (1U << 6U);
}

uint16_t AFEC0_ReadChannel(uint8_t ch) {
    /* Start conversion */
    AFEC0_REGS->AFEC_CR = (1U << AFEC_CR_START_Pos);

    while ((((AFEC0_REGS->AFEC_ISR >> (uint32_t) ch) & 0x1U) == 0U));

    AFEC0_REGS->AFEC_CSELR = (uint32_t) ch;
    return (uint16_t) (AFEC0_REGS->AFEC_CDR);
}

uint16_t AFEC0_ReadChannelFromSequence(uint8_t ch) {
    /* Start conversion (sequence) */
    AFEC0_REGS->AFEC_CR = AFEC_CR_START_Msk;

    while (!(AFEC0_REGS->AFEC_ISR & AFEC_ISR_EOC10_Msk));

    AFEC0_REGS->AFEC_CSELR = ch;

    return (uint16_t) AFEC0_REGS->AFEC_CDR;
}

uint16_t AFEC1_ReadChannel(uint8_t ch) {
    /* Start conversion */
    AFEC1_REGS->AFEC_CR = AFEC_CR_START_Msk;

    while (!(AFEC1_REGS->AFEC_ISR & AFEC_ISR_EOC6_Msk));

    AFEC1_REGS->AFEC_CSELR = (uint32_t) ch;
    return (uint16_t) (AFEC1_REGS->AFEC_CDR);
}

void ADC_Read(void) {
    /* AFEC0: 1 START, ch? EOC channel cu?i (CH5) */
    (void) AFEC0_REGS->AFEC_ISR;
    AFEC0_REGS->AFEC_CR = AFEC_CR_START_Msk;
    while ((AFEC0_REGS->AFEC_ISR & AFEC_ISR_EOC5_Msk) == 0U);

    AFEC0_REGS->AFEC_CSELR = (uint32_t) AFEC_CH8;
    g_adc_raw[0] = (uint16_t) (AFEC0_REGS->AFEC_CDR);

    AFEC0_REGS->AFEC_CSELR = (uint32_t) AFEC_CH9;
    g_adc_raw[1] = (uint16_t) (AFEC0_REGS->AFEC_CDR);

    /* AFEC1: 1 START, ch? EOC channel cu?i (CH10) */
    (void) AFEC1_REGS->AFEC_ISR;
    AFEC1_REGS->AFEC_CR = AFEC_CR_START_Msk;
    while ((AFEC1_REGS->AFEC_ISR & AFEC_ISR_EOC10_Msk) == 0U);
    
    AFEC1_REGS->AFEC_CSELR = (uint32_t) AFEC_CH6;
    g_adc_raw[2] = (uint16_t) (AFEC1_REGS->AFEC_CDR);

    AFEC1_REGS->AFEC_CSELR = (uint32_t) AFEC_CH0;
    g_adc_raw[3] = (uint16_t) (AFEC1_REGS->AFEC_CDR);
    
    AFEC1_REGS->AFEC_CSELR = (uint32_t) AFEC_CH2;
    g_adc_raw[4] = (uint16_t) (AFEC1_REGS->AFEC_CDR);

    AFEC1_REGS->AFEC_CSELR = (uint32_t) AFEC_CH1;
    g_adc_raw[5] = (uint16_t) (AFEC1_REGS->AFEC_CDR);

    AFEC1_REGS->AFEC_CSELR = (uint32_t) AFEC_CH9;
    g_adc_raw[6] = (uint16_t) (AFEC1_REGS->AFEC_CDR);

    AFEC1_REGS->AFEC_CSELR = (uint32_t) AFEC_CH4;
    g_adc_raw[7] = (uint16_t) (AFEC1_REGS->AFEC_CDR);

#if (HW_VERSION == HW_VERSION_V110)
    uint32_t to = 100000U;
    while (((AFEC1_REGS->AFEC_ISR & AFEC_ISR_EOC10_Msk) == 0U) && (--to > 0U));
    if (to > 0U) {
        AFEC1_REGS->AFEC_CSELR = (uint32_t) AFEC_CH10;
        g_board_temp_raw = (uint16_t) (AFEC1_REGS->AFEC_CDR);
    }
#endif
}

void ADC_UpdateAverage(void) {
    uint8_t valid_count;

    for (uint8_t ch = 0U; ch < ADC_NTC_COUNT; ch++) {
        g_adc_sum[ch] -= g_adc_hist[ch][g_adc_hist_index];
        g_adc_sum[ch] += g_adc_raw[ch];

        g_adc_hist[ch][g_adc_hist_index] = g_adc_raw[ch];

        if (g_adc_hist_full) {
            valid_count = ADC_AVG_SAMPLE_COUNT;
        } else {
            valid_count = (uint8_t) (g_adc_hist_index + 1U);
        }

        g_adc_avg[ch] = (uint16_t) (g_adc_sum[ch] / (uint32_t) valid_count);
    }

    /* Advance index */
    g_adc_hist_index++;
    if (g_adc_hist_index >= ADC_AVG_SAMPLE_COUNT) {
        g_adc_hist_index = 0U;
        g_adc_hist_full = true;
    }
}
