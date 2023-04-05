#include "wst.h"

int main(int argc, char** argv) {
    char* cmd = "firefox --new-window";
    char* url = "en.wikipedia.org";

    if (argc > 1) {
        url = "https://en.wikipedia.org/w/index.php?search";
        char* query = argv[1];
        for (int i = 2; i < argc; ++i) {
            query = wst_string_join(query, argv[i], "+");
        }
        url = wst_string_join(url, query, "=");
    }

    cmd = wst_string_join(cmd, url, " ");
    system(cmd);
}
