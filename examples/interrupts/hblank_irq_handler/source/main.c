// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020-2022 Antonio Niño Díaz

// Example that shows how to use the HBLANK interrupt to create visual effects.

#include <ugba/ugba.h>

#include "graphics/city_map_bin.h" // Autogenerated from city.png
#include "graphics/city_palette_bin.h"
#include "graphics/city_tiles_bin.h"

#define CITY_MAP_PALETTE    (0)
#define CITY_TILES_BASE     MEM_BG_TILES_BLOCK_ADDR(0)
#define CITY_MAP_BASE       MEM_BG_MAP_BLOCK_ADDR(8)

void copy_map_to_sbb(const void *source, void *dest, int width, int height)
{
    uint16_t *src_ptr = (uint16_t *)source;
    uint16_t *dst_ptr = (uint16_t *)dest;

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            int index = 0;

            if (i >= 32)
                index += 32 * 32;
            if (j >= 32)
                index += 32 * 64;

            index += i % 32;
            index += (j % 32) * 32;

            dst_ptr[index] = *src_ptr++;
        }
    }
}

void load_bg(void)
{
    // Load the palette
    VRAM_BGPalette16Copy(city_palette_bin, city_palette_bin_size,
                         CITY_MAP_PALETTE);

    // Load the tiles
    SWI_CpuSet_Copy16(city_tiles_bin, (void *)CITY_TILES_BASE,
                      city_tiles_bin_size);

    // Load the map
    copy_map_to_sbb(city_map_bin, (void *)CITY_MAP_BASE, 64, 64);

    // Setup background
    BG_RegularInit(0, BG_REGULAR_512x512, BG_16_COLORS,
                   CITY_TILES_BASE, CITY_MAP_BASE);
}

int x = 80, y = 120;
uint16_t horizontal_offsets[160];

void fill_array(void)
{
    // The first HBL interrupt happens  after line 0 is drawn. It is needed to
    // offset the copy by one index. The last HBL interrupt happens right before
    // the VBL, so it isn't seen until line 0, and the value copied during that
    // HBL should be the corresponding one to line 0.

    int dst = 159;

    for (int i = 0; i < 160; i++)
    {
        horizontal_offsets[dst] = x + ((i & 16) ? (i & 15) : 15 - (i & 15));

        dst++;
        if (dst == 160)
            dst = 0;
    }
}

void hbl_handler(void)
{
    uint16_t vcount = REG_VCOUNT;

    if (vcount < 160)
        REG_BG0HOFS = horizontal_offsets[vcount];
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    load_bg();
    fill_array();
    REG_BG0VOFS = y;

    // Enable interrupts
    IRQ_SetHandler(IRQ_HBLANK, hbl_handler);
    IRQ_Enable(IRQ_VBLANK);
    IRQ_Enable(IRQ_HBLANK);

    // Set the display to mode 0 so that all backgrounds are in regular mode,
    // and turn on background 0.
    DISP_ModeSet(0);
    DISP_LayersEnable(1, 0, 0, 0, 0);

    while (1)
    {
        SWI_VBlankIntrWait();

        KEYS_Update();

        uint16_t keys = KEYS_Held();

        if (keys & KEY_UP)
            y--;
        else if (keys & KEY_DOWN)
            y++;

        if (keys & KEY_RIGHT)
            x++;
        else if (keys & KEY_LEFT)
            x--;

        fill_array();
        REG_BG0VOFS = y;
    }
}
