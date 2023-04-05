#include "status_colors.h"
#include "wst.h"
#include "time.h"
#include "status_format.h"
#include "grv.h"

grv_str format_day_of_month(struct tm* t) {
    grv_str time_str = {0};
    s32 day = t->tm_mday;
    char* suffix = 
        day == 1 || day == 21 || day == 31 ? "st" :
        day == 2 || day == 22 ? "nd" :
        day == 3 || day == 23 ? "rd" :
        "th";
    
    grv_str result = grv_str_from_s32(day);
    grv_str_append_cstr(&result, suffix);
    return result;
}

grv_str format_day_of_week(struct tm* t) {
    grv_str day = {0}; 
    size_t len = strftime(grv_str_cstr(&day), grv_str_get_capacity(&day), "%a", t);
    grv_str_set_size(&day, len); 
    return day;
}

grv_str format_hours_minutes(struct tm* t) {
    grv_str time_str = {0};
    size_t len = strftime(grv_str_cstr(&time_str), grv_str_get_capacity(&time_str), "%H:%M", t);
    grv_str_set_size(&time_str, len);
    return time_str;
}


char* formatCurrentTime() {
    time_t now = time(0);
    struct tm* t = localtime(&now);

    grv_str hm = format_hours_minutes(t);
    grv_str dom = format_day_of_month(t);
    grv_str dow = format_day_of_week(t);

    grv_str timestr = grv_str_copy(&dow);
    grv_str_append_char(&timestr, ' ');
    grv_str_append(&timestr, &dom);
    grv_str_append_char(&timestr, ' ');
    grv_str_append(&timestr, &hm);

    grv_str_free(&hm);
    grv_str_free(&dom);
    grv_str_free(&dow);

    int hour = t->tm_hour;
    char* color = 
        hour >= 23 || hour < 5 ? COLOR_RED 
        : hour >= 22 ? COLOR_YELLOW 
        : COLOR_WHITE; 
    char* result = formatStatusField(grv_str_cstr(&timestr), color);
    
    grv_str_free(&timestr);

    return result;
}
