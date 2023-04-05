#include "wst.h"

int main(int argc, char** argv)
{

    char* cmd = "yt-dlp --progress --extract-audio --audio-format mp3 --embed-thumbnail  --add-metadata";
    if (argc == 1) {
        printf("Please specify the URL to convert.\n");
        exit(1);
    }

    system(wst_string_join(cmd, argv[1], " "));

    return 0;
}
