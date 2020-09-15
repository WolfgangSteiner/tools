#include "wst.h"

int main(int argc, char** argv)
{
    char* cmd = "firefox --new-window";

    if (argc > 1)
        for (int i = 1; i < argc; ++i)
            system(wst_string_join(cmd, argv[i], " "));
    else
        system(cmd);

    return 0;
}
