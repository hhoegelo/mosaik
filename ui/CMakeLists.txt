add_subdirectory(midi-ui)
add_subdirectory(touch-ui)

cmake_minimum_required(VERSION 3.20)

add_library(mosaik-ui STATIC
        SharedState.cpp
        SharedState.h)

target_link_libraries(mosaik-ui mosaik-touchui mosaik-midiui)

