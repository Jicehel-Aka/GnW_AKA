// Nu, pogodi! — module AKA.
// Art : pawel-winiecki/nu-pogodi-html5-game (SVG), (c) 2014 Pawel Winiecki, MIT.
// Mecanique facon Game & Watch : le loup rattrape dans son panier les oeufs qui
// devalent 4 goulottes ; on place le panier a l'un des 4 coins.
#include "game_module.h"
#include "gb_graphics.h"
#include "gb_common.h"
#include "assets_nupo.h"
#include "gnw/gnw_scores.h"
#include "gametexts.h"
#include <stdio.h>

extern gb_graphics gfx;

// lanes: 0=UL 1=DL 2=UR 3=DR
struct Pt { int x, y; };
static const Pt TOP[4]   = { {8,18}, {8,224}, {312,18}, {312,224} };
static const Pt CATCH[4] = { {112,92}, {112,150}, {196,92}, {196,150} };
static const int STEPS = 20;

struct Egg { bool on; int step, tick; };
static Egg   egg[4];
static int   score, lives, spawnTick, speed;
static bool  sideL = true, levelU = true;

enum St { HOME, RUN, OVER };
static St st;

static void reset() {
    score = 0; lives = 3; speed = 6; spawnTick = 30;
    sideL = true; levelU = true;
    for (auto& e : egg) { e.on = false; e.step = e.tick = 0; }
}

static int wolfLane() { return (sideL ? 0 : 2) + (levelU ? 0 : 1); }
static const NupoImg& wolfImg() {
    if (sideL) return levelU ? wolf_ul : wolf_dl;
    return              levelU ? wolf_ur : wolf_dr;
}

static void nupo_enter() { st = HOME; }

static void nupo_update(const Keys& k) {
    if (st == HOME) { if (k.pressed & GB_KEY_A) { reset(); st = RUN; } return; }
    if (st == OVER) { if (k.pressed & GB_KEY_A) { scores_submit_named("NUPO", (uint32_t)score); st = HOME; } return; }

    if (k.pressed & GB_KEY_LEFT)  sideL  = true;
    if (k.pressed & GB_KEY_RIGHT) sideL  = false;
    if (k.pressed & GB_KEY_UP)    levelU = true;
    if (k.pressed & GB_KEY_DOWN)  levelU = false;

    // spawn
    if (--spawnTick <= 0) {
        int free = -1;
        for (int i = 0; i < 4; ++i) if (!egg[i].on) { free = i; break; }
        if (free >= 0) { egg[free].on = true; egg[free].step = 0; egg[free].tick = speed; }
        spawnTick = 24 - score / 40; if (spawnTick < 8) spawnTick = 8;
    }
    // advance eggs
    for (int i = 0; i < 4; ++i) {
        if (!egg[i].on) continue;
        if (--egg[i].tick > 0) continue;
        egg[i].tick = speed;
        if (++egg[i].step >= STEPS) {                 // reached the catch point
            egg[i].on = false;
            if (wolfLane() == i) { score++; if (speed > 2 && score % 10 == 0) speed--; }
            else { if (--lives <= 0) st = OVER; }
        }
    }
}

static inline void chute(int i) {
    gfx.setColor(gfx.makeColor(150, 130, 80));
    gfx.drawLine(TOP[i].x, TOP[i].y, CATCH[i].x, CATCH[i].y);
    gfx.drawLine(TOP[i].x, TOP[i].y + 1, CATCH[i].x, CATCH[i].y + 1);
}

static void nupo_render() {
    gfx.clear(gfx.makeColor(150, 170, 140));          // LCD green
    if (st == HOME) {
        gfx.setColor(gfx.makeColor(30, 40, 30));
        gfx.move_cursor(96, 100); gfx.print_str("NU, POGODI!");
        gfx.move_cursor(108, 130); gfx.print_str("A : JOUER");
        gfx.update(); return;
    }
    for (int i = 0; i < 4; ++i) chute(i);
    // eggs
    for (int i = 0; i < 4; ++i) {
        if (!egg[i].on) continue;
        int t = egg[i].step;
        int x = TOP[i].x + (CATCH[i].x - TOP[i].x) * t / STEPS;
        int y = TOP[i].y + (CATCH[i].y - TOP[i].y) * t / STEPS;
        int fr = 1 + t * 4 / STEPS; if (fr > 5) fr = 5;
        char nm[16];
        const NupoImg* e;
        // pick egg frame sprite
        const NupoImg* L[5] = { &egg_left_1,&egg_left_2,&egg_left_3,&egg_left_4,&egg_left_5 };
        const NupoImg* R[5] = { &egg_right_1,&egg_right_2,&egg_right_3,&egg_right_4,&egg_right_5 };
        e = (i < 2) ? L[fr-1] : R[fr-1];
        gfx.drawImage(x - e->w/2, y - e->h/2, e->d, e->w, e->h, NUPO_KEY);
        (void)nm;
    }
    // wolf
    const NupoImg& w = wolfImg();
    gfx.drawImage(154 - w.w/2, 118 - w.h/2, w.d, w.w, w.h, NUPO_KEY);
    // HUD
    gfx.setColor(gfx.makeColor(30, 40, 30));
    char b[16]; snprintf(b, sizeof b, "%04d", score);
    gfx.move_cursor(140, 8); gfx.print_str(b);
    for (int i = 0; i < lives; ++i)
        gfx.drawImage(6 + i * 18, 6, life.d, life.w, life.h, NUPO_KEY);
    if (st == OVER) { gfx.move_cursor(110, 210); gfx.print_str("GAME OVER"); }
    gfx.update();
}

extern const uint16_t NUPO_ICON[];
extern const GameModule nupogodi_module = {
    "Nu, pogodi!", nupo_enter, nupo_update, nupo_render,
    NUPO_ICON, 96, 96, "NUPO", NUPO_NAME, NUPO_RULES, NUPO_CTRL
};
