#include "grv/grv.h"

int main(int argc, char** argv) {
    grv_str_t ag_cmd = grv_str_ref("ag");
    grv_str_t fzf_cmd = grv_str_ref("fzf");
    grv_strarr_t args = grv_strarr_new_from_cstrarr(argv+1, argc-1);

    for (int i = 0; i < args.size; i++) {
         grv_str_append_space(&ag_cmd);
         grv_str_append_str(&ag_cmd, args.arr[i]);
    }

    if (grv_str_eq(ag_cmd, grv_str_ref("ag"))) {
        grv_str_append_cstr(&ag_cmd, " -l");
    }

    grv_str_t cmd = grv_str_cat_str_cstr(ag_cmd, " | ");
    grv_str_append_str(&cmd, fzf_cmd);
    grv_strarr_t result = grv_system_with_capture(cmd);
    
    for (int i = 0; i < result.size; i++) {
        grv_str_print(result.arr[i]);
    }

    return 0;
}

