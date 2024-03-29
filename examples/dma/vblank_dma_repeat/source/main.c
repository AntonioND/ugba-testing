// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020-2022 Antonio Niño Díaz

// Example of how to use DMA triggered by the VBL period.

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

#define PTR_REG_BG0HOFS     ((uint16_t *)PTR_REG_16(OFFSET_BG0HOFS))
#define PTR_REG_BG0VOFS     ((uint16_t *)PTR_REG_16(OFFSET_BG0VOFS))

uint16_t x = 80, y = 120;

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    load_bg();

    // Transfer BG scroll values automatically during the VBL using DMA

    DMA_Transfer(0, &x, PTR_REG_BG0HOFS, 2,
                 DMACNT_DST_FIXED | DMACNT_SRC_FIXED | DMACNT_TRANSFER_16_BITS |
                 DMACNT_START_VBLANK | DMACNT_REPEAT_ON);
    DMA_Transfer(1, &y, PTR_REG_BG0VOFS, 2,
                 DMACNT_DST_FIXED | DMACNT_SRC_FIXED | DMACNT_TRANSFER_16_BITS |
                 DMACNT_START_VBLANK | DMACNT_REPEAT_ON);

    // Enable interrupts. This is needed for SWI_VBlankIntrWait() to work.
    IRQ_Enable(IRQ_VBLANK);

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
    }
}
