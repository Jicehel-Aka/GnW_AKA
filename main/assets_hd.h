#pragma once
#include <stdint.h>

// Generated from a27ds/Parachute sprites (890x530 RGBA frames),
// cropped to their used area, scaled to the 320x240 screen, converted
// to native BGR565 with a magenta transparency key. (x,y) is the
// top-left on screen, letterboxed vertically.

struct HdImage { int16_t x, y; uint16_t w, h; const uint16_t* data; };

static const uint16_t HD_TRANS = 0xF81F;
static const int HD_OFFY = 24;

extern const HdImage img_A_1;
extern const HdImage img_A_2;
extern const HdImage img_A_3;
extern const HdImage img_A_4;
extern const HdImage img_A_5;
extern const HdImage img_A_6;
extern const HdImage img_A_7;
extern const HdImage img_A_Water;
extern const HdImage img_B_1;
extern const HdImage img_B_2;
extern const HdImage img_B_3;
extern const HdImage img_B_4;
extern const HdImage img_B_5;
extern const HdImage img_B_6;
extern const HdImage img_B_Water;
extern const HdImage img_C_1;
extern const HdImage img_C_2;
extern const HdImage img_C_3;
extern const HdImage img_C_4;
extern const HdImage img_C_5;
extern const HdImage img_C_Water;
extern const HdImage img_Clock;
extern const HdImage img_Dead_1;
extern const HdImage img_Dead_2;
extern const HdImage img_Dead_3;
extern const HdImage img_GameA;
extern const HdImage img_GameB;
extern const HdImage img_GameB_Var_C_1;
extern const HdImage img_GameB_Var_C_2;
extern const HdImage img_Helicopter;
extern const HdImage img_Life_1;
extern const HdImage img_Life_2;
extern const HdImage img_Life_3;
extern const HdImage img_Miss;
extern const HdImage img_Monkey;
extern const HdImage img_Monkey_Var_A_1;
extern const HdImage img_Monkey_Var_A_2;
extern const HdImage img_P_A;
extern const HdImage img_P_B;
extern const HdImage img_P_C;
extern const HdImage img_Shark_1;
extern const HdImage img_Shark_2;
extern const HdImage img_Shark_3;
extern const HdImage img_Shark_4;
extern const HdImage img_Shark_5;
extern const HdImage img_parachute_background;

extern const HdImage* const hd_paraA[7];
extern const HdImage* const hd_paraB[6];
extern const HdImage* const hd_paraC[5];
extern const HdImage* const hd_water[3];
extern const HdImage* const hd_dead[3];
extern const HdImage* const hd_shark[5];
extern const HdImage* const hd_life[3];
extern const HdImage* const hd_boat[3];
