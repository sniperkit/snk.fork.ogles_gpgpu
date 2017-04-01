# Copyright (c) 2015-2017, David Hirvonen, Ruslan Baratov
# All rights reserved.

cmake_minimum_required(VERSION 3.0)

include("cmake/HunterGate.cmake")
HunterGate(
  URL "https://github.com/ruslo/hunter/archive/v0.18.29.tar.gz"
  SHA1 "ebea5d2fa4aa70de1234b05c4bf1b69705abb1ba"
  LOCAL
  )

project(ogles_gpgpu VERSION 0.2.0)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# See: cmake/Hunter/config.cmake
hunter_add_package(Sugar)
include("${SUGAR_ROOT}/cmake/Sugar")
include(sugar_include)

string(COMPARE EQUAL "${CMAKE_SYSTEM_NAME}" "Linux" is_linux)

message("Android: ${ANDROID}")
message("iOS    : ${IOS}")
message("Linux  : ${is_linux}")
message("Xcode  : ${XCODE}")

if(XCODE)
  set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configuration types")
endif()

option(OGLES_GPGPU_BUILD_EXAMPLES "Build examples" OFF)
option(OGLES_GPGPU_INSTALL "Perform installation" ON)
option(OGLES_GPGPU_VERBOSE "Perform per filter logging" OFF)

## #################################################################
## Dependencies - OpenGL stuff
## #################################################################

if(MSVC)
  hunter_add_package(glew)
  find_package(glew CONFIG REQUIRED)
endif()

## #################################################################
## Testing: 
## #################################################################

if(NOT (IOS OR ANDROID OR MSVC))
  
  option(OGLES_GPGPU_BUILD_TESTS "Build shader unit tests" OFF)  
  if(OGLES_GPGPU_BUILD_TESTS)

    enable_testing()    

    hunter_add_package(GTest)
    find_package(GTest CONFIG REQUIRED)
    list(APPEND OGLES_GPGPU_TEST_LIBS GTest::gtest)

    # Include glfw for lightweight hidden window opengl context:
    # Alternatives: boost or glm for iOS and Android
    hunter_add_package(glfw)
    find_package(glfw3 REQUIRED)
    list(APPEND OGLES_GPGPU_TEST_LIBS glfw)
    
    hunter_add_package(OpenCV)
    find_package(OpenCV REQUIRED)
    list(APPEND OGLES_GPGPU_TEST_LIBS "${OpenCV_LIBS}")
    
  endif()
endif()

## #################################################################
## Project
## #################################################################

add_subdirectory(ogles_gpgpu)

## #################################################################
## Examples
## #################################################################

if(OGLES_GPGPU_BUILD_EXAMPLES)
  message("BUILD EXAMPLES.......")
  add_subdirectory(examples)
endif()
