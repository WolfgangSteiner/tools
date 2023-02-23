cmake_minimum_required(VERSION 3.0)
project(mytools)
include_directories(grv/include)

add_executable(pavolume pavolume.c grv/src/grv.c)