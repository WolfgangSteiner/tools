#include "grv/grvbld.h"

static grvbld_config_t* config = 0;

grvbld_config_t* create_config(int argc, char** argv) {
    grvbld_config_t* config =  grvbld_config_new(argc, argv);
    grvbld_config_add_include_directory(config, "grv/include");
    grvbld_config_add_include_directory(config, ".");
    return config;
}

grvbld_target_t* make_target(char* name) {
    grvbld_target_t* target = grvbld_target_create(name, GRVBLD_EXECUTABLE);
    grvbld_target_add_src(target, "grv/src/grv.c");
    return target;
}

void build_exe(char* name) {
    grvbld_target_t* target = make_target(name);
    char* src = grvbld_cstr_cat(name, ".c");
    grvbld_target_add_src(target, src);
    grvbld_build_target(config, target);
}

int main(int argc, char** argv) {
    GRV_CHECK_AND_REBUILD();    
    config = create_config(argc, argv);

    grvbld_target_t* status = make_target("status");
    grvbld_target_add_src(status, "status/status.c");
    grvbld_target_add_src(status, "wst.c");
    grvbld_build_target(config, status);

    build_exe("battery_watch");
    build_exe("dim");
    build_exe("fzvim");
    build_exe("agfzf");
    build_exe("psfzf");
    build_exe("youtube-to-mp3");
    build_exe("pwgen");

    return 0;
}
