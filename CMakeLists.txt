# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020-2022 Antonio Niño Díaz

cmake_minimum_required(VERSION 3.15)
project(ugba)
enable_language(CXX C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Detect if this project is a submodule
# -------------------------------------

get_directory_property(PROJECT_HAS_PARENT PARENT_DIRECTORY)
set(PROJECT_IS_SUBMODULE ${PROJECT_HAS_PARENT})

# Build options
# -------------

option(USE_DEVKITARM "Use devkitARM to build GBA binaries" ON)

# Add libugba submodule
# ---------------------

set(LUA_INTERPRETER_ENABLED ON CACHE BOOL "")
add_subdirectory(libugba)

# Look for an ARM toolchain
# -------------------------

if(NOT "${BUILD_GBA}" STREQUAL OFF)
    if("${ARM_GCC_PATH}" STREQUAL "")
        message(STATUS "ugba-testing: GBA toolchain not specified")
        ugba_search_toolchain_gba(BUILD_GBA)
    else()
        message(STATUS "ugba-testing: GBA toolchain has been specified")
        set(BUILD_GBA ON)
    endif()
endif()

if(BUILD_GBA)
    message(STATUS "ugba-testing: GBA toolchain: ${ARM_GCC_PATH}")
endif()

# Save it to a regular variable so that it can be disabled per-folder.
set(BUILD_GBA_INTERNAL "${BUILD_GBA}")

# Add testing
# -----------

enable_testing()

add_subdirectory(tools)

# GiiBiiAdvance is needed for unit tests

get_filename_component(GIIBIIADVANCE_PATH "giibiiadvance" ABSOLUTE)

if(EXISTS "${GIIBIIADVANCE_PATH}/.git")
    set(ENABLE_CAMERA OFF CACHE BOOL "")
    add_subdirectory(${GIIBIIADVANCE_PATH} giibiiadvance)
else()
    message(FATAL_ERROR "Git submodules not initialized."
            "Please run `git submodule update --init --recursive`")
endif()

# UMOD Player is needed by the tests that use it

get_filename_component(UMOD_PLAYER_PATH "umod-player" ABSOLUTE)

if(EXISTS "${UMOD_PLAYER_PATH}/.git")
    add_subdirectory(${UMOD_PLAYER_PATH})
else()
    message(FATAL_ERROR "Git submodules not initialized."
            "Please run `git submodule update --init --recursive`")
endif()

include(cmake/compiler_flags.cmake)
include(cmake/example.cmake)

# Add tests
# ---------

add_subdirectory(examples)
add_subdirectory(unittests)
