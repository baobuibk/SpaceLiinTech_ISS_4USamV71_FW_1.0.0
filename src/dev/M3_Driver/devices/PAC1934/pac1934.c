#include "pac1934.h"
#include "define.h"

static uint32_t pac1934_cmd(pac1934_dev_t *dev, uint8_t cmd)
{
    return i2c_io_send(dev->i2c_bus, dev->slaveAdd, (const char *)&cmd, 1);
}

static uint32_t pac1934_write_reg(pac1934_dev_t *dev, uint8_t reg, uint8_t val)
{
    uint8_t frame[2] = {reg, val};
    return i2c_io_send(dev->i2c_bus, dev->slaveAdd, (const char *)frame, 2);
}

static uint32_t pac1934_read_reg(pac1934_dev_t *dev, uint8_t reg, uint8_t *buf, uint8_t len)
{
    uint32_t st = i2c_io_send(dev->i2c_bus, dev->slaveAdd, (const char *)&reg, 1);
    if (st != ERROR_OK) return st;
    return i2c_io_recv(dev->i2c_bus, dev->slaveAdd, (char *)buf, (int)len);
}

static void pac1934_delay_1ms(void)
{
    volatile uint32_t d = 60000U;
    while (d--);
}

uint32_t PAC1934_init(pac1934_dev_t *dev)
{
    if (dev == NULL) return ERROR_INVALID_PARAM;

    uint8_t pid = 0U;
    uint32_t st;

    st = pac1934_read_reg(dev, PAC1934_PID_REG, &pid, 1U);
    if (st != ERROR_OK || pid != PAC1934_PRODUCT_ID) {
        dev->init_status = false;
        return ERROR_FAIL;
    }

    st = pac1934_cmd(dev, PAC1934_REFRESH);
    if (st != ERROR_OK) return st;
    pac1934_delay_1ms();

    st = pac1934_write_reg(dev, PAC1934_CTRL, 0x00U);
    if (st != ERROR_OK) return st;
    st = pac1934_write_reg(dev, PAC1934_NEG_PWR, 0x00U);
    if (st != ERROR_OK) return st;
    st = pac1934_write_reg(dev, PAC1934_CHANNEL_DIS, 0x00U);
    if (st != ERROR_OK) return st;
    st = pac1934_write_reg(dev, PAC1934_SLOW, 0x00U);
    if (st != ERROR_OK) return st;

    st = pac1934_cmd(dev, PAC1934_REFRESH);
    if (st != ERROR_OK) return st;
    pac1934_delay_1ms();

    dev->init_status = true;
    return ERROR_OK;
}

uint32_t PAC1934_read_all(pac1934_dev_t *dev, pac1934_data_t *data)
{
    if (dev == NULL || data == NULL) return ERROR_INVALID_PARAM;
    if (!dev->init_status) return ERROR_NOT_READY;

    uint8_t buf[2];
    uint32_t st;

    st = pac1934_cmd(dev, PAC1934_REFRESH_V);
    if (st != ERROR_OK) return st;
    pac1934_delay_1ms();
    pac1934_delay_1ms();

    for (uint8_t ch = 0U; ch < PAC1934_CHANNELS; ch++) {
        st = pac1934_read_reg(dev, (uint8_t)(PAC1934_VBUS1_AVG + ch), buf, 2U);
        if (st != ERROR_OK) return st;
        uint16_t raw = ((uint16_t)buf[0] << 8U) | (uint16_t)buf[1];
        data->vbus_v[ch] = (float)raw * 32.0f / 65536.0f;
    }

    for (uint8_t ch = 0U; ch < PAC1934_CHANNELS; ch++) {
        st = pac1934_read_reg(dev, (uint8_t)(PAC1934_VSENSE1_AVG + ch), buf, 2U);
        if (st != ERROR_OK) return st;
        uint16_t raw = ((uint16_t)buf[0] << 8U) | (uint16_t)buf[1];
        float vsense_mv = (float)raw * 100.0f / 65536.0f;
        data->current_a[ch] = vsense_mv / (float)dev->rsense_mohm;
    }

    return ERROR_OK;
}
