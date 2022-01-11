# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020-2021 Antonio Niño Díaz

function(enable_debug_example)

    get_filename_component(EXECUTABLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

    target_compile_definitions(${EXECUTABLE_NAME} PUBLIC -DUGBA_DEBUG)
    target_compile_definitions(${EXECUTABLE_NAME}_gba PUBLIC -DUGBA_DEBUG)

endfunction()

function(define_example)

    # Get name of the folder we are in
    # --------------------------------

    get_filename_component(EXECUTABLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

    # Add source code files
    # ---------------------

    # Macro that searches all the source files in the specified directory in
    # 'dir' and saves them in 'var'
    macro(search_source_files dir var)
        file(GLOB_RECURSE ${var} CONFIGURE_DEPENDS ${dir}/*.c ${dir}/*.h)
    endmacro()

    search_source_files(source FILES_SOURCE)
    search_source_files(built_assets FILES_BUILT_ASSETS)
    set(ALL_FILES_SOURCE
        ${FILES_SOURCE}
        ${FILES_BUILT_ASSETS}
    )

    get_filename_component(INCLUDE_PATH_SOURCE "source" ABSOLUTE)
    get_filename_component(INCLUDE_PATH_BUILT_ASSETS "built_assets" ABSOLUTE)

    set(INCLUDE_PATHS
        ${INCLUDE_PATH_SOURCE}
        ${INCLUDE_PATH_BUILT_ASSETS}
    )

    add_subdirectory(sdl2)

    # Build GBA version if requested
    # ------------------------------

    if(BUILD_GBA_INTERNAL)
        add_subdirectory(gba)
    endif()

endfunction()

macro(example_build_gba)

    toolchain_gba()

    # Define library target
    # ---------------------

    add_executable(${EXECUTABLE_NAME}_gba)
    target_link_libraries(${EXECUTABLE_NAME}_gba libugba_gba)
    if(NOT USE_DEVKITARM)
        target_link_libraries(${EXECUTABLE_NAME}_gba libsysgba_gba)
    endif()
    target_link_libraries(${EXECUTABLE_NAME}_gba umod_player_gba)

    # Source code, include directories and global definitions
    # -------------------------------------------------------

    target_sources(${EXECUTABLE_NAME}_gba PRIVATE ${ALL_FILES_SOURCE})
    target_include_directories(${EXECUTABLE_NAME}_gba PRIVATE ${INCLUDE_PATHS})

    # Build options
    # -------------

    gba_set_compiler_options(${EXECUTABLE_NAME}_gba)
    if(NOT USE_DEVKITARM)
        libsysgba_set_compiler_options(${EXECUTABLE_NAME}_gba)
    endif()

    set(ARGS_C -Wall -Wextra -Wno-unused-parameter)

    target_compile_options(${EXECUTABLE_NAME}_gba PRIVATE
        $<$<COMPILE_LANGUAGE:C>:${ARGS_C}>
    )

    target_link_options(${EXECUTABLE_NAME}_gba PRIVATE
        -flto
        -Wno-stringop-overflow -Wno-stringop-overread
    )

    # Generate GBA ROM from the ELF file
    # ----------------------------------

    make_gba_rom(${EXECUTABLE_NAME}_gba ${EXECUTABLE_NAME}_gba "UGBAEXAMPLE" "UGBA")

    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_NAME}_gba.gba
        DESTINATION
            .
    )

endmacro()

macro(example_build_sdl2)

    toolchain_sdl2()

    # Define library target
    # ---------------------

    add_executable(${EXECUTABLE_NAME})
    target_link_libraries(${EXECUTABLE_NAME} libugba)
    target_link_libraries(${EXECUTABLE_NAME} umod_player)

    # Source code, include directories and global definitions
    # -------------------------------------------------------

    target_sources(${EXECUTABLE_NAME} PRIVATE ${ALL_FILES_SOURCE})
    target_include_directories(${EXECUTABLE_NAME} PRIVATE ${INCLUDE_PATHS})

    install(
        TARGETS
            ${EXECUTABLE_NAME} libugba
        DESTINATION
            .
    )

endmacro()

# TODO: There must be some way to make a generic function for any number of
# screenshots.

function(unittest_screenshot)

    # Get name of the folder we are in
    # --------------------------------

    get_filename_component(EXECUTABLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    get_filename_component(PARENT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
    get_filename_component(GROUP_NAME "${PARENT_PATH}" NAME)

    # SDL2 test
    # ---------

    set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test-sdl2.lua")
    if(NOT EXISTS ${TEST_SCRIPT})
        set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test.lua")
    endif()

    set(REF_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-sdl2.png")
    if(NOT EXISTS ${REF_PNG})
        set(REF_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference.png")
    endif()

    set(CMD1 "$<TARGET_FILE:${EXECUTABLE_NAME}> --lua ${TEST_SCRIPT}")
    set(CMD2 "$<TARGET_FILE:pngmatch> ${REF_PNG} screenshot.png")

    add_test(NAME ${EXECUTABLE_NAME}_test
        COMMAND ${CMAKE_COMMAND}
                    -DCMD1=${CMD1}
                    -DCMD2=${CMD2}
                    -P ${CMAKE_SOURCE_DIR}/cmake/runcommands.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    set_tests_properties(${EXECUTABLE_NAME}_test
        PROPERTIES LABELS "${GROUP_NAME}"
    )

    # Emulator test
    # -------------

    if(BUILD_GBA_INTERNAL)
        set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test-gba.lua")
        if(NOT EXISTS ${TEST_SCRIPT})
            set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test.lua")
        endif()

        set(REF_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-gba.png")
        if(NOT EXISTS ${REF_PNG})
            set(REF_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference.png")
        endif()

        set(GBA_ROM "${CMAKE_CURRENT_BINARY_DIR}/gba/${EXECUTABLE_NAME}_gba.gba")

        set(CMD1 "$<TARGET_FILE:giibiiadvance> --lua ${TEST_SCRIPT} ${GBA_ROM}")
        set(CMD2 "$<TARGET_FILE:pngmatch> ${REF_PNG} screenshot.png")

        add_test(NAME ${EXECUTABLE_NAME}_gba_test
            COMMAND ${CMAKE_COMMAND}
                        -DCMD1=${CMD1}
                        -DCMD2=${CMD2}
                        -P ${CMAKE_SOURCE_DIR}/cmake/runcommands.cmake
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
        set_tests_properties(${EXECUTABLE_NAME}_gba_test
            PROPERTIES LABELS "${GROUP_NAME};gba"
        )
    endif()

endfunction()

function(unittest_two_screenshots)

    # Get name of the folder we are in
    get_filename_component(EXECUTABLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    get_filename_component(PARENT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
    get_filename_component(GROUP_NAME "${PARENT_PATH}" NAME)

    set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test-sdl2.lua")
    if(NOT EXISTS ${TEST_SCRIPT})
        set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test.lua")
    endif()

    set(REF_1_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-1-sdl2.png")
    if(NOT EXISTS ${REF_1_PNG})
        set(REF_1_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-1.png")
    endif()

    set(REF_2_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-2-sdl2.png")
    if(NOT EXISTS ${REF_2_PNG})
        set(REF_2_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-2.png")
    endif()

    set(CMD1 "$<TARGET_FILE:${EXECUTABLE_NAME}> --lua ${TEST_SCRIPT}")
    set(CMD2 "$<TARGET_FILE:pngmatch> ${REF_1_PNG} screenshot-1.png")
    set(CMD3 "$<TARGET_FILE:pngmatch> ${REF_2_PNG} screenshot-2.png")

    add_test(NAME ${EXECUTABLE_NAME}_test
        COMMAND ${CMAKE_COMMAND}
                    -DCMD1=${CMD1}
                    -DCMD2=${CMD2}
                    -DCMD3=${CMD3}
                    -P ${CMAKE_SOURCE_DIR}/cmake/runcommands.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    set_tests_properties(${EXECUTABLE_NAME}_test
        PROPERTIES LABELS "${GROUP_NAME}"
    )

    # Emulator test
    # -------------

    if(BUILD_GBA_INTERNAL)
        set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test-gba.lua")
        if(NOT EXISTS ${TEST_SCRIPT})
            set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test.lua")
        endif()

        set(REF_1_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-1-gba.png")
        if(NOT EXISTS ${REF_1_PNG})
            set(REF_1_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-1.png")
        endif()

        set(REF_2_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-2-gba.png")
        if(NOT EXISTS ${REF_2_PNG})
            set(REF_2_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-2.png")
        endif()

        set(GBA_ROM "${CMAKE_CURRENT_BINARY_DIR}/gba/${EXECUTABLE_NAME}_gba.gba")

        set(CMD1 "$<TARGET_FILE:giibiiadvance> --lua ${TEST_SCRIPT} ${GBA_ROM}")
        set(CMD2 "$<TARGET_FILE:pngmatch> ${REF_1_PNG} screenshot-1.png")
        set(CMD3 "$<TARGET_FILE:pngmatch> ${REF_2_PNG} screenshot-2.png")

        add_test(NAME ${EXECUTABLE_NAME}_gba_test
            COMMAND ${CMAKE_COMMAND}
                        -DCMD1=${CMD1}
                        -DCMD2=${CMD2}
                        -DCMD3=${CMD3}
                        -P ${CMAKE_SOURCE_DIR}/cmake/runcommands.cmake
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
        set_tests_properties(${EXECUTABLE_NAME}_gba_test
            PROPERTIES LABELS "${GROUP_NAME};gba"
        )
    endif()

endfunction()

function(unittest_three_screenshots)

    # Get name of the folder we are in
    get_filename_component(EXECUTABLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    get_filename_component(PARENT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
    get_filename_component(GROUP_NAME "${PARENT_PATH}" NAME)

    set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test-sdl2.lua")
    if(NOT EXISTS ${TEST_SCRIPT})
        set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test.lua")
    endif()

    set(REF_1_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-1-sdl2.png")
    if(NOT EXISTS ${REF_1_PNG})
        set(REF_1_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-1.png")
    endif()

    set(REF_2_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-2-sdl2.png")
    if(NOT EXISTS ${REF_2_PNG})
        set(REF_2_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-2.png")
    endif()

    set(REF_3_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-3-sdl2.png")
    if(NOT EXISTS ${REF_3_PNG})
        set(REF_3_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-3.png")
    endif()

    set(CMD1 "$<TARGET_FILE:${EXECUTABLE_NAME}> --lua ${TEST_SCRIPT}")
    set(CMD2 "$<TARGET_FILE:pngmatch> ${REF_1_PNG} screenshot-1.png")
    set(CMD3 "$<TARGET_FILE:pngmatch> ${REF_2_PNG} screenshot-2.png")
    set(CMD4 "$<TARGET_FILE:pngmatch> ${REF_3_PNG} screenshot-3.png")

    add_test(NAME ${EXECUTABLE_NAME}_test
        COMMAND ${CMAKE_COMMAND}
                    -DCMD1=${CMD1}
                    -DCMD2=${CMD2}
                    -DCMD3=${CMD3}
                    -DCMD4=${CMD4}
                    -P ${CMAKE_SOURCE_DIR}/cmake/runcommands.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    set_tests_properties(${EXECUTABLE_NAME}_test
        PROPERTIES LABELS "${GROUP_NAME}"
    )

    # Emulator test
    # -------------

    if(BUILD_GBA_INTERNAL)
        set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test-gba.lua")
        if(NOT EXISTS ${TEST_SCRIPT})
            set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test.lua")
        endif()

        set(REF_1_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-1-gba.png")
        if(NOT EXISTS ${REF_1_PNG})
            set(REF_1_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-1.png")
        endif()

        set(REF_2_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-2-gba.png")
        if(NOT EXISTS ${REF_2_PNG})
            set(REF_2_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-2.png")
        endif()

        set(REF_3_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-3-gba.png")
        if(NOT EXISTS ${REF_3_PNG})
            set(REF_3_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-3.png")
        endif()

        set(GBA_ROM "${CMAKE_CURRENT_BINARY_DIR}/gba/${EXECUTABLE_NAME}_gba.gba")

        set(CMD1 "$<TARGET_FILE:giibiiadvance> --lua ${TEST_SCRIPT} ${GBA_ROM}")
        set(CMD2 "$<TARGET_FILE:pngmatch> ${REF_1_PNG} screenshot-1.png")
        set(CMD3 "$<TARGET_FILE:pngmatch> ${REF_2_PNG} screenshot-2.png")
        set(CMD4 "$<TARGET_FILE:pngmatch> ${REF_3_PNG} screenshot-3.png")

        add_test(NAME ${EXECUTABLE_NAME}_gba_test
            COMMAND ${CMAKE_COMMAND}
                        -DCMD1=${CMD1}
                        -DCMD2=${CMD2}
                        -DCMD3=${CMD3}
                        -DCMD4=${CMD4}
                        -P ${CMAKE_SOURCE_DIR}/cmake/runcommands.cmake
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
        set_tests_properties(${EXECUTABLE_NAME}_gba_test
            PROPERTIES LABELS "${GROUP_NAME};gba"
        )
    endif()

endfunction()

function(unittest_audio)

    # Get name of the folder we are in
    # --------------------------------

    get_filename_component(EXECUTABLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    get_filename_component(PARENT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
    get_filename_component(GROUP_NAME "${PARENT_PATH}" NAME)

    # SDL2 test
    # ---------

    set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test-sdl2.lua")
    if(NOT EXISTS ${TEST_SCRIPT})
        set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test.lua")
    endif()

    set(REF_WAV "${CMAKE_CURRENT_SOURCE_DIR}/reference-sdl2.wav")
    if(NOT EXISTS ${REF_WAV})
        set(REF_WAV "${CMAKE_CURRENT_SOURCE_DIR}/reference.wav")
    endif()

    set(CMD1 "$<TARGET_FILE:${EXECUTABLE_NAME}> --lua ${TEST_SCRIPT}")
    set(CMD2 "${CMAKE_COMMAND} -E compare_files ${REF_WAV} audio.wav")

    add_test(NAME ${EXECUTABLE_NAME}_test
        COMMAND ${CMAKE_COMMAND}
                    -DCMD1=${CMD1}
                    -DCMD2=${CMD2}
                    -P ${CMAKE_SOURCE_DIR}/cmake/runcommands.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    set_tests_properties(${EXECUTABLE_NAME}_test
        PROPERTIES LABELS "${GROUP_NAME}"
    )

    # Emulator test
    # -------------

    if(BUILD_GBA_INTERNAL)
        set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test-gba.lua")
        if(NOT EXISTS ${TEST_SCRIPT})
            set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test.lua")
        endif()

        set(REF_WAV "${CMAKE_CURRENT_SOURCE_DIR}/reference-gba.wav")
        if(NOT EXISTS ${REF_WAV})
            set(REF_WAV "${CMAKE_CURRENT_SOURCE_DIR}/reference.wav")
        endif()

        set(GBA_ROM "${CMAKE_CURRENT_BINARY_DIR}/gba/${EXECUTABLE_NAME}_gba.gba")

        set(CMD1 "$<TARGET_FILE:giibiiadvance> --lua ${TEST_SCRIPT} ${GBA_ROM}")
        set(CMD2 "${CMAKE_COMMAND} -E compare_files ${REF_WAV} audio.wav")

        add_test(NAME ${EXECUTABLE_NAME}_gba_test
            COMMAND ${CMAKE_COMMAND}
                        -DCMD1=${CMD1}
                        -DCMD2=${CMD2}
                        -P ${CMAKE_SOURCE_DIR}/cmake/runcommands.cmake
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
        set_tests_properties(${EXECUTABLE_NAME}_gba_test
            PROPERTIES LABELS "${GROUP_NAME};gba"
        )
    endif()

endfunction()

function(unittest_audio_screenshot)

    # Get name of the folder we are in
    # --------------------------------

    get_filename_component(EXECUTABLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    get_filename_component(PARENT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
    get_filename_component(GROUP_NAME "${PARENT_PATH}" NAME)

    # SDL2 test
    # ---------

    set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test-sdl2.lua")
    if(NOT EXISTS ${TEST_SCRIPT})
        set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test.lua")
    endif()

    set(REF_WAV "${CMAKE_CURRENT_SOURCE_DIR}/reference-sdl2.wav")
    if(NOT EXISTS ${REF_WAV})
        set(REF_WAV "${CMAKE_CURRENT_SOURCE_DIR}/reference.wav")
    endif()

    set(REF_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-sdl2.png")
    if(NOT EXISTS ${REF_PNG})
        set(REF_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference.png")
    endif()

    set(CMD1 "$<TARGET_FILE:${EXECUTABLE_NAME}> --lua ${TEST_SCRIPT}")
    set(CMD2 "${CMAKE_COMMAND} -E compare_files ${REF_WAV} audio.wav")
    set(CMD3 "$<TARGET_FILE:pngmatch> ${REF_PNG} screenshot.png")

    add_test(NAME ${EXECUTABLE_NAME}_test
        COMMAND ${CMAKE_COMMAND}
                    -DCMD1=${CMD1}
                    -DCMD2=${CMD2}
                    -DCMD3=${CMD3}
                    -P ${CMAKE_SOURCE_DIR}/cmake/runcommands.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    set_tests_properties(${EXECUTABLE_NAME}_test
        PROPERTIES LABELS "${GROUP_NAME}"
    )

    # Emulator test
    # -------------

    if(BUILD_GBA_INTERNAL)
        set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test-gba.lua")
        if(NOT EXISTS ${TEST_SCRIPT})
            set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test.lua")
        endif()

        set(REF_WAV "${CMAKE_CURRENT_SOURCE_DIR}/reference-gba.wav")
        if(NOT EXISTS ${REF_WAV})
            set(REF_WAV "${CMAKE_CURRENT_SOURCE_DIR}/reference.wav")
        endif()

        set(REF_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-gba.png")
        if(NOT EXISTS ${REF_PNG})
            set(REF_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference.png")
        endif()

        set(GBA_ROM "${CMAKE_CURRENT_BINARY_DIR}/gba/${EXECUTABLE_NAME}_gba.gba")

        set(CMD1 "$<TARGET_FILE:giibiiadvance> --lua ${TEST_SCRIPT} ${GBA_ROM}")
        set(CMD2 "${CMAKE_COMMAND} -E compare_files ${REF_WAV} audio.wav")
        set(CMD3 "$<TARGET_FILE:pngmatch> ${REF_PNG} screenshot.png")

        add_test(NAME ${EXECUTABLE_NAME}_gba_test
            COMMAND ${CMAKE_COMMAND}
                        -DCMD1=${CMD1}
                        -DCMD2=${CMD2}
                        -DCMD3=${CMD3}
                        -P ${CMAKE_SOURCE_DIR}/cmake/runcommands.cmake
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
        set_tests_properties(${EXECUTABLE_NAME}_gba_test
            PROPERTIES LABELS "${GROUP_NAME};gba"
        )
    endif()

endfunction()

function(unittest_sram)

    # Get name of the folder we are in
    # --------------------------------

    get_filename_component(EXECUTABLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    get_filename_component(PARENT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
    get_filename_component(GROUP_NAME "${PARENT_PATH}" NAME)

    # SDL2 test
    # ---------

    set(TEST_SCRIPT_1 "${CMAKE_CURRENT_SOURCE_DIR}/test-1.lua")
    set(TEST_SCRIPT_2 "${CMAKE_CURRENT_SOURCE_DIR}/test-2.lua")
    set(REF_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference.png")

    set(CMD1 "$<TARGET_FILE:${EXECUTABLE_NAME}> --lua ${TEST_SCRIPT_1}")
    set(CMD2 "$<TARGET_FILE:${EXECUTABLE_NAME}> --lua ${TEST_SCRIPT_2}")
    set(CMD3 "$<TARGET_FILE:pngmatch> ${REF_PNG} screenshot.png")

    add_test(NAME ${EXECUTABLE_NAME}_test
        COMMAND ${CMAKE_COMMAND}
                    -DCMD1=${CMD1}
                    -DCMD2=${CMD2}
                    -DCMD3=${CMD3}
                    -P ${CMAKE_SOURCE_DIR}/cmake/runcommands.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    set_tests_properties(${EXECUTABLE_NAME}_test
        PROPERTIES LABELS "${GROUP_NAME}"
    )

    # Emulator test
    # -------------

    if(BUILD_GBA_INTERNAL)
        set(GBA_ROM "${CMAKE_CURRENT_BINARY_DIR}/gba/${EXECUTABLE_NAME}_gba.gba")

        set(CMD1 "$<TARGET_FILE:giibiiadvance> --lua ${TEST_SCRIPT_1} ${GBA_ROM}")
        set(CMD2 "$<TARGET_FILE:giibiiadvance> --lua ${TEST_SCRIPT_2} ${GBA_ROM}")
        set(CMD3 "$<TARGET_FILE:pngmatch> ${REF_PNG} screenshot.png")

        add_test(NAME ${EXECUTABLE_NAME}_gba_test
            COMMAND ${CMAKE_COMMAND}
                        -DCMD1=${CMD1}
                        -DCMD2=${CMD2}
                        -DCMD3=${CMD3}
                        -P ${CMAKE_SOURCE_DIR}/cmake/runcommands.cmake
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
        set_tests_properties(${EXECUTABLE_NAME}_gba_test
            PROPERTIES LABELS "${GROUP_NAME};gba"
        )
    endif()

endfunction()
