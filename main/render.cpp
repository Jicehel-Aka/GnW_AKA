#include "render.h"
#include <cstdio>
#include "gb_graphics.h"
#include "gb_common.h"
#include "Global.h"        // game state, firstSpriteColumn, sprite indices
#include "assets_hd.h"     // positioned BGR565 frames from a27ds/Parachute

extern gb_graphics gfx;    // defined in app_main.cpp

// Each HD frame already carries its screen position; blit as-is (color key).
static inline void draw(const HdImage& im) {
    gfx.drawImage(im.x, im.y, im.data, im.w, im.h, HD_TRANS);
}
static inline void draw_bg() {   // full background, opaque
    gfx.drawImage(img_parachute_background.x, img_parachute_background.y,
                  img_parachute_background.data,
                  img_parachute_background.w, img_parachute_background.h);
}

// Parachutist logical index (0..17, cf. firstSpriteColumn) -> HD frame.
static const HdImage& hd_para(int p) {
    if (p <= 6)  return *hd_paraA[p];
    if (p <= 12) return *hd_paraB[p - 7];
    return              *hd_paraC[p - 13];
}

// --- Police digitale 7 segments (dessinee, aucune dependance de fichier) ---
//   aaa
//  f   b
//   ggg
//  e   c
//   ddd
static void draw_digit(int ox, int oy, int L, int T, int d) {
    // bits : a=1 b=2 c=4 d=8 e=16 f=32 g=64
    static const uint8_t M[10] = { 0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F };
    if (d < 0 || d > 9) return;
    uint8_t m = M[d];
    if (m & 1)  gfx.fillRect(ox + T,     oy,               L, T);  // a
    if (m & 2)  gfx.fillRect(ox + T + L, oy + T,           T, L);  // b
    if (m & 4)  gfx.fillRect(ox + T + L, oy + 2*T + L,     T, L);  // c
    if (m & 8)  gfx.fillRect(ox + T,     oy + 2*T + 2*L,   L, T);  // d
    if (m & 16) gfx.fillRect(ox,         oy + 2*T + L,     T, L);  // e
    if (m & 32) gfx.fillRect(ox,         oy + T,           T, L);  // f
    if (m & 64) gfx.fillRect(ox + T,     oy + T + L,       L, T);  // g
}

static void draw_score(uint16_t value) {
    char buf[8];
    snprintf(buf, sizeof buf, "%u", value);
    const int L = 4, T = 1;                 // compact (hauteur ~11px, comme l'horloge)
    const int adv = (2*T + L) + 2;          // largeur d'un chiffre + espacement
    int x = 91, y = 78;                     // emplacement de l'horloge (sur l'ecran de jeu)
    gfx.setColor(gfx.makeColor(20, 20, 20));
    for (const char* p = buf; *p; ++p) { draw_digit(x, y, L, T, *p - '0'); x += adv; }
}

void render_home() {
    draw_bg();
    draw(img_Clock);     // 12:00
    draw(img_GameA);
    gfx.update();
}

void render_run() {
    draw_bg();
    draw(img_Helicopter);

    // (1) pale d'helicoptere animee (procedurale : aucun asset de pale)
    {
        const int cx = img_Helicopter.x + img_Helicopter.w / 2;
        const int ry = img_Helicopter.y;        // haut du fuselage (ajuster si besoin)
        gfx.setColor(gfx.makeColor(35, 35, 35));
        if (helicopterAnimation & 1) gfx.fillRect(cx - 6,  ry, 12, 2);   // vue de profil
        else                         gfx.fillRect(cx - 18, ry, 36, 2);   // pale etendue
    }

    // (3) marqueurs "miss" (requins) CUMULATIFS : on dessine tous les slots atteints
    for (int m = 0; m < misses && m < 3; ++m) draw(*hd_life[m]);
    draw(img_Miss);                             // label "MISS" (taille native)

    for (uint8_t c = 0; c < parachuteLaunchCount; ++c)
        draw(hd_para(parachutes[c]));

    if (floodedAnimation > -1 && floodedAnimation < 6) {   // drown sequence (tunable)
        if (floodedAnimation < 3) draw(*hd_water[floodedAnimation]);
        else                      draw(*hd_dead[floodedAnimation - 3]);
    }
    if (sharkAnimation > -1 && floodedAnimation < 6)
        draw(*hd_shark[sharkAnimation]);

    draw(*hd_boat[boat.spriteIndex]);
    draw_score(score);
    gfx.update();
}

void render_pause() {
    draw_bg();
    draw(*hd_boat[boat.spriteIndex]);
    gfx.setColor(gfx.makeColor(30, 30, 30));
    gfx.move_cursor(135, 110);
    gfx.print_str("PAUSE");
    gfx.update();
}

void render_gameover() {
    draw_bg();
    gfx.setColor(gfx.makeColor(30, 30, 30));
    gfx.move_cursor(120, 110);
    gfx.print_str("GAME OVER");
    gfx.update();
}
