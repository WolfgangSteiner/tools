#include <stdio.h>
#include <string.h>
#include "grv.h"
#include "math.h"

void print_error()
{
    printf("Usage: pavolume [inc|dec]\n");
}

float get_volume(grv_str* sink_id)
{
    grv_str cmd = grv_str_new("pactl get-sink-volume ");
    grv_str_append(&cmd, sink_id);
    grv_strarr result_arr = grv_system(&cmd);
    if (grv_strarr_size(&result_arr) == 0) {
        grv_strarr_free(&result_arr);
        grv_str_free(&cmd);
        return 0.0f;
    }
    grv_str* result = grv_strarr_front(&result_arr);
    float cur_volume = 0.0f;
    grv_str cur_volume_str = grv_str_split_tail_from_back(result, " / ");
    cur_volume = grv_str_to_f32(&cur_volume_str);
    grv_str_free(&cur_volume_str);
    grv_strarr_free(&result_arr);
    grv_str_free(&cmd);
    return cur_volume;
}

bool is_analog_headphones(grv_str* sink_id)
{
    bool result = false;
    if (grv_str_ends_with_cstr(sink_id, "analog-stereo"))
    {
        grv_strarr result_arr = grv_system_cstr("pactl list sinks | grep Active");
        if (grv_strarr_size(&result_arr) > 0)
        {
            grv_str* result_str = grv_strarr_front(&result_arr);
            result = grv_str_ends_with_cstr(result_str, "headphones");
        }
        grv_strarr_free(&result_arr);
    }
    return result;
}

grv_str get_active_sink()
{
    grv_str cmd = grv_str_new("pactl get-default-sink");
    grv_strarr result_arr = grv_system(&cmd);
    grv_str result = grv_strarr_pop_front(&result_arr);
    grv_strarr_free(&result_arr);
    return result;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        print_error();
        return (-1);
    }

    int direction = 0;
    if (strcmp(argv[1], "inc") == 0) {
        direction = 1;
    }
    else if (strcmp(argv[1], "dec") == 0) {
        direction = -1;
    }
    else {
        print_error();
        return(-1);
    }   

    grv_str sink_id = get_active_sink();
    bool headphones = is_analog_headphones(&sink_id);
    f32 cur_volume = get_volume(&sink_id);
    f32 min_volume = -60.0f;
    f32 max_volume = headphones ? -18.0f : 0.0f;
    f32 delta_volume = 3.0f;

    char* value_str; 

    if (cur_volume < min_volume || direction == -1 && cur_volume - delta_volume <= min_volume) {
        value_str = headphones ? "10%" : "20%";
    }
    else if (cur_volume > max_volume || direction == 1 && cur_volume + delta_volume >= max_volume) {
        value_str = headphones ? "50%" : "100%";
    }
    else if (direction == 1) {
        value_str = "+3db";
    } 
    else if (direction == -1) {
        value_str = "-3db";
    } 

    grv_str pactl_cmd = grv_str_new("pactl set-sink-volume ");
    grv_str_append(&pactl_cmd, &sink_id);
    grv_str_append_cstr(&pactl_cmd, " ");
    grv_str_append_cstr(&pactl_cmd, value_str);
    grv_system(&pactl_cmd);

    return 0;
}
