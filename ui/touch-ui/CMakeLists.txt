cmake_minimum_required(VERSION 3.20)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBS REQUIRED IMPORTED_TARGET gtkmm-4.0)

add_library(mosaik-touchui STATIC Ui.cpp
        Window.cpp
        Window.h)

target_include_directories(mosaik-touchui PRIVATE ${CMAKE_SOURCE_DIR} ${LIBS_INCLUDE_DIRS})
target_link_libraries(mosaik-touchui mosaik-core ${LIBS_LIBRARIES})

