// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz

// Example of playing music with GBT Player:
//
//     https://github.com/AntonioND/gbt-player

#include <ugba/ugba.h>

#include "gbt_player.h"

extern const uint8_t *template[];

void vbl_handler(void)
{
    gbt_update();
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_SetHandler(IRQ_VBLANK, vbl_handler);
    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(0);

    CON_InitDefault();

    CON_Print("GBT Player example");

    // The sound hardware needs to be enabled to write to any other register.
    SOUND_MasterEnable(1);

    gbt_play(template, 0);

    while (1)
        SWI_VBlankIntrWait();

    return 0;
}
