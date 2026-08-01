#pragma once
// Minimal Gamebuino-Meta compatibility shim for the AKA.
// Provides the SUBSET of the META API used by the G&W game engines
// (buttons, frameCount, sound.fx, save) so their logic compiles unchanged.
// Rendering is NOT provided here (each engine's Renderer transfer layer is
// swapped for gnw_present). Extend this shim if a build reports a missing symbol.
#include <stdint.h>
#include "core/input.h"     // Keys, GB_KEY_* (via gb_common)
#include "gb_common.h"
#include "gb_display.h"
#include <setjmp.h>
#include <cstdint>
#include <cstdlib>   // rand
#include <cstdio>    // fopen (blob save)
#include <sys/stat.h>// mkdir
#include <cstring>   // strlen
typedef uint8_t byte;
typedef bool    boolean;

// String minimal (le moteur DKJr affiche le score via String(value))
struct String {
    char b[12];
    String()        { b[0] = 0; }
    String(long v)  { snprintf(b, sizeof b, "%ld", v); }
    uint8_t     length() const   { return (uint8_t)strlen(b); }
    char        charAt(int i) const { return b[i]; }
    const char* c_str() const    { return b; }
};

#ifndef F
#define F(x) (x)                       // Arduino flash-string macro -> no-op
#endif
inline long random(long hi)          { return hi > 0 ? (long)(rand() % hi) : 0; }
inline long random(long lo, long hi) { return hi > lo ? lo + (long)(rand() % (hi - lo)) : lo; }

struct SaveDefault { int index; int type; uint32_t size; int32_t defaultValue; };
enum { SAVETYPE_INT = 0, SAVETYPE_BLOB = 1 };

enum Button : uint8_t { BUTTON_LEFT, BUTTON_RIGHT, BUTTON_UP, BUTTON_DOWN,
                        BUTTON_A, BUTTON_B, BUTTON_MENU, BUTTON_HOME };

namespace Gamebuino_Meta {
    enum class Sound_FX_Wave : uint8_t { NOISE = 0, SQUARE = 1 };
    struct Sound_FX { Sound_FX_Wave type; int16_t a, b, c, d, e, f; };
}

struct GbButtons {
    uint16_t st = 0, prev = 0;
    void set(uint16_t bits) { prev = st; st = bits; }
    bool held(Button b) const;
    bool pressed(Button b) const;   // rising edge
    bool repeat(Button b, uint8_t) const { return pressed(b); }
};
struct GbSound {
    void fx(const Gamebuino_Meta::Sound_FX* fx);
    void playTick(); void playOK(); void playCancel();
    void play(const char* /*wav*/);   // routed to a tone (no SD dependency)
};
struct GbLights { void fill(uint16_t = 0) {} void clear() {}
                 void drawImage(int, int, gbc::Image&) {} };  // no RGB LEDs on AKA
struct GbSave  {
    int32_t get(int index); void set(int index, int32_t v);        // slots int32 (ex: DKJr)
    bool config(const SaveDefault*) { return true; }
    bool config(const void*)        { return true; }
    template<class T> void get(int index, T& blob) {               // blobs (ex: highscores firebuino)
        char p[96]; snprintf(p, sizeof p, MOUNT_POINT "/GnW_AKA/SAVE_%d.BIN", index);
        FILE* f = fopen(p, "rb"); if (f) { size_t r=fread(&blob,1,sizeof(T),f); (void)r; fclose(f); }
    }
    template<class T> void set(int index, const T& blob) {
        mkdir(MOUNT_POINT "/GnW_AKA", 0777);
        char p[96]; snprintf(p, sizeof p, MOUNT_POINT "/GnW_AKA/SAVE_%d.BIN", index);
        FILE* f = fopen(p, "wb"); if (f) { fwrite(&blob,1,sizeof(T),f); fclose(f); }
    }
};

struct Gb {
    uint32_t  frameCount = 0;
    GbButtons buttons;
    GbSound   sound;
    GbLights  lights;
    gbc::GbDisplay display;
    gbc::GbGui     gui;
    void begin() {}
    bool update();                 // frame gate + input + present (loop-owning games)
    void pickRandomSeed();
    void getDefaultName(char* dst) { dst[0]='A'; dst[1]='A'; dst[2]='A'; dst[3]=0; }
    GbSave    save;
};
extern Gb gb;

// Called once per frame by the shell before the engine's loop().
void gnw_gb_update(const Keys& k);

// firebuino uses these unqualified:
using gbc::Image;
using gbc::Color;
static const gbc::Color WHITE=gbc::WHITE, BLACK=gbc::BLACK, RED=gbc::RED,
                      YELLOW=gbc::YELLOW, DARKBLUE=gbc::DARKBLUE, GREEN=gbc::GREEN;
enum { INDEX_GREEN = gbc::INDEX_GREEN };

// loop-ownership bridge (setjmp): gb.update() longjmps here to exit to selector.
extern jmp_buf     gbc_exit_ctx;
extern volatile bool gbc_want_exit;
