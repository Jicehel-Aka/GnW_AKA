#include <cstddef>
#include <cstring>
#include "gnw_scores.h"
#include "gb_common.h"
#include "gb_graphics.h"
#include "core/input.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern gb_graphics gfx;

static void path_for(const char* id, char* out, size_t n) {
    snprintf(out, n, MOUNT_POINT "/GnW_AKA/%s.DAT", id);
}

void scores_init() { mkdir(MOUNT_POINT "/GnW_AKA", 0777); }

void scores_top(const char* id, ScoreEntry out[GNW_TOP_N]) {
    memset(out, 0, sizeof(ScoreEntry) * GNW_TOP_N);
    char p[96]; path_for(id, p, sizeof p);
    FILE* f = fopen(p, "rb");
    if (!f) { printf("[SCORES] pas de fichier %s\n", p); return; }
    size_t n = fread(out, sizeof(ScoreEntry), GNW_TOP_N, f);
    fclose(f);
    printf("[SCORES] lu %u entrees <- %s (top=%s %lu)\n", (unsigned)n, p,
           out[0].name[0] ? out[0].name : "-", (unsigned long)out[0].value);
}

uint32_t scores_best(const char* id) {
    ScoreEntry t[GNW_TOP_N]; scores_top(id, t); return t[0].value;
}

int scores_rank(const char* id, uint32_t value) {
    ScoreEntry t[GNW_TOP_N]; scores_top(id, t);
    for (int i = 0; i < GNW_TOP_N; ++i) if (value > t[i].value) return i;
    return -1;
}

static void scores_write(const char* id, ScoreEntry t[GNW_TOP_N]) {
    mkdir(MOUNT_POINT "/GnW_AKA", 0777);          // s'assurer que le dossier existe
    char p[96]; path_for(id, p, sizeof p);
    FILE* f = fopen(p, "wb");
    if (!f) { printf("[SCORES] ECHEC ecriture %s (errno)\n", p); return; }
    size_t n = fwrite(t, sizeof(ScoreEntry), GNW_TOP_N, f);
    fclose(f);
    printf("[SCORES] ecrit %u entrees -> %s\n", (unsigned)n, p);
}

void scores_add(const char* id, const char* name, uint32_t value) {
    int pos = scores_rank(id, value);
    if (pos < 0) return;
    ScoreEntry t[GNW_TOP_N]; scores_top(id, t);
    for (int i = GNW_TOP_N - 1; i > pos; --i) t[i] = t[i - 1];
    memset(&t[pos], 0, sizeof(ScoreEntry));
    strncpy(t[pos].name, name ? name : "", 8);
    t[pos].value = value;
    scores_write(id, t);
}

// ---- on-screen name entry (1..8 chars) -----------------------------------
static const char CHARSET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ";
#define NCHARS (sizeof(CHARSET) - 1)

void scores_name_entry(char out[9]) {
    int idx[8] = {0}, len = 3, pos = 0;   // start with 3 slots
    for (;;) {
        Keys k; input_poll(k);
        if (k.pressed & GB_KEY_UP)    idx[pos] = (idx[pos] + 1) % NCHARS;
        if (k.pressed & GB_KEY_DOWN)  idx[pos] = (idx[pos] + NCHARS - 1) % NCHARS;
        if (k.pressed & GB_KEY_RIGHT) { if (pos < len - 1) pos++; else if (len < 8) { len++; pos++; } }
        if (k.pressed & GB_KEY_LEFT)  { if (pos > 0) pos--; }
        if (k.pressed & GB_KEY_B)     { if (len > 1) { for (int i = pos; i < len - 1; ++i) idx[i] = idx[i+1]; len--; if (pos >= len) pos = len - 1; } }
        if (k.pressed & GB_KEY_A)     break;

        gfx.clear(gfx.makeColor(16, 20, 28));
        gfx.setColor(gfx.makeColor(255, 220, 90));
        gfx.move_cursor(110, 60); gfx.print_str("NOUVEAU SCORE");
        int x0 = 160 - (len * 20) / 2;
        for (int i = 0; i < len; ++i) {
            char c[2] = { CHARSET[idx[i]], 0 };
            gfx.setColor(i == pos ? gfx.makeColor(90, 200, 235) : gfx.makeColor(210, 215, 225));
            gfx.move_cursor(x0 + i * 20, 120); gfx.print_str(c);
            if (i == pos) { gfx.fillRect(x0 + i * 20, 134, 12, 2); }
        }
        gfx.setColor(gfx.makeColor(150, 160, 175));
        gfx.move_cursor(16, 176); gfx.print_str("< > V ^ : Modifier");
        gfx.move_cursor(16, 190); gfx.print_str("B: Effacer   A: Valider");
        gfx.update();
        vTaskDelay(pdMS_TO_TICKS(30));
    }
    int j = 0;
    for (int i = 0; i < len; ++i) { char c = CHARSET[idx[i]]; if (c != ' ' || j > 0) out[j++] = c; }
    while (j > 0 && out[j-1] == ' ') j--;   // trim trailing spaces
    if (j == 0) { strcpy(out, "AAA"); } else out[j] = 0;
}

bool scores_submit_named(const char* id, uint32_t value) {
    if (scores_rank(id, value) < 0) return false;
    char name[9]; scores_name_entry(name);
    scores_add(id, name, value);
    return true;
}
