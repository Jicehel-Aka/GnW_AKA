#pragma once
#include <stdint.h>
// Per-game high scores on SD: /sdcard/SCORES/<id>.DAT (5 named entries).
#define GNW_TOP_N 5

struct ScoreEntry { char name[9]; uint32_t value; };   // name: up to 8 chars + NUL

void     scores_init();
void     scores_top(const char* id, ScoreEntry out[GNW_TOP_N]);
uint32_t scores_best(const char* id);
int      scores_rank(const char* id, uint32_t value);           // slot 0..4, or -1
void     scores_add(const char* id, const char* name, uint32_t value);  // silent insert
bool     scores_submit_named(const char* id, uint32_t value);
void     scores_name_entry(char out[9]);   // on-screen 1..8 char entry   // prompts a name if it ranks
