cmake_minimum_required(VERSION 3.20)

add_library(mosaik-dsp STATIC 
  Dsp.cpp
  api/control/Api.cpp
)
target_include_directories(mosaik-dsp PRIVATE ../tools ./)



