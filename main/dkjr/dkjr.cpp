// Donkey Kong Jr — shell module. Steph's engine runs unchanged; only its
// Renderer transfer layer (-> gnw_present) and the Gamebuino API (-> gnw shim)
// were swapped. gb is refreshed from Keys each frame before loop().
#include "arduino_compat.h"
#include "game_module.h"
#include "gametexts.h"
#include "icons.h"
#include "src_meta/engine/Game.h"
#include "gnw/Gamebuino-Meta.h"

static Game s_game;
static bool s_started = false;

static void dkjr_enter()  { s_game.setup(); s_started = true; }
static void dkjr_update(const Keys& k) { gnw_gb_update(k); if (s_started) s_game.loop(); }
static void dkjr_render() { if (s_started) s_game.draw(); }

extern const GameModule dkjr_module = { "Donkey Kong Jr", dkjr_enter, dkjr_update, dkjr_render, DKJR_ICON, ICON_W, ICON_H,
  "DKJR", DKJR_NAME, DKJR_RULES, DKJR_CTRL };
