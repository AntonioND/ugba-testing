# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020 Antonio Niño Díaz

function(define_example)
    # Name this example the same way as the folder it's in
    get_filename_component(EXECUTABLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

    project(${EXECUTABLE_NAME})

    add_executable(${EXECUTABLE_NAME})

    target_link_libraries(${EXECUTABLE_NAME} libgbaline)

    # Add source code files
    # ---------------------

    # Macro that searches all the source files in the specified directory in
    # 'dir' and saves them in 'var'
    macro(search_source_files dir var)
        file(GLOB ${var} CONFIGURE_DEPENDS ${dir}/*.c ${dir}/*.h)
    endmacro()

    search_source_files(source FILES_SOURCE)

    target_sources(${EXECUTABLE_NAME} PRIVATE ${FILES_SOURCE})

    # Add graphics files
    # ------------------

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/graphics)
        add_grit_files(graphics ${EXECUTABLE_NAME})
    endif()
endfunction()
