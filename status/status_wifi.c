#include "grv/grv.h"
#include <stdio.h>

bool hasWifiConnection(void) {
    grv_strarr_t arr = grv_readlines(grv_str_ref("/proc/net/wireless"));
    bool result = grv_strarr_any_contains_cstr(arr, "wlp")
	    || grv_strarr_any_contains_cstr(arr, "wlan0");
    grv_strarr_free(&arr);
    return result;
}

static bool starts_with_wlp(grv_str_t str) {
    str = grv_str_lstrip(str);
    return grv_str_starts_with_cstr(str, "wlp") || grv_str_starts_with_cstr(str, "wlan"); 
}

float parse_wifi_signal_level(void) {
    grv_strarr_t arr = grv_readlines(grv_str_ref("/proc/net/wireless"));
    //grv_strarr_t arr = grv_readlines(grv_str_ref("/tmp/test.txt"));
    grv_strarr_t arr2 = grv_strarr_filter(arr, starts_with_wlp);
    float result = 0.0f;

    if (arr2.size > 0) {
        grv_str_t line = grv_str_lstrip(*grv_strarr_at(arr2, 0));
        grv_strarr_t token_arr = grv_str_split_whitespace(line);
        grv_str_t* level_str = grv_strarr_at(token_arr, 3);
        result = grv_str_to_f32(*level_str);
        grv_strarr_free(&token_arr);
        grv_str_free(&line);
    }
    grv_strarr_free(&arr);
    grv_strarr_free(&arr2);
    return result;
}

float get_wifi_quality(void) {
    const float NOISE_FLOOR_DBM = -100.0f;
    const float SIGNAL_MAX_DBM = -50.0f;
    float xbm = parse_wifi_signal_level();
    xbm = grv_max_f32(NOISE_FLOOR_DBM, grv_min_f32(SIGNAL_MAX_DBM, xbm));
    return 2.0f * (xbm + 100) + 0.5f;
}

char* get_wifi_color(float quality) {
    return quality >= 50.0f ? COLOR_GREEN : quality >= 25.0f ? COLOR_YELLOW : COLOR_RED;
}

grv_str_t get_wifi_ssid(void) {
    grv_strarr_t arr = grv_system_with_capture_cstr("nmcli con show --active");
    grv_str_t* wifi_str = grv_strarr_find_str_containing_cstr(arr, "wifi");
    grv_str_t ssid;
    if (wifi_str == NULL) {
        ssid = grv_str_ref("???");
    } else {
        ssid = grv_str_split_head_at_char(*wifi_str, ' ');
        if (grv_str_eq_cstr(ssid, "AuroraBorealis")) {
            ssid = grv_str_ref("W");
        } else {
            ssid = grv_str_copy(ssid);
        }
    }
    grv_strarr_free(&arr);
    return ssid;
}

char* formatWifiStatus(void) {
    if (hasWifiConnection()) {
        const float wifi_quality = get_wifi_quality();
        grv_str_t ssid = get_wifi_ssid();
        grv_str_t wifi_str = grv_str_format(grv_str_ref("{str} {f32:.0}%"), ssid, wifi_quality);
        char* result = formatStatusField(grv_str_cstr(wifi_str), get_wifi_color(wifi_quality));
        grv_str_free(&ssid);
        grv_str_free(&wifi_str);
        return result;
    } else {
        return formatStatusField("no connection", COLOR_RED);
    }
}
