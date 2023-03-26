#include <float.h>
#include <math.h>
#include "wst.h"
#include "status_format.h"
#include "status_colors.h"


char* readStringFromFile(char* fileName)
{
    wst_string_array* arr = wst_readlines(fileName);
    if (arr->count == 0)
    {
        return NULL;
    }

    char* result = wst_string_init(arr->strings[0]);
    wst_string_array_delete(arr);
    return result;
}

float readFloatFromFile(char* fileName)
{
    char* str = readStringFromFile(fileName);

    if (str == NULL)
    {
        return 0.0f;
    }
    else
    {
        float result = atof(str);
        wst_string_delete(str);
        return result;
    }
}

bool isBatteryDischarging()
{
    char* str = readStringFromFile("/sys/class/power_supply/BAT0/status");
    bool result = wst_string_equal(str, "Discharging");
    wst_string_delete(str);
    return result;
}


float getFullBatteryCharge()
{
    return readFloatFromFile("/sys/class/power_supply/BAT0/charge_full");
}

float getCurrentBatteryCharge()
{
    return readFloatFromFile("/sys/class/power_supply/BAT0/charge_now");
}

float getBatteryPercent()
{
    return getCurrentBatteryCharge() / getFullBatteryCharge() * 100.0f;
}

float getChargingCurrent()
{
    return readFloatFromFile("/sys/class/power_supply/BAT0/current_now");
}


enum TBatStatus { STATUS_INIT, STATUS_BAT, STATUS_CHR }; 


float getAverageChargingCurrent()
{
    static float averageCurrent = FLT_MAX;
    static enum TBatStatus batteryStatus = STATUS_INIT;

    const bool batteryDischarging = isBatteryDischarging();

    if (batteryDischarging && batteryStatus == STATUS_CHR
        || !batteryDischarging && batteryStatus == STATUS_BAT)
    {
        batteryStatus = batteryDischarging ? STATUS_BAT : STATUS_CHR;
        averageCurrent = FLT_MAX;
    }


    if (averageCurrent == FLT_MAX)
    {
        averageCurrent = getChargingCurrent();
    }
    else
    {
        float diff = getChargingCurrent() - averageCurrent;
        float a = 0.05f;
        averageCurrent += a * diff;
    }

    return averageCurrent;
}

float getRemainingBatteryTime()
{
    return getCurrentBatteryCharge() / getAverageChargingCurrent();
}

float getRemainingChargingTime()
{
    float remainingCharge = getFullBatteryCharge() - getCurrentBatteryCharge();
    return remainingCharge / getAverageChargingCurrent();
}

char* formatTime(float time)
{
    int hours = time;
    int minutes = (time - hours) * 60;
    return wst_string_format("(%02d:%02d)", hours, minutes);
}

char* batteryColor(float percent)
{
    return percent > 20 ? COLOR_GREEN : percent > 10 ? COLOR_YELLOW : COLOR_RED;
}

char* chargingColor(float percent)
{
    return percent > 90.0f ? COLOR_GREEN : COLOR_YELLOW;
}

char* formatBattery()
{
    const float percent = getBatteryPercent();
    char* batteryStatus = NULL;
    char* result = NULL;
    char* timeStr = NULL;

    if (isBatteryDischarging())
    {
        batteryStatus = wst_string_format("BAT %.0f%%", percent);
        float t = getRemainingBatteryTime();
        timeStr = formatTime(t);
        batteryStatus = wst_string_append(batteryStatus, timeStr);
        result = formatStatusField(batteryStatus, batteryColor(percent));
    }
    else
    {
        batteryStatus = wst_string_format("CHR %.0f%%", percent);
        float t = getRemainingChargingTime();

        if (percent < 100.0f)
        {
            timeStr = formatTime(t);
            batteryStatus = wst_string_append(batteryStatus, timeStr);
        }
        result = formatStatusField(batteryStatus, chargingColor(percent));
    }

    wst_string_delete(timeStr);
    wst_string_delete(batteryStatus);
    return result;
}
