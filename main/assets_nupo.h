#pragma once
#include <stdint.h>
struct NupoImg { uint16_t w,h; const uint16_t* d; };
static const uint16_t NUPO_KEY=0xF81F;
extern const NupoImg wolf_ul;
extern const NupoImg wolf_dl;
extern const NupoImg wolf_ur;
extern const NupoImg wolf_dr;
extern const NupoImg egg_left_1;
extern const NupoImg egg_left_2;
extern const NupoImg egg_left_3;
extern const NupoImg egg_left_4;
extern const NupoImg egg_left_5;
extern const NupoImg egg_right_1;
extern const NupoImg egg_right_2;
extern const NupoImg egg_right_3;
extern const NupoImg egg_right_4;
extern const NupoImg egg_right_5;
extern const NupoImg life;
extern const NupoImg hare;
