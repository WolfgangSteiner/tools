#include "wst.h"


int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage: build <filename.c> ...\n");
        printf("Builds a number of c files using gcc with standard settings.\n");
        printf("The executable will be named after the first c file.\n");
    }

    char* executableName = wst_string_splitFromFront(argv[1], ".");

    wst_string_array cmd = wst_string_array_new();
    cmd = wst_string_array_append(cmd, "gcc");

    if (wst_string_endsWith(argv[0], "-debug"))
    {
        printf("Building for debug.\n");
        cmd = wst_string_array_append(cmd, "-g");
    }
    else
    {
        cmd = wst_string_array_append(cmd, "-O2");
    }

    cmd = wst_string_array_append(cmd, "-o");
    cmd = wst_string_array_append(cmd, executableName);
    cmd = wst_string_array_appendStrings(cmd, argv + 1, argc - 1);

    system(wst_string_array_join(cmd, " "));

    return 0;
}
