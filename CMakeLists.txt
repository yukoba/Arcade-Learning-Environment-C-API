cmake_minimum_required (VERSION 3.14)

# Build the C++ shared library
option(BUILD_CPP_LIB "Build C++ Interface" ON)

# Enable SDL for screen and audio support
option(SDL_SUPPORT "Enable SDL support" OFF)
# Append VCPKG manifest feature
if(SDL_SUPPORT)
  list(APPEND VCPKG_MANIFEST_FEATURES "sdl")
endif()

# Set cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# Overlay VCPKG custom triplets
if(NOT DEFINED VCPKG_OVERLAY_TRIPLETS)
  set(VCPKG_OVERLAY_TRIPLETS
      "${CMAKE_MODULE_PATH}/custom-triplets"
      CACHE STRING "")
endif()

# Discover VCPKG default triplet
if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
  set(VCPKG_TARGET_TRIPLET
      "$ENV{VCPKG_DEFAULT_TRIPLET}"
      CACHE STRING "")
endif()

# Discover VCPKG toolchain
if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  # VCPKG_ROOT is what Microsoft recommends,
  # VCPKG_INSTALLATION_ROOT is what's defined on Azure
  if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE
        "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
  elseif(DEFINED ENV{VCPKG_INSTALLATION_ROOT})
    set(CMAKE_TOOLCHAIN_FILE
        "$ENV{VCPKG_INSTALLATION_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
  endif()
endif()

# Don't allow running cmake in root directory
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
  message(FATAL_ERROR [=[
    Source and build directories cannot be the same.
    You should probably also remove CMakeFiles/ and CMakeCache.txt.
  ]=])
endif()

project(ale VERSION "0.10.2"
            DESCRIPTION "The Arcade Learning Environment (ALE) - a platform for AI research."
            HOMEPAGE_URL "http://www.arcadelearningenvironment.org"
            LANGUAGES CXX)

# Main ALE src directory
add_subdirectory(src/ale)

# Only include tests in the main project
# if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
#  enable_testing()
#  add_subdirectory(tests)
# endif()
