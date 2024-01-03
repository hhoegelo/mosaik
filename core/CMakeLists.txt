cmake_minimum_required(VERSION 3.20)

add_library(mosaik-core STATIC Core.cpp api/Interface.cpp)
target_include_directories(mosaik-core PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(mosaik-core mosaik-dsp mosaik-tools)

