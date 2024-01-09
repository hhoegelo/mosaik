cmake_minimum_required(VERSION 3.20)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBS REQUIRED IMPORTED_TARGET gtkmm-4.0)

add_library(mosaik-midiui STATIC Ui.cpp)
target_include_directories(mosaik-midiui PRIVATE ${CMAKE_SOURCE_DIR} ${LIBS_INCLUDE_DIRS})
target_link_libraries(mosaik-midiui mosaik-core mosaik-midi ${LIBS_LIBRARIES})


