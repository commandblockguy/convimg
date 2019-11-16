/*
 * Copyright 2017-2019 Matt "MateoConLechuga" Waltz
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "color.h"

#include <stdbool.h>

/*
 * Color conversion functions
 */
static void color_888_to_1555(liq_color *in, uint16_t *out)
{
    uint8_t r5 = (in->r * 31 + 128) / 255;
    uint8_t g6 = (in->g * 63 + 128) / 255;
    uint8_t b5 = (in->b * 31 + 128) / 255;

    *out = (r5 << 10) | (g6 << 5) | b5;
}

static void color_888_to_565(liq_color *in, uint16_t *out)
{
    uint8_t r5 = (in->r * 31 + 128) / 255;
    uint8_t g6 = (in->g * 63 + 128) / 255;
    uint8_t b5 = (in->b * 31 + 128) / 255;

    *out = (g6 << 15) | (r5 << 10) | ((g6 >> 1) << 5) | b5;
}

static void color_1555_to_888(uint16_t *in, liq_color *out)
{
    uint8_t r5 = (*in >> 10) & 31;
    uint8_t g6 = ((*in >> 4) & 62) | (*in >> 15);
    uint8_t b5 = *in & 31;

    liq_color color =
        {
            .r = (r5 * 527 + 23) >> 6,
            .g = (g6 * 255 + 32) / 63,
            .b = (b5 * 527 + 23) >> 6,
            .a = 255,
        };

    *out = color;
}

static void color_565_to_888(uint16_t *in, liq_color *out)
{
    uint8_t r5 = (*in >> 11) & 31;
    uint8_t g6 = (*in >> 5) & 63;
    uint8_t b5 = *in & 31;

    liq_color color =
        {
            .r = (r5 * 527 + 23) >> 6,
            .g = (g6 * 255 + 32) / 63,
            .b = (b5 * 527 + 23) >> 6,
            .a = 255,
        };

    *out = color;
}

/*
 * Converts an RGB color to the nearest target color, and back.
 * Used for color quanization to avoid duplicate entries.
 */
void color_convert(color_t *color, color_mode_t mode)
{
    switch (mode)
    {
        case COLOR_MODE_1555_GRGB:
        case COLOR_MODE_1555_GBGR:
            color_888_to_1555(&color->rgb, &color->target);
            color_1555_to_888(&color->target, &color->rgb);
            break;

        case COLOR_MODE_565_RGB:
        case COLOR_MODE_565_BGR:
            color_888_to_565(&color->rgb, &color->target);
            color_565_to_888(&color->target, &color->rgb);
            break;
    }
}