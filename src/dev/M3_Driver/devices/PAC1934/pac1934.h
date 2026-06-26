#ifndef PAC1934_H_
#define PAC1934_H_

#include "stdint.h"
#include "stdbool.h"
#include "i2c_io.h"

#define PAC1934_REFRESH         0x00U
#define PAC1934_CTRL            0x01U
#define PAC1934_CHANNEL_DIS     0x1CU
#define PAC1934_NEG_PWR         0x1DU
#define PAC1934_REFRESH_G       0x1EU
#define PAC1934_REFRESH_V       0x1FU
#define PAC1934_SLOW            0x20U
#define PAC1934_VBUS1_AVG       0x0FU
#define PAC1934_VSENSE1_AVG     0x13U
#define PAC1934_PID_REG         0xFDU
#define PAC1934_PRODUCT_ID      0x5BU
#define PAC1934_CHANNELS        4U

typedef struct {
    i2c_io_t *i2c_bus;
    uint8_t   slaveAdd;
    uint32_t  rsense_mohm;
    bool      init_status;
} pac1934_dev_t;

typedef struct {
    float vbus_v[PAC1934_CHANNELS];
    float current_a[PAC1934_CHANNELS];
} pac1934_data_t;

uint32_t PAC1934_init(pac1934_dev_t *dev);
uint32_t PAC1934_read_all(pac1934_dev_t *dev, pac1934_data_t *data);

#endif /* PAC1934_H_ */
