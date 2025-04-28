#include "grv/grv.h"

char* mupdf = 
"L, R           Rotate page left (clockwise) or right (counter-clockwise).\n"
"h, j, k, l     Scroll page left, down, up, or right.\n"
"+, -           Zoom in or out.\n"
"w              Shrinkwrap window to fit the page.\n"
"r              Reload file.\n"
". right space  Go to the next page\n"
", left b       Go to the previous page\n"
"<, >           Skip back/forth 10 pages at a time.\n"
"m              Mark page for snap back.\n"
"t              Pop back to the latest mark.\n"
"[0-9]m         Save the current page number in the numbered register.\n"
"[0-9]t         Go to the page saved in the numbered register.\n"
"123g           Go to page 123.\n"
"/              Search for text.\n"
"n, N           Find the next/previous search result.\n"
"c              Toggle between color and grayscale rendering.\n";


int main(int argc, char** argv) {
    if (argc == 1 || argc > 2) {
        grv_log_info(grv_str_ref("Usage: cheat <name-of-cheat>"));
        return 1;
    }

    char* name = argv[1];
    char* urls[10] = {0};
    char* str = NULL;

    if (grv_cstr_eq(name, "tmux")) {
        urls[0] = "https://tmuxcheatsheet.com/";
    } else if (grv_cstr_eq(name, "pico8")) {
        urls[0] = "https://www.lexaloffle.com/media/13822/41_PICO-8%20Cheat%20Sheet%20(4k%20-%20Dark).png";
        urls[1] = "https://www.lexaloffle.com/bbs/files/13845/glyphd.png";
	} else if (grv_cstr_eq(name, "git")) {
	str = "git submodule update --init --recursive\n";
    } else if (grv_cstr_eq(name, "mupdf")) {
        str = mupdf;
	} else if (grv_cstr_eq(name, "nmcli")) {
		str = "nmcli device wifi connect <SSID> password <PASSWORD>\n";
    } else {
        urls[0] = grv_cstr_new_with_format("https://www.google.com/search?q=cheatsheet%%20%s", name);
    }
    
    if (urls[0] != NULL) {
        char** url = urls;
        while (*url != NULL) {
            char* cmd = grv_cstr_new_with_format("xdg-open \"%s\"", *url);
            system(cmd);
            url++;
        }
    } else if (str != NULL) {
        printf("%s", str);
    } else {
        printf("Unknown topic %s\n", name);
    }

    return 0;
}
