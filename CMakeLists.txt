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

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBS REQUIRED IMPORTED_TARGET cxxopts)

add_subdirectory(tools)
add_subdirectory(core)
add_subdirectory(dsp)
add_subdirectory(audio-midi)
add_subdirectory(ui)

add_executable(mosaik main.cpp)

target_include_directories(mosaik PRIVATE ${LIBS_INCLUDE_DIRS})
target_link_libraries(mosaik PRIVATE ${LIBS_LIBRARIES} mosaik-core mosaik-dsp mosaik-audiomidi mosaik-midiui mosaik-touchui)
