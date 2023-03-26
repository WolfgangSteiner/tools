cmake_minimum_required(VERSION 3.0)
project(mytools)

set(GRV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../grv)
set(INSTALL_DIR ~/bin)

include_directories(${GRV_DIR}/include)
add_compile_options(-Wall)

add_executable(pavolume pavolume.c ${GRV_DIR}/src/grv.c)
add_executable(ff ff.c ${GRV_DIR}/src/grv.c)
add_executable(gmail gmail.c)
add_executable(google google.c ${GRV_DIR}/src/grv.c)

add_executable(diary diary.c)
target_link_libraries(diary -lncurses)


add_custom_target(install_files
    COMMAND cp pavolume ${INSTALL_DIR}
    COMMAND cp ff ${INSTALL_DIR}
    COMMAND cp gmail ${INSTALL_DIR}
    COMMAND cp google ${INSTALL_DIR}
    DEPENDS pavolume ff gmail google
    COMMENT "Installing mytools"
)