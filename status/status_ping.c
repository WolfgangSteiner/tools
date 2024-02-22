#include <unistd.h>
#include <pthread.h>
#include "wst.h"

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
        const float a = abs(pingValue - s_pingValue) > 20.0f ? 0.5f : 0.1f;
        s_pingValue += a * (pingValue - s_pingValue);
    }
}

void decrementMissedPings() {
    s_missedPings = wst_max(0, s_missedPings - 1); 
}

void incrementMissedPings() {
    s_missedPings = wst_min(MAX_MISSED_PINGS, s_missedPings + 1);
}

void setNetworkUnreachable() {
    s_pingValue = MAX_PING;
    s_missedPings = MAX_MISSED_PINGS;
    s_networkStatus = KNetworkUnreachable;
}

void* pingThreadFunc(void* userData) {
    char* start;
    char* timeToken = "time=";

    while (true) {
        wst_strarr* arr = wst_system("ping -c 1 -W 0.5 1.1.1.1");

        if (arr->count == 0 || wst_string_contains(arr->strings[0], "unreachable")) {
            setNetworkUnreachable();
        } else if (start = strstr(arr->strings[1], timeToken)) {
            const float newPingValue = strtof(start + strlen(timeToken), 0);
            updatePingValue(newPingValue);
            decrementMissedPings();
        } else {
            incrementMissedPings();
        }

        wst_strarr_delete(arr);
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

char* formatPing() {
    if (s_pingValue == MAX_PING) {
        return formatStatusField("------", "#ff0000");
    } else {
        char* pingTimeStr = wst_string_format("%2.0f ms", s_pingValue);
        char* result = formatStatusField(pingTimeStr, pingColor(s_pingValue, s_missedPings));
        wst_string_delete(pingTimeStr);
        return result;
    }
}

void startPingThread() {
    pthread_t pingThread;
    pthread_create(&pingThread, 0, pingThreadFunc, 0);
}
