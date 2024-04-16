#include "grv/grv.h"

int main(int argc, char** argv) {
    if (argc == 1 || argc > 2) {
        grv_log_info(grv_str_ref("Usage: cheat <name-of-cheat>"));
        return 1;
    }

    char* name = argv[1];
    char* url = NULL;

    if (grv_cstr_eq(name, "tmux")) {
        url = "https://tmuxcheatsheet.com/";
    } else if (grv_cstr_eq(name, "pico8")) {
        url = "https://www.lexaloffle.com/media/13822/41_PICO-8%20Cheat%20Sheet%20(4k%20-%20Dark).png";
    } else {
        url = grv_cstr_new_with_format("https://www.google.com/search?q=cheatsheet%%20%s", name);
    }
    
    char* cmd = grv_cstr_new_with_format("xdg-open \"%s\"", url);
    system(cmd);

    return 0;
}
