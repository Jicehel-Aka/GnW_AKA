#pragma once
#include <stdint.h>
// Reusable presenter for the "sliced HD" META rendering pattern (Steph/jicehel).
// Any engine that composes 160x128 in horizontal RGB565 slices can run on the
// AKA by sending each slice here; a flush scales the assembled 160x128 image to
// the 320x240 screen. Colors are swapped RGB565(META) -> BGR565(AKA native).
void gnw_present_slice(uint8_t sliceY, uint8_t sliceH, const uint16_t* slice_rgb565, uint16_t width);
void gnw_present_slice_bswap(uint8_t sliceY, uint8_t sliceH, const uint16_t* slice_be565, uint16_t width); // pour donnees big-endian
void gnw_present_flush();   // scale 160x128 -> screen + gfx.update()
