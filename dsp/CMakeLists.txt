cmake_minimum_required(VERSION 3.16.3)

project(mosaik-dsp)

set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBS REQUIRED IMPORTED_TARGET gstreamermm-1.0)

add_library(${PROJECT_NAME} STATIC
        Dsp.cpp
        PointerExchange.h
        tools/Tools.cpp
        tools/Tools.h
        Types.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE mosaik-tools ${LIBS_INCLUDE_DIRS})


