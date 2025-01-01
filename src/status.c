#include "grv/grv.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define COLOR_RED    "#ff0000"
#define COLOR_YELLOW "#ffbf00"
#define COLOR_GREEN  "#00ff00"
#define COLOR_WHITE  "#ffffff"

static char* formatStatusField(char* text, char* color) {
    char* formatString = "{\"full_text\":\" %s \",\"color\":\"%s\"}";
    char* result = grv_cstr_new_with_format(formatString, text, color);
    return result;
}

#include "status/status_battery.c"
#include "status/status_ping.c"
#include "status/status_time.c"
#include "status/status_wifi.c"
#include "status/status_memory.c"

int main(int argc, char** argv) {
    grv_strarr_t args = grv_strarr_new_from_cstrarr(argv + 1, argc - 1);
    bool no_sleep = grv_strarr_contains_cstr(args, "--no-sleep");
    grv_strarr_free(&args);

    startPingThread();
    start_charging_current_thread();

    printf("{\"version\":1}\n");
    printf("[\n");

    while (true) {
        printf("[\n");
        
        char* battery_status = formatBattery();
        char* wifi_status = formatWifiStatus();
        char* ping_status = formatPing();
        char* time_status = formatCurrentTime();
        char* memory_status = format_memory_usage();

        grv_strarr_t arr = grv_strarr_new(); 
        grv_strarr_push(&arr, grv_str_ref(battery_status));
        grv_strarr_push(&arr, grv_str_ref(wifi_status));
        grv_strarr_push(&arr, grv_str_ref(ping_status));
        grv_strarr_push(&arr, grv_str_ref(memory_status));
        grv_strarr_push(&arr, grv_str_ref(time_status));
        grv_str_t result = grv_strarr_join(arr, grv_str_ref(","));

        printf("%s\n", grv_str_cstr(result));
        printf("],\n");

        fflush(stdout);

        free(battery_status);
        free(wifi_status);
        free(ping_status);
        free(time_status);
        free(memory_status);

        grv_strarr_free(&arr);
        grv_str_free(&result);

        if (!no_sleep) sleep(1);
    }
}
