/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

/* Board Support includes. */
#include "bme688.h"

/* ======================================================== Wrapper ========================================================== */
static Std_ReturnType BMP688_Write(bme688_dev_t* dev, uint8_t reg, uint8_t TX_data)
{
	uint8_t frame[2] = {reg, TX_data};

	return i2c_io_send(dev->i2c_bus, dev->slaveAdd, (const char*)frame, 2);
}

static Std_ReturnType BMP688_Read(bme688_dev_t* dev, uint8_t reg, uint8_t *p_RX_buffer, uint8_t byte_count)
{
	// 1) Write the register pointer WITHOUT STOP
    if (i2c_io_send(dev->i2c_bus, dev->slaveAdd, (const char*)&reg, 1) != 0u)
	{
		return ERROR_FAIL;
	}

    // 2) Next recv performs a REPEATED-START automatically, then STOP
    return i2c_io_recv(dev->i2c_bus, dev->slaveAdd, (char*)p_RX_buffer, (int)byte_count);
}

static void bme688_delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms; i++);
}

static Std_ReturnType bme688_read_calib(bme688_dev_t* dev)
{
    /*
     * Bosch ??c 2 vùng vào 1 m?ng liên ti?p:
     *   buf[0..22]  ? vùng 1: reg 0x8A..0xA0 (23 bytes)
     *   buf[23..36] ? vùng 2: reg 0xE1..0xEE (14 bytes)
     *
     * Index map (theo bme68x_defs.h chính th?c c?a Bosch):
     *   T2_LSB=0  T2_MSB=1  T3=2
     *   P1_LSB=4  P1_MSB=5  P2_LSB=6  P2_MSB=7  P3=8
     *   P4_LSB=10 P4_MSB=11 P5_LSB=12 P5_MSB=13
     *   P7=14     P6=15
     *   P8_LSB=18 P8_MSB=19 P9_LSB=20 P9_MSB=21 P10=22
     *   H2_MSB=23 H2_LSB=24 H1_MSB=25 (shared byte v?i H2_LSB)
     *   H3=26 H4=27 H5=28 H6=29 H7=30
     *   T1_LSB=31 T1_MSB=32           ? ?ây là l?i chính
     */
    uint8_t buf[42] = {0};

    /* Vùng 1: 0x8A..0xA0 (23 bytes) */
    if (BMP688_Read(dev, 0x8Au, &buf[0], 23u) != ERROR_OK)
    {
        return ERROR_FAIL;
    }
    /* Vùng 2: 0xE1..0xEE (14 bytes) */
    if (BMP688_Read(dev, 0xE1u, &buf[23], 14u) != ERROR_OK)
    {
        return ERROR_FAIL;
    }

    bme688_calib_t* c = &dev->calib;

    /* --- Temperature --- */
    c->T2 = (int16_t) ((uint16_t)buf[1]  << 8u | buf[0]);   /* idx 0,1  ? reg 0x8A,0x8B */
    c->T3 = (int8_t)   buf[2];                               /* idx 2    ? reg 0x8C      */
    c->T1 = (uint16_t)((uint16_t)buf[32] << 8u | buf[31]);  /* idx 31,32? reg 0xE9,0xEA ? S?A */

    /* --- Pressure --- */
    c->P1  = (uint16_t)((uint16_t)buf[5]  << 8u | buf[4]);  /* idx 4,5  */
    c->P2  = (int16_t) ((uint16_t)buf[7]  << 8u | buf[6]);  /* idx 6,7  */
    c->P3  = (int8_t)   buf[8];                              /* idx 8    */
    c->P4  = (int16_t) ((uint16_t)buf[11] << 8u | buf[10]); /* idx 10,11*/
    c->P5  = (int16_t) ((uint16_t)buf[13] << 8u | buf[12]); /* idx 12,13*/
    c->P7  = (int8_t)   buf[14];                             /* idx 14   ? P7 tr??c P6 */
    c->P6  = (int8_t)   buf[15];                             /* idx 15   ? P6 sau  P7 */
    c->P8  = (int16_t) ((uint16_t)buf[19] << 8u | buf[18]); /* idx 18,19*/
    c->P9  = (int16_t) ((uint16_t)buf[21] << 8u | buf[20]); /* idx 20,21*/
    c->P10 = buf[22];                                        /* idx 22   */

    /* --- Humidity --- */
    /* H2: MSB ? buf[23], 4 bit th?p c?a buf[24] là ph?n chung v?i H1 */
    c->H2 = (uint16_t)(((uint16_t)buf[23] << 4u) | (buf[24] >> 4u));  /* idx 23,24 */
    c->H1 = (uint16_t)(((uint16_t)buf[25] << 4u) | (buf[24] & 0x0Fu));/* idx 24,25 ? S?A H1_MSB=25 */
    c->H3 = (int8_t)buf[26];  /* idx 26 */
    c->H4 = (int8_t)buf[27];  /* idx 27 */
    c->H5 = (int8_t)buf[28];  /* idx 28 */
    c->H6 = buf[29];           /* idx 29 */
    c->H7 = (int8_t)buf[30];  /* idx 30 */

    return ERROR_OK;
}

