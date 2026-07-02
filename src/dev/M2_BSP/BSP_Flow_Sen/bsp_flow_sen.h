/* 
 * File:   bsp_flow_sen.h
 * Author: HTSANG
 *
 * Created on April 7, 2026, 2:20 PM
 */

#ifndef BSP_FLOW_SEN_H
#define	BSP_FLOW_SEN_H

#include "define.h"
#include "M3_Driver/components/dio/do.h"
#include "M3_Driver/components/i2c/i2c_io.h"
#include "M3_Driver/devices/slf3s_flow/slf3s_flow.h"

#define FLOW_SENSOR_NUMBER      1

typedef struct {
    slf3s_data_t slf3s_dat;
    bool read_oke;
} flow_sensor_data_t;

typedef struct {
    i2c_io_t *i2c;
    uint8_t   address;
    do_t     *nIRQ_pin;
    do_t     *ena_pin;
    slf3s_data_t data;
    Std_InitType init_state;
} flow_dev_t;

extern flow_dev_t flow_dev[FLOW_SENSOR_NUMBER];

Std_ReturnType bsp_flow_sen_init(uint8_t index);

Std_ReturnType bsp_flow_sen_read_all(uint8_t index);

#endif	/* BSP_FLOW_SEN_H */

