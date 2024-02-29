cmake_minimum_required(VERSION 3.16.3)

project(mosaik-midi)

set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBS REQUIRED IMPORTED_TARGET alsa glibmm-2.4 giomm-2.4)

add_library(${PROJECT_NAME} STATIC AlsaIn.cpp Monitor.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBS_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} mosaik-dsp ${LIBS_LIBRARIES})
