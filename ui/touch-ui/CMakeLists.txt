cmake_minimum_required(VERSION 3.20)

add_library(mosaik-touchui STATIC Ui.cpp)
target_include_directories(mosaik-touchui PRIVATE ../../core)
target_link_libraries(mosaik-touchui mosaik-core)


