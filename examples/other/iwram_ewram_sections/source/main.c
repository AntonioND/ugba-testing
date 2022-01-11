// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021-2022 Antonio Niño Díaz

// Example that shows how to place code in IWRAM and checks that it is faster
// than the same code in EWRAM. It also tests that a function in ARM is slower
// than in Thumb because of being placed in ROM. ROM has a 16-bit bus, so Thumb
// instructions can be read in one go, ARM instructions need two reads.
//
// This also tests that the IWRAM and EWRAM macros correctly place .code, .data
// and .bss sections in IWRAM or EWRAM as expected.
//
// This test doesn't work in the SDL2 port because there is no IWRAM, or EWRAM,
// and code can't be built as ARM or Thumb either.
//
// It is simply built to make sure that the SDL2 port ignores the defines used to
// place code in different locations works, as well as the macros to force code
// to be compiled as ARM or Thumb.

#include <inttypes.h>
#include <stdio.h>

#include <ugba/ugba.h>

#ifdef __GBA__

IWRAM_CODE uint32_t divide_iwram(uint32_t a, uint32_t b);
EWRAM_CODE uint32_t divide_ewram(uint32_t a, uint32_t b);
uint32_t divide_arm(uint32_t a, uint32_t b);
uint32_t divide_thumb(uint32_t a, uint32_t b);

#define REFERENCE_DATA_IWRAM { \
        15, 16, 17, 18, 19, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, \
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, \
    }

#define REFERENCE_DATA_EWRAM { \
        19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, \
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, \
    }

IWRAM_DATA uint8_t data_array_iwram[40] = REFERENCE_DATA_IWRAM;
EWRAM_DATA uint8_t data_array_ewram[40] = REFERENCE_DATA_EWRAM;

IWRAM_BSS uint8_t bss_array_iwram[40];
EWRAM_BSS uint8_t bss_array_ewram[40];

volatile uint32_t result;

#define DO_TEST(fn_name, result_var_name)                   \
                                                            \
    TM_TimerStart(0, 0, 256, 0);                            \
                                                            \
    for (uint32_t a = 100; a < 200; a++)                    \
    {                                                       \
        for (uint32_t b = 1; b < 100; b++)                  \
        result = fn_name(a, b);                             \
    }                                                       \
                                                            \
    uint32_t result_var_name = REG_TM0CNT_L;                \
                                                            \
    TM_TimerStop(0);                                        \

int is_pointer_in_iwram(void *p)
{
    uintptr_t v = (uintptr_t)p;

    if ((v >= MEM_IWRAM_ADDR) && (v < (MEM_IWRAM_ADDR + MEM_IWRAM_SIZE)))
        return 1;
    return 0;
}

int is_pointer_in_ewram(void *p)
{
    uintptr_t v = (uintptr_t)p;

    if ((v >= MEM_EWRAM_ADDR) && (v < (MEM_EWRAM_ADDR + MEM_EWRAM_SIZE)))
        return 1;
    return 0;
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    // Test functions in IWRAM and EWRAM

    DO_TEST(divide_iwram, time_iwram);
    DO_TEST(divide_ewram, time_ewram);

    // Test ARM and Thumb functions

    DO_TEST(divide_arm, time_arm);
    DO_TEST(divide_thumb, time_thumb);

    // After testing the timings, setup the display

    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(0);

    CON_InitDefault();

    char string[100];

    // Check contents of pre-initialized arrays

    uint8_t reference_iwram[] = REFERENCE_DATA_IWRAM;
    for (size_t i = 0; i < sizeof(reference_iwram); i++)
    {
        if (reference_iwram[i] != data_array_iwram[i])
        {
            snprintf(string, sizeof(string),
                    "IWRAM data mismatch\nIndex %zu\n", i);
            CON_Print(string);
            CON_Print("Test FAILED");
            while (1)
                SWI_VBlankIntrWait();
        }
    }
    CON_Print("IWRAM data matches\n");

    uint8_t reference_ewram[] = REFERENCE_DATA_EWRAM;
    for (size_t i = 0; i < sizeof(reference_ewram); i++)
    {
        if (reference_ewram[i] != data_array_ewram[i])
        {
            snprintf(string, sizeof(string),
                    "EWRAM data mismatch\nIndex %zu\n", i);
            CON_Print(string);
            CON_Print("Test FAILED");
            while (1)
                SWI_VBlankIntrWait();
        }
    }
    CON_Print("EWRAM data matches\n");

    // Check locations of arrays and functions

    if (!is_pointer_in_iwram(&data_array_iwram[0]))
    {
        CON_Print("IWRAM .data not in IWRAM\n");
        CON_Print("Test FAILED");
        while (1)
            SWI_VBlankIntrWait();
    }
    else if (!is_pointer_in_iwram(&bss_array_iwram[0]))
    {
        CON_Print("IWRAM .bss not in IWRAM\n");
        CON_Print("Test FAILED");
        while (1)
            SWI_VBlankIntrWait();
    }
    else if (!is_pointer_in_iwram(divide_iwram))
    {
        CON_Print("IWRAM .code not in IWRAM\n");
        CON_Print("Test FAILED");
        while (1)
            SWI_VBlankIntrWait();
    }
    else
    {
        CON_Print("IWRAM sections OK\n");
    }

    if (!is_pointer_in_ewram(&data_array_ewram[0]))
    {
        CON_Print("EWRAM .data not in EWRAM\n");
        CON_Print("Test FAILED");
        while (1)
            SWI_VBlankIntrWait();
    }
    else if (!is_pointer_in_ewram(&bss_array_ewram[0]))
    {
        CON_Print("EWRAM .bss not in EWRAM\n");
        CON_Print("Test FAILED");
        while (1)
            SWI_VBlankIntrWait();
    }
    else if (!is_pointer_in_ewram(divide_ewram))
    {
        CON_Print("EWRAM .code not in EWRAM\n");
        CON_Print("Test FAILED");
        while (1)
            SWI_VBlankIntrWait();
    }
    else
    {
        CON_Print("EWRAM sections OK\n");
    }

    // Print execution timings

    // A function in IWRAM will always be faster than the same function in
    // EWRAM. Don't compare them, as it is enough test to see if the code is in
    // the right memory region.
    snprintf(string, sizeof(string),
             "\n"
             "Execution times:\n"
             "IWRAM: %" PRIu32 "\n"
             "EWRAM: %" PRIu32 "\n"
             "ARM:   %" PRIu32 "\n"
             "Thumb: %" PRIu32 "\n"
             "\n",
             time_iwram, time_ewram,
             time_arm, time_thumb);
    CON_Print(string);

    // A function in ARM takes up more space than in Thumb, which actually makes
    // it slower when they are both placed in ROM. Add a margin of error in
    // case a define doesn't work, but a difference of just a few cycles makes
    // the test pass by chance.
    int margin = 1000;
    int timings_ok = time_arm > (time_thumb + margin);

    if (timings_ok)
        CON_Print("Timings OK");
    else
        CON_Print("Unexpected timings");

    while (1)
    {
        SWI_VBlankIntrWait();

        KEYS_Update();

        uint16_t keys = KEYS_Held();

        if (keys & KEY_A)
            break;
    }

    CON_Clear();

    if (timings_ok)
        CON_Print("Test OK");
    else
        CON_Print("Test FAILED");

    while (1)
        SWI_VBlankIntrWait();
}

#else

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(0);

    CON_InitDefault();

    CON_Print("Test skipped!\n");

    while (1)
        SWI_VBlankIntrWait();
}

#endif // __GBA__
