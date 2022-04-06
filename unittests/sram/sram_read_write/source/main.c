// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz

// Test that makes sure that SRAM_Write() and SRAM_Read() work as expected.

#include <stdio.h>

#include <ugba/ugba.h>

EWRAM_DATA uint8_t buffer_big[MEM_SRAM_SIZE * 2] = { 0 };
EWRAM_DATA uint8_t buffer_exact[MEM_SRAM_SIZE] = { 0 };
EWRAM_DATA uint8_t buffer_small[MEM_SRAM_SIZE / 2] = { 0 };

#define EXPECT(code) \
    if (ret != code) \
    { \
        char string[100]; \
        snprintf(string, sizeof(string), "%d: Got %d, expected: %d\n", \
                 __LINE__, ret, code); \
        CON_Print(string); \
    }

void SRAM_Location_Tests(void)
{
    // Only test this on GBA builds, there is no EWRAM in SDL2 builds
#ifdef __GBA__
    uintptr_t read_addr = (uintptr_t)&SRAM_Read;
    uintptr_t write_addr = (uintptr_t)&SRAM_Write;

    if ((read_addr < MEM_EWRAM_ADDR) ||
        (read_addr > (MEM_EWRAM_ADDR + MEM_EWRAM_SIZE)))
    {
        char string[100];
        snprintf(string, sizeof(string), "%d: SRAM_Read not in EWRAM\n",
                 __LINE__);
        CON_Print(string);
    }

    if ((write_addr < MEM_EWRAM_ADDR) ||
        (write_addr > (MEM_EWRAM_ADDR + MEM_EWRAM_SIZE)))
    {
        char string[100];
        snprintf(string, sizeof(string), "%d: SRAM_Write not in EWRAM\n",
                 __LINE__);
        CON_Print(string);
    }
#endif
}

void SRAM_Write_Tests(void)
{
    int ret;

    // 1) Buffer too big

    // Start before start of SRAM
    ret = SRAM_Write(NULL, buffer_big, sizeof(buffer_big));
    EXPECT(-1);

    // Start in SRAM
    ret = SRAM_Write(MEM_SRAM, buffer_big, sizeof(buffer_big));
    EXPECT(-1);

    // 2) Buffer exact size

    // Start before start of SRAM
    ret = SRAM_Write(NULL, buffer_exact, sizeof(buffer_exact));
    EXPECT(-2);

    // Start in SRAM
    ret = SRAM_Write(MEM_SRAM, buffer_exact, sizeof(buffer_exact));
    EXPECT(0);

    // Start after start of SRAM
    ret = SRAM_Write((uintptr_t *)MEM_SRAM + 1, buffer_exact,
                     sizeof(buffer_exact));
    EXPECT(-3);

    // 3) Buffer small

    // Start before start of SRAM
    ret = SRAM_Write(NULL, buffer_small, sizeof(buffer_small));
    EXPECT(-2);

    // Start in SRAM
    ret = SRAM_Write(MEM_SRAM, buffer_small, sizeof(buffer_small));
    EXPECT(0);

    // Start after start of SRAM
    ret = SRAM_Write((uintptr_t *)MEM_SRAM + 1024, buffer_small,
                     sizeof(buffer_small));
    EXPECT(0);

    // Start after end of SRAM
    ret = SRAM_Write((uintptr_t *)MEM_SRAM + MEM_SRAM_SIZE, buffer_small,
                     sizeof(buffer_small));
    EXPECT(-3);

    // 4) Verify contents after a write

    for (size_t i = 0; i < MEM_SRAM_SIZE / sizeof(uint32_t); i++)
        ((uint32_t *)buffer_exact)[i] = i;

    ret = SRAM_Write(MEM_SRAM, buffer_exact, sizeof(buffer_exact));
    EXPECT(0);

    for (size_t i = 0; i < MEM_SRAM_SIZE; i++)
    {
        uint8_t s = ((uint8_t *)MEM_SRAM)[i];
        uint8_t b = buffer_exact[i];

        if (s != b)
        {
            char string[100];
            snprintf(string, sizeof(string), "%d: Got %u, expected: %u\n",
                    __LINE__, b, s);
            CON_Print(string);
            break;
        }
    }
}

void SRAM_Read_Tests(void)
{
    int ret;

    // 1) Buffer too big

    // Start before start of SRAM
    ret = SRAM_Read(buffer_big, NULL, sizeof(buffer_big));
    EXPECT(-1);

    // Start in SRAM
    ret = SRAM_Read(buffer_big, MEM_SRAM, sizeof(buffer_big));
    EXPECT(-1);

    // 2) Buffer exact size

    // Start before start of SRAM
    ret = SRAM_Read(buffer_exact, NULL, sizeof(buffer_exact));
    EXPECT(-2);

    // Start in SRAM
    ret = SRAM_Read(buffer_exact, MEM_SRAM, sizeof(buffer_exact));
    EXPECT(0);

    // Start after start of SRAM
    ret = SRAM_Read(buffer_exact, (uintptr_t *)MEM_SRAM + 1,
                    sizeof(buffer_exact));
    EXPECT(-3);

    // 3) Buffer small

    // Start before start of SRAM
    ret = SRAM_Read(buffer_small, NULL, sizeof(buffer_small));
    EXPECT(-2);

    // Start in SRAM
    ret = SRAM_Read(buffer_small, MEM_SRAM, sizeof(buffer_small));
    EXPECT(0);

    // Start after start of SRAM
    ret = SRAM_Read(buffer_small, (uintptr_t *)MEM_SRAM + 1024,
                    sizeof(buffer_small));
    EXPECT(0);

    // Start after end of SRAM
    ret = SRAM_Read(buffer_small, (uintptr_t *)MEM_SRAM + MEM_SRAM_SIZE,
                    sizeof(buffer_small));
    EXPECT(-3);

    // 4) Verify contents after a read

    for (size_t i = 0; i < MEM_SRAM_SIZE; i++)
        ((uint8_t *)MEM_SRAM)[i] = i;

    ret = SRAM_Read(buffer_exact, MEM_SRAM, sizeof(buffer_exact));
    EXPECT(0);

    for (size_t i = 0; i < MEM_SRAM_SIZE; i++)
    {
        uint8_t s = ((uint8_t *)MEM_SRAM)[i];
        uint8_t b = buffer_exact[i];

        if (s != b)
        {
            char string[100];
            snprintf(string, sizeof(string), "%d: Got %u, expected: %u\n",
                    __LINE__, b, s);
            CON_Print(string);
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(0);

    CON_InitDefault();

    CON_Print("Location of functions\n");
    SRAM_Location_Tests();

    CON_Print("SRAM_Write()\n");
    SRAM_Write_Tests();

    CON_Print("SRAM_Read()\n");
    SRAM_Read_Tests();

    CON_Print("Tests completed.\n");

    while (1)
        SWI_VBlankIntrWait();
}
