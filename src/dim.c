#include "grv/grv.h"

#define MAX_LEVEL 13
f32 brightness_levels[MAX_LEVEL+1] = { 0.25,  0.5,    1,    1,    1,    1,   10,   25,   50,   60,   70,   80,   90,  100 };
i32 temp_levels[MAX_LEVEL+1] =       { 1500, 1500, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5000, 5500, 5500, 5500 };

char* dim_file = "~/.dim";

void write_level(i32 level) {
    grv_str_t dim_file_expanded = grv_path_expand_tilde(grv_str_ref(dim_file));
    FILE* fp = fopen(grv_str_cstr(dim_file_expanded), "w");
    if (!fp) {
        grv_log_error(grv_str_ref("Could not open ~/.dim for writing"));
        exit(1);
    }
    fprintf(fp, "%d", level);
    fclose(fp);
}

i32 read_level(void) {
    int level = 10;
    grv_str_t dim_file_expanded = grv_path_expand_tilde(grv_str_ref(dim_file));
    FILE* fp = fopen(grv_str_cstr(dim_file_expanded), "r");
    fp && fscanf(fp, "%d", &level);
    return level;
}

i32 get_level_for_current_time(void) {
    f32 time = grv_local_time_f32();
    return grv_is_in_range_f32(time, 7, 18) ? MAX_LEVEL : grv_is_in_range_f32(time, 19, 21) ? 3 : 1;
}

void print_usage(void) {
    printf("Usage: dim LEVEL\n");
    printf("LEVEL must be an integer between 0 and %d\n", MAX_LEVEL);
    exit(1);
}

f32 calc_redshift_dim(f32 brightness) {
    return brightness >= 1.0 ? 1.0 : brightness;
}

void set_dim_level(i32 level) {
    assert(level > 0 && level <= MAX_LEVEL);
    f32 brightness = brightness_levels[level];
    i32 temp = temp_levels[level];
    f32 redshift_dim = calc_redshift_dim(brightness);
    char* light_cmd = grv_cstr_new_with_format("light -S %.2f", grv_max_f32(1.0f, brightness));
    char* redshift_cmd = grv_cstr_new_with_format("redshift -P -O %d -b %.2f", temp, redshift_dim);
    system(light_cmd);
    system(redshift_cmd);
}

int main(int argc, char** argv) {
    i32 level = read_level();
    grv_strarr_t args = grv_strarr_new_from_cstrarr(argv + 1, argc - 1);
    printf("args.size: %d\n", args.size);
    if (args.size == 0) {
        grv_log_info(grv_str_ref("Resetting dim based on time"));
        level = get_level_for_current_time();
    } else if (args.size > 1) {
        print_usage();  
    } else {
        grv_str_t arg = grv_strarr_get(args, 0);
        if (grv_str_eq(arg, "inc")) {
            level = grv_clamp_i32(level + 1, 0, MAX_LEVEL);
        } else if (grv_str_eq(arg, "dec")) {
            level = grv_clamp_i32(level - 1, 0, MAX_LEVEL);
        } else if (grv_str_is_int(arg)) {
            level = grv_clamp_i32(grv_str_to_int(arg), 0, MAX_LEVEL);
        } else {
            print_usage();
        }
    }

    printf("level: %d\n", level);
    set_dim_level(level);
    write_level(level);
}
