#pragma once
#include "core/input.h"   // Keys

// A game plugs into the shell through this tiny interface.
struct GameModule {
    const char* name;
    void (*enter)();                 // reset/init when selected
    void (*update)(const Keys& k);   // per-frame logic
    void (*render)();                 // per-frame draw (+ gfx.update())
    // Optional tile artwork for the selector (native BGR565). Leave null for a
    // colored placeholder. Existing brace-inits stay valid (trailing = zero).
    const uint16_t* icon;
    uint16_t        icon_w;
    uint16_t        icon_h;
    // Per-game metadata (all optional; trailing -> existing inits stay valid).
    const char*         save_id;        // score file id, e.g. "PARACHUTE"
    const char* const*  name_l10n;      // [LANG_COUNT] display name
    const char* const*  rules_l10n;     // [LANG_COUNT] rules (\n-separated lines)
    const char* const*  controls_l10n;  // [LANG_COUNT] controls (\n-separated lines)
};

extern const GameModule* const GAME_LIST[];
extern const int GAME_COUNT;

// A module (or the in-game menu "return to selection") sets this to leave
// the current game and go back to the game-select screen.
extern bool g_exit_to_menu;
