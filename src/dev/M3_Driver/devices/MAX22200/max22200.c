#include <stdlib.h>
#include <string.h>
#include "max22200.h"
#include "peripheral/usart/plib_usart2_spi.h"
#include "do.h"
#include "BSP_Power/bsp_power.h"

/* Helper macros for bit manipulation without no-OS */
#define FIELD_GET(mask, reg) (((reg) & (mask)) >> __builtin_ctz(mask))
#define FIELD_PREP(mask, val) (((val) << __builtin_ctz(mask)) & (mask))

/* SPI support function --------------------------------------------------*/
static inline void csLOW(struct max22200_desc *desc) {
    do_reset(desc->cs);
}

static inline void csHIGH(struct max22200_desc *desc) {
    do_set(desc->cs);
}

/* Big Endian helpers for MAX22200 32-bit registers */
static uint32_t get_be32(const uint8_t *buf) {
    return ((uint32_t) buf[0] << 24) | ((uint32_t) buf[1] << 16) | ((uint32_t) buf[2] << 8) | (uint32_t) buf[3];
}

static void put_be32(uint32_t val, uint8_t *buf) {
    buf[0] = (uint8_t) val;
    buf[1] = (uint8_t) (val >> 8);
    buf[2] = (uint8_t) (val >> 16);
    buf[3] = (uint8_t) (val >> 24);
}

/* Simple delay helper - adjust if project has a specific delay_us */
static void _delay_us(uint32_t us) {
    for (volatile uint32_t i = 0; i < us * 10; i++);
}

/**
 * @brief Internal SPI transfer helper using project API
 */
static int _spi_transfer(struct max22200_desc *desc, uint8_t len) {
    csLOW(desc);
    USART2_SPI_WriteRead(desc->buff, len, desc->buff, len);
    while (USART2_SPI_IsTransmitterBusy());
    csHIGH(desc);
    return 0;
}

/**
 * @brief Read data from desired register for MAX22200
 * @param desc - MAX22200 device descriptor.
 * @param reg - Register address to read data from.
 * @param val - Value read from the register.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_reg_read(struct max22200_desc *desc, uint32_t reg, uint32_t *val) {
    if (!desc || !val) return -1;

    /* Phase 1: Send Command Byte (CMD high) */
    desc->buff[0] = FIELD_PREP(MAX22200_RW_MASK, 0) | FIELD_PREP(MAX22200_ADDR_MASK, reg);

    do_set(desc->cmd);
    if (_spi_transfer(desc, 1) != 0)
        return -1;

    /* Phase 2: Read Data (CMD low) */
    uint8_t bytes = MAX22200_FRAME_SIZE;
    memset(desc->buff, 0, MAX22200_FRAME_SIZE);

    do_reset(desc->cmd);
    if (_spi_transfer(desc, bytes) != 0) return -1;

    *val = get_be32(desc->buff);
    return 0;
}

/**
 * @brief Write data to desired register for MAX22200
 * @param desc - MAXX22200 device descriptor.
 * @param reg - Register address to write data to.
 * @param val - Value to be written to the register.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_reg_write(struct max22200_desc *desc, uint32_t reg, uint32_t val) {
    if (!desc) return -1;

    /* Phase 1: Send Command Byte (CMD high) */
    desc->buff[0] = FIELD_PREP(MAX22200_RW_MASK, 1) | FIELD_PREP(MAX22200_ADDR_MASK, reg);
    do_set(desc->cmd);
    if (_spi_transfer(desc, 1) != 0) return -1;

    /* Phase 2: Send Data (CMD low) */
    uint8_t bytes;
    put_be32(val, desc->buff);
    bytes = MAX22200_FRAME_SIZE;
    do_reset(desc->cmd);
    return _spi_transfer(desc, bytes);
}

/**
 * @brief Update data in the desired register.
 * @param desc - MAX22200 device descriptor.
 * @param reg - Register address to update data to.
 * @param mask - Bit mask of the field to be updated.
 * @param val - Value to be updated in the desired register.
 * 		Should be bit shifted by using FIELD_PREP(mask, val)
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_reg_update(struct max22200_desc *desc, uint32_t reg, uint32_t mask, uint32_t val) {
    int ret;
    uint32_t reg_val = 0;

    ret = max22200_reg_read(desc, reg, &reg_val);
    if (ret)
        return ret;

    reg_val &= ~mask;
    reg_val |= (mask & val);

    return max22200_reg_write(desc, reg, reg_val);
}

/**
 * @brief Set channel state for specific channel
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel number.
 * @param ch_state - Channel's state to be changed into.
 * @return 0 in case of succces, negative error code otherwise.
 */
