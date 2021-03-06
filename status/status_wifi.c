#include "wst.h"
#include "status_wifi.h"
#include "status_format.h"
#include "status_colors.h"


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
        char* qualityStr = wst_formatPercentBar(wifiQuality, 1, "( -+*#)");
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

