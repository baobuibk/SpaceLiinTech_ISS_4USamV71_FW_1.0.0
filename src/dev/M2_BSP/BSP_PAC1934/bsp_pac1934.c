#include "bsp_pac1934.h"
#include "bsp_board/bsp_core.h"

#define PAC1934_RSENSE_MOHM     10U

pac1934_dev_t pac1934_hi = {
    .i2c_bus      = &i2c0,
    .slaveAdd     = 0x1FU,
    .rsense_mohm  = PAC1934_RSENSE_MOHM,
    .init_status  = false,
};

uint32_t bsp_pac1934_init(pac1934_dev_t *dev)
{
    if (dev == NULL) return ERROR_INVALID_PARAM;
    return PAC1934_init(dev);
}

uint32_t bsp_pac1934_read_all(pac1934_dev_t *dev, pac1934_data_t *data)
{
    return PAC1934_read_all(dev, data);
}