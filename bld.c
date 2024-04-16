#include "lib/grv/grvbld.h"

static grvbld_config_t* config = 0;
static grvbld_target_t* libgrv = 0;

grvbld_config_t* create_config(int argc, char** argv) {
    grvbld_config_t* config =  grvbld_config_new(argc, argv);
    grvbld_config_add_include_directory(config, "lib/grv/include");
    grvbld_config_add_include_directory(config, ".");
    return config;
}

void build_exe(char* name) {
    grvbld_target_t* target = grvbld_target_create_executable(name);
    char* src = grvbld_cstr_new_with_format("src/%s.c", name); 
    grvbld_target_add_src(target, src);
    grvbld_target_link(target, libgrv);
    grvbld_build_target(config, target);
}

void build_grv(void) {
    libgrv = grvbld_target_create_static_library("grv");
    grvbld_target_add_src(libgrv, "lib/grv/src/grv.c");
    grvbld_build_target(config, libgrv);
}

int main(int argc, char** argv) {
    GRV_CHECK_AND_REBUILD();    
    config = create_config(argc, argv);
    build_grv();

    build_exe("battery_watch");
    build_exe("dim");
    build_exe("fzvim");
    build_exe("agfzf");
    build_exe("psfzf");
    build_exe("youtube-to-mp3");
    build_exe("pwgen");
    build_exe("status");
    build_exe("sleep_watch");

    return 0;
}
