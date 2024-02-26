#include "grv/grv.h"

int main(int argc, char** argv) {
    grv_str_t ps_cmd = grv_str_ref("ps aux");
    grv_str_t fzf_cmd = grv_str_ref("fzf");
    grv_strarr_t args = grv_strarr_new_from_cstrarr(argv+1, argc-1);
    grv_str_t cmd = grv_str_cat_str_cstr(ps_cmd, " | ");
    grv_str_append_str(&cmd, fzf_cmd);
    grv_strarr_t result = grv_system_with_capture(cmd);
    
    for (int i = 0; i < result.size; i++) {
        grv_str_print(result.arr[i]);
    }

    return 0;
}

