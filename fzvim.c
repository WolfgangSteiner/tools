#include "grv/grv.h"
#include "grv/grv_str.h"
#include "grv/grv_util.h"

int main(int argc, char** argv) {
    grv_str_t vim_cmd = grv_str_ref("nvim");
    grv_str_t ag_cmd = grv_str_ref("ag -l --silent");
    grv_str_t fzf_cmd = grv_str_ref("fzf");
    grv_strarr_t args = grv_strarr_new_from_cstrarr(argv+1, argc-1);

    for (int i = 0; i < args.size; i++) {
        grv_str_append_space(&ag_cmd);
        grv_str_append_str(&ag_cmd, args.arr[i]);
    }

    grv_str_t cmd = grv_str_cat_str_cstr(ag_cmd, " | ");
    grv_str_append_str(&cmd, fzf_cmd);
    grv_strarr_t result = grv_system_with_capture(cmd);

    if (result.size) {
        grv_strarr_push_front(&result, vim_cmd);
        grv_system(grv_strarr_join(result, grv_str_ref(" ")));
    }

    return 0;
}

