cmake_minimum_required(VERSION 3.20)

add_library(mosaik-midiui STATIC Ui.cpp)
target_include_directories(mosaik-midiui PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(mosaik-midiui mosaik-core mosaik-midi)


