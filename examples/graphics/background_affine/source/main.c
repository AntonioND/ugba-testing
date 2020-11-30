// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Example of how to load an affine background

#include <ugba/ugba.h>

#include "city.h" // Autogenerated from city.png

#define CITY_TILES_BASE     MEM_BG_TILES_BLOCK_ADDR(0)
#define CITY_MAP_BASE       MEM_BG_MAP_BLOCK_ADDR(8)

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    // Enable interrupts. This is needed for SWI_VBlankIntrWait() to work.
    IRQ_Enable(IRQ_VBLANK);

    // Load the palette
    VRAM_BGPalette256Copy(cityPal, cityPalLen);

    // Load the tiles
    SWI_CpuSet_Copy16(cityTiles, (void *)CITY_TILES_BASE, cityTilesLen);

    // Load the map
    SWI_CpuSet_Copy16(cityMap, (void *)CITY_MAP_BASE, cityMapLen);

    // Setup background
    BG_AffineInit(2, BG_AFFINE_256x256, CITY_TILES_BASE, CITY_MAP_BASE, 0);

    int x = 100, y = -7;
    int angle = 0x20;

    bg_affine_src bg_src_start = {
        x << 8, y << 8,
        0, 0,
        1 << 8, 1 << 8,
        angle << 8,
        0
    };

    bg_affine_dst bg_dst;

    SWI_BgAffineSet(&bg_src_start, &bg_dst, 1);

    BG_AffineTransformSet(2, &bg_dst);

    // Set the display to mode 2 so that background 2 is affine, and turn it on.
    REG_DISPCNT = DISPCNT_BG_MODE(2) | DISPCNT_BG2_ENABLE;

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

        if (keys & KEY_L)
            angle++;
        else if (keys & KEY_R)
            angle--;

        bg_affine_src bg_src = {
            x << 8, y << 8,
            0, 0,
            1 << 8, 1 << 8,
            angle << 8,
            0
        };

        SWI_BgAffineSet(&bg_src, &bg_dst, 1);

        BG_AffineTransformSet(2, &bg_dst);
    }
}