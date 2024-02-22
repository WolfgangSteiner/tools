#include "grv/grvbld.h"

int main(int argc, char** argv) {
    GRV_CHECK_AND_REBUILD();    
    grvbld_config_t* config =  grvbld_config_new(argc, argv);
    grvbld_config_add_include_directory(config, "grv/include");
    grvbld_config_add_include_directory(config, ".");
    
    grvbld_target_t* battery_watch = grvbld_target_create("battery_watch", GRVBLD_EXECUTABLE);
    grvbld_target_add_src(battery_watch, "battery_watch.c");
    grvbld_target_add_src(battery_watch, "grv/src/grv.c");
    grvbld_build_target(config, battery_watch);

    grvbld_target_t* status = grvbld_target_create("status", GRVBLD_EXECUTABLE);
    grvbld_target_add_src(status, "status/status.c");
    grvbld_target_add_src(status, "wst.c");
    grvbld_target_add_src(status, "grv/src/grv.c");
    grvbld_build_target(config, status);

    grvbld_target_t* dim = grvbld_target_create("dim", GRVBLD_EXECUTABLE);
    grvbld_target_add_src(dim, "dim.c");
    grvbld_target_add_src(dim, "grv/src/grv.c");
    grvbld_build_target(config, dim);

    return 0;
}
