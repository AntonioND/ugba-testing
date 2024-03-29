# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020-2022 Antonio Niño Díaz

function(define_unittest)

    # Get name of the folder we are in
    # --------------------------------

    get_filename_component(EXECUTABLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

    # Define toolchain
    # ----------------

    ugba_toolchain_sdl2()

    # Define executable name
    # ----------------------

    add_executable(${EXECUTABLE_NAME})

    # Set compiler and linker options
    # -------------------------------

    compiler_flags_sdl2(${EXECUTABLE_NAME})
    linker_flags_sdl2(${EXECUTABLE_NAME})

    # Link with libugba and other system libraries
    # --------------------------------------------

    target_link_libraries(${EXECUTABLE_NAME} libugba)

    if(NOT WIN32)
        target_link_libraries(${EXECUTABLE_NAME} -lm)
    endif()

    # Add source code files
    # ---------------------

    # Macro that searches all the source files in the specified directory in
    # 'dir' and saves them in 'var'
    macro(search_source_files dir var)
        file(GLOB ${var} CONFIGURE_DEPENDS ${dir}/*.c ${dir}/*.h)
    endmacro()

    search_source_files(source FILES_SOURCE)

    target_sources(${EXECUTABLE_NAME} PRIVATE ${FILES_SOURCE})

    # Define CMake test
    # -----------------

    add_test(NAME ${EXECUTABLE_NAME}_unittest
        COMMAND ${EXECUTABLE_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

endfunction()
