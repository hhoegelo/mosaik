cmake_minimum_required(VERSION 3.20)

add_library(mosaik-audiomidi STATIC AudioMidi.cpp)
target_include_directories(mosaik-audiomidi PRIVATE ../dsp)
target_link_libraries(mosaik-audiomidi mosaik-dsp)

