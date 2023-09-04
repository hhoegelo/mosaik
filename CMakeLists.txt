cmake_minimum_required(VERSION 3.20)

project(mosaik)

option(ENABLE_TESTING OFF)

if(ENABLE_TESTING)
  find_package(Catch2 REQUIRED)
  list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
  include(CTest)
  include(Catch)
endif()

set (CMAKE_CXX_STANDARD 20)

add_subdirectory(tools)
add_subdirectory(core)
add_subdirectory(dsp)
add_subdirectory(audio-midi)
add_subdirectory(ui)

add_executable(mosaik main.cpp)
target_link_libraries(mosaik mosaik-core mosaik-dsp mosaik-audiomidi mosaik-midiui mosaik-touchui)