/* ====================================================== Configuration ====================================================== */
Std_ReturnType bme688_init(bme688_dev_t* dev)
{
    uint8_t chip_id = 0u;
    uint8_t reg_val = 0u;

    if (dev == NULL)
    {
        return ERROR_FAIL;
    }

    dev->init_status = INIT_NONE;
    memset(&dev->calib, 0, sizeof(bme688_calib_t));

    /* ---- Step 1: Soft reset ---- */
    if (BMP688_Write(dev, BME688_REG_RESET, BME688_RESET_CMD) != ERROR_OK)
    {
        return ERROR_FAIL;
    }
    bme688_delay_ms(10u);

    /* ---- Step 2: Verify chip ID ---- */
    if (BMP688_Read(dev, BME688_REG_CHIP_ID, &chip_id, 1u) != ERROR_OK)
    {
        return ERROR_FAIL;
    }
    if (chip_id != BME688_CHIP_ID)
    {
        return ERROR_FAIL;   /* sai chip ho?c l?i bus */
    }

    /* ---- Step 3: Load NVM trimming coefficients ---- */
    if (bme688_read_calib(dev) != ERROR_OK)
    {
        return ERROR_FAIL;
    }

    /* ---- Step 4a: Humidity oversampling × 1 ----
     *  ctrl_hum (0x72): osrs_h[2:0] = bits [2:0]
     *  Chú ý: ctrl_hum ph?i write TR??C ctrl_meas (datasheet §3.6.1)
     */
    reg_val = BME688_OS_1X;  /* 0x01 */
    if (BMP688_Write(dev, BME688_REG_CTRL_HUM, reg_val) != ERROR_OK)
    {
        return ERROR_FAIL;
    }

    /* ---- Step 4b: Temperature × 2, Pressure × 4, mode = SLEEP ----
     *  ctrl_meas (0x74): osrs_t[2:0] = bits[7:5]
     *                    osrs_p[2:0] = bits[4:2]
     *                    mode[1:0]   = bits[1:0]  ? 0b00 (sleep)
     */
    reg_val = (uint8_t)((BME688_OS_2X  << 5u) |
                        (BME688_OS_4X  << 2u) |
                         BME688_MODE_SLEEP);
    if (BMP688_Write(dev, BME688_REG_CTRL_MEAS, reg_val) != ERROR_OK)
    {
        return ERROR_FAIL;
    }

    /* ---- Step 5: IIR filter coefficient = 3 ----
     *  config (0x75): filter[2:0] = bits[4:2]
     *  Coefficient 3 gi?m nhi?u áp su?t t?t mà không lag quá nhi?u.
     */
    reg_val = (uint8_t)(BME688_FILTER_COEFF_3 << 2u);  /* 0x08 */
    if (BMP688_Write(dev, BME688_REG_CONFIG, reg_val) != ERROR_OK)
    {
        return ERROR_FAIL;
    }

    /*
     * ---- Step 6: Gas sensor OFF ----
     * ctrl_gas_1 (0x71): bit[5] run_gas = 0  (default sau reset)
     * Không c?n write gì thêm ? reset ?ã clear v? 0x00.
     * N?u mu?n t??ng minh:
     *   BMP688_Write(dev, 0x71u, 0x00u);
     */

    dev->init_status = INIT_DONE;
    return ERROR_OK;
}

