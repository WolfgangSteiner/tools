#include "status_colors.h"
#include "wst.h"
#include "time.h"
#include "status_format.h"

char* currentTime()
{
    time_t now = time(0);
    struct tm* currentTime = localtime(&now);
    char* result = wst_string_alloc(32);
    strftime(result, 32, "%H:%M", currentTime);

    return result;
}

char* formatCurrentTime()
{
    char* timeString = currentTime();
    char* result = formatStatusField(timeString, COLOR_WHITE);

    wst_string_delete(timeString);
    return result;
}
