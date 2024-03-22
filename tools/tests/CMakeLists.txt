cmake_minimum_required(VERSION 3.16.3)

target_sources(mosaik-tests PRIVATE ReactiveVarTests.cpp)
target_link_libraries(mosaik-tests PRIVATE mosaik-tools)
