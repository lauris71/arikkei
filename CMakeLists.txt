cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED YES)

project(arikkei)

#if you don't want the full compiler output, remove the following line
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)

add_subdirectory(arikkei)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
