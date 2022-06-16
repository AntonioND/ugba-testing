// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz

// Make sure that interrupts happen in the right order during a scanline.
// VCOUNT comes right at the start, HBLANK comes afterwards, when HBLANK starts.
// When VBLANK starts, VCOUNT and VBLANK happen right when the switch to
// VCOUNT=160 happens. HBLANK happens afterwards. This test tests line 160. It
// waits for line 158 to set up everything, then runs the test.

#include <stdio.h>

#include <ugba/ugba.h>

volatile int count;
volatile int vbl_value, hbl_value, vcount_value;
volatile int vcount_ref;

void vbl_handler(void)
{
    if (vbl_value != -1)
        return;

    vbl_value = count;
    count++;
}

void hbl_handler_line160(void)
{
    // This handler may be called in line 159 as well
    if (REG_VCOUNT != 160)
        return;

    if (hbl_value != -1)
        return;

    hbl_value = count;
    count++;
    IRQ_Disable(IRQ_HBLANK);
}

void vcount_handler(void)
{
    if (vcount_value != -1)
        return;

    vcount_value = count;
    count++;
    IRQ_Disable(IRQ_VCOUNT);
}

void irq_setup(void)
{
    IRQ_SetHandler(IRQ_VBLANK, vbl_handler);
    IRQ_SetHandler(IRQ_HBLANK, hbl_handler_line160);
    IRQ_SetHandler(IRQ_VCOUNT, vcount_handler);

    IRQ_Enable(IRQ_HBLANK);

    IRQ_SetReferenceVCOUNT(160);

    // Make sure that we had time to finish this in time
    vcount_ref = REG_VCOUNT;
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    DISP_ModeSet(0);
    DISP_LayersEnable(1, 0, 0, 0, 0);

    CON_InitDefault();

    count = 0;
    vbl_value = -1;
    hbl_value = -1;
    vcount_value = -1;
    vcount_ref = -1;

    IRQ_Enable(IRQ_VBLANK);
    IRQ_Enable(IRQ_VCOUNT);

    // Enable interrupts at the right time
    SWI_VBlankIntrWait();
    IRQ_SetReferenceVCOUNT(158);
    IRQ_SetHandler(IRQ_VCOUNT, irq_setup);

    SWI_VBlankIntrWait();
    SWI_VBlankIntrWait();

    {
        char text[100];
        snprintf(text, sizeof(text),
                 "VBL:    %d\n"
                 "HBL:    %d\n"
                 "VCOUNT: %d\n"
                 "\n"
                 "Should be 158: %d\n"
                 "\n",
                 vbl_value, hbl_value, vcount_value, vcount_ref);
        CON_Print(text);
    }

    while (1)
        SWI_VBlankIntrWait();
}