int max22200_set_ch_state(struct max22200_desc *desc, uint32_t ch, bool ch_state) {
    if (ch > MAX22200_MAX_CHN_IDX) return -1;

    return max22200_reg_update(desc, MAX22200_STATUS_REG, MAX22200_ONCH_MASK(ch),
            FIELD_PREP(MAX22200_ONCH_MASK(ch), ch_state));
}

/**
 * @brief Set fault mask bits in the status register.
 * @param desc - MAX22200 device descriptor.
 * @param fault_mask - Desired fault mask bit to set.
 * @param enabled - enabled/disabled state of the mask.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_fault_mask_set(struct max22200_desc *desc, enum max22200_fault_mask fault_mask, bool enabled) {
    if (fault_mask == MAX22200_M_HHF && desc->chan_drive == MAX22200_VOLTAGE_DRIVE)
        return -1;

    return max22200_reg_update(desc, MAX22200_STATUS_REG,
            MAX22200_STATUS_FAULT_MASK(fault_mask),
            FIELD_PREP(MAX22200_STATUS_FAULT_MASK(fault_mask), enabled));
}

/**
 * @brief Set chopping frequency value.
 * @param desc - MAX22200 device descriptor.
 * @param chopping_freq - Chopping frequency value.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_set_chop_freq(struct max22200_desc *desc,
        enum max22200_chopping_freq chopping_freq) {
    return max22200_reg_update(desc, MAX22200_STATUS_REG, MAX22200_STATUS_FREQ_MASK,
            FIELD_PREP(MAX22200_STATUS_FREQ_MASK, chopping_freq));
}

/**
 * @brief Set channel HIT time and HIT current.
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel for which HIT time is to be set.(0 based)
 * @param hit_current - HIT current value, it shall not be greater than 127
 * 			since the value is stored only on 7 bits in the
 * 			channel's configuration register.
 * @param hit_time - HIT time value to be set.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_set_ch_hit(struct max22200_desc *desc, uint32_t ch,
        uint8_t hit_current, uint8_t hit_time) {
    int ret;
    if (ch > MAX22200_MAX_CHN_IDX || hit_current > MAX22200_HIT_MAX_VAL)
        return -1;

    ret = max22200_reg_update(desc, MAX22200_CFG_CH(ch), MAX22200_HIT_MASK,
            FIELD_PREP(MAX22200_HIT_MASK, (uint32_t) hit_current));
    if (ret) return ret;

    return max22200_reg_update(desc, MAX22200_CFG_CH(ch), MAX22200_HIT_T_MASK,
            FIELD_PREP(MAX22200_HIT_T_MASK, (uint32_t) hit_time));
}

/**
 * @brief Set channel hold current
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel's number (0 based).
 * @param hold_current - Hold current value to be set. It shall not be greater
 * 			 than 127 since the value it's stored on 7 bits in
 * 			 the channel's configuration register.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_set_ch_hold(struct max22200_desc *desc, uint32_t ch, uint8_t hold_current) {
    if (ch > MAX22200_MAX_CHN_IDX || hold_current > MAX22200_HIT_MAX_VAL)
        return -1;

    return max22200_reg_update(desc, MAX22200_CFG_CH(ch), MAX22200_HOLD_MASK,
            FIELD_PREP(MAX22200_HOLD_MASK, (uint32_t) hold_current));
}

/**
 * @brief Set channel's scale to fullscale or half fullscale.
 * @param desc - MAX22200 device dscriptor.
 * @param ch - Channel's number (0 based).
 * @param scale - Scale value.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_set_ch_scale(struct max22200_desc *desc, uint32_t ch, enum max22200_scale scale) {
    if (ch > MAX22200_MAX_CHN_IDX)
        return -1;

    if (scale == MAX22200_HALF_FULL_SCALE && desc->chan_side == MAX22200_HIGH_SIDE)
        return -1;

    return max22200_reg_update(desc, MAX22200_CFG_CH(ch), MAX22200_HFS_MASK, FIELD_PREP(MAX22200_HFS_MASK, scale));
}

/**
 * @brief Set channel's trigger to be either SPI or external trigger.
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel's number (0 based).
 * @param trig - Trigger to be set. If external trigger is requested but no
 * 		 external's trigger descriptor was initialized, will return
 * 		 negative error code.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_set_ch_trig(struct max22200_desc *desc, uint32_t ch, enum max22200_trig trig) {
    if (ch > MAX22200_MAX_CHN_IDX) return -1;
    return max22200_reg_update(desc, MAX22200_CFG_CH(ch), MAX22200_TRGNSP_IO_MASK,
            FIELD_PREP(MAX22200_TRGNSP_IO_MASK, trig));
}

/**
 * @brief Set chanmel operation mode, high-side/low-side and drive.
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel number (0 based).
 * @param ch_drive - Voltage/Current drive.
 * @param ch_side - High-Side/Low-Side.
 * @param ch_op_mode - Independent/Half-Bridge/Full-Bridge mode.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_set_ch_mode(struct max22200_desc *desc, uint32_t ch,
        enum max22200_ch_drive ch_drive,
        enum max22200_ch_side ch_side,
        enum max22200_ch_op_mode ch_op_mode) {
    int ret;
    uint32_t mask, val;

    if (ch > MAX22200_MAX_CHN_IDX) return -1;

    if (ch_side == MAX22200_HIGH_SIDE && ch_drive == MAX22200_CURRENT_DRIVE)
        return -1;

    mask = MAX22200_CH_MODE_MASK(ch);
    val = FIELD_PREP(MAX22200_CH_MODE_MASK(ch), (uint32_t) ch_op_mode);

    ret = max22200_reg_update(desc, MAX22200_STATUS_REG, mask, val);
    if (ret)
        return ret;

    /* Channels are configured 2 by 2, either they are configured as
       Independent, Half-Bridge or Full-Bridge, thereby ch_config[0] is
       the configuration for channels 0 and 1, ch_config[1] is the
       configuration for channels 1 and 2, and so on. */
    desc->ch_config[ch / 2] = ch_op_mode;

    mask = MAX22200_VDRNCDR_MASK | MAX22200_HSNLS_MASK;
    val = FIELD_PREP(MAX22200_VDRNCDR_MASK, (uint32_t) ch_drive) |
            FIELD_PREP(MAX22200_HSNLS_MASK, (uint32_t) ch_side);

    ret = max22200_reg_update(desc, MAX22200_CFG_CH(ch), mask, val);
    if (ret)
        return ret;

    desc->chan_side = ch_side;

    return 0;
}

