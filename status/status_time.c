#include "wst.h"
#include "time.h"
#include "grv/grv_str.h"

grv_str_t format_day_of_month(struct tm* t) {
    s32 day = t->tm_mday;
    char* suffix = 
        day == 1 || day == 21 || day == 31 ? "st" :
        day == 2 || day == 22 ? "nd" :
        day == 3 || day == 23 ? "rd" :
        "th";
    
    return grv_str_new_with_format("%d%s", day, suffix);
}

grv_str_t format_day_of_week(struct tm* t) {
    char buffer[64];
    size_t len = strftime(buffer, 64, "%a", t);
    return grv_str_new(buffer);
}

grv_str_t format_hours_minutes(struct tm* t) {
    char buffer[64];
    size_t len = strftime(buffer, 64, "%H:%M", t);
    return grv_str_new(buffer);
}

char* formatCurrentTime() {
    time_t now = time(0);
    struct tm* t = localtime(&now);

    grv_str_t hm = format_hours_minutes(t);
    grv_str_t dom = format_day_of_month(t);
    grv_str_t dow = format_day_of_week(t);

    grv_str_t timestr = grv_str_copy(dow);
    grv_str_append_char(&timestr, ' ');
    grv_str_append(&timestr, dom);
    grv_str_append_char(&timestr, ' ');
    grv_str_append(&timestr, hm);

    grv_str_free(&hm);
    grv_str_free(&dom);
    grv_str_free(&dow);

    int hour = t->tm_hour;
    char* color = 
        hour >= 23 || hour < 5 ? COLOR_RED 
        : hour >= 22 ? COLOR_YELLOW 
        : COLOR_WHITE; 
    char* result = formatStatusField(grv_str_cstr(timestr), color);
    
    grv_str_free(&timestr);

    return result;
}
