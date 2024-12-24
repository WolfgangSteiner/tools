#include "grv/grv.h"

int main(int argc, char** argv)
{
    grv_str_t ff_cmd = grv_str_new("firefox --new-window");

    if (argc > 1) {
        grv_strarr_t args = grv_strarr_new_from_cstrarr(argv + 1, argc - 1);
        grv_strarr_push_front(&args, ff_cmd);
        grv_str_t cmd = grv_strarr_join(args, grv_str_ref(" "));
        grv_system(cmd);
    } else {
        grv_system(ff_cmd);
    }
    return 0;
}
