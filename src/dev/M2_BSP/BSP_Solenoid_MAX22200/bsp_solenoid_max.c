#include "bsp_solenoid_max.h"
#include "BSP_Board/bsp_core.h"
#include "M3_Driver/devices/MAX22200/max22200.h"
#include "define.h"
#include "M1_SysApp/xlog/xlog.h"

max22200_dev_t* g_max22200_dev[MAX22200_NUM_DEVICES] = {0};

static do_t *g_cs[MAX22200_NUM_DEVICES] = {
    &sole_A_cs,
    &sole_B_cs,
};

static do_t *g_cmd[MAX22200_NUM_DEVICES] = {
    &sole_A_cmd,
    &sole_B_cmd,
};

/* ------------------------------------------------------------------ */
/*  Internal helper                                                     */
/* ------------------------------------------------------------------ */

/*
 * Gi?i mã pair index toàn c?c (0..TOTAL_PAIRS-1) thành:
 *   dev     : device index (0..NUM_DEVICES-1)
 *   high_ch : channel ch?n  ? high-side switch c?a c?p
 *   low_ch  : channel l?    ? low-side switch c?a c?p
 *
 * Ví d?: pair=5 ? dev=1, local=1 ? high_ch=2, low_ch=3
 */
static void pair_decode(uint8_t pair,
                        uint8_t *dev,
                        uint8_t *high_ch,
                        uint8_t *low_ch)
{
    *dev     = pair / MAX22200_PAIRS_PER_DEVICE;
    uint8_t local = pair % MAX22200_PAIRS_PER_DEVICE;
    *high_ch = local * 2u;
    *low_ch  = local * 2u + 1u;
}

/* ------------------------------------------------------------------ */
/*  Init                                                                */
/* ------------------------------------------------------------------ */

int bsp_solenoid_max_init(void)
{
    struct max22200_init_param init_param = {0};
    int status;

    init_param.spi = &spi2;

    for (uint8_t i = 0; i < MAX22200_CHANNELS_CONFIG; i++)
        init_param.ch_config[i] = MAX22200_HALF_BRIDGE_MODE;

    for (uint8_t dev = 0; dev < MAX22200_NUM_DEVICES; dev++) {
        init_param.cs  = g_cs[dev];
        init_param.cmd = g_cmd[dev];

        status = max22200_init(&g_max22200_dev[dev], &init_param);
        if (status)
            return status;

        for (uint8_t ch = 0; ch < MAX22200_CHANNELS; ch++) {
            status = max22200_set_ch_mode(
                    g_max22200_dev[dev],
                    ch,
                    MAX22200_VOLTAGE_DRIVE,
                    MAX22200_HIGH_SIDE,
                    MAX22200_HALF_BRIDGE_MODE);
            if (status)
                return status;

            status = max22200_set_ch_hit(
                    g_max22200_dev[dev],
                    ch,
                    127,
                    MAX22200_HIT_NO_TIME);
            if (status)
                return status;

            status = max22200_set_ch_hold(
                    g_max22200_dev[dev],
                    ch,
                    127);
            if (status)
                return status;

            status = max22200_set_ch_scale(
                    g_max22200_dev[dev],
                    ch,
                    MAX22200_FULLSCALE);
            if (status)
                return status;

            status = max22200_set_ch_trig(
                    g_max22200_dev[dev],
                    ch,
                    MAX22200_ONCH_SPI);
            if (status)
                return status;
        }
    }

    return 0;
}

/* ------------------------------------------------------------------ */
/*  Control                                                             */
/* ------------------------------------------------------------------ */

int bsp_solenoid_set(uint8_t pair, sol_direction_t dir)
{
    if (pair >= MAX22200_TOTAL_PAIRS)
        return ERROR_INVALID_PARAM;

    uint8_t dev_ind, high_ch, low_ch;
    pair_decode(pair, &dev_ind, &high_ch, &low_ch);

    int ret;
    
    if (g_max22200_dev[dev_ind]->init_state != INIT_DONE) {
        bsp_solenoid_max_init();
        xlog("SLN_INIT");
    }

    switch (dir) {
    case SOL_DIR_FORWARD:
        ret = max22200_set_ch_state(g_max22200_dev[dev_ind], low_ch,  false);
        if (ret) return ret;
        ret = max22200_set_ch_state(g_max22200_dev[dev_ind], high_ch, true);
        break;

    case SOL_DIR_REVERSE:
        ret = max22200_set_ch_state(g_max22200_dev[dev_ind], high_ch, false);
        if (ret) return ret;
        ret = max22200_set_ch_state(g_max22200_dev[dev_ind], low_ch,  true);
        break;

    case SOL_DIR_OFF:
    default:
        ret = max22200_set_ch_state(g_max22200_dev[dev_ind], high_ch, false);
        if (ret) return ret;
        ret = max22200_set_ch_state(g_max22200_dev[dev_ind], low_ch,  false);
        break;
    }

    return ret;
}