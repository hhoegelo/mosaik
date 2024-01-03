cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBS REQUIRED IMPORTED_TARGET alsa)

add_library(mosaik-midi STATIC AlsaIn.cpp Monitor.cpp)
target_include_directories(mosaik-midi PRIVATE ${LIBS_INCLUDE_DIRS})
target_link_libraries(mosaik-midi mosaik-dsp ${LIBS_LIBRARIES})