static int32_t g_t_fine = 0;  /* shared gi?a các hàm compensate */

static float bme688_compensate_temp(const bme688_calib_t* c, uint32_t adc_T)
{
    float var1, var2;

    var1 = (((float)adc_T / 16384.0f) - ((float)c->T1 / 1024.0f));
    var2 = (((float)adc_T / 131072.0f) - ((float)c->T1 / 8192.0f)) *
           (((float)adc_T / 131072.0f) - ((float)c->T1 / 8192.0f));

    g_t_fine = (int32_t)((var1 * (float)c->T2) +
                          (var2 * (float)c->T3 * 16.0f));

    return (float)g_t_fine / 5120.0f;
}

static float bme688_compensate_press(const bme688_calib_t* c, uint32_t adc_P)
{
    float var1, var2, var3, comp_P;

    var1 = ((float)g_t_fine / 2.0f) - 64000.0f;
    var2 = var1 * var1 * ((float)c->P6 / 131072.0f);
    var2 = var2 + (var1 * (float)c->P5 * 2.0f);
    var2 = (var2 / 4.0f) + ((float)c->P4 * 65536.0f);
    var1 = (((float)c->P3 * var1 * var1 / 16384.0f) +
             ((float)c->P2 * var1)) / 524288.0f;
    var1 = (1.0f + (var1 / 32768.0f)) * (float)c->P1;

    if (var1 == 0.0f) { return 0.0f; }

    comp_P = 1048576.0f - (float)adc_P;
    comp_P = ((comp_P - (var2 / 4096.0f)) * 6250.0f) / var1;
    var1   = ((float)c->P9 * comp_P * comp_P) / 2147483648.0f;
    var2   = comp_P * ((float)c->P8 / 32768.0f);
    var3   = (comp_P / 256.0f) * (comp_P / 256.0f) * (comp_P / 256.0f) *
             ((float)c->P10 / 131072.0f);

    comp_P = comp_P + (var1 + var2 + var3 + ((float)c->P7 * 128.0f)) / 16.0f;

    return comp_P;
}

static float bme688_compensate_hum(const bme688_calib_t* c, uint16_t adc_H)
{
    float temp_comp, var1, var2, var3, var4, calc_hum;

    /* temp_comp = nhi?t ?? hi?n t?i (°C), tính t? g_t_fine */
    temp_comp = ((float)g_t_fine) / 5120.0f;

    var1 = (float)adc_H -
           ((float)c->H1 * 16.0f) -
           (((float)c->H3 / 2.0f) * temp_comp);

    var2 = var1 *
           ((float)c->H2 / 262144.0f) *
           (1.0f +
            ((float)c->H4 / 16384.0f) * temp_comp +
            ((float)c->H5 / 1048576.0f) * temp_comp * temp_comp);

    var3 = (float)c->H6 / 16384.0f;
    var4 = (float)c->H7 / 2097152.0f;

    calc_hum = var2 + ((var3 + (var4 * temp_comp)) * var2 * var2);

    if (calc_hum > 100.0f) { calc_hum = 100.0f; }
    if (calc_hum < 0.0f)   { calc_hum = 0.0f;   }

    return calc_hum;
}

/* ============================================================
 * bme688_read_forced()
 *
 * Trigger 1 chu k? ?o, ch? new_data, tr? v? T/P/H ?ã compensate.
 * Sau khi ?o xong sensor t? v? sleep.
 *
 * Timeout: n?u sau ~200 ms v?n ch?a có data ? tr? v? ERROR_FAIL.
 * ============================================================ */
