cmake_minimum_required(VERSION 3.20)

project(AstralAirEmulator)

option(BUILD_TESTS "Build Testing" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)

add_executable(AstralAir main.cpp)

target_link_libraries(AstralAir PRIVATE Audio Formats Image)

if(${BUILD_TESTS})
  add_subdirectory(tests)
endif()
