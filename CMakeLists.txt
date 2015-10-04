cmake_minimum_required(VERSION 3.0)

project(CadMockup)

if(NOT (BUILD_WINDOWS OR CMAKE_BUILD_TYPE))
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release." FORCE)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib CACHE INTERNAL "Single output directory for building all static libraries.")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all dynamic libraries.")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")

add_subdirectory(source)
