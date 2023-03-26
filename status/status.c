#include "wst.h"
#include <stdio.h>
#include <unistd.h>
#include "status_colors.h"
#include "status_format.h"
#include "status_time.h"
#include "status_ping.h"
#include "status_wifi.h"
#include "status_battery.h"

int main() {
    startPingThread();

    printf("{\"version\":1}\n");
    printf("[\n");

    while (true) {
        printf("[\n");

        
        char* battery_status = formatBattery();
        char* wifi_status = formatWifiStatus();
        char* ping_status = formatPing();
        char* time_status = formatCurrentTime();

        wst_strarr* arr = wst_strarr_new();
        arr = wst_strarr_append(arr, battery_status);
        arr = wst_strarr_append(arr, wifi_status);
        arr = wst_strarr_append(arr, ping_status);
        arr = wst_strarr_append(arr, time_status);
        char* result = wst_strarr_join(arr, ",");

        printf("%s\n", result);
        printf("],\n");

        fflush(stdout);

        free(battery_status);
        free(wifi_status);
        free(ping_status);
        free(time_status);

        wst_strarr_delete(arr);
        wst_string_delete(result);

        sleep(1);
    }
}
