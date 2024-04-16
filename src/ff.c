#include "grv.h"

int main(int argc, char** argv)
{
    grv_str ff_cmd = grv_str_new("firefox --new-window");

    if (argc > 1) {
        grv_strarr args = grv_strarr_from_cstr_array(argv + 1, argc - 1);
        grv_strarr_push_front(&args, &ff_cmd);
        grv_str cmd = grv_strarr_join(&args, " ");
        grv_system(&cmd);
    } else {
        grv_system(&ff_cmd);
    }
    return 0;
}
