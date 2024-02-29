cmake_minimum_required(VERSION 3.16.3)

add_library(mosaik-tools INTERFACE SignalSlot.h)

if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

