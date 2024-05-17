cmake_minimum_required(VERSION 3.16.3)

project(mosaik-tools)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBS REQUIRED IMPORTED_TARGET glibmm-2.4)

add_library(${PROJECT_NAME}
        ReactiveVar.h
        ReactiveVar.cpp
        Expiration.cpp
        Expiration.h
        Math.h
        Format.cpp
        Format.h)

target_include_directories(${PROJECT_NAME} PRIVATE ${LIBS_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${LIBS_LIBRARIES})

if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif ()

