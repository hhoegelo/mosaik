cmake_minimum_required(VERSION 3.20)

project(mosaik-core)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBS REQUIRED IMPORTED_TARGET nlohmann_json)

add_library(${PROJECT_NAME} STATIC Core.cpp DataModel.cpp api/Interface.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR} ${LIBS_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} mosaik-dsp mosaik-tools ${LIBS_LIBRARIES})

