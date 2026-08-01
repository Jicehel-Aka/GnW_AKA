#include <cstddef>
#include "gnw_present.h"
#include "gb_graphics.h"
#include "gb_common.h"

extern gb_graphics gfx;

// Assembled native-order image at META resolution.
static const int GW = 160, GH = 128;
static uint16_t s_img[GW * GH];

static inline uint16_t swap_rb(uint16_t c) {
    return (uint16_t)((c >> 11) | (c & 0x07E0) | ((c & 0x001F) << 11));
}

void gnw_present_slice(uint8_t sliceY, uint8_t sliceH, const uint16_t* slice, uint16_t width) {
    for (int row = 0; row < sliceH; ++row) {
        int y = sliceY + row;
        if (y >= GH) break;
        const uint16_t* s = slice + (size_t)row * width;
        uint16_t* d = s_img + (size_t)y * GW;
        for (int x = 0; x < GW && x < width; ++x) d[x] = swap_rb(s[x]);
    }
}


void gnw_present_slice_bswap(uint8_t sliceY, uint8_t sliceH, const uint16_t* slice, uint16_t width) {
    // Donnees stockees en big-endian (ex: Octopus/Frakasss) : inverser les 2 octets, pas de swap R/B
    for (int row = 0; row < sliceH; ++row) {
        int y = sliceY + row;
        if (y >= GH) break;
        const uint16_t* src = slice + (size_t)row * width;
        uint16_t* d = s_img + (size_t)y * GW;
        for (int x = 0; x < GW && x < width; ++x) {
            uint16_t v = (uint16_t)((src[x] >> 8) | (src[x] << 8));            // octets (big-endian source)
            d[x] = (uint16_t)((v >> 11) | (v & 0x07E0) | ((v & 0x1F) << 11)); // + swap R/B (BGR)
        }
    }
}

void gnw_present_flush() {
    gfx.drawImageScaled(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, s_img, GW, GH);  // fill screen
    gfx.update();
}
