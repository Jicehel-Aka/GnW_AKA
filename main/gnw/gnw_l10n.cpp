#include <cstddef>
#include <cstring>
#include "gnw_l10n.h"

int gnw_lang_index = LANG_EN;

const char* L(const char* const table[]) {
    if (!table) return "";
    const char* s = table[gnw_lang_index];
    if (!s) s = table[LANG_EN];      // fallback to English
    return s ? s : "";
}