/**
 * @brief Set channel's frequency.
 * @param desc - MAX22200 device descriptor
 * @param ch - Channel's number (0 based).
 * @param ch_freq - 80KHz/100KHz frequency.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_set_ch_freq(struct max22200_desc *desc, uint32_t ch,
        enum max22200_ch_freq ch_freq) {
    if (ch > MAX22200_MAX_CHN_IDX) return -1;

    return max22200_reg_update(desc, MAX22200_CFG_CH(ch),
            MAX22200_FREQ_CFG_MASK,
            FIELD_PREP(MAX22200_FREQ_CFG_MASK, ch_freq));
}

/**
 * @brief Set channel's enables for different checks, detection and functions.
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel's number (0 based).
 * @param ch_enable - Channel enable to be set.
 * @param enabled - Disabled/Enabled state.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_set_ch_enable(struct max22200_desc *desc, uint32_t ch,
        enum max22200_ch_enable ch_enable, bool enabled) {
    if (ch > MAX22200_MAX_CHN_IDX) return -1;

    return max22200_reg_update(desc, MAX22200_CFG_CH(ch),
            MAX22200_CH_ENABLE_MASK(ch_enable),
            FIELD_PREP(MAX22200_CH_ENABLE_MASK(ch_enable), enabled));
}

/**
 * @brief Set configuration DPM.
 * @param desc - MAX22200 device descriptor.
 * @param dpm_istart_byte - Detection of plunger movement starting current
 * @param dpm_tdeb_ipth_byte - Detection of plunger movement debounce time.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_set_cfg_dpm(struct max22200_desc *desc, uint8_t dpm_istart_byte,
        uint8_t dpm_tdeb_ipth_byte) {
    uint32_t val, mask;

    if (dpm_istart_byte > MAX22200_HIT_MAX_VAL) return -1;

    mask = MAX22200_DPM_ISTART_MASK | MAX22200_DPM_TDEB_MASK |
            MAX22200_DPM_IPTH_MASK;
    val = FIELD_PREP(MAX22200_DPM_ISTART_MASK, dpm_istart_byte) |
            FIELD_PREP(MAX22200_DPM_TDEB_MASK | MAX22200_DPM_IPTH_MASK,
            dpm_tdeb_ipth_byte);

    return max22200_reg_update(desc, MAX22200_CFG_DPM_REG, mask, val);
}

/**
 * @brief Fault mask bit get function
 * @param desc - MAX22200 device descriptor.
 * @param fault_mask - Fault mask specific bit to read the state for.
 * @param enabled - State of the bit.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_fault_mask_get(struct max22200_desc * desc,
        enum max22200_fault_mask fault_mask, bool *enabled) {
    int ret;
    uint32_t reg_val;

    ret = max22200_reg_read(desc, MAX22200_STATUS_REG, &reg_val);
    if (ret)
        return ret;

    *enabled = (bool) FIELD_GET(MAX22200_STATUS_FAULT_MASK(fault_mask), reg_val);

    return 0;
}

/**
 * @brief Read the state of a channel
 * @param desc - MAX22200 device descriptor
 * @param ch - Channel number (0 based).
 * @param ch_state - State of the channel.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_get_ch_state(struct max22200_desc *desc, uint32_t ch,
        bool *ch_state) {
    int ret;
    uint32_t reg_val;

    if (ch >= MAX22200_CHANNELS) return -1;

    ret = max22200_reg_read(desc, MAX22200_STATUS_REG, &reg_val);
    if (ret)
        return ret;

    *ch_state = (bool) FIELD_GET(MAX22200_ONCH_MASK(ch), reg_val);

    return 0;
}

/**
 * @brief Read chopping frequency
 * @param desc - MAX22200 device descriptor.
 * @param chopping_freq - Pointer of the variable holding the value of the
 * 			  chopping frequency.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_get_chop_freq(struct max22200_desc *desc,
        enum max22200_chopping_freq *chopping_freq) {
    int ret;
    uint32_t reg_val;

    ret = max22200_reg_read(desc, MAX22200_STATUS_REG, &reg_val);
    if (ret)
        return ret;

    *chopping_freq = (enum max22200_chopping_freq)FIELD_GET(MAX22200_STATUS_FREQ_MASK, reg_val);

    return 0;
}

/**
 * @brief Read channel HIT configuration.
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel's number (0 based).
 * @param hit_current - HIT current value read from the device.
 * @param hit_time - HIT time value read from the device.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_get_ch_hit(struct max22200_desc *desc, uint32_t ch,
        uint8_t *hit_current, uint8_t *hit_time) {
    int ret;
    uint32_t reg_val;

    if (ch > MAX22200_MAX_CHN_IDX) return -1;

    ret = max22200_reg_read(desc, MAX22200_CFG_CH(ch), &reg_val);
    if (ret)
        return ret;

    *hit_current = (uint8_t) FIELD_GET(MAX22200_HIT_MASK, reg_val);
    *hit_time = (uint8_t) FIELD_GET(MAX22200_HIT_T_MASK, reg_val);

    return 0;
}

/**
 * @brief Read channel HOLD configuration.
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel's number (0 based).
 * @param hold_current - HOLD current value read from the device.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_get_ch_hold(struct max22200_desc *desc, uint32_t ch,
        uint8_t *hold_current) {
    int ret;
    uint32_t reg_val;

    if (ch > MAX22200_MAX_CHN_IDX) return -1;

    ret = max22200_reg_read(desc, MAX22200_CFG_CH(ch), &reg_val);
    if (ret)
        return ret;

    *hold_current = (uint8_t) FIELD_GET(MAX22200_HOLD_MASK, reg_val);

    return 0;
}

/**
 * @brief Read channel's scale.
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel's number (0 based).
 * @param scale - Scale read from device's channel.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_get_ch_scale(struct max22200_desc *desc, uint32_t ch,
        enum max22200_scale *scale) {
    int ret;
    uint32_t reg_val;

    if (ch > MAX22200_MAX_CHN_IDX) return -1;

    ret = max22200_reg_read(desc, MAX22200_CFG_CH(ch), &reg_val);
    if (ret)
        return ret;

    *scale = (enum max22200_scale)FIELD_GET(MAX22200_HFS_MASK, reg_val);

    return 0;
}

/**
 * @brief Read channel's selected trigger for specific channel.
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel's number (0 based).
 * @param trig - Selected trigger for the channel requested read from the
 * 		 device.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_get_ch_trig(struct max22200_desc *desc, uint32_t ch,
        enum max22200_trig *trig) {
    int ret;
    uint32_t reg_val;

    if (ch > MAX22200_MAX_CHN_IDX) return -1;

    ret = max22200_reg_read(desc, MAX22200_CFG_CH(ch), &reg_val);
    if (ret)
        return ret;

    *trig = (enum max22200_trig)FIELD_GET(MAX22200_TRGNSP_IO_MASK, reg_val);

    return 0;
}

/**
 * @brief Read channel's mode configuration
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel's number (0 based).
 * @param ch_drive - Channel's drive read from the device.
 * @param ch_side - Channel's high-side/low-side configuraton read from the
 * 		    device.
 * @param ch_op_mode - Channel's operation mode read from the device.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_get_ch_mode(struct max22200_desc *desc, uint32_t ch,
        enum max22200_ch_drive *ch_drive,
        enum max22200_ch_side *ch_side,
        enum max22200_ch_op_mode *ch_op_mode) {
    int ret;
    uint32_t reg_val;

    if (ch > MAX22200_MAX_CHN_IDX) return -1;

    ret = max22200_reg_read(desc, MAX22200_CFG_CH(ch),
            &reg_val);
    if (ret)
        return ret;

    *ch_drive = (enum max22200_ch_drive)FIELD_GET(MAX22200_VDRNCDR_MASK, reg_val);
    *ch_side = (enum max22200_ch_side)FIELD_GET(MAX22200_HSNLS_MASK, reg_val);

    ret = max22200_reg_read(desc, MAX22200_STATUS_REG, &reg_val);
    if (ret)
        return ret;

    *ch_op_mode = (enum max22200_ch_op_mode)FIELD_GET(MAX22200_CH_MODE_MASK(ch), reg_val);

    return 0;
}

/**
 * @brief Read channel's frequency
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel's number (0 based).
 * @param ch_freq - Channel's frequency value read from the device.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_get_ch_freq(struct max22200_desc *desc, uint32_t ch,
        enum max22200_ch_freq *ch_freq) {
    int ret;
    uint32_t reg_val;

    if (ch > MAX22200_MAX_CHN_IDX) return -1;

    ret = max22200_reg_read(desc, MAX22200_CFG_CH(ch), &reg_val);
    if (ret)
        return ret;

    *ch_freq = (enum max22200_ch_freq)FIELD_GET(MAX22200_FREQ_CFG_MASK, reg_val);

    return 0;
}

/**
 * @brief Read channel's enable bits state
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel's number (0 based).
 * @param ch_enable - Requested channel enable bit to read the state for.
 * @param enabled - Requested enabled bit state.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_get_ch_enable(struct max22200_desc *desc, uint32_t ch,
        enum max22200_ch_enable ch_enable, bool *enabled) {
    int ret;
    uint32_t reg_val;

    if (ch > MAX22200_MAX_CHN_IDX) return -1;

    ret = max22200_reg_read(desc, MAX22200_CFG_CH(ch),
            &reg_val);
    if (ret)
        return ret;

    *enabled = (bool) FIELD_GET(MAX22200_CH_ENABLE_MASK(ch_enable), reg_val);

    return 0;
}

/**
 * @brief Read configuration for the detection of the plunger movement of the
 * 	  device
 * @param desc - MAX22200 device descriptor.
 * @param dpm_istart_byte - Detection of plunger movement starting current
 * 			    value.
 * @param dpm_tdeb_ipth_byte - Detection of plunger movement debounce time
 * 			       value.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_get_cfg_dpm(struct max22200_desc *desc, uint8_t *dpm_istart_byte,
        uint8_t *dpm_tdeb_ipth_byte) {
    int ret;
    uint32_t reg_val;

    ret = max22200_reg_read(desc, MAX22200_CFG_DPM_REG, &reg_val);
    if (ret)
        return ret;

    *dpm_istart_byte = (uint8_t) FIELD_GET(MAX22200_DPM_ISTART_MASK, reg_val);
    *dpm_tdeb_ipth_byte = (uint8_t) (FIELD_GET(MAX22200_DPM_TDEB_MASK, reg_val) |
            FIELD_GET(MAX22200_DPM_IPTH_MASK, reg_val));

    return 0;
}

/**
 * @brief MAX22200 descriptor initialization function.
 * @param desc - MAX22200 device descriptor.
 * @param init_param - Initialization parameter containing data about the device
 * 		       descriptor to be initialized.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_init(struct max22200_desc **desc_ptr, const struct max22200_init_param *init_param) {
    uint32_t reg_val;
    uint32_t status_reg = 0;
    int ret;
    uint8_t i;
    struct max22200_desc *desc;

    if (!desc_ptr || !init_param)
        return -1;

    desc = (struct max22200_desc *) calloc(1, sizeof (struct max22200_desc));
    if (!desc) return -1;
    *desc_ptr = desc; // Assign the allocated memory to the caller's pointer

    if (!init_param->spi || !init_param->cs || !init_param->cmd) {
        free(desc); // Free allocated memory on error
        return -1;
    }

    /* Save hardware resources */
    desc->spi = init_param->spi;
    desc->cs = init_param->cs;
    desc->cmd = init_param->cmd;

    /* Step 1: Power up */
    bsp_power_sln_hd_on();
    /* Default pin state */
    do_set(desc->cs);
    do_reset(desc->cmd);
    /* Wait after power-up */
    _delay_us(500);
    /* Read STATUS register to verify communication */
    ret = max22200_reg_read(desc, MAX22200_STATUS_REG, &reg_val);
    if (ret) {
        free(desc);
        return ret;
    }

    /* Step 2: Hardware configuration */
    for (i = 0; i < MAX22200_CHANNELS_CONFIG; i++) {
        desc->ch_config[i] = init_param->ch_config[i];
        status_reg |= FIELD_PREP(MAX22200_CH_MODE_MASK(i), init_param->ch_config[i]);
    }
    status_reg |= FIELD_PREP(MAX22200_ACTIVE_MASK, 1);
    ret = max22200_reg_write(desc, MAX22200_STATUS_REG, status_reg);
    if (ret) {
        free(desc); // Free allocated memory on error
        return ret;
    }

    /* Step 3: Configure every channel */
    for (i = 0; i < MAX22200_CHANNELS; i++) {
        ret = max22200_set_ch_scale(desc, i, MAX22200_FULLSCALE);
        if (ret) {
            free(desc); // Free allocated memory on error
            return ret;
        }

        ret = max22200_set_ch_hold(desc, i, MAX22200_HIT_MAX_VAL);
        if (ret) {
            free(desc); // Free allocated memory on error
            return ret;
        }

        ret = max22200_set_ch_trig(desc, i, MAX22200_ONCH_SPI);
        if (ret) {
            free(desc); // Free allocated memory on error
            return ret;
        }

        ret = max22200_set_ch_hit(desc, i, MAX22200_HIT_MAX_VAL, MAX22200_HIT_NO_TIME);
        if (ret) {
            free(desc); // Free allocated memory on error
            return ret;
        }
    }
    
    _delay_us(2500);

    /* Step 4: Verify Status Register */
    ret = max22200_reg_read(desc, MAX22200_STATUS_REG, &reg_val);
    return ret;
}

/**
 * @brief Deallocates all the resources used at initialization.
 * @param desc - MAX22200 device descriptor.
 * @return 0 in case of succes, negative error code otherwise.
 */
int max22200_remove(struct max22200_desc *desc) {
    int ret, i;

    if (!desc) return 0; // Already removed or not initialized

    for (i = 0; i < MAX22200_CHANNELS; i++) {
        ret = max22200_set_ch_state(desc, i, false);
        if (ret)
            return ret;
    }

    ret = max22200_reg_update(desc, MAX22200_STATUS_REG,
            MAX22200_ACTIVE_MASK, 0);
    if (ret)
        return ret;

    free(desc);

    return 0;
}