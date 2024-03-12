cmake_minimum_required(VERSION 3.16.3)

target_sources(mosaik-tests PRIVATE CoreTests.cpp)
target_link_libraries(mosaik-tests PRIVATE mosaik-core)
