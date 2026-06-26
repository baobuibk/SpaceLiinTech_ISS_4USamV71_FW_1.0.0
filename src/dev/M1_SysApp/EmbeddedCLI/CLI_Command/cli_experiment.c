#include "cli_experiment.h"
#include "M0_App/AppOS/App_Experiment/app_experiment.h"

void CMD_DLS_setup(EmbeddedCli *cli, char *args, void *context) {

    const char *channelStr = embeddedCliGetToken(args, 1);
    const char *laserCurrentStr = embeddedCliGetToken(args, 2);
    const char *preStr = embeddedCliGetToken(args, 3);
    const char *sampleStr = embeddedCliGetToken(args, 4);
    const char *postStr = embeddedCliGetToken(args, 5);
    const char *photoRateStr = embeddedCliGetToken(args, 6);
    const char *laserRateStr = embeddedCliGetToken(args, 7);

    char *endptr;
    char buf[256];

    if (channelStr == NULL ||
            laserCurrentStr == NULL ||
            preStr == NULL ||
            sampleStr == NULL ||
            postStr == NULL ||
            photoRateStr == NULL ||
            laserRateStr == NULL) {

        embeddedCliPrint(cli,
                "Usage: dls_setup <channel> <laser_intensity> <pre_ms> <sampling_ms> <post_ms> <photo_rate_ksps> <laser_rate_ksps>");
        return;
    }

    /* Parse channel */
    uint32_t channel = strtoul(channelStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid channel");
        return;
    }

    /* Parse laser current */
    uint32_t laser_intensity = strtoul(laserCurrentStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid laser_intensity");
        return;
    }

    /* Parse pre time */
    uint32_t pre_time = strtoul(preStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid pre_time_ms");
        return;
    }

    /* Parse sampling time */
    uint32_t sampling_time = strtoul(sampleStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid main_time_ms");
        return;
    }

    /* Parse post time */
    uint32_t post_time = strtoul(postStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid post_time_ms");
        return;
    }

    /* Parse photo sampling rate */
    uint32_t photo_rate = strtoul(photoRateStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid photo_rate_ksps");
        return;
    }

    /* Parse laser sampling rate */
    uint32_t laser_rate = strtoul(laserRateStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid laser_rate_ksps");
        return;
    }
    if (laser_rate > 10) {
        embeddedCliPrint(cli, "Invalid laser_rate_lsps: recommended laser_rate is lower than 10kHz");
        return;
    }
    if (laser_rate > photo_rate) {
        embeddedCliPrint(cli, "Invalid laser_rate_lsps: recommended laser_rate is lower than photo_rate");
        return;
    }

    /* Range check */
    if (channel < 0 || channel > 24) {
        embeddedCliPrint(cli, "Invalid channel (1-24)");
        return;
    }

    if (laser_intensity > 100) {
        embeddedCliPrint(cli, "Invalid laser_intensity (0-100)");
        return;
    }

    if (pre_time > 60000 ||
            sampling_time > 60000 ||
            post_time > 60000) {

        embeddedCliPrint(cli, "Time value too large");
        return;
    }

    if (photo_rate == 0 || photo_rate > 5000) {
        embeddedCliPrint(cli, "Invalid photo_rate_ksps");
        return;
    }

    if (laser_rate == 0 || laser_rate > 5000) {
        embeddedCliPrint(cli, "Invalid laser_rate_ksps");
        return;
    }

    /* Fill profile */
    exp_profile_t profile;
    profile.channel = (uint8_t) channel;
    profile.laser_intensity = (uint16_t) laser_intensity;
    profile.pre_time_ms = (uint16_t) pre_time;
    profile.main_time_ms = (uint16_t) sampling_time;
    profile.post_time_ms = (uint16_t) post_time;
    profile.photo_adc_rate_khz = (uint16_t) photo_rate;
    profile.laser_adc_rate_khz = (uint16_t) laser_rate;
    experiment_set_profile(&profile);

    snprintf(buf, sizeof (buf),
            "DLS setup done: ch=%u, laser=%u %%, pre=%u ms, sample=%u ms, post=%u ms, photo_rate=%u kS/s, laser_rate=%u kS/s",
            (unsigned int) channel,
            (unsigned int) laser_intensity,
            (unsigned int) pre_time,
            (unsigned int) sampling_time,
            (unsigned int) post_time,
            (unsigned int) photo_rate,
            (unsigned int) laser_rate);

    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "");
}

void CMD_DLS_start(EmbeddedCli *cli, char *args, void *context) {
    exp_cmd_t cmd = EXP_CMD_RUN;

    if (experiment_command_queue == NULL) {
        embeddedCliPrint(cli, "Queue not init");
        return;
    }

    if (xQueueSend(experiment_command_queue, &cmd, 0) == pdPASS) {
        embeddedCliPrint(cli, "Experiment start OK");
    } else {
        embeddedCliPrint(cli, "Queue full, cannot start");
    }
}