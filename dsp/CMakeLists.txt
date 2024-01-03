cmake_minimum_required(VERSION 3.20)

add_library(mosaik-dsp STATIC
        Dsp.cpp
)

target_link_libraries(mosaik-dsp PRIVATE mosaik-tools)



