cmake_minimum_required(VERSION 3.20)

add_library(mosaik-tools INTERFACE SignalSlot.h)

if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

