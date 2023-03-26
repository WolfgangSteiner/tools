#include "grv/include/grv.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    grv_strarr args;
    grv_str executable_name;
    bool debug;
} build_settings;


build_settings parse_args(int argc, char** argv) {
    build_settings settings = {0};
    settings.args = grv_strarr_from_cstr_array(argv, argc);
    
    // get rid of the executable name
    grv_strarr_remove_front(&settings.args);

    if (grv_str_starts_with_cstr(grv_strarr_front(&settings.args), "--debug")) {
        grv_strarr_remove_front(&settings.args);
        settings.debug = true;
    }

    if (grv_strarr_size(&settings.args) == 0) {
        printf("Usage: build <filename.c> ...\n");
        printf("Builds a number of c files using gcc with standard settings.\n");
        printf("The executable will be named after the first c file.\n");
        exit(1);
    }

    settings.executable_name = grv_str_split_head_from_front(grv_strarr_front(&settings.args), ".");
    return settings;
}

int main(int argc, char** argv) {
    build_settings settings = parse_args(argc, argv);
    
    grv_strarr cmd = grv_strarr_new();
    grv_strarr_push_cstr(&cmd, "gcc");

    if (settings.debug) {
        printf("Building for debug.\n");
        grv_strarr_push_cstr(&cmd, "-g");
    } else {
        grv_strarr_push_cstr(&cmd, "-O2");
    }

    grv_strarr_push_cstr(&cmd, "-o");
    grv_strarr_push(&cmd, &settings.executable_name);
    grv_strarr_push_cstr(&cmd, "-lm");
    grv_strarr_push_cstr(&cmd, "-Igrv/include");
    grv_strarr_append(&cmd, &settings.args);
    grv_strarr_push_cstr(&cmd, "grv/src/grv.c");
    grv_str cmd_str = grv_strarr_join(&cmd, " ");
    grv_system(&cmd_str);

    return 0;
}
