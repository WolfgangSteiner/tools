#include <float.h>
#include <pthread.h>
#include "grv/grv.h"
#include "grv/grv_base.h"
#include <stdatomic.h>

float readFloatFromFile(char* filename) {
    float result = 0.0f;
    grv_str_return_t res = grv_read_file(grv_str_ref(filename));
    grv_str_t str = grv_str_strip_char(res.str, '\n');
    if (grv_str_is_float(str)) {
        result = grv_str_to_f32(str);
    }
    grv_str_free(&str);
    return result;
}

bool isBatteryDischarging(void) {
    grv_str_return_t res = grv_read_file(grv_str_ref("/sys/class/power_supply/BAT0/status"));
    grv_str_t status_str = res.str;
    bool result = grv_str_starts_with_cstr(status_str, "Discharging");
    grv_str_free(&status_str);
    return result;
}

float getFullBatteryCharge(void) {
    float charge = readFloatFromFile("/sys/class/power_supply/BAT0/charge_full");
    return grv_max_f32(charge, 1);
}

float getCurrentBatteryCharge(void) {
    return readFloatFromFile("/sys/class/power_supply/BAT0/charge_now");
}

float getBatteryPercent(void) {
    float res = getCurrentBatteryCharge() / getFullBatteryCharge() * 100.0f;
    return grv_clamp_f32(res, 0.0f, 100.0f);
}

float getChargingCurrent(void) {
    return readFloatFromFile("/sys/class/power_supply/BAT0/current_now");
}

enum TBatStatus { STATUS_INIT, STATUS_BAT, STATUS_CHR }; 

_Atomic double averageCurrent = FLT_MAX;
static enum TBatStatus battery_status = STATUS_INIT;

void* update_average_charging_current(void* user_data) {
    GRV_UNUSED(user_data);
    while (true) {
        const bool batteryDischarging = isBatteryDischarging();
        double avg_current_local = atomic_load(&averageCurrent);

        if ((batteryDischarging && battery_status == STATUS_CHR)
            || (!batteryDischarging && battery_status == STATUS_BAT)) {
            battery_status = batteryDischarging ? STATUS_BAT : STATUS_CHR;
            avg_current_local = FLT_MAX;
        }

        if (avg_current_local == FLT_MAX) {
            avg_current_local = getChargingCurrent();
        } else {
            double diff = getChargingCurrent() - averageCurrent;
            double a = 0.01;
            avg_current_local += a * diff;
        }
        atomic_store(&averageCurrent, avg_current_local);
        usleep(500000);
    }
    return NULL;
}

void start_charging_current_thread(void) {
    pthread_t charging_current_thread;
    pthread_create(&charging_current_thread, 0, update_average_charging_current, 0);
}

float getRemainingBatteryTime(void) {
    return getCurrentBatteryCharge() / atomic_load(&averageCurrent);
}

float getRemainingChargingTime(void) {
    float remainingCharge = getFullBatteryCharge() - getCurrentBatteryCharge();
    return remainingCharge / atomic_load(&averageCurrent);
}

grv_str_t formatTime(float time) {
    int time_in_seconds = time * 3600.0f;
    time_in_seconds = (time_in_seconds + 150) / 300 * 300;
    i32 hours = time_in_seconds / 3600;
    i32 minutes = (time_in_seconds / 60) % 60;
    return grv_str_format(grv_str_ref(" {int}:{int:02}"), hours, minutes);
}

char* batteryColor(float percent) {
    return percent > 20 ? COLOR_GREEN : percent > 10 ? COLOR_YELLOW : COLOR_RED;
}

char* chargingColor(float percent) {
    return percent > 90.0f ? COLOR_GREEN : COLOR_YELLOW;
}

char* formatBattery(void) {
    const float percent = getBatteryPercent();
    char* result = NULL;
    grv_str_t time_str = {0};
    grv_str_t battery_status = {0};

    if (isBatteryDischarging()) {
        battery_status = grv_str_format(grv_str_ref("BAT {f32:02.0}%"), percent);
        float t = getRemainingBatteryTime();
        time_str = formatTime(t);
        grv_str_append(&battery_status, time_str);
        result = formatStatusField(grv_str_cstr(battery_status), batteryColor(percent));
    } else {
        battery_status = grv_str_format(grv_str_ref("CHR {f32:02.0}%"), percent);
        float t = getRemainingChargingTime();

        if (percent < 100.0f) {
            time_str = formatTime(t);
            grv_str_append(&battery_status, time_str);
        }
        result = formatStatusField(grv_str_cstr(battery_status), chargingColor(percent));
    }

    grv_str_free(&time_str);
    grv_str_free(&battery_status);
    return result;
}
