cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED YES)

project(arikkei)

#if you don't want the full compiler output, remove the following line
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)

add_subdirectory(arikkei)

include(CTest)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

# If this is part of another project, inform the parent build system
if(NOT PROJECT_IS_TOP_LEVEL)
  set(HAS_ARIKKEI true PARENT_SCOPE)
endif()
