cmake_minimum_required (VERSION 3.26)

# Load Visual Studio environment for Clang-CL
if (NOT CMAKE_GENERATOR MATCHES "Visual Studio" AND CMAKE_CXX_COMPILER MATCHES "clang-cl")
  set(Vcvars_MSVC_ARCH "64" CACHE STRING "" FORCE)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-FindVcvars")
  include(${CMAKE_CURRENT_SOURCE_DIR}/msvcToolchain.cmake)
endif() # Visual Studio environment for Clang-CL

project (test-beast  DESCRIPTION  "<description>"
                HOMEPAGE_URL "https://github.com/Challanger524/template-cpp"
                LANGUAGES     CXX)

# # Setup: main project # #

add_executable            (${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/include"         # project potable  library
                                                   "${PROJECT_SOURCE_DIR}/src/int"         # project internal library
                                                   "${PROJECT_SOURCE_DIR}/src"           ) # project sources (.cpp/.hpp)
target_precompile_headers (${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src/config.hpp") # force include to all source files
set_target_properties     (${PROJECT_NAME} PROPERTIES
  CXX_STANDARD          23
  CXX_STANDARD_REQUIRED ON
  RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}$<0:>" # put executable near the `res` resource folder
  EXPORT_COMPILE_COMMANDS   ON
)

# # Apply: compile definitions # #
target_compile_definitions(${PROJECT_NAME} PRIVATE "${_DEFINITIONS_}")

# # Gather: sources # #
file(GLOB_RECURSE SOURCE_FILES_ CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
list(FILTER SOURCE_FILES_ EXCLUDE REGEX ".*\\/_.*\\..*") # exclude files starting with uncderscore `_<file>`: https://regex101.com/r/DT6qGi/1
target_sources        (${PROJECT_NAME} PRIVATE ${SOURCE_FILES_})

# # Apply: compiler and linker flags (from: file:///./CMakePresets.json) # #
target_link_options   (${PROJECT_NAME} PRIVATE "${_FLAGS_LINKER_}")    # flags: linker
target_link_options   (${PROJECT_NAME} PRIVATE "${_FLAGS_COMP_LINK_}") # flags: compiler & linker (mutual)
target_compile_options(${PROJECT_NAME} PRIVATE "${_FLAGS_COMP_LINK_}") #
target_compile_options(${PROJECT_NAME} PRIVATE "${_FLAGS_COMPILE_0_}"  # flags: compliler
                                               "${_FLAGS_COMPILE_1_}"  #
                                               "${_FLAGS_COMPILE_2_}") #

# Print Clang-CL related variables
if (NOT CMAKE_GENERATOR MATCHES "Visual Studio" AND CMAKE_CXX_COMPILER MATCHES "clang-cl")
  message("== Vcvars_BATCH_FILE: ${Vcvars_BATCH_FILE}")
  message("== Vcvars_LAUNCHER  : ${Vcvars_LAUNCHER}"  )

  message("== CMAKE_CXX_SIMULATE_ID              : ${CMAKE_CXX_SIMULATE_ID}"              ) # MSVC
  message("== CMAKE_CXX_COMPILER_FRONTEND_VARIANT: ${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}") # MSVC
  message("== CMAKE_CXX_SIMULATE_VERSION         : ${CMAKE_CXX_SIMULATE_VERSION}"         ) # 19.39
  message("== MSVC_VERSION                       : ${MSVC_VERSION}"                       ) # 1939
  message("== MSVC                               : ${MSVC}"                               ) # 1
  message("== CMAKE_CXX_COMPILER                 : ${CMAKE_CXX_COMPILER}"                 ) # .../clang-cl.exe
  message("== CMAKE_CXX_COMPILER_ID              : ${CMAKE_CXX_COMPILER_ID}"              ) # Clang
  message("== CMAKE_GENERATOR                    : ${CMAKE_GENERATOR}"                    ) # Ninja
endif() # clang-cl #]]

# # Setup: libraries # #
# Boost #
set(BOOST_SOURCE_DIR_ "${CMAKE_SOURCE_DIR}/../boost")
set(BOOST_INCLUDE_LIBRARIES beast)
add_subdirectory("${BOOST_SOURCE_DIR_}" "${CMAKE_CURRENT_BINARY_DIR}/dep/boost" EXCLUDE_FROM_ALL)

# [Workaround] https://github.com/boostorg/context/pull/288
# Suppress build: `masm`: logo, and `clang` warning argument unused during compilation
#if    (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
#  target_compile_options(boost_context PRIVATE "/nologo")
#elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
#  target_compile_options(boost_context PRIVATE "-Wno-unused-command-line-argument")
#endif()

# [Workaround] https://github.com/boostorg/beast/issues/672#issuecomment-318225620
# Link missing: `ws2_32` library for windows sockets
if (MINGW)
  target_link_libraries(boost_asio INTERFACE ws2_32) # mswsock wsock32
endif()

# # Apply: project dependencies # #

target_link_libraries     (${PROJECT_NAME} PRIVATE Boost::beast)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
  "${CMAKE_SOURCE_DIR}/dep"
) # rooted dirs will be treated as SYSTEM, even if opposite stated explicitly (with: -I)


###Stash###

#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "" FORCE) # debug compiling

# debug file(GLOB_RECURSE)
#string (REPLACE ";" "\n" SOURCE_FILES_ "${SOURCE_FILES_}")
#message(FATAL_ERROR ${SOURCE_FILES_})

#cmake_path(GET CMAKE_CACHEFILE_DIR FILENAME PRESET_)
#message(${CMAKE_CACHEFILE_DIR})
#message(${PRESET_})
