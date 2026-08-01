#include "select_screen.h"
#include "game_module.h"
#include "gb_graphics.h"
#include "gb_common.h"
#include "core/input.h"
#include "core/graphics.h"     // gfx_text_center
#include "gnw/gnw_l10n.h"
#include "ui/menu.h"
#include "info_screen.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern gb_graphics gfx;

// distinct placeholder hue per game (used when a module has no icon)
static uint16_t tile_color(int i) {
    static const uint8_t rgb[][3] = {
        {40,120,200}, {200,150,40}, {150,60,170}, {200,70,50},
        {60,170,110}, {170,90,140}
    };
    const uint8_t* c = rgb[i % 6];
    return gfx.makeColor(c[0], c[1], c[2]);
}

static void draw_tile(int cx, int cy, int w, int h, const GameModule* g,
                      int index, bool selected) {
    int x = cx - w / 2, y = cy - h / 2;
    uint16_t bg  = tile_color(index);
    uint16_t brd = selected ? gfx.makeColor(255, 220, 90) : gfx.makeColor(70, 80, 95);

    if (g->icon) {
        gfx.drawImageScaled(x, y, w, h, g->icon, g->icon_w, g->icon_h);
    } else {
        gfx.setColor(bg);            gfx.fillRoundRect(x, y, w, h, 8);
    }
    // border (double for the selected one)
    gfx.setColor(brd); gfx.drawRoundRect(x, y, w, h, 8);
    if (selected) gfx.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 9);
}

int select_screen_run() {
    static float scroll = 0.0f;      // eased fractional index
    int sel = 0;
    const int SPACING = 92;
    const int BASE    = 88;          // selected tile size
    const int CY      = 128;

    for (;;) {
        Keys k; input_poll(k);
        if (k.pressed & GB_KEY_RIGHT) sel = (sel + 1 < GAME_COUNT) ? sel + 1 : sel;
        if (k.pressed & GB_KEY_LEFT)  sel = (sel > 0) ? sel - 1 : sel;
        if (k.pressed & GB_KEY_A)     return sel;
        if (k.pressed & GB_KEY_B)     info_show(GAME_LIST[sel]);
        if (k.pressed & GB_KEY_MENU)  { menu_open(false); }   // language / volume / music (saved)

        scroll += (sel - scroll) * 0.25f;                 // smooth slide
        if (scroll < sel + 0.01f && scroll > sel - 0.01f) scroll = sel;

        // background
        gfx.clear(gfx.makeColor(16, 20, 28));
        gfx.setColor(gfx.makeColor(24, 30, 42));
        gfx.fillRect(0, 0, SCREEN_WIDTH, 44);             // header band
        gfx.fillRect(0, 206, SCREEN_WIDTH, 34);           // footer band

        // header
        gfx_text_center(12, "GAMEBUINO  AKA", gfx.makeColor(90, 200, 235));
        gfx_text_center(28, "GAME & WATCH REMAKES", gfx.makeColor(150, 160, 175));

        // tiles: draw from the edges inward so the centered one is on top
        for (int pass = 0; pass < 2; ++pass) {
            for (int i = 0; i < GAME_COUNT; ++i) {
                bool selectedTile = (i == sel);
                if ((pass == 0) == selectedTile) continue; // pass0: others, pass1: selected
                float d  = i - scroll;
                if (d < -2.4f || d > 2.4f) continue;       // offscreen
                int   cx = SCREEN_WIDTH / 2 + (int)(d * SPACING);
                float k2 = 1.0f - 0.22f * (d < 0 ? -d : d);
                if (k2 < 0.55f) k2 = 0.55f;
                int   sz = (int)(BASE * k2);
                draw_tile(cx, CY, sz, sz, GAME_LIST[i], i, selectedTile);
            }
        }

        // pagination dots
        {
            int dw = 10, total = (GAME_COUNT - 1) * dw, x0 = SCREEN_WIDTH/2 - total/2;
            for (int i = 0; i < GAME_COUNT; ++i) {
                bool on = (i == sel);
                gfx.setColor(on ? gfx.makeColor(255,220,90) : gfx.makeColor(70,80,95));
                gfx.fillCircle(x0 + i*dw, 182, on ? 3 : 2);
            }
        }
        // selected game name
        { const GameModule* gsel = GAME_LIST[sel];
          const char* nm = gsel->name_l10n ? L(gsel->name_l10n) : gsel->name;
          gfx_text_center(190, nm, gfx.makeColor(255, 220, 90)); }

        // arrows
        uint16_t arr = gfx.makeColor(120, 130, 145);
        gfx.setColor(arr);
        if (sel > 0)              gfx.fillTriangle(14, CY, 26, CY - 10, 26, CY + 10);
        if (sel < GAME_COUNT - 1) gfx.fillTriangle(306, CY, 294, CY - 10, 294, CY + 10);

        // footer hint
        gfx_text_center(216, "A JOUER   B INFOS   MENU OPTIONS   < >", gfx.makeColor(150, 160, 175));

        gfx.update();
        vTaskDelay(pdMS_TO_TICKS(30));
    }
}
