// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz

// Make VBLANK, HBLANK and VCOUNT interrupts happen while IME = 0. Then, set
// IME = 1 so that the global interrupt handler has to handle all of them at the
// same time. The global interrupt handler will prioritize HBLANK, then VCOUNT,
// then VBLANK.

#include <stdio.h>

#include <ugba/ugba.h>

volatile int count;
volatile int vbl_value, hbl_value, vcount_value;

void vbl_handler(void)
{
    if (vbl_value != -1)
        return;

    vbl_value = count;
    count++;
}

void hbl_handler(void)
{
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

    // Make it so that all VBL, HBL and VCOUNT bits are set in IF.

    REG_IME = 0;

    IRQ_SetHandler(IRQ_VBLANK, vbl_handler);
    IRQ_SetHandler(IRQ_HBLANK, hbl_handler);
    IRQ_SetHandler(IRQ_VCOUNT, vcount_handler);

    IRQ_SetReferenceVCOUNT(160);

    IRQ_Enable(IRQ_VBLANK);
    IRQ_Enable(IRQ_HBLANK);
    IRQ_Enable(IRQ_VCOUNT);

    // SWI_VBlankIntrWait() internally calls SWI_IntrWait(1, IRQF_VBLANK). This
    // BIOS calls sets REG_IME to 1 because of passing 1 in the first argument.
    // This behaviour makes SWI_VBlankIntrWait() unsuited for this test.
    // SWI_Halt() doesn't have this problem.

    while (REG_VCOUNT <= 160)
        SWI_Halt();
    while (REG_VCOUNT >= 160)
        SWI_Halt();
    while (REG_VCOUNT <= 160)
        SWI_Halt();
    while (REG_VCOUNT >= 160)
        SWI_Halt();

    // Make sure that all 3 interrupts are pending.
    {
        char text[100];
        snprintf(text, sizeof(text),
                 "IF: 0x%04X\n"
                 "\n",
                 REG_IF);
        CON_Print(text);
    }

    REG_IME = 1;
    UGBA_RegisterUpdatedOffset(OFFSET_IME);
    // Wait for all pending interrupts to be handled
    SWI_VBlankIntrWait();

    // Make sure that all interrupts have been handled.
    {
        char text[100];
        snprintf(text, sizeof(text),
                 "IF: 0x%04X\n"
                 "\n",
                 REG_IF);
        CON_Print(text);
    }

    {
        char text[100];
        snprintf(text, sizeof(text),
                 "VBL:    %d\n"
                 "HBL:    %d\n"
                 "VCOUNT: %d\n",
                 vbl_value, hbl_value, vcount_value);
        CON_Print(text);
    }

    while (1)
        SWI_VBlankIntrWait();
}
