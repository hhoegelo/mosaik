cmake_minimum_required(VERSION 3.20)

add_library(mosaik-core STATIC Core.cpp api/Interface.cpp)
target_include_directories(mosaik-core PRIVATE ../dsp ../tools ./)
target_link_libraries(mosaik-core mosaik-dsp)

