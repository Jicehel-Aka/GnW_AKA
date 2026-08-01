#pragma once
#include <stdint.h>

// ColorMode au scope global (firebuino: gb.display.init(80,64,ColorMode::index))
enum class ColorMode : uint8_t { rgb565, index };
// Minimal Gamebuino META `gb.display` + `Image` compatibility for the AKA,
// targeting the low-res 80x64 INDEXED games (e.g. firebuino / Fire!).
// Everything is composed into an 80x64 native-order (BGR565) back-buffer, then
// scaled to fill 320x240 by gb_update() (which also paces, reads input and
// presents). Text is drawn on top at present time (see gb_display.cpp).
//
// VERIFY ON BUILD (cannot be validated here):
//  1) the exact INDEX_GREEN 16-color palette (placeholder below).
//  2) the indexed image header length + nibble packing (see IMG_HDR).
//  3) frame animation cadence.

namespace gbc {

// Gamebuino colors used by firebuino, expressed directly in RGB565.
enum Color : uint16_t {
    BLACK    = 0x0000,
    WHITE    = 0xFFFF,
    RED      = 0xF800,
    YELLOW   = 0xFFE0,
    DARKBLUE = 0x0011,
    GREEN    = 0x07E0,
};

// colorMode marker found in the image headers (value is not important to us;
// we always treat `Image` data as 4-bit indexed here).
enum { INDEX_GREEN = 3 };   // vert de fond des sprites firebuino = transparent

// An animated indexed image, constructed from a `const uint8_t[]` blob whose
// header is {width, height, frames, frameLoop, transparentIndex, colorMode, ...}.
struct Image {
    const uint8_t* buf;
    uint8_t        frame;
    Image() : buf(nullptr), frame(0) {}
    Image(const uint8_t* data) : buf(data), frame(0) {}
    Image(const uint16_t* data) : buf(reinterpret_cast<const uint8_t*>(data)), frame(0) {}  // sprites RGB565 (ex: LED), non dessines
    uint8_t width()  const { return buf ? buf[0] : 0; }
    uint8_t height() const { return buf ? buf[1] : 0; }
    uint8_t frames() const { return buf ? buf[2] : 1; }
};

class GbDisplay {
  public:
    int16_t  cursorX = 0, cursorY = 0;
    uint8_t  fontSize = 1;
    uint16_t colorFg = WHITE, colorBg = BLACK;

    void init(uint16_t, uint16_t, int = 0) {}
    void init(uint16_t, uint16_t, ColorMode) {}
    uint8_t width()  const { return 80; }
    uint8_t height() const { return 64; }
    uint8_t fontWidth  = 4;
    uint8_t fontHeight = 6;
    uint8_t getFontWidth()  const { return 4; }
    uint8_t getFontHeight() const { return 6; }
    void begin() {}

    void clear();
    void clear(uint16_t c);
    void fill(uint16_t c);
    void setColor(uint16_t fg)             { colorFg = fg; }
    void setColor(uint16_t fg, uint16_t bg){ colorFg = fg; colorBg = bg; }
    void fillRect(int x, int y, int w, int h);
    void drawRect(int x, int y, int w, int h);
    void drawImage(int x, int y, Image& img);      // draws current frame, then advances

    // text (deferred to present time; see .cpp)
    void print(const char* s);
    void print(long v);
    void println(const char* s);
    void println(long v);
};

extern char g_last_name[16];   // dernier nom saisi au clavier

class GbGui {
  public:
    // returns entered length; writes up to `len` chars into `dst` (uses our
    // on-screen 1..8 char entry).
    int keyboard(const char* title, char* dst, uint8_t len);
    int keyboard(const char* title, char* dst) { return keyboard(title, dst, 13); }
};

} // namespace gbc
