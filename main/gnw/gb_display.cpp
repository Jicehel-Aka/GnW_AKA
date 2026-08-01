#include "gb_display.h"
#include "gb_graphics.h"
#include "gb_common.h"
#include "core/input.h"
#include "gnw_scores.h"       // reuse the name-entry UI for gb.gui.keyboard
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include <string.h>
#include <stdio.h>

extern gb_graphics gfx;

namespace gbc {

// ---- 80x64 native (BGR565) back-buffer -----------------------------------
static const int W = 80, H = 64;
static uint16_t bb[W * H];

// Palette Gamebuino META (PICO-8), en BGR565 natif AKA (R en bits bas).
static const uint16_t PALETTE[16] = {
    0x0000, 0x5143, 0x512F, 0x5420, 0x3295, 0x4AAB, 0xC618, 0xEF9F, 0x481F, 0x051F, 0x277F, 0x3720, 0xFD65, 0x9BB0, 0xABBF, 0xAE7F
};

static inline void pset(int x, int y, uint16_t c) {
    if ((unsigned)x < W && (unsigned)y < H) bb[y * W + x] = c;
}

void GbDisplay::clear()          { for (int i = 0; i < W * H; ++i) bb[i] = colorBg; cursorX = cursorY = 0; }
void GbDisplay::clear(uint16_t c){ for (int i = 0; i < W * H; ++i) bb[i] = c; cursorX = cursorY = 0; }
void GbDisplay::fill(uint16_t c) { for (int i = 0; i < W * H; ++i) bb[i] = c; }

void GbDisplay::fillRect(int x, int y, int w, int h) {
    for (int j = 0; j < h; ++j) for (int i = 0; i < w; ++i) pset(x + i, y + j, colorFg);
}
void GbDisplay::drawRect(int x, int y, int w, int h) {
    for (int i = 0; i < w; ++i) { pset(x + i, y, colorFg); pset(x + i, y + h - 1, colorFg); }
    for (int j = 0; j < h; ++j) { pset(x, y + j, colorFg); pset(x + w - 1, y + j, colorFg); }
}

// Indexed image: header {w,h,frames,frameLoop,transparent,colorMode, <pad?>}
// then 4-bit indices (2 px/byte, high nibble first). VERIFY header length.
#define IMG_HDR 6
void GbDisplay::drawImage(int x, int y, Image& img) {
    if (!img.buf) return;
    uint8_t w = img.buf[0], h = img.buf[1], frames = img.buf[2] ? img.buf[2] : 1;
    uint8_t transparent = img.buf[4];
    const uint8_t* px = img.buf + IMG_HDR;
    uint32_t frameLen = (w * h + 1) / 2;              // nibbles -> bytes
    const uint8_t* f = px + (uint32_t)(img.frame % frames) * frameLen;
    for (int j = 0; j < h; ++j) {
        for (int i = 0; i < w; ++i) {
            uint32_t n = j * w + i;
            uint8_t byte = f[n >> 1];
            uint8_t idx = (n & 1) ? (byte & 0x0F) : (byte >> 4);
            if (idx == transparent || idx == INDEX_GREEN) continue;   // saute le vert de fond
            pset(x + i, y + j, PALETTE[idx & 0x0F]);
        }
    }
    if (frames > 1) img.frame = (img.frame + 1) % frames;   // auto-advance
}

// ---- deferred text (rendered on top at present time) ---------------------
struct TextItem { int x, y; uint16_t fg; char s[24]; };
static TextItem texts[16];
static int textCount = 0;

static void queue_text(GbDisplay& d, const char* s) {
    if (textCount < 16) {
        TextItem& t = texts[textCount++];
        t.x = d.cursorX; t.y = d.cursorY; t.fg = d.colorFg;
        strncpy(t.s, s, sizeof t.s - 1); t.s[sizeof t.s - 1] = 0;
    }
    d.cursorX += (int)strlen(s) * 4;
}
void GbDisplay::print(const char* s)   { queue_text(*this, s); }
void GbDisplay::print(long v)          { char b[16]; snprintf(b, sizeof b, "%ld", v); queue_text(*this, b); }
void GbDisplay::println(const char* s) { queue_text(*this, s); cursorX = 0; cursorY += 6; }
void GbDisplay::println(long v)        { char b[16]; snprintf(b, sizeof b, "%ld", v); println(b); }

// ---- present : scale 80x64 -> 320x240, then draw queued text -------------
void present() {
    gfx.drawImageScaled(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bb, W, H);
    const int sx = SCREEN_WIDTH / W, sy = SCREEN_HEIGHT / H;   // ~4 x ~3
    for (int i = 0; i < textCount; ++i) {
        gfx.setColor(texts[i].fg);
        gfx.move_cursor(texts[i].x * sx, texts[i].y * sy);
        gfx.print_str(texts[i].s);
    }
    textCount = 0;
    gfx.update();
}

char g_last_name[16] = {0};   // dernier nom saisi (pour alimenter gnw_scores sans redemander)
int GbGui::keyboard(const char*, char* dst, uint8_t len) {
    char name[9]; scores_name_entry(name);            // reuse the 1..8 char UI
    strncpy(dst, name, len); if (len) dst[len - 1] = 0;
    strncpy(g_last_name, name, sizeof(g_last_name) - 1); g_last_name[sizeof(g_last_name)-1] = 0;
    return (int)strlen(dst);
}

} // namespace gbc
