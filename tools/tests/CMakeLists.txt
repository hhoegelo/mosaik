cmake_minimum_required (VERSION 3.16.3)


add_executable(mosaik-tools-tests 
  SignalSlotTests.cpp
)

target_link_libraries(mosaik-tools-tests PRIVATE Catch2::Catch2WithMain mosaik-tools)
target_include_directories(mosaik-tools-tests PRIVATE ../)

catch_discover_tests(mosaik-tools-tests)