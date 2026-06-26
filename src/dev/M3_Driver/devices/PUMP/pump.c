#include "pump.h"

extern i2c_io_t i2c2;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
Std_ReturnType _mp_i2c_write_block(pump_dev_t* dev, uint8_t reg, uint8_t val) {
    uint8_t frame[2] = {reg, val};
    return i2c_io_send(dev->i2c_bus, dev->slaveAdd, (const char *) frame, 2);
}

Std_ReturnType _mp_i2c_read_block(pump_dev_t* dev, uint8_t reg, uint8_t *buf, uint8_t len) {
    uint32_t st = i2c_io_send(dev->i2c_bus, dev->slaveAdd, (const char *) &reg, 1);
    if (st != ERROR_OK) return st;
    return i2c_io_recv(dev->i2c_bus, dev->slaveAdd, (char *) buf, (int) len);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
Std_ReturnType Highdriver_init(pump_dev_t* dev) {
    if (dev == NULL) {
        dev->init_status = false;
        return ERROR_INVALID_PARAM;
    }

    Std_ReturnType st;

    st = _mp_i2c_write_block(dev, I2C_POWERMODE, 0x01);
    if (st != ERROR_OK) return st;

    st = _mp_i2c_write_block(dev, I2C_FREQUENCY, 0x40);
    if (st != ERROR_OK) return st;

    st = _mp_i2c_write_block(dev, I2C_SHAPE, 0x00);
    if (st != ERROR_OK) return st;

    st = _mp_i2c_write_block(dev, I2C_BOOST, 0x00);
    if (st != ERROR_OK) return st;

    st = _mp_i2c_write_block(dev, I2C_AUDIO, 0x00);
    if (st != ERROR_OK) return st;

    st = _mp_i2c_write_block(dev, I2C_P1VOLTAGE, 0x00);
    if (st != ERROR_OK) return st;

    st = _mp_i2c_write_block(dev, I2C_P2VOLTAGE, 0x00);
    if (st != ERROR_OK) return st;

    st = _mp_i2c_write_block(dev, I2C_P3VOLTAGE, 0x00);
    if (st != ERROR_OK) return st;

    st = _mp_i2c_write_block(dev, I2C_P4VOLTAGE, 0x00);
    if (st != ERROR_OK) return st;

    st = _mp_i2c_write_block(dev, I2C_UPDATEVOLTAGE, 0x01);
    if (st != ERROR_OK) return st;

    dev->init_status = true;
    return ERROR_OK;
}

Std_ReturnType Highdriver_enable(pump_dev_t* dev, uint8_t is_enable) {
    uint8_t v = (is_enable != 0) ? 0x01u : 0x00u; // normalize to 0/1
    return _mp_i2c_write_block(dev, I2C_POWERMODE, v); // writes [0x01, v]
}

Std_ReturnType Highdriver_setvoltage(pump_dev_t* dev, uint8_t channel, uint8_t _voltage) {
    float temp = _voltage;
    temp *= 31.0f;
    temp /= 250.0f;
    uint8_t PumpVoltage = constrain((int) temp, 0, 31);

    Std_ReturnType st;
    uint8_t idx = channel - 1;

    if (channel == 0) {
        for (int i = 0; i < 4; i++) {
            st = _mp_i2c_write_block(dev, I2C_P1VOLTAGE + i, PumpVoltage);
        }
    } else {
        st = _mp_i2c_write_block(dev, I2C_P1VOLTAGE + idx, PumpVoltage);
        if (st != ERROR_OK) return st;
    }

    st = _mp_i2c_write_block(dev, I2C_UPDATEVOLTAGE, 0x01);
    if (st != ERROR_OK) return st;

    return ERROR_OK;
}

Std_ReturnType Highdriver_setfrequency(pump_dev_t* dev, uint16_t _frequency) {
    uint8_t nFrequencyByte;

    if (_frequency >= 800) {
        nFrequencyByte = 0xFF;
    } else if (_frequency >= 400) { // 400-800 Hz
        _frequency -= 400;
        _frequency *= 64;
        _frequency /= 400;
        nFrequencyByte = (uint8_t) (_frequency | 0xC0);
    } else if (_frequency >= 200) { // 200-400 Hz
        _frequency -= 200;
        _frequency *= 64;
        _frequency /= 200;
        nFrequencyByte = (uint8_t) (_frequency | 0x80);
    } else if (_frequency >= 100) { // 100-200 Hz
        _frequency -= 100;
        _frequency *= 64;
        _frequency /= 100;
        nFrequencyByte = (uint8_t) (_frequency | 0x40);
    } else if (_frequency >= 50) { // 50-100 Hz
        _frequency -= 50;
        _frequency *= 64;
        _frequency /= 50;
        nFrequencyByte = (uint8_t) (_frequency | 0x00);
    } else { // outside of valid area
        nFrequencyByte = 0x00;
    }

    return _mp_i2c_write_block(dev, I2C_FREQUENCY, nFrequencyByte);
}
