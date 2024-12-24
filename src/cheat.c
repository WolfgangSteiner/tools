#include "grv/grv.h"

char* mupdf = 
"L, R   Rotate page left (clockwise) or right (counter-clockwise).\n"
"h, j, k, l\n"
"        Scroll page left, down, up, or right.\n"
"+, -   Zoom in or out.\n"
"w      Shrinkwrap window to fit the page.\n"
"r      Reload file.\n"
". pgdn right space\n"
"        Go to the next page\n"
", pgup left b\n"
"        Go to the previous page\n"
"<, >   Skip back/forth 10 pages at a time.\n"
"m      Mark page for snap back.\n"
"t      Pop back to the latest mark.\n"
"[0-9]m Save the current page number in the numbered register.\n"
"[0-9]t Go to the page saved in the numbered register.\n"
"123g   Go to page 123.\n"
"/      Search for text.\n"
"n, N   Find the next/previous search result.\n"
"c      Toggle between color and grayscale rendering.\n";

int main(int argc, char** argv) {
    if (argc == 1 || argc > 2) {
        grv_log_info(grv_str_ref("Usage: cheat <name-of-cheat>"));
        return 1;
    }

    char* name = argv[1];
    char* url = NULL;
    char* str = NULL;

    if (grv_cstr_eq(name, "tmux")) {
        url = "https://tmuxcheatsheet.com/";
    } else if (grv_cstr_eq(name, "pico8")) {
        url = "https://www.lexaloffle.com/media/13822/41_PICO-8%20Cheat%20Sheet%20(4k%20-%20Dark).png";
    } else if (grv_cstr_eq(name, "mupdf")) {
        str = mupdf;
    } else {
        url = grv_cstr_new_with_format("https://www.google.com/search?q=cheatsheet%%20%s", name);
    }
    
    if (url != NULL) {
        char* cmd = grv_cstr_new_with_format("xdg-open \"%s\"", url);
        system(cmd);
    } else if (str != NULL) {
        printf("%s", str);
    } else {
        printf("Unknown topic %s\n", name);
    }

    return 0;
}
