cmake_minimum_required(VERSION 3.20)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBS REQUIRED IMPORTED_TARGET alsa)

add_library(mosaik-audiomidi STATIC AudioMidi.cpp AlsaOut.cpp)
target_include_directories(mosaik-audiomidi PRIVATE ../dsp ${LIBS_INCLUDE_DIRS})
target_link_libraries(mosaik-audiomidi mosaik-dsp ${LIBS_LIBRARIES})

