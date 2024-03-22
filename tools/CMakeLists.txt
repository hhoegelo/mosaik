cmake_minimum_required(VERSION 3.16.3)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBS REQUIRED IMPORTED_TARGET glibmm-2.4)

add_library(mosaik-tools
        ReactiveVar.h
        ReactiveVar.cpp)

target_include_directories(mosaik-tools PRIVATE ${LIBS_INCLUDE_DIRS})
target_link_libraries(mosaik-tools ${LIBS_LIBRARIES})

if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif ()

