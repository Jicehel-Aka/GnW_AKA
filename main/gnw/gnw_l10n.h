#pragma once
// Lightweight per-game localization: each game carries its own string tables
// ([LANG_COUNT]); L() picks the current language, falling back to English when
// a translation is missing. The current index tracks the console setting.
enum { LANG_FR, LANG_EN, LANG_DE, LANG_ES, LANG_IT, LANG_COUNT };

extern int gnw_lang_index;                 // 0..LANG_COUNT-1 (kept in sync by the shell)

const char* L(const char* const table[]);  // table[LANG_COUNT], EN fallback
