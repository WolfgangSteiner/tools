#include <stdio.h>
#include "grv/grv.h"
#include "grv/grv_str.h"
#include "grv/grv_strarr.h"

int time_str_to_sec(grv_str_t str) {
    int hours = 0;
    int minutes = 0;
    int seconds = 0;

    grv_str_t sep = grv_str_ref(":");
    grv_strpair_t pair = grv_str_split_head_front(str, sep);
    if (grv_str_contains_char(pair.second, ':')) {
        hours = grv_str_to_int(pair.first);
        pair = grv_str_split_head_front(pair.second, sep);
        minutes = grv_str_to_int(pair.first);
        seconds = grv_str_to_int(pair.second);
    } else {
        minutes = grv_str_to_int(pair.first);
        seconds = grv_str_to_int(pair.second);
    }

    return hours * 3600 + minutes * 60 + seconds;
}
        
grv_str_t sec_to_time_str(int sec) {
    int hours = sec / 3600;
    sec -= hours * 3600;
    int minutes = sec / 60;
    int seconds = sec % 60;
    grv_str_t res;

    if (hours > 0) {
        res = grv_str_format(grv_str_ref("{int}:{int:02}:{int:02}"), hours, minutes, seconds);
    } else {
        res = grv_str_format(grv_str_ref("{int}:{int:02}"), minutes, seconds);
    }

    return res;
}
    
int convert_miles_to_k(int sec_per_mile) {
    int sec_per_k = (int)(sec_per_mile / 1.609f + 0.5f);
    return sec_per_k;
}


int main(int argc, char** argv) {
    if (argc == 1) {
        printf("Missing min/mile pace");
        exit(1);
    }

    grv_strarr_t args = grv_strarr_new_from_cstrarr(argv + 1,  argc - 1);
    bool compute_marathon_pace = false;

    while (grv_strarr_size(args)) {
        grv_str_t arg = grv_strarr_pop_front(&args);
        if (grv_str_eq_cstr(arg, "--marathon")) {
            compute_marathon_pace = true;
        } else if (grv_str_starts_with(arg, "--")) {
            grv_log_error(grv_str_format(grv_str_ref("Invalid option {str}"), arg));
            exit(1);
        } else if (compute_marathon_pace) {
            int total_time = time_str_to_sec(arg);
            int sec_per_k = (int)((float)total_time / 42.195f);
            grv_str_t res = sec_to_time_str(sec_per_k);
            grv_str_t msg = grv_str_format(grv_str_ref("{str} marathon time --> {str} min/km\n"), arg, res);
            grv_str_print(msg);
        } else {
            int sec_per_mile = time_str_to_sec(arg);
            int sec_per_k = convert_miles_to_k(sec_per_mile);
            grv_str_t res = sec_to_time_str(sec_per_k);
            grv_str_t msg = grv_str_format(grv_str_ref("{str} min/mile --> {str} min/km\n"), arg, res);
            grv_str_print(msg);
        }
    }
}
