#include "mcp4902.h"
#include "M3_Driver/components/qspi/qspi_io.h"

/* -------- Helpers -------- */

static inline void mcp4902_latch_pulse(mcp4902_dev_t *dev) {
    do_reset(dev->latch);
    do_set(dev->latch);
}

static int mcp4902_write_frame(mcp4902_dev_t *dev, uint16_t frame) {
    uint8_t tx[2] = {(uint8_t) (frame >> 8), (uint8_t) frame};
    uint8_t rx[2];
    uint32_t st;

    do_reset(dev->cs);
    if (dev->bus == MCP_BUS_SPI) {
        spi_io_set_mode(dev->spi, 0);
        st = spi_io_transfer_sync(dev->spi, tx, rx, 2);
    } else if (dev->bus == MCP_BUS_QSPI) {
        qspi_io_set_mode(dev->qspi, 0);
        st = qspi_io_transfer_sync(dev->qspi, tx, rx, 2);
    }
    do_set(dev->cs);

    mcp4902_latch_pulse(dev);

    return (st == ERROR_OK) ? (int) ERROR_OK : (int) st;
}

static uint16_t mcp4902_build_frame(uint8_t channel, uint8_t code, uint8_t gain1x, uint8_t enable) {
    uint16_t f = 0;

    if (channel == MCP4902_CHB) f |= (1u << MCP4902_AB_BIT);
    if (gain1x) f |= (1u << MCP4902_GA_BIT); /* 1 = 1x */
    if (enable) f |= (1u << MCP4902_SHDN_BIT); /* 1 = bật */

    f |= ((uint16_t) code << 4);
    return f;
}

/* -------- Public API -------- */

uint8_t mcp4902_vol_2_code(uint16_t mv) {
    /* round( mv * 255 / Vref ) */
    uint32_t num = (uint32_t) mv * 255u + (MCP4902_VREF_MV / 2u);
    uint32_t code = num / MCP4902_VREF_MV;
    if (code > 255u) code = 255u;
    return (uint8_t) code;
}

uint16_t mcp4902_code_2_vol(uint8_t code) {
    /* round( code * Vref / 255 ) */
    uint32_t num = (uint32_t) code * MCP4902_VREF_MV + 127u;
    return (uint16_t) (num / 255u);
}

int mcp4902_dev_spi_init(mcp4902_dev_t *dev, spi_io_t *spi, do_t *cs, do_t *latch) {

    dev->spi = spi;
    dev->cs = cs;
    dev->latch = latch;
    dev->bus = MCP_BUS_SPI;

    do_set(dev->cs);
    do_set(dev->latch);

    dev->dac_channel[MCP4902_CHA] = 0u;
    dev->dac_channel[MCP4902_CHB] = 0u;

    int rc = mcp4902_flush(dev);
    if (rc != (int) ERROR_OK) return rc;

    rc = mcp4902_shutdown(dev, MCP4902_CHA);
    if (rc != (int) ERROR_OK) return rc;

    rc = mcp4902_shutdown(dev, MCP4902_CHB);
    return rc;
}

int mcp4902_dev_qspi_init(mcp4902_dev_t *dev, qspi_io_t *qspi, do_t *cs, do_t *latch) {

    dev->qspi = qspi;
    dev->cs = cs;
    dev->latch = latch;
    dev->bus = MCP_BUS_QSPI;

    do_set(dev->cs);
    do_set(dev->latch);

    dev->dac_channel[MCP4902_CHA] = 0u;
    dev->dac_channel[MCP4902_CHB] = 0u;

    int rc = mcp4902_flush(dev);
    if (rc != (int) ERROR_OK) return rc;

    rc = mcp4902_shutdown(dev, MCP4902_CHA);
    if (rc != (int) ERROR_OK) return rc;

    rc = mcp4902_shutdown(dev, MCP4902_CHB);
    return rc;
}

int mcp4902_shutdown(mcp4902_dev_t *dev, uint8_t channel) {
    if (!dev || channel >= MCP4902_NUM_CHANNEL) return (int) ERROR_INVALID_PARAM;

    /* GA=1 (1x), SHDN=0 (tắt) theo phong cách cũ */
    uint16_t frame = mcp4902_build_frame(channel, 0u, /*gain1x=*/1u, /*enable=*/0u);
    return mcp4902_write_frame(dev, frame);
}

int mcp4902_set_dac(mcp4902_dev_t *dev, uint8_t channel, uint8_t code) {
    if (!dev || channel >= MCP4902_NUM_CHANNEL) return (int) ERROR_INVALID_PARAM;
    dev->dac_channel[channel] = code;
    return mcp4902_flush(dev);
}

int mcp4902_set_vol(mcp4902_dev_t *dev, uint8_t channel, uint16_t mv) {
    if (!dev || channel >= MCP4902_NUM_CHANNEL) return (int) ERROR_INVALID_PARAM;
    dev->dac_channel[channel] = mcp4902_vol_2_code(mv);
    return mcp4902_flush(dev);
}

int mcp4902_flush(mcp4902_dev_t *dev) {
    if (!dev) return (int) ERROR_INVALID_PARAM;

    /* Gửi lần lượt kênh A rồi B (giữ nguyên trình tự/toggle latch như mã gốc) */
    uint16_t fA = mcp4902_build_frame(MCP4902_CHA, dev->dac_channel[MCP4902_CHA],
            /*gain1x=*/1u, /*enable=*/1u);
    int rc = mcp4902_write_frame(dev, fA);
    if (rc != (int) ERROR_OK) return rc;

    uint16_t fB = mcp4902_build_frame(MCP4902_CHB, dev->dac_channel[MCP4902_CHB],
            /*gain1x=*/1u, /*enable=*/1u);
    return mcp4902_write_frame(dev, fB);
}
