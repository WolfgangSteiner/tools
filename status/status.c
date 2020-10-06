#include "wst.h"
#include <stdio.h>
#include <unistd.h>
#include "status_colors.h"
#include "status_format.h"
#include "status_time.h"
#include "status_ping.h"
#include "status_wifi.h"
#include "status_battery.h"

int main()
{
    startPingThread();

    printf("{\"version\":1}\n");
    printf("[\n");

    while (true)
    {
        printf("[\n");

        wst_string_array* arr = wst_string_array_new();
        arr = wst_string_array_append(arr, formatBattery());
        arr = wst_string_array_append(arr, formatWifiStatus());
        arr = wst_string_array_append(arr, formatPing());
        arr = wst_string_array_append(arr, formatCurrentTime());
        char* result = wst_string_array_join(arr, ",");

        printf("%s\n", result);
        printf("],\n");

        fflush(stdout);

        wst_string_array_delete(arr);
        wst_string_delete(result);

        sleep(1);
    }
}
