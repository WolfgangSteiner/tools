#include <unistd.h>
#include <pthread.h>
#include "grv/grv_cstr.h"
#include "grv/grv_str.h"
#include "grv/grv_util.h"
#include "grv/grv_log.h"
#include "grv/grv_common.h"

enum TNetworkStatus {
    KNetworkUnreachable,
    KNetworkReachable
};

#define MAX_PING 1.0e9
#define MAX_MISSED_PINGS 10
volatile float s_pingValue = MAX_PING;
volatile int s_missedPings = 0;
volatile enum TNetworkStatus s_networkStatus = KNetworkUnreachable; 


void updatePingValue(float pingValue) {
    if (s_networkStatus == KNetworkUnreachable) {
        s_pingValue = pingValue;
        s_networkStatus = KNetworkReachable;
        s_missedPings = 0;
    } else {
        const float a = grv_abs_f32(pingValue - s_pingValue) > 20.0f ? 0.5f : 0.1f;
        s_pingValue += a * (pingValue - s_pingValue);
    }
}

void decrementMissedPings(void) {
    s_missedPings = grv_max_s32(0, s_missedPings - 1); 
}

void incrementMissedPings(void) {
    s_missedPings = grv_min_s32(MAX_MISSED_PINGS, s_missedPings + 1);
}

void setNetworkUnreachable(void) {
    s_pingValue = MAX_PING;
    s_missedPings = MAX_MISSED_PINGS;
    s_networkStatus = KNetworkUnreachable;
}

void* pingThreadFunc(void* userData) {
    grv_str_t time_token = grv_str_ref("time=");
    while (true) {
        grv_strarr_t arr = grv_system_with_capture_cstr("ping -c 1 -W 0.5 1.1.1.1");
        if (arr.size < 2 || grv_str_contains_cstr(arr.arr[0], "unreachable")) {
            setNetworkUnreachable();
        } else if (grv_str_contains_str(*grv_strarr_at(arr, 1), time_token)) {
            grv_str_t response = *grv_strarr_at(arr, 1);
            const float new_ping_value = grv_str_to_f32(grv_str_split_tail_from_front(response, time_token));
            updatePingValue(new_ping_value);
            decrementMissedPings();
        } else {
            incrementMissedPings();
        }
        grv_strarr_free(&arr);
        sleep(1);
    }
}

char* pingColor(float pingValue, int missedPings) {
    if (pingValue == MAX_PING || missedPings >= 8) {
        return COLOR_RED;
    } else if (pingValue > 50.0f || missedPings > 0) {
        return COLOR_YELLOW;
    } else {
        return COLOR_GREEN;
    }
}

char* formatPing(void) {
    if (s_pingValue == MAX_PING) {
        return formatStatusField("------", "#ff0000");
    } else {
        grv_str_t ping_time_str = grv_str_format(grv_str_ref("{f32:2.0}ms"), s_pingValue);
        char* result = formatStatusField(grv_str_cstr(ping_time_str), pingColor(s_pingValue, s_missedPings));
        grv_str_free(&ping_time_str);
        return result;
    }
}

void startPingThread(void) {
    pthread_t pingThread;
    pthread_create(&pingThread, 0, pingThreadFunc, 0);
}
