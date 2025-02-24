# msvcToolchain.cmake
# Load Visual Studio environment in CMake
#
# Script was intitially published by Nicolas: https://www.linkedin.com/in/nicolasgoffinet (brilliant implementation!) on CMake forum:
#  https://discourse.cmake.org/t/is-there-a-way-to-integrate-the-call-to-vcvarsall-bat-into-the-cmakepresets-json-file/3100/15?u=int_main
#  and additionally formatted by Me (this file)
#
# Depends on         : https://github.com/scikit-build/cmake-FindVcvars
# Usage reference    : https://github.com/Challanger524/test-boost-beast
# Usage example below:

# git submodule add https://github.com/scikit-build/cmake-FindVcvars
#
#cmake_minimum_required(...
#
#if (NOT CMAKE_GENERATOR MATCHES "Visual Studio" AND CMAKE_CXX_COMPILER MATCHES "clang-cl")
#  set(Vcvars_MSVC_ARCH "64" CACHE STRING "" FORCE)
#  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-FindVcvars")
#  include(${CMAKE_CURRENT_SOURCE_DIR}/msvcToolchain.cmake)
#endif() # Visual Studio environment set
#
#project(...

cmake_minimum_required(VERSION 3.5)

# List variables loaded from `CMakePresets.json`
# `project()` is not called yet, so CMake is not configured at this stage and has no nothing else
message("[msvcToolchain.cmake] -- CMAKE_GENERATOR   : ${CMAKE_GENERATOR}"   )
message("[msvcToolchain.cmake] -- CMAKE_C_COMPILER  : ${CMAKE_C_COMPILER}"  )
message("[msvcToolchain.cmake] -- CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message("[msvcToolchain.cmake] -- Vcvars_MSVC_ARCH  : ${Vcvars_MSVC_ARCH}"  )

#include(${CMAKE_CURRENT_SOURCE_DIR}/cmake-FindVcvars/FindVcvars.cmake) # CMake Error not stops configure
find_package(Vcvars REQUIRED MODULE)

get_filename_component(Vcvars_BATCH_NAME "${Vcvars_BATCH_FILE}" NAME)

message("[msvcToolchain.cmake] -- Vcvars_BATCH_NAME: ${Vcvars_BATCH_NAME}")
message("[msvcToolchain.cmake] -- Vcvars_BATCH_FILE: ${Vcvars_BATCH_FILE}")
message("[msvcToolchain.cmake] -- Vcvars_LAUNCHER  : ${Vcvars_LAUNCHER}"  )
message("[msvcToolchain.cmake]")
message("[msvcToolchain.cmake] -- ${Vcvars_BATCH_NAME} - executing...")

# DEBUG : dump env after vcvars*.bat call
#execute_process(COMMAND ${Vcvars_LAUNCHER} set OUTPUT_FILE "${CMAKE_CURRENT_LIST_DIR}/env-vcvars.txt" OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${Vcvars_LAUNCHER} set OUTPUT_VARIABLE env_dump OUTPUT_STRIP_TRAILING_WHITESPACE)

message("[msvcToolchain.cmake] -- ${Vcvars_BATCH_NAME} - executed")
#message("[msvcToolchain.cmake] -- Visual Studio environment - loading..."  )

# Process and load environment variables loaded by `vcvarsXX.bat`
# 1. escaping troublesome chars
string(REPLACE ";"  "__semicolon__"   env_dump "${env_dump}")
string(REPLACE "\\" "__backslash__"   env_dump "${env_dump}")
string(REPLACE "\"" "__doublequote__" env_dump "${env_dump}")

string(REGEX  REPLACE "[\r\n]+" ";"   env_dump ${env_dump}) # 2. multi-line => one line
list  (FILTER env_dump INCLUDE REGEX ".+\=.+")              # 3. keep only lines looking like xx=yy

# 4. setting captured env var right here
foreach(key_value ${env_dump})
  string(REPLACE "=" ";" key_value_as_list ${key_value})

  list(GET key_value_as_list 0 key  )
  list(GET key_value_as_list 1 value)

  string(REPLACE "__semicolon__"   "\;" key "${key}")
  string(REPLACE "__backslash__"   "\\" key "${key}")
  string(REPLACE "__doublequote__" "\"" key "${key}")

  string(REPLACE "__semicolon__"   ";"  value "${value}")
  string(REPLACE "__backslash__"   "\\" value "${value}")
  string(REPLACE "__doublequote__" "\"" value "${value}")

  set(ENV{${key}} "${value}")

  unset(key  )
  unset(value)
endforeach() # key_value ${env_dump}

# 5. adjust cmake vars to find msvc headers & resolve libs path
include_directories(AFTER SYSTEM $ENV{INCLUDE})
link_directories   (AFTER        $ENV{LIB}    )
unset(env_dump)

message("[msvcToolchain.cmake] -- Visual Studio environment - loaded")


# # Stash # #

# Attempt: Avoid `vcvars64.bat` call on non-first CMake configure
# +: This setup allows to save-load ENV: include, lib.
# -: But not a Visual Studio environment - variables not loaded
# Maybe: Should: save `${env_dump}` and Always Run: `set(ENV{${key}} "${value}")`

# *placed on top*    of this script
#if(__msvcToolchain_run_once__)
#  include_directories(AFTER SYSTEM "${__msvcToolchain_env_inc__}")
#  link_directories   (AFTER        "${__msvcToolchain_env_lib__}")
#  return()
#endif()

# *placed on bottom* of this script
#set(__msvcToolchain_run_once__  TRUE           CACHE BOOL   "" FORCE)
#set(__msvcToolchain_env_inc__  "$ENV{INCLUDE}" CACHE STRING "" FORCE)
#set(__msvcToolchain_env_lib__  "$ENV{LIB}"     CACHE STRING "" FORCE)
