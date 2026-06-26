#include "bsp_temperature.h"
#include "M0_App/AppOS/Database/DB_Temperature_Profile/db_temperature_profile.h"

static uint8_t profile_id_target = 0xFF;
static uint8_t step_count = 0;
static profileData_t profile_init;

void bsp_temperature_prepare_to_config_profile(uint8_t profile_id) {
    profile_id_target = profile_id;
}

void bsp_temperature_config_main_ntc(uint8_t main_ntc) {
    profile_init.main_ntc = main_ntc;
}

void bsp_temperature_config_sec_ntc(uint8_t sec_ntc) {
    profile_init.sec_ntc = sec_ntc;
}

void bsp_temperature_config_tec_mask(uint8_t tec_mask) {
    profile_init.tec_mask = tec_mask;
}

void bsp_temperature_config_heater_mask(uint8_t heater_mask) {
    profile_init.heater_mask = heater_mask;
}

void bsp_temperature_config_setpoint(int16_t setpoint) {
    profile_init.set_point = setpoint;
}

void bsp_temperature_config_main_sec_delta(int16_t main_sec_delta) {
    profile_init.main_sec_delta = main_sec_delta;
}

void bsp_temperature_config_step(uint8_t step_id, uint16_t start_temp, uint16_t stop_temp, uint16_t duration, uint8_t mode) {
    step_count++;
    profile_init.steps[step_id].start_point = start_temp;
    profile_init.steps[step_id].set_point = stop_temp;
    profile_init.steps[step_id].duration = duration;
    profile_init.steps[step_id].control_mode = mode;
}

uint8_t bsp_temperature_config_save(void) {
    // Save
    profile_init.step_count = step_count;
    DB_temp_profile_write(profile_id_target, &profile_init);
    step_count = 0;
    return profile_id_target;
}

void bsp_temperature_config_cancel(void) {
    // Clear
    profile_id_target = 0xFF;
    step_count = 0;
}