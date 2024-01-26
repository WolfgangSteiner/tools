#include "grvbld.h"

#ifdef GRV_BUILD_CONFIGURED

int main(int argc, char** argv) {
    GRV_CHECK_AND_REBUILD();    
    grvbld_config_t* config =  grvbld_config_new(argc, argv);
    grvbld_config_add_include_directory(config, "../grv/include");
    
    grvbld_target_t* battery_watch = grvbld_target_create("battery_watch", GRVBLD_EXECUTABLE);
    grvbld_target_add_src(battery_watch, "battery_watch.c");
    grvbld_target_add_src(battery_watch, "../grv/src/grv.c");
    grvbld_build_target(config, battery_watch);

    return 0;
}

#endif