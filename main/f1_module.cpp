// Formula 1 (joyrider3774) — module AKA. Coeur portable (f1/src) inchange ;
// seuls present / boutons / son / savestate sont remplaces.
#include "game_module.h"
#include "gb_graphics.h"
#include "gb_common.h"        // framebuffer[], GB_KEY_*
#include "gametexts.h"
#include "gnw/gnw_scores.h"
#include "icons.h"
#include "esp_heap_caps.h"   // heap_caps_malloc (PSRAM)
#include "commonvars.h"       // fb, buttons, prevbuttons, gameState, Score, masks, gs*
#include "framebuffer.h"
#include "game.h"
#include "intro.h"
#include "sound.h"
#include "savestate.h"

extern gb_graphics gfx;

static uint16_t* f1_buf = nullptr;   // 320x240 alloue en PSRAM (hors DRAM interne)
static inline uint16_t swap_rb(uint16_t c) { return (c >> 11) | (c & 0x07E0) | ((c & 0x1F) << 11); }

static void f1_present() {
    for (int i = 0; i < 320 * 240; ++i) {
        uint16_t rgb = __builtin_bswap16(f1_buf[i]);   // fb is RGB565 big-endian
        framebuffer[i] = swap_rb(rgb);                 // -> AKA native BGR565
    }
    gfx.update();
}

static void f1_enter() {
    if (!f1_buf)
        f1_buf = (uint16_t*)heap_caps_malloc(320 * 240 * sizeof(uint16_t), MALLOC_CAP_SPIRAM);
    fb.buffer = f1_buf; fb.width = 320; fb.height = 240;
    fb.littleEndian = 0; fb.bgr = 0;
    initSound(); loadSaveState();
    gameState = gsInitIntro; buttons = prevbuttons = 0;
}

static void f1_update(const Keys& k) {
    static uint8_t prevState = 255;
    prevbuttons = buttons;
    uint8_t b = 0;
    if (k.raw & GB_KEY_A)    b |= BUTTON_A_MASK;
    if (k.raw & GB_KEY_B)    b |= BUTTON_B_MASK;
    if (k.raw & GB_KEY_MENU) b |= BUTTON_C_MASK;
    if (k.raw & GB_KEY_UP)   b |= BUTTON_UP_MASK;
    if (k.raw & GB_KEY_DOWN) b |= BUTTON_DOWN_MASK;
    if (k.raw & GB_KEY_LEFT)  b |= BUTTON_A_MASK;   // gauche = MoveLeft
    if (k.raw & GB_KEY_RIGHT) b |= BUTTON_C_MASK;   // droite = MoveRight
    buttons = b;

    switch (gameState) {
        case gsInitIntro:     case gsIntro:     intro();     break;
        case gsInitGame:      case gsGame:      game();      break;
        case gsInitGameIntro: case gsGameIntro: gameIntro(); break;
        case gsInitGameOver:  case gsGameOver:  gameOver();  break;
        default: break;
    }
    // on entering game over, offer a named high-score entry
    if ((gameState == gsGameOver || gameState == gsInitGameOver) &&
        prevState != gsGameOver && prevState != gsInitGameOver) {
        scores_submit_named("F1", (uint32_t)Score);
        extern void f1HiScoreReload(void); f1HiScoreReload();  // rafraichir le hi-score en cache
    }
    prevState = gameState;
}

static void f1_render() { f1_present(); }

extern const GameModule f1_module = { "Formula 1", f1_enter, f1_update, f1_render,
                               F1_ICON, ICON_W, ICON_H, "F1", F1_NAME, F1_RULES, F1_CTRL };
