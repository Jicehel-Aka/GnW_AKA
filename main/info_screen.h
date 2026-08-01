#pragma once
struct GameModule;
// Modal page for one game: name, controls, rules and high scores, all localized.
// B or MENU closes it. Blocks like the selector / global menu.
void info_show(const GameModule* g);
