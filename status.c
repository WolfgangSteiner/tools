#include "wst.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>

static char* COLOR_RED    = "#ff0000";
static char* COLOR_YELLOW = "#ffbf00";
static char* COLOR_GREEN  = "#00ff00";
static char* COLOR_WHITE  = "#ffffff";

enum TNetworkStatus
{
    KNetworkUnreachable,
    KNetworkReachable
};

char* currentTime()
{
    time_t now = time(0);
    struct tm* currentTime = localtime(&now);
    char* result = wst_string_alloc(32);
    strftime(result, 32, "%H:%M", currentTime);

    return result;
}

char* formatStatusField(char* text, char* color)
{
    char* formatString = "{\"full_text\":\"%s\",\"color\":\"%s\"}";
    char* result = wst_string_format(formatString, text, color);

    return result;
}

char* formatCurrentTime()
{
    char* timeString = currentTime();
    char* result = formatStatusField(timeString, COLOR_WHITE);

    wst_string_delete(timeString);
    return result;
}


#define MAX_PING 1.0e9
#define MAX_MISSED_PINGS 10
volatile float s_pingValue = MAX_PING;
volatile int s_missedPings = 0;
volatile enum TNetworkStatus s_networkStatus = KNetworkUnreachable; 


void updatePingValue(float pingValue)
{
    if (s_networkStatus == KNetworkUnreachable)
    {
        s_pingValue = pingValue;
        s_networkStatus = KNetworkReachable;
        s_missedPings = 0;
    }
    else
    {
        const float a = abs(pingValue - s_pingValue) > 20.0f ? 0.5f : 0.1f;
        s_pingValue += a * (pingValue - s_pingValue);
    }
}

void decrementMissedPings()
{
    s_missedPings = wst_max(0, s_missedPings - 1); 
}

void incrementMissedPings()
{
    s_missedPings = wst_min(MAX_MISSED_PINGS, s_missedPings + 1);
}

void setNetworkUnreachable()
{
    s_pingValue = MAX_PING;
    s_missedPings = MAX_MISSED_PINGS;
    s_networkStatus = KNetworkUnreachable;
}

void* pingThreadFunc(void* userData)
{
    char* start;
    char* timeToken = "time=";

    while (true)
    {
        wst_string_array* arr = wst_system("ping -c 1 -W 0.5 1.1.1.1");

        if (arr->count == 0 || wst_string_contains(arr->strings[0], "unreachable"))
        {
            setNetworkUnreachable();
        }
        else if (start = strstr(arr->strings[1], timeToken))
        {
            const float newPingValue = strtof(start + strlen(timeToken), 0);
            updatePingValue(newPingValue);
            decrementMissedPings();
        }
        else
        {
            incrementMissedPings();
        }

        wst_string_array_delete(arr);
        sleep(1);
    }
}


char* pingColor(float pingValue, int missedPings)
{
    if (pingValue == MAX_PING || missedPings >= 8)
    {
        return COLOR_RED;
    }
    else if (pingValue > 50.0f || missedPings > 0)
    {
        return COLOR_YELLOW;
    }
    else
    {
        return COLOR_GREEN;
    }
}


char* formatPing()
{
    if (s_pingValue == MAX_PING)
    {
        return formatStatusField("------", "#ff0000");
    }
    else
    {
        char* pingTimeStr = wst_string_format("%3.0f ms", s_pingValue);
        char* result = formatStatusField(pingTimeStr, pingColor(s_pingValue, s_missedPings));
        wst_string_delete(pingTimeStr);
        return result;
    }
}


bool hasWifiConnection()
{
    wst_string_array* arr = wst_readlines("/proc/net/wireless");
    bool result = wst_string_array_containsSubString(arr, "wlp");
    wst_string_array_delete(arr);
    return result;
}


float parseWifiSignalLevel()
{
    wst_string_array* arr = wst_readlines("/proc/net/wireless");
    float result = 0.0f;
    
    for (int i = 0; i < arr->count; ++i)
    {
        char* line = arr->strings[i];
        if (wst_string_startsWith(line, "wlp"))
        {
            char* token = wst_string_getColumn(line, 3);
            result = strtof(token, 0);
            wst_string_delete(token);
            break;
        }
    }

    wst_string_array_delete(arr);
    return result;
}

float getWifiQuality()
{
    const float NOISE_FLOOR_DBM = -100.0f;
    const float SIGNAL_MAX_DBM = -50.0f;
    float xbm = parseWifiSignalLevel();
    xbm = wst_maxf(NOISE_FLOOR_DBM, wst_minf(SIGNAL_MAX_DBM, xbm));
    return 2.0f * (xbm + 100) + 0.5f;
}

char* getWifiColor(float wifiQuality)
{
    if (wifiQuality >= 50.0f)
    {
        return COLOR_GREEN;
    }
    else if (wifiQuality >= 25.0f)
    {
        return COLOR_YELLOW;
    }
    else
    {
        return COLOR_RED;
    }
}

char* getWifiSsid()
{
    wst_string_array* arr = wst_system("nmcli con show --active");
    wst_string_array* arr2 = wst_string_array_grep(arr, "wifi");
    char* ssid;
    if (arr2->count > 0)
    {
        ssid = wst_string_getColumn(arr2->strings[0], 0);
    }
    else
    {
        ssid = wst_string_init("???");
    }

    wst_string_array_delete(arr2);
    wst_string_array_delete(arr);

    return ssid;
}


char* formatWifiStatus()
{
    if (hasWifiConnection())
    {
        const float wifiQuality = getWifiQuality();
        char* wifiStr = getWifiSsid();
        char* qualityStr = wst_formatPercentBar(wifiQuality, 5, " -+*#");
        wifiStr = wst_string_append(wifiStr, qualityStr);
        char* result = formatStatusField(wifiStr, getWifiColor(wifiQuality));
        wst_string_delete(qualityStr);
        wst_string_delete(wifiStr);
        return result;
    }
    else
    {
        return formatStatusField("no connection", COLOR_RED);
    }
}


int main()
{
    pthread_t pingThread;
    pthread_create(&pingThread, 0, pingThreadFunc, 0);

    printf("{\"version\":1}\n");
    printf("[\n");

    while (true)
    {
        printf("[\n");

        wst_string_array* arr = wst_string_array_new();
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
