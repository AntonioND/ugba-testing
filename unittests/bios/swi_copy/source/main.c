// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz

// Test that checks that the emulated BIOS copy functions work.

#include <stdlib.h>
#include <string.h>

#include <ugba/ugba.h>

uint8_t source[128], destination[128];
uint16_t fill_16;
uint32_t fill_32;

void check_array(const char *name)
{
    CON_Print(name);
    CON_Print(": ");

    int ok = 1;
    size_t i = 0;

    for ( ; i < (sizeof(source) / 2); i++)
    {
        if (source[i] != destination[i])
        {
            ok = 0;
            break;
        }
    }
    for ( ; i < sizeof(source); i++)
    {
        if (destination[i] != 0)
        {
            ok = 0;
            break;
        }
    }

    CON_Print(ok ? "OK\n" : "FAIL\n");
}

void copy_prepare_array(void)
{
    size_t i = 0;

    for ( ; i < (sizeof(source) / 2); i++)
        source[i] = rand();
    for ( ; i < sizeof(source); i++)
        source[i] = 0xFF;

    for (i = 0; i < sizeof(destination); i++)
        destination[i] = 0;
}

void fill_prepare_array_16(void)
{
    size_t i = 0;

    uint16_t b = rand();

    uint16_t *src = (uint16_t *)&source[0];
    size_t elements = sizeof(source) / sizeof(uint16_t);

    for ( ; i < (elements / 2); i++)
        src[i] = b;
    for ( ; i < elements; i++)
        src[i] = 0xFFFF;

    for (i = 0; i < sizeof(destination); i++)
        destination[i] = 0;
}

void fill_prepare_array_32(void)
{
    size_t i = 0;

    uint32_t b = rand();

    uint32_t *src = (uint32_t *)&source[0];
    size_t elements = sizeof(source) / sizeof(uint32_t);

    for ( ; i < (elements / 2); i++)
        src[i] = b;
    for ( ; i < elements; i++)
        src[i] = 0xFFFFFFFF;

    for (i = 0; i < sizeof(destination); i++)
        destination[i] = 0;
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    DISP_ModeSet(0);
    DISP_LayersEnable(1, 0, 0, 0, 0);

    CON_InitDefault();

    copy_prepare_array();
    SWI_CpuSet_Copy16(source, destination, sizeof(destination) / 2);
    check_array("SWI_CpuSet_Copy16");

    copy_prepare_array();
    SWI_CpuSet_Copy32(source, destination, sizeof(destination) / 2);
    check_array("SWI_CpuSet_Copy32");

    copy_prepare_array();
    SWI_CpuFastSet_Copy32(source, destination, sizeof(destination) / 2);
    check_array("SWI_CpuFastSet_Copy32");

    copy_prepare_array();
    SWI_CpuSet(source, destination,
               (sizeof(destination) / 4) | SWI_MODE_COPY | SWI_MODE_16BIT);
    check_array("SWI_CpuSet(COPY | 16)");

    copy_prepare_array();
    SWI_CpuSet(source, destination,
               (sizeof(destination) / 8) | SWI_MODE_COPY | SWI_MODE_32BIT);
    check_array("SWI_CpuSet(COPY | 32)");

    copy_prepare_array();
    SWI_CpuFastSet(source, destination,
                   (sizeof(destination) / 8) | SWI_MODE_COPY);
    check_array("SWI_CpuFastSet(COPY)");

    CON_Print("\n");

    fill_prepare_array_16();
    SWI_CpuSet_Fill16(source, destination, sizeof(destination) / 2);
    check_array("SWI_CpuSet_Fill16");

    fill_prepare_array_32();
    SWI_CpuSet_Fill32(source, destination, sizeof(destination) / 2);
    check_array("SWI_CpuSet_Fill32");

    fill_prepare_array_32();
    SWI_CpuFastSet_Fill32(source, destination, sizeof(destination) / 2);
    check_array("SWI_CpuFastSet_Fill32");

    fill_prepare_array_16();
    SWI_CpuSet(source, destination,
               (sizeof(destination) / 4) | SWI_MODE_FILL | SWI_MODE_16BIT);
    check_array("SWI_CpuSet(FILL | 16)");

    fill_prepare_array_32();
    SWI_CpuSet(source, destination,
               (sizeof(destination) / 8) | SWI_MODE_FILL | SWI_MODE_32BIT);
    check_array("SWI_CpuSet(FILL | 32)");

    fill_prepare_array_32();
    SWI_CpuFastSet(source, destination,
                   (sizeof(destination) / 8) | SWI_MODE_FILL);
    check_array("SWI_CpuFastSet(FILL)");

    while (1)
        SWI_VBlankIntrWait();
}
