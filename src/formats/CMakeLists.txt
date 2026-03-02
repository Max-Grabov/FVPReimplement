cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(Formats INTERFACE
  bin.cpp
  bin.hpp 
)
set_target_properties(Formats PROPERTIES INTERFACE_LINK_LIBRARIES stdc++fs)
