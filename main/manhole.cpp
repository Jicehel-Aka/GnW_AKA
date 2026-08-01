// Manhole — module AKA (premier jet, positions/sprites à ajuster au build).
// Art : sprites IA originaux + décor briques (fournis). Aucun rip Nintendo.
// Mécanique simplifiée façon G&W : des piétons approchent de 4 trous ; on glisse
// la plaque sous le bon trou pour qu'ils passent ; sinon ils tombent (une vie).
#include "game_module.h"
#include "gb_graphics.h"
#include "gb_common.h"
#include "assets_manhole.h"
#include "gnw/gnw_scores.h"
#include "gametexts.h"
#include <stdio.h>

extern gb_graphics gfx;

// 4 trous (approx sur le décor briques — À AJUSTER une fois vu à l'écran)
struct P { int x, y; };
static const P GAP[4] = { {70,116}, {210,116}, {70,198}, {210,198} };
static const P START[4] = { {6,116}, {314,116}, {6,198}, {314,198} };  // arrivée piéton
static const int STEPS = 18;

struct Ped { bool on; int gap, step, tick; bool fell; };
static Ped ped[4];
static int  score, lives, spawnTick, speed;
static bool sideL = true, levelU = true;

enum St { HOME, RUN, OVER };
static St st;

static int plateGap() { return (sideL ? 0 : 1) + (levelU ? 0 : 2); }

static void reset() {
    score = 0; lives = 3; speed = 6; spawnTick = 26; sideL = levelU = true;
    for (auto& p : ped) { p.on = false; p.step = p.tick = 0; p.fell = false; }
}
static void mh_enter() { st = HOME; }

static void mh_update(const Keys& k) {
    if (st == HOME) { if (k.pressed & GB_KEY_A) { reset(); st = RUN; } return; }
    if (st == OVER) { if (k.pressed & GB_KEY_A) { scores_submit_named("MANHOLE",(uint32_t)score); st = HOME; } return; }

    if (k.pressed & GB_KEY_LEFT)  sideL  = true;
    if (k.pressed & GB_KEY_RIGHT) sideL  = false;
    if (k.pressed & GB_KEY_UP)    levelU = true;
    if (k.pressed & GB_KEY_DOWN)  levelU = false;

    if (--spawnTick <= 0) {
        int free = -1; for (int i = 0; i < 4; ++i) if (!ped[i].on) { free = i; break; }
        if (free >= 0) { ped[free] = { true, free, 0, speed, false }; }
        spawnTick = 22 - score / 40; if (spawnTick < 8) spawnTick = 8;
    }
    for (int i = 0; i < 4; ++i) {
        Ped& p = ped[i];
        if (!p.on) continue;
        if (--p.tick > 0) continue;
        p.tick = speed;
        if (++p.step >= STEPS) {
            p.on = false;
            if (plateGap() == p.gap) { score++; if (speed > 2 && score % 10 == 0) speed--; }
            else { if (--lives <= 0) st = OVER; }
        }
    }
}

static void mh_render() {
    if (st == HOME) {
        gfx.clear(gfx.makeColor(210,210,210));
        gfx.setColor(gfx.makeColor(30,30,30));
        gfx.move_cursor(120,100); gfx.print_str("MANHOLE");
        gfx.move_cursor(112,130); gfx.print_str("A : JOUER");
        gfx.update(); return;
    }
    gfx.drawImage(0, 0, MH_BG, 320, 240);                 // décor briques

    for (int i = 0; i < 4; ++i) {                          // piétons qui approchent
        Ped& p = ped[i]; if (!p.on) continue;
        int x = START[i].x + (GAP[i].x - START[i].x) * p.step / STEPS;
        int y = START[i].y;
        const MhImg& s = ped_1;                            // pose de marche (approx)
        gfx.drawImage(x - s.w/8, y - s.h/4, s.d, s.w, s.h, MH_KEY);
    }
    // plaque au trou choisi
#if MH_HAS_PLATE
    const MhImg& pl = plate;
    gfx.drawImage(GAP[plateGap()].x - pl.w/6, GAP[plateGap()].y, pl.d, pl.w, pl.h, MH_KEY);
#endif
    gfx.setColor(gfx.makeColor(20,20,20));
    char b[16]; snprintf(b,sizeof b,"%04d",score); gfx.move_cursor(140,6); gfx.print_str(b);
    for (int i = 0; i < lives; ++i) { gfx.setColor(gfx.makeColor(180,30,30)); gfx.fillRect(6+i*12,6,8,8); }
    if (st == OVER) { gfx.setColor(gfx.makeColor(20,20,20)); gfx.move_cursor(120,120); gfx.print_str("GAME OVER"); }
    gfx.update();
}

extern const GameModule manhole_module = {
    "Manhole", mh_enter, mh_update, mh_render,
    nullptr, 0, 0, "MANHOLE", MANHOLE_NAME, MANHOLE_RULES, MANHOLE_CTRL
};
