# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020-2022 Antonio Niño Díaz

cmake_minimum_required(VERSION 3.15)
project(ugba)
enable_language(CXX C ASM)

set(C_STANDARD 11)
set(C_EXTENSIONS OFF)
set(C_STANDARD_REQUIRED ON)

# Detect if this project is a submodule
# -------------------------------------

get_directory_property(PROJECT_HAS_PARENT PARENT_DIRECTORY)
set(PROJECT_IS_SUBMODULE ${PROJECT_HAS_PARENT})

# Build options
# -------------

option(BUILD_GBA "Build GBA binaries in addition to PC binaries" OFF)

option(USE_DEVKITARM "Use devkitARM to build GBA binaries" ON)

# Save it to a regular variable so that it can be disabled per-folder.
set(BUILD_GBA_INTERNAL "${BUILD_GBA}")

# Add libugba submodule
# ---------------------

set(LUA_INTERPRETER_ENABLED ON CACHE BOOL "")
add_subdirectory(libugba)

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

include(cmake/example.cmake)

# Add tests
# ---------

add_subdirectory(examples)
add_subdirectory(unittests)
