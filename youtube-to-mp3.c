#include "grv/grv_cstr.h"
#include "grv/grv_log.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    char* cmd = grv_cstr_new("yt-dlp --progress --extract-audio --audio-format mp3 --embed-thumbnail --add-metadata ");
    if (argc == 1) {
        printf("Please specify the URL to convert.\n");
        exit(1);
    }

    cmd = grv_cstr_append(cmd, argv[1]);
    grv_log_info(grv_str_ref(cmd));
    system(cmd);

    return 0;
}
