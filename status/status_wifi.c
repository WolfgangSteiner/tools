#include "wst.h"
#include "grv/grv.h"
#include <stdio.h>

bool hasWifiConnection() {
    wst_strarr* arr = wst_readlines("/proc/net/wireless");
    bool result = wst_strarr_containsSubString(arr, "wlp")
	    || wst_strarr_containsSubString(arr, "wlan0");
    wst_strarr_delete(arr);
    return result;
}

static bool starts_with_wlp(grv_str_t str) {
    str = grv_str_lstrip(str);
    return grv_str_starts_with_cstr(str, "wlp") || grv_str_starts_with_cstr(str, "wlan"); 
}

float parse_wifi_signal_level() {
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

float get_wifi_quality() {
    const float NOISE_FLOOR_DBM = -100.0f;
    const float SIGNAL_MAX_DBM = -50.0f;
    float xbm = parse_wifi_signal_level();
    xbm = grv_max_f32(NOISE_FLOOR_DBM, wst_minf(SIGNAL_MAX_DBM, xbm));
    return 2.0f * (xbm + 100) + 0.5f;
}

char* get_wifi_color(float quality) {
    return quality >= 50.0f ? COLOR_GREEN : quality >= 25.0f ? COLOR_YELLOW : COLOR_RED;
}

char* get_wifi_ssid() {
    wst_strarr* arr = wst_system("nmcli con show --active");
    wst_strarr* arr2 = wst_strarr_grep(arr, "wifi");
    char* ssid;
    if (arr2->count > 0) {
        ssid = wst_string_getColumn(arr2->strings[0], 0);
    } else {
        ssid = wst_string_init("???");
    }

    if (wst_string_startsWith(ssid, "AuroraBorealis")) {
        wst_string_delete(ssid);
        ssid = wst_string_init("W");
    }

    wst_strarr_delete(arr2);
    wst_strarr_delete(arr);
    return ssid;
}

char* formatWifiStatus() {
    if (hasWifiConnection()) {
        const float wifi_quality = get_wifi_quality();
        char* ssid = get_wifi_ssid();
        char* wifi_str = grv_cstr_new_with_format("%s %.0f%%", ssid, wifi_quality);
        char* result = formatStatusField(wifi_str, get_wifi_color(wifi_quality));
        free(ssid);
        grv_cstr_free(wifi_str);
        return result;
    } else {
        return formatStatusField("no connection", COLOR_RED);
    }
}
