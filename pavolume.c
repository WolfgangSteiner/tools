#include "wst.h"
#include "math.h"

void print_error()
{
    printf("Usage: pavolume [inc|dec]\n");
}

float get_volume(char* sink_id)
{
    char* cmd = wst_string_concatenate("pactl get-sink-volume ", sink_id);
    wst_strarr* result_arr = wst_system(cmd);
    char* result = wst_strarr_at(result_arr, 0);
    float cur_volume = 0.0f;
    char* cur_volume_str = strrchr(result, '/');
    sscanf(cur_volume_str + 2, "%f dB", &cur_volume);
    wst_strarr_delete(result_arr);
    free(cmd);
    return cur_volume;
}

bool is_analog_headphones(char* sink_id)
{
    bool result = false;
    if (wst_string_endsWith(sink_id, "analog-stereo"))
    {
        wst_strarr* result_arr = wst_system("pactl list sinks | grep Active");
        result = wst_string_endsWith(wst_strarr_at(result_arr, 0), "headphones");
        wst_strarr_delete(result_arr);
    }
    return result;
}

char* get_active_sink()
{
    wst_strarr* result_arr = wst_system("pactl get-default-sink");
    char* result = wst_string_init(wst_strarr_at(result_arr, 0));
    wst_strarr_delete(result_arr);
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

    char* sink_id = get_active_sink();
    bool headphones = is_analog_headphones(sink_id);
    f32 cur_volume = get_volume(sink_id);
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

    char pactl_cmd[2048];
    sprintf(pactl_cmd, "pactl set-sink-volume %s %s", sink_id, value_str);
    wst_system(pactl_cmd);

    return 0;
}
