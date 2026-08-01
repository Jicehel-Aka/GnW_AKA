// fire_hd.cpp — Mode "AKA HD" de Fire! : rendu natif 320x240. Reprend l'esprit
// exact du moteur classique (fire/firebuino.inc) : plusieurs survivants
// independants en vol, chacun avec son etage de depart, son delai, et un
// rebond en 2 temps (check puis resolution) — adapte a nos 23 positions HD
// (au lieu des 20 du classique) et au joueur a 3 voies fixes.
#include "assets_fire_hd.h"
#include "game_module.h"
#include "gb_graphics.h"          // vrai type gb_graphics (SDK), pour gfx
#include "gnw/Gamebuino-Meta.h"   // Keys, GB_KEY_*
#include "esp_heap_caps.h"        // heap_caps_malloc (PSRAM)
#include "esp_random.h"           // esp_random() (delais/etages/difficulte)
#include "gnw/gnw_scores.h"       // scores_submit_named
#include "core/audio.h"           // g_sfx.play_tone (sons procedureaux)
#include <cstdint>
#include <cstdio>
#include <cstring>

extern gb_graphics gfx;

namespace fire_hd {

// --- positions exactes validees par Jicehel (espace natif 320x240) ---
struct Pt { int16_t x, y; };
static const Pt SURV_POS[FIREHD_SURV_COUNT] = {
    {22,91},{22,42},{37,69},{42,105},{40,140},{44,173},{70,143},
    {74,106},{75,73},{100,45},{112,76},{115,107},{120,142},
    {124,174},{150,138},{150,103},{168,70},{185,102},{188,139},
    {205,173},{219,143},{232,108},{254,135}
};
static const Pt RATE_POS[3] = { {36,206}, {114,206}, {189,206} };
static const int RATE_DW = 64, RATE_DH = 26;
static const Pt BRANC_POS[3] = { {30,181}, {107,181}, {183,181} };
static const Pt FUMEE_POS = {49,0};
static const Pt MISS_TXT_POS = {263,4};
static const Pt MISS_SYM_POS[3] = { {253,18}, {274,18}, {295,18} };

// Fenetre d'ecran du boitier (cadre bezel_fire.png) — ESTIMATION a confirmer.
static const int BEZEL_WIN_X = 102, BEZEL_WIN_Y = 84, BEZEL_WIN_W = 122, BEZEL_WIN_H = 82;

enum class DisplayMode { FULLSCREEN, CONSOLE };
static DisplayMode s_disp = DisplayMode::FULLSCREEN;

struct Rect { int x, y, w, h; };
static Rect map_rect(int x, int y, int w, int h) {
    if (s_disp == DisplayMode::FULLSCREEN) return { x, y, w, h };
    float sx = (float)BEZEL_WIN_W / 320.0f, sy = (float)BEZEL_WIN_H / 240.0f;
    float s = (sx < sy) ? sx : sy;
    int offx = BEZEL_WIN_X + (int)((BEZEL_WIN_W - 320 * s) / 2);
    int offy = BEZEL_WIN_Y + (int)((BEZEL_WIN_H - 240 * s) / 2);
    return { offx + (int)(x * s), offy + (int)(y * s), (int)(w * s + 0.5f), (int)(h * s + 0.5f) };
}
static void blit(int x, int y, const uint16_t* data, int sw, int sh, bool transparent) {
    Rect d = map_rect(x, y, sw, sh);
    if (transparent) gfx.drawImageScaled(d.x, d.y, d.w, d.h, data, sw, sh, FIREHD_TRANS);
    else              gfx.drawImageScaled(d.x, d.y, d.w, d.h, data, sw, sh);
}
static void blit_dst(int x, int y, int dw, int dh, const uint16_t* data, int sw, int sh) {
    Rect d = map_rect(x, y, dw, dh);
    gfx.drawImageScaled(d.x, d.y, d.w, d.h, data, sw, sh, FIREHD_TRANS);
}
static void filled_rect(int x, int y, int w, int h) {
    Rect d = map_rect(x, y, w, h);
    gfx.fillRect(d.x, d.y, d.w, d.h);
}
static uint32_t rnd(uint32_t n) { return n ? (esp_random() % n) : 0; }

// --- survivants : plusieurs en vol simultanement (comme le classique) ---
struct Survivor {
    bool active = false;
    int  floor = 0;     // etage de depart (0,1,2)
    int  step  = 0;      // index dans SURV_POS/SURV[] ; demarre a 'floor', pas toujours 0
    int  delay = 0;      // attente avant le premier mouvement
    bool bounced = false;
    bool dead  = false;
    int  dead_lane = -1; // voie ou il a echoue (pour choisir le bon sprite R + sa position)
    int  ko_timer = 0;   // duree d'affichage du KO avant disparition
};
static const int MAX_SURVIVORS = 6;
static Survivor s_surv[MAX_SURVIVORS];
static bool     s_occupied_window[3] = { false, false, false };
static int      s_no_survivors = 0;

// Check un pas avant chaque rebond (le joueur doit etre sur la bonne voie a CET instant),
// resolution au pas suivant (rebondi -> continue ; loupe -> mort). Nos positions 5/13/19
// sont geometriquement les points bas des 3 rebonds (mesure sur les coordonnees fournies).
static const int CHECK_STEP[3]   = { 1, 9, 15 };   // fenetre elargie (etait 4/12/18 = quasi instantane)
static const int RESOLVE_STEP[3] = { 5, 13, 19 };

static bool     s_classic = false;
static uint32_t s_tick = 0;
static uint32_t s_score = 0;
static int      s_lives = 3;
static int      s_lane = 1;
static bool     s_game_over = false;
static bool     s_gameover_pending = false;
static int      s_gameover_timer = 0;

static const uint16_t* const SURV[FIREHD_SURV_COUNT] = {
    FIREHD_SURV_0, FIREHD_SURV_1, FIREHD_SURV_2, FIREHD_SURV_3, FIREHD_SURV_4,
    FIREHD_SURV_5, FIREHD_SURV_6, FIREHD_SURV_7, FIREHD_SURV_8, FIREHD_SURV_9,
    FIREHD_SURV_10, FIREHD_SURV_11, FIREHD_SURV_12, FIREHD_SURV_13, FIREHD_SURV_14,
    FIREHD_SURV_15, FIREHD_SURV_16, FIREHD_SURV_17, FIREHD_SURV_18, FIREHD_SURV_19,
    FIREHD_SURV_20, FIREHD_SURV_21, FIREHD_SURV_22
};
static const uint16_t* const FUMEE[4] = { FIREHD_FUMEE_1, FIREHD_FUMEE_2, FIREHD_FUMEE_3, FIREHD_FUMEE_4 };

// Sequence de la fumee/flamme fournie par Jicehel (17 pas, combinant plusieurs
// frames simultanement) : bit0=frame1, bit1=frame2, bit2=frame3, bit3=frame4.
static const uint8_t FUMEE_SEQ[17] = {
    0b0001,             // 1
    0b0011,             // 1 et 2
    0b0111,             // 1, 2 et 3
    0b1110,             // 2, 3 et 4
    0b1100,             // 3 et 4
    0b1000,             // 4
    0b0000,             // rien
    0b0001,             // 1
    0b0010,             // 2
    0b0100,             // 3
    0b1000,             // 4
    0b0000,             // rien
    0b0001,             // 1
    0b0010,             // 2
    0b0100,             // 3
    0b1001,             // 1 et 4
    0b0000,             // rien
};
static const uint16_t* const RATE[3]  = { FIREHD_RATE_1, FIREHD_RATE_2, FIREHD_RATE_3 };   // sprite original (le nouveau etait pour MISS_SYM, pas ici)
static const uint16_t* const BRANC[3] = { FIREHD_BRANC_1, FIREHD_BRANC_2, FIREHD_BRANC_3 };

// --- score : police 7-segments procedurale ---
static void draw_digit(int ox, int oy, int L, int T, int d) {
    static const uint8_t M[10] = { 0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F };
    if (d < 0 || d > 9) return;
    uint8_t m = M[d];
    if (m & 1)  filled_rect(ox + T,     oy,               L, T);
    if (m & 2)  filled_rect(ox + T + L, oy + T,           T, L);
    if (m & 4)  filled_rect(ox + T + L, oy + 2*T + L,     T, L);
    if (m & 8)  filled_rect(ox + T,     oy + 2*T + 2*L,   L, T);
    if (m & 16) filled_rect(ox,         oy + 2*T + L,     T, L);
    if (m & 32) filled_rect(ox,         oy + T,           T, L);
    if (m & 64) filled_rect(ox + T,     oy + T + L,       L, T);
}
static void draw_score(uint32_t value) {
    char buf[10]; snprintf(buf, sizeof buf, "%04lu", (unsigned long)value);
    const int L = 6, T = 2;
    const int adv = (2*T + L) + 3;
    int x = 185, y = 4;
    gfx.setColor(gfx.makeColor(30, 30, 30));
    for (const char* p = buf; *p; ++p) { draw_digit(x, y, L, T, *p - '0'); x += adv; }
}

// Fonds indexes (8 bits/pixel + palette), decodes en PSRAM une fois (comme F1).
static uint16_t* s_bg_buf = nullptr;
static bool      s_bg_decoded_classic = false;
static bool      s_bg_valid = false;
static void decode_background(bool classic) {
    if (!s_bg_buf)
        s_bg_buf = (uint16_t*)heap_caps_malloc(320 * 240 * sizeof(uint16_t), MALLOC_CAP_SPIRAM);
    if (!s_bg_buf) return;
    const uint8_t*  idx = classic ? FIREHD_BG_CLASSIC_IDX : FIREHD_BG_NEW_IDX;
    const uint16_t* pal = classic ? FIREHD_BG_CLASSIC_PAL : FIREHD_BG_NEW_PAL;
    for (int i = 0; i < 320 * 240; ++i) s_bg_buf[i] = pal[idx[i]];
    s_bg_decoded_classic = classic;
    s_bg_valid = true;
}

// --- cycle de vie d'un survivant (fidele a spawnSurvivor/moveSurvivors/checkBounces) ---
static int s_spawn_timer = 10;   // premier envol assez tot

static int spawn_wait_ticks() {
    // court quand l'ecran est vide (pas de temps mort), puis de plus en plus long a
    // mesure que des survivants sont deja engages (evite la surcharge/le chaos).
    int base = 8 + s_no_survivors * 7 - (int)(s_score / 150);
    if (base < 5) base = 5;                    // plancher : jamais instantane non plus
    return base + (int)rnd(base / 2 + 1);
}

static void try_spawn() {
    if (--s_spawn_timer > 0) return;

    for (int i = 0; i < MAX_SURVIVORS; ++i) {
        if (s_surv[i].active) continue;
        int floor_, delay_;
        if (s_score < 300)      { floor_ = 0;               delay_ = 1 + (int)rnd(5); }
        else if (s_score < 600) { floor_ = (int)rnd(2);      delay_ = 2 + (int)rnd(4); }
        else                     { floor_ = (int)rnd(3);      delay_ = 3 + (int)rnd(3); }
        while (s_occupied_window[floor_]) {                 // evite 2 sauts a la meme fenetre
            if (floor_ == 0) { floor_ = -1; break; }
            --floor_;
        }
        if (floor_ < 0) break;   // tout est occupe : on retentera plus tard
        s_surv[i] = Survivor{};
        s_surv[i].active = true; s_surv[i].floor = floor_; s_surv[i].step = floor_; s_surv[i].delay = delay_;
        s_occupied_window[floor_] = true;
        ++s_no_survivors;
        break;
    }
    s_spawn_timer = spawn_wait_ticks();   // depend du nombre de survivants desormais engages
}

static void logic_tick() {
    try_spawn();
    for (int i = 0; i < MAX_SURVIVORS; ++i) {
        Survivor& s = s_surv[i];
        if (!s.active) continue;

        if (s.dead) {
            if (--s.ko_timer <= 0) { s.active = false; --s_no_survivors; }
            continue;
        }

        if (s.delay > 0) { --s.delay; continue; }
        ++s.step;

        // liberation de la fenetre de depart (evite d'y faire sauter quelqu'un d'autre trop tot)
        if ((s.floor == 0 && s.step == 1) || (s.floor == 1 && s.step == 2) || (s.floor == 2 && s.step == 3))
            s_occupied_window[s.floor] = false;

        // check : le joueur doit etre sur la bonne voie A UN MOMENT DANS LA FENETRE (elargie :
        // avant, c'etait un seul tick, quasi injouable ; maintenant une petite marge de reaction)
        for (int lane = 0; lane < 3; ++lane)
            if (s.step >= CHECK_STEP[lane] && s.step < RESOLVE_STEP[lane] && s_lane == lane) s.bounced = true;

        // resolution, un pas plus tard
        for (int lane = 0; lane < 3; ++lane) {
            if (s.step != RESOLVE_STEP[lane]) continue;
            if (s.bounced) {
                s.bounced = false;
                s_score += 5;
                g_sfx.play_tone(1200.0f, 60, 0.4f);
            } else {
                s.dead = true; s.dead_lane = lane; s.ko_timer = 25;
                g_sfx.play_tone(180.0f, 150, 0.5f);
                if (s_lives > 0) --s_lives;
                if (s_lives == 0) { s_gameover_pending = true; s_gameover_timer = 45; }
            }
        }

        if (!s.dead && s.step >= FIREHD_SURV_COUNT) {   // livre a l'ambulance
            s_score += 20;
            s.active = false; --s_no_survivors;
        }
    }
}

void enter() {
    s_tick = 0; s_bg_valid = false;
    s_score = 0; s_lives = 3; s_lane = 1; s_game_over = false;
    s_gameover_pending = false; s_gameover_timer = 0;
    s_spawn_timer = 10;
    for (auto& s : s_surv) s = Survivor{};
    s_occupied_window[0] = s_occupied_window[1] = s_occupied_window[2] = false;
    s_no_survivors = 0;
}

static void do_game_over() {
    g_sfx.play_tone(220.0f, 400, 0.5f);
    s_game_over = true;
    scores_submit_named("FIRE", s_score);   // meme id que le module Fire! (sinon absent du tableau du menu)
}

void update(const Keys& k) {
    ++s_tick;
    if (k.pressed & GB_KEY_A) s_classic = !s_classic;
    if (k.pressed & GB_KEY_B)
        s_disp = (s_disp == DisplayMode::FULLSCREEN) ? DisplayMode::CONSOLE : DisplayMode::FULLSCREEN;

    if (s_game_over) {
        if (k.pressed & GB_KEY_A) enter();
        return;
    }
    if (k.pressed & GB_KEY_LEFT)  { if (s_lane > 0) --s_lane; }
    if (k.pressed & GB_KEY_RIGHT) { if (s_lane < 2) ++s_lane; }

    if (s_gameover_pending) {
        if (--s_gameover_timer <= 0) do_game_over();
        return;
    }

    if ((s_tick % 6) == 0) logic_tick();   // encore un peu ralenti (etait %5)
}

void render() {
    if (s_disp == DisplayMode::CONSOLE)
        gfx.drawImage(0, 0, FIREHD_BEZEL, FIREHD_BEZEL_W, FIREHD_BEZEL_H);

    if (!s_bg_valid || s_bg_decoded_classic != s_classic) decode_background(s_classic);
    if (s_bg_valid) blit(0, 0, s_bg_buf, 320, 240, false);

    int fstep = (s_tick / 4) % 17;
    uint8_t fmask = FUMEE_SEQ[fstep];
    for (int b = 0; b < 4; ++b)
        if (fmask & (1 << b))
            blit(FUMEE_POS.x, FUMEE_POS.y, FUMEE[b], FIREHD_FUMEE_1_W, FIREHD_FUMEE_1_H, true);

    for (int i = 0; i < MAX_SURVIVORS; ++i) {
        const Survivor& s = s_surv[i];
        if (!s.active) continue;
        if (s.dead) {
            int lane = s.dead_lane < 0 ? 0 : s.dead_lane;
            blit_dst(RATE_POS[lane].x, RATE_POS[lane].y, RATE_DW, RATE_DH, RATE[lane], FIREHD_RATE_1_W, FIREHD_RATE_1_H);
        } else if (s.delay == 0) {
            int st = s.step; if (st < 0) st = 0; if (st >= FIREHD_SURV_COUNT) st = FIREHD_SURV_COUNT - 1;
            blit_dst(SURV_POS[st].x, SURV_POS[st].y, 32, 32, SURV[st], 16, 16);
        }
    }

    blit(MISS_TXT_POS.x, MISS_TXT_POS.y, FIREHD_MISS_TXT, FIREHD_MISS_TXT_W, FIREHD_MISS_TXT_H, true);
    for (int i = 0; i < s_lives; ++i)
        blit(MISS_SYM_POS[i].x, MISS_SYM_POS[i].y, FIREHD_MISS_SYM2, FIREHD_MISS_SYM2_W, FIREHD_MISS_SYM2_H, true);

    blit(BRANC_POS[s_lane].x, BRANC_POS[s_lane].y, BRANC[s_lane],
         s_lane==2 ? FIREHD_BRANC_3_W : FIREHD_BRANC_1_W, FIREHD_BRANC_1_H, true);

    draw_score(s_score);

    if (s_game_over) {
        const int BOX_X = 60, BOX_Y = 100, BOX_W = 200, BOX_H = 50, CHAR_W = 8;
        gfx.setColor(gfx.makeColor(0, 0, 0));
        filled_rect(BOX_X, BOX_Y, BOX_W, BOX_H);
        const char* l1 = "GAME OVER";
        const char* l2 = "A : rejouer";
        int l1x = BOX_X + (BOX_W - (int)strlen(l1) * CHAR_W) / 2;
        int l2x = BOX_X + (BOX_W - (int)strlen(l2) * CHAR_W) / 2;
        gfx.setColor(gfx.makeColor(255, 60, 60));
        Rect p1 = map_rect(l1x, BOX_Y + 12, 0, 0); gfx.move_cursor(p1.x, p1.y); gfx.print_str(l1);
        gfx.setColor(gfx.makeColor(255, 255, 255));
        Rect p2 = map_rect(l2x, BOX_Y + 30, 0, 0); gfx.move_cursor(p2.x, p2.y); gfx.print_str(l2);
    }

    gfx.update();
}

} // namespace fire_hd
