cmake_minimum_required(VERSION 3.20)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBS REQUIRED IMPORTED_TARGET gtkmm-3.0)

add_library(mosaik-debug-midiui STATIC
        Window.h Window.cpp
)

target_compile_options(mosaik-debug-midiui PRIVATE -fmodules-ts)
target_include_directories(mosaik-debug-midiui PRIVATE ${CMAKE_SOURCE_DIR} ${LIBS_INCLUDE_DIRS})
target_link_libraries(mosaik-debug-midiui mosaik-core ${LIBS_LIBRARIES})

