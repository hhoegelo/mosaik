cmake_minimum_required(VERSION 3.16.3)

project(mosaik-tools-tests)

add_executable(${PROJECT_NAME}
        SignalSlotTests.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain mosaik-tools)
target_include_directories(${PROJECT_NAME} PRIVATE ../)

catch_discover_tests(${PROJECT_NAME})