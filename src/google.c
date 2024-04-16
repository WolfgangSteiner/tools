#include "grv.h"

int main(int argc, char** argv) {
    grv_str cmd = grv_str_new("firefox --new-window");
    grv_str url = grv_str_new("google.com");

    if (argc > 1) {
        url = grv_str_new("https://www.google.com/search?q");
        grv_strarr args = grv_strarr_from_cstr_array(argv + 1, argc - 1);
        grv_str query = grv_strarr_join(&args, "+");
        url = grv_str_join(&url, &query, "=");
    }

    cmd = grv_str_join(&cmd, &url, " ");
    grv_system(&cmd);
}
