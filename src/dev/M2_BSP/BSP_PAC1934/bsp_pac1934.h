#ifndef BSP_PAC1934_H_
#define BSP_PAC1934_H_

#include "define.h"
#include "M3_Driver/devices/PAC1934/pac1934.h"

extern pac1934_dev_t pac1934_hi;
extern pac1934_dev_t pac1934_lo;

uint32_t bsp_pac1934_init(pac1934_dev_t *dev);
uint32_t bsp_pac1934_read_all(pac1934_dev_t *dev, pac1934_data_t *data);

#endif /* BSP_PAC1934_H_ */
