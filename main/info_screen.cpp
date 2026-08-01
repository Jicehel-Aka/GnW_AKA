#include <cstddef>
#include <cstring>
#include "info_screen.h"
#include "game_module.h"
#include "gb_graphics.h"
#include "gb_common.h"
#include "core/input.h"
#include "gnw/gnw_l10n.h"
#include "gnw/gnw_scores.h"
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern gb_graphics gfx;

// Localized section headings (FR/EN/DE/ES/IT; EN fallback for the rest).
static const char* const H_CTRL[LANG_COUNT]  = { "COMMANDES", "CONTROLS", "STEUERUNG", "CONTROLES", "COMANDI" };
static const char* const H_RULES[LANG_COUNT] = { "REGLES", "RULES", "REGELN", "REGLAS", "REGOLE" };
static const char* const H_SCORE[LANG_COUNT] = { "MEILLEURS SCORES", "HIGH SCORES", "BESTENLISTE", "MEJORES", "RECORD" };
static const char* const H_BACK[LANG_COUNT]  = { "B RETOUR", "B BACK", "B ZURUCK", "B VOLVER", "B INDIETRO" };

static int draw_multiline(int y, const char* text, uint16_t color) {
    gfx.setColor(color);
    char buf[256]; strncpy(buf, text ? text : "", sizeof buf - 1); buf[sizeof buf - 1] = 0;
    char* line = buf;
    while (line && *line) {
        char* nl = strchr(line, '\n');
        if (nl) *nl = 0;
        gfx.move_cursor(20, y); gfx.print_str(line);
        y += 12;
        line = nl ? nl + 1 : nullptr;
    }
    return y;
}

void info_show(const GameModule* g) {
    for (;;) {
        Keys k; input_poll(k);
        if ((k.pressed & GB_KEY_B) || (k.pressed & GB_KEY_MENU)) return;

        gfx.clear(gfx.makeColor(16, 20, 28));

        // title
        gfx.setColor(gfx.makeColor(255, 220, 90));
        gfx.move_cursor(20, 14);
        gfx.print_str(g->name_l10n ? L(g->name_l10n) : g->name);

        int y = 40;
        gfx.setColor(gfx.makeColor(90, 200, 235)); gfx.move_cursor(20, y); gfx.print_str(L(H_CTRL));
        y = draw_multiline(y + 14, g->controls_l10n ? L(g->controls_l10n) : "", gfx.makeColor(220, 225, 235)) + 6;

        gfx.setColor(gfx.makeColor(90, 200, 235)); gfx.move_cursor(20, y); gfx.print_str(L(H_RULES));
        y = draw_multiline(y + 14, g->rules_l10n ? L(g->rules_l10n) : "", gfx.makeColor(220, 225, 235)) + 6;

        // high scores
        gfx.setColor(gfx.makeColor(90, 200, 235)); gfx.move_cursor(20, y); gfx.print_str(L(H_SCORE));
        y += 14;
        if (g->save_id) {
            ScoreEntry top[GNW_TOP_N]; scores_top(g->save_id, top);
            gfx.setColor(gfx.makeColor(220, 225, 235));
            for (int i = 0; i < GNW_TOP_N; ++i) {
                char row[40];
                snprintf(row, sizeof row, "%d. %-8s %lu", i + 1,
                         top[i].name[0] ? top[i].name : "---", (unsigned long)top[i].value);
                gfx.move_cursor(24, y); gfx.print_str(row); y += 12;
            }
        }

        gfx.setColor(gfx.makeColor(150, 160, 175));
        gfx.move_cursor(20, 224); gfx.print_str(L(H_BACK));
        gfx.update();
        vTaskDelay(pdMS_TO_TICKS(30));
    }
}
