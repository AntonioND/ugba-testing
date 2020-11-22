// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <gbaline.h>

static uint16_t keys_held, keys_pressed, keys_released;
static uint16_t keys_held_last;

void KEYS_Update(void)
{
    keys_held_last = keys_held;
    keys_held = ~REG_KEYINPUT;

    keys_pressed = keys_held & ~keys_held_last;
    keys_released = keys_held_last & ~keys_held;
}

uint16_t KEYS_Pressed(void)
{
    return keys_pressed;
}

uint16_t KEYS_Held(void)
{
    return keys_held;
}

uint16_t KEYS_Released(void)
{
    return keys_released;
}