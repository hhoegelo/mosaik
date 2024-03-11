cmake_minimum_required(VERSION 3.16.3)

project(mosaik-core-tests)

find_package(Catch2 REQUIRED)

add_executable(${PROJECT_NAME}
        CoreTests.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE ../)
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain mosaik-core)
catch_discover_tests(${PROJECT_NAME})