#pragma once
// Minimal Arduino compatibility so the ported game logic (Animate.h, the
// state updates) keeps using random() exactly as on the META.
#include <cstdlib>

static inline long random(long howbig) {
    return (howbig > 0) ? (long)(rand() % howbig) : 0;
}
static inline long random(long howsmall, long howbig) {
    if (howsmall >= howbig) return howsmall;
    return howsmall + (rand() % (howbig - howsmall));   // max exclusive, like Arduino
}
