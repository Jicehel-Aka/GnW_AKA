#pragma once
#include <stdint.h>
struct MhImg { uint16_t w,h; const uint16_t* d; };
static const uint16_t MH_KEY=0xF81F;
extern const uint16_t MH_BG[];
extern const MhImg player_0;
extern const MhImg player_1;
extern const MhImg player_2;
extern const MhImg player_3;
extern const MhImg ped_0;
extern const MhImg ped_1;
extern const MhImg ped_2;
extern const MhImg ped_3;
extern const MhImg ped_4;
extern const MhImg ped_5;
extern const MhImg ped_6;
extern const MhImg ped_7;
extern const MhImg plate;
#define MH_NPLAYER 4
#define MH_NPED 8
#define MH_HAS_PLATE 1
