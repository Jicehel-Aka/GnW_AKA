/*
  ui/highscores.cpp — Meilleurs scores sur /sdcard/GnW_AKA/SCORES.DAT.
    - highscores_submit() : saisie du pseudo + tri + ecriture SD (bloquant)
    - highscores_show()   : rendu depuis le cache (auto-flush)
    - highscores_refresh(): recharge le cache depuis la SD
  Porte sur le nouveau core (graphics/input/i18n) : plus aucune dependance lib/.
*/
#include "highscores.h"
#include "core/graphics.h"
#include "core/input.h"
#include "core/i18n.h"
#include "core/sdcard.h"
#include <algorithm>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cinttypes>
#include <cerrno>
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gb_ll_common.h"   // EXPANDER_KEY_*
#include "game_module.h"    // GAME_LIST / GAME_COUNT
#include "gnw/gnw_scores.h"  // scores par jeu

#define HIGHSCORE_FILE "/sdcard/GnW_AKA/SCORES.DAT"
#define GnW_AKA_DIR    "/sdcard/GnW_AKA"

static std::vector<HighscoreEntry> s_cache;

static void ensure_dir() { sd_mkdir(GnW_AKA_DIR); }

void highscores_init() {
    ensure_dir();
    FILE* f = fopen(HIGHSCORE_FILE, "rb");
    if (!f) { f = fopen(HIGHSCORE_FILE, "wb"); if (f) fclose(f); }
    else    { fclose(f); }
    highscores_refresh();
}

void highscores_refresh() {
    s_cache.clear();
    FILE* f = fopen(HIGHSCORE_FILE, "rb");
    if (!f) return;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    size_t count = (size > 0) ? (size_t)(size / sizeof(HighscoreEntry)) : 0;
    s_cache.resize(count);
    if (count > 0) fread(s_cache.data(), sizeof(HighscoreEntry), count, f);
    fclose(f);
}

// --- Saisie du pseudo (front B pour valider) ---
static std::string input_name() {
    const std::string alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    std::string name;
    int idx = 0;

    for (;;) {
        Keys k;
        input_poll(k);

        if (k.pressed & EXPANDER_KEY_LEFT)  { if (--idx < 0) idx = (int)alpha.size()-1; }
        if (k.pressed & EXPANDER_KEY_RIGHT) { if (++idx >= (int)alpha.size()) idx = 0; }
        if ((k.pressed & EXPANDER_KEY_A) && (int)name.size() < 8) { name.push_back(alpha[idx]); idx = 0; }
        if ((k.pressed & EXPANDER_KEY_C) && !name.empty())        name.pop_back();
        if (k.pressed & EXPANDER_KEY_B)  break;   // front B = valider

        gfx_clear(COLOR_BLACK);
        gfx_text(40, 70, "Pseudo :", COLOR_YELLOW);
        std::string display = "  " + name + alpha[idx];
        gfx_text(40, 100, display.c_str(), COLOR_WHITE);
        gfx_text(40, 150, "A: lettre  C: effacer  B: ok", COLOR_WHITE);
        gfx_flush();
        vTaskDelay(pdMS_TO_TICKS(90));
    }
    return name;
}

void highscores_submit(int32_t score) {
    std::string name = input_name();
    if (name.empty()) name = "---";

    HighscoreEntry entry{};
    strncpy(entry.name, name.c_str(), sizeof(entry.name)-1);
    entry.score = score;
    s_cache.push_back(entry);

    std::sort(s_cache.begin(), s_cache.end(),
              [](const HighscoreEntry& a, const HighscoreEntry& b){ return a.score > b.score; });
    if ((int)s_cache.size() > MAX_SCORES) s_cache.resize(MAX_SCORES);

    ensure_dir();
    FILE* f = fopen(HIGHSCORE_FILE, "wb");
    if (!f) { printf("[HS] ERREUR fopen (errno=%d)\n", errno); return; }
    fwrite(s_cache.data(), sizeof(HighscoreEntry), s_cache.size(), f);
    fclose(f);
}

void highscores_show() {
    // Affiche le meilleur score de CHAQUE jeu, lu dans /GnW_AKA/<ID>.DAT
    gfx_clear(COLOR_BLACK);
    gfx_text_center(10, i18n::T(i18n::STR_HIGHSCORES), COLOR_YELLOW);
    int y = 44;
    for (int gi = 0; gi < GAME_COUNT; ++gi) {
        const GameModule* g = GAME_LIST[gi];
        if (!g || !g->save_id) continue;
        ScoreEntry top[GNW_TOP_N];
        scores_top(g->save_id, top);
        char buf[64];
        if (top[0].name[0])
            snprintf(buf, sizeof(buf), "%-14s %-8s %9lu",
                     g->name, top[0].name, (unsigned long)top[0].value);
        else
            snprintf(buf, sizeof(buf), "%-14s %s", g->name, "---");
        gfx_text(24, y, buf, COLOR_WHITE);
        y += 20;
    }
    gfx_text_center(212, i18n::T(i18n::STR_BACK), COLOR_YELLOW);
    gfx_flush();
}