Std_ReturnType bme688_read_forced(bme688_dev_t* dev, bme688_data_t* out)
{
    uint8_t  ctrl_meas = 0u;
    uint8_t  status    = 0u;
    uint8_t  raw[8]    = {0u};
    uint32_t adc_T, adc_P;
    uint16_t adc_H;
    uint8_t  retries   = 0u;

    if ((dev == NULL) || (out == NULL) || (!dev->init_status))
    {
        return ERROR_FAIL;
    }

    /* ---- ??c ctrl_meas hi?n t?i ?? gi? osrs bits, ch? ??i mode ---- */
    if (BMP688_Read(dev, BME688_REG_CTRL_MEAS, &ctrl_meas, 1u) != ERROR_OK)
    {
        return ERROR_FAIL;
    }

    /* Set mode = FORCED (0b01), gi? nguyên osrs_t và osrs_p */
    ctrl_meas = (ctrl_meas & 0xFCu) | BME688_MODE_FORCED;
    if (BMP688_Write(dev, BME688_REG_CTRL_MEAS, ctrl_meas) != ERROR_OK)
    {
        return ERROR_FAIL;
    }

    /*
     * ---- Poll new_data_0 (bit[5] c?a meas_status_0 = 0x1D) ----
     * Th?i gian ?o ?i?n hình ? osrs_T×2ms + osrs_P×4ms + osrs_H×1ms
     * = 2×2 + 4×4 + 1×1 = 21 ms. Poll m?i 5ms, t?i ?a 40 l?n (200ms).
     */
    for (retries = 0u; retries < 40u; retries++)
    {
        bme688_delay_ms(5u);

        if (BMP688_Read(dev, BME688_REG_MEAS_STATUS0, &status, 1u) != ERROR_OK)
        {
            return ERROR_FAIL;
        }
        if ((status & BME688_NEW_DATA_MASK) != 0u)
        {
            break;
        }
    }

    if ((status & BME688_NEW_DATA_MASK) == 0u)
    {
        return ERROR_FAIL;  /* timeout */
    }

    /*
     * ---- Burst read raw ADC (8 bytes liên ti?p t? 0x1F) ----
     *  0x1F: press_msb[7:0]
     *  0x20: press_lsb[7:0]
     *  0x21: press_xlsb[7:4] ? adc_P = [19:0]
     *  0x22: temp_msb[7:0]
     *  0x23: temp_lsb[7:0]
     *  0x24: temp_xlsb[7:4] ? adc_T = [19:0]
     *  0x25: hum_msb[7:0]
     *  0x26: hum_lsb[7:0]   ? adc_H = [15:0]
     */
    if (BMP688_Read(dev, BME688_REG_PRESS_MSB, raw, 8u) != ERROR_OK)
    {
        return ERROR_FAIL;
    }

    adc_P = ((uint32_t)raw[0] << 12u) |
            ((uint32_t)raw[1] << 4u)  |
            ((uint32_t)raw[2] >> 4u);

    adc_T = ((uint32_t)raw[3] << 12u) |
            ((uint32_t)raw[4] << 4u)  |
            ((uint32_t)raw[5] >> 4u);

    adc_H = ((uint16_t)raw[6] << 8u) | raw[7];

    /* ---- Compensation ? th? t?: T tr??c ?? có t_fine ---- */
    out->temperature = bme688_compensate_temp(&dev->calib, adc_T);
    out->pressure    = bme688_compensate_press(&dev->calib, adc_P);
    out->humidity    = bme688_compensate_hum(&dev->calib, adc_H);

    return ERROR_OK;
}

Std_ReturnType bme688_read_temperature(bme688_dev_t* dev, float* temperature)
{
    bme688_data_t data = {0};

    if ((dev == NULL) || (temperature == NULL)) return ERROR_FAIL;

    if (bme688_read_forced(dev, &data) != ERROR_OK) return ERROR_FAIL;

    *temperature = data.temperature;
    return ERROR_OK;
}

Std_ReturnType bme688_read_pressure(bme688_dev_t* dev, float* pressure)
{
    bme688_data_t data = {0};

    if ((dev == NULL) || (pressure == NULL)) return ERROR_FAIL;

    if (bme688_read_forced(dev, &data) != ERROR_OK) return ERROR_FAIL;

    *pressure = data.pressure;
    return ERROR_OK;
}

Std_ReturnType bme688_read_humidity(bme688_dev_t* dev, float* humidity)
{
    bme688_data_t data = {0};

    if ((dev == NULL) || (humidity == NULL)) return ERROR_FAIL;

    if (bme688_read_forced(dev, &data) != ERROR_OK) return ERROR_FAIL;

    *humidity = data.humidity;
    return ERROR_OK;
}