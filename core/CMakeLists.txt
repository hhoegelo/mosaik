cmake_minimum_required(VERSION 3.16.3)

project(mosaik-core)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBS REQUIRED IMPORTED_TARGET sigc++-2.0 glibmm-2.4)

add_library(${PROJECT_NAME} STATIC Types.h Core.cpp api/Interface.cpp
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR} ${LIBS_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC mosaik-dsp mosaik-tools ${LIBS_LIBRARIES})

if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif ()

