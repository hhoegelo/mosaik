cmake_minimum_required(VERSION 3.20)

project(mosaik)

set (CMAKE_CXX_STANDARD 20)

add_subdirectory(core)
add_subdirectory(dsp)
add_subdirectory(audio-midi)
add_subdirectory(ui)


add_executable(mosaik main.cpp)
target_link_libraries(mosaik mosaik-core mosaik-dsp mosaik-audiomidi mosaik-midiui mosaik-touchui)