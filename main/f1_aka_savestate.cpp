#include "savestate.h"
#include "gnw/gnw_scores.h"
void     loadSaveState(void)   {}
void     saveSaveState(void)   {}
void     setHiScore(int)       {}                       // table written on game over
static uint32_t s_f1_hi = 0;
static bool     s_f1_hi_loaded = false;
uint32_t getHiScore(void) {                 // lu une seule fois puis mis en cache
    if (!s_f1_hi_loaded) { s_f1_hi = scores_best("F1"); s_f1_hi_loaded = true; }
    return s_f1_hi;
}
void f1HiScoreReload(void) { s_f1_hi = scores_best("F1"); s_f1_hi_loaded = true; }
