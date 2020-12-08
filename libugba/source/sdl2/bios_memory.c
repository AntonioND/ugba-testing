// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <stdlib.h>
#include <string.h>

#include <ugba/ugba.h>

#include "debug_utils.h"

uint32_t SWI_GetBiosChecksum(void)
{
    // Pretend the emulated system is a regular GBA
    return SWI_CHECKSUM_GBA;
}

void SWI_CpuSet(const void *src, void *dst, uint32_t len_mode)
{
    int count = len_mode & 0x001FFFFF;
    uint32_t mode = len_mode & ~0x001FFFFF;

    if (mode & SWI_MODE_32BIT)
    {
        uint32_t *src_ = (uint32_t *)((uintptr_t)src & ~3);
        uint32_t *dst_ = (uint32_t *)((uintptr_t)dst & ~3);

        if (mode & SWI_MODE_FILL)
        {
            uint32_t fill = *src_;
            while (count--)
                *dst_++ = fill;
        }
        else // Copy
        {
            while (count--)
                *dst_++ = *src_++;
        }
    }
    else // 16 bit
    {
        uint16_t *src_ = (uint16_t *)((uintptr_t)src & ~1);
        uint16_t *dst_ = (uint16_t *)((uintptr_t)dst & ~1);

        if (mode & SWI_MODE_FILL)
        {
            uint16_t fill = *src_;
            while (count--)
                *dst_++ = fill;
        }
        else // Copy
        {
            while (count--)
                *dst_++ = *src_++;
        }
    }
}

void SWI_CpuFastSet(const void *src, void *dst, uint32_t len_mode)
{
    int count = len_mode & 0x001FFFF8; // Must be a multiple of 8 words
    uint32_t mode = len_mode & ~0x001FFFFF;

    uint32_t *src_ = (uint32_t *)((uintptr_t)src & ~3);
    uint32_t *dst_ = (uint32_t *)((uintptr_t)dst & ~3);

    if (mode & SWI_MODE_FILL)
    {
        uint32_t fill = *src_;
        while (count--)
            *dst_++ = fill;
    }
    else // Copy
    {
        while (count--)
            *dst_++ = *src_++;
    }
}

// The only difference between LZ77UnCompReadNormalWrite8bit() and
// LZ77UnCompReadNormalWrite16bit() is the width of the writes to the
// destination. There is no difference in the emulated BIOS.
static void SWI_UncompressLZ77(const void *source, void *dest)
{
    const uint8_t *src = source;
    uint16_t *dst = dest;

    // The header is 32 bits
    uint32_t header = *(uint32_t *)src;
    src += 4;

    uint32_t compression_type = (header >> 4) & 7;
    if (compression_type != 1)
    {
        Debug_Log("%s: Invalid type: %d", __func__, compression_type);
        return;
    }

    uint32_t size = (header >> 8) & 0x00FFFFFF;

    uint8_t *buffer = malloc(size + 2);
    if (buffer == NULL)
    {
        Debug_Log("%s: Not enough memory", __func__);
        return;
    }

    uint8_t *buffertmp = buffer;
    uint32_t total = 0;
    while (size > total)
    {
        uint8_t flag = *src++;

        for (int i = 0; i < 8; i++)
        {
            if (flag & 0x80)
            {
                // Compressed - Copy N+3 Bytes from Dest-Disp-1 to Dest

                uint16_t info = ((uint16_t)*src++) << 8;
                info |= (uint16_t)*src++;

                uint32_t displacement = (info & 0x0FFF);
                int num = 3 + ((info >> 12) & 0xF);
                uint32_t offset = total - displacement - 1;
                if (offset > total) // This also checks for negative values
                {
                    Debug_Log("%s: Error while decoding", __func__);
                    free(buffer);
                    return;
                }
                while (num--)
                {
                    *buffertmp++ = ((uint8_t *)buffer)[offset++];
                    total++;
                    if (size <= total)
                        break;
                }
            }
            else
            {
                // Uncompressed - Copy 1 Byte from Source to Dest
                *buffertmp++ = *src++;
                total++;
                if (size <= total)
                    break;
            }
            flag <<= 1;
        }
    }

    // Copy to destination
    memcpy(dst, buffer, size);

    free(buffer);
}

void SWI_LZ77UnCompReadNormalWrite8bit(const void *source, void *dest)
{
    SWI_UncompressLZ77(source, dest);
}

void SWI_LZ77UnCompReadNormalWrite16bit(const void *source, void *dest)
{
    SWI_UncompressLZ77(source, dest);
}

static void GBA_Diff8bitUnFilter(const void *source, void *dest)
{
    const uint8_t *src = source;
    uint8_t *dst = dest;

    uint32_t header = *(uint32_t *)source;

    src += 4;

    // TODO: Check extra fields in header

    int32_t size = (header >> 8) & 0x00FFFFFF;

    uint8_t value = 0;

    while (size > 0)
    {
        value += *src++;
        *dst++ = value;
        size--;
    }
}
void SWI_Diff8bitUnFilterWram(const void *source, void *dest)
{
    // TODO: Check alignment of arguments

    GBA_Diff8bitUnFilter(source, dest);
}

void SWI_Diff8bitUnFilterVram(const void *source, void *dest)
{
    // TODO: Check alignment of arguments
    // TODO: Can the size be not a multiple of 2?

    GBA_Diff8bitUnFilter(source, dest);
}

void SWI_Diff16bitUnFilter(const void *source, void *dest)
{
    // TODO: Check alignment of arguments

    const uint16_t *src = source;
    uint16_t *dst = dest;

    uint32_t header = *(uint32_t *)source;

    src += 2;

    // TODO: Check extra fields in header

    int32_t size = (header >> 8) & 0x00FFFFFF;

    uint16_t value = 0;

    while (size > 0)
    {
        value += *src++;
        *dst++ = value;
        size -= 2;
    }
}