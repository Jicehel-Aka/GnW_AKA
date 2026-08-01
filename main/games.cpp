#include "game_module.h"

extern const GameModule parachute_module;
extern const GameModule octopus_module;
extern const GameModule f1_module;
// --- pas encore reactives ---
extern const GameModule dkjr_module;
extern const GameModule fire_module;
// extern const GameModule nupogodi_module;
// extern const GameModule manhole_module;

bool g_exit_to_menu = false;

const GameModule* const GAME_LIST[] = {
    &parachute_module,
    &octopus_module,
    &f1_module,
    &fire_module,
    &dkjr_module,
};
const int GAME_COUNT = (int)(sizeof(GAME_LIST) / sizeof(GAME_LIST[0]));
