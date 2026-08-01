#ifndef PARACHUTE_GLOBAL
#define PARACHUTE_GLOBAL

#include <stdint.h>

// -------------------------------------------------------------------------
// Global constants
// (The META HD slice constants sliceHeight/slices are gone: the AKA has a
//  full 320x240 framebuffer, no manual slicing.)
// -------------------------------------------------------------------------

constexpr uint8_t  screenWidth    = 160;   // playfield, centered in 320x240
constexpr uint8_t  screenHeight   = 128;
constexpr uint16_t transColor     = 0xdfff; // key in the ORIGINAL RGB565 data
                                            // (see assets_rt.h -> TRANS for the
                                            //  native-order value used at draw)

constexpr uint8_t  idSpritesheetA = 0;
constexpr uint8_t  idSpritesheetB = 1;

constexpr uint8_t  firstSpriteColumn[] { 0, 7, 13, 17 };

// -------------------------------------------------------------------------
// Global variables (declared here, defined once in globals.cpp so several
// translation units can share them without multiple-definition errors).
// -------------------------------------------------------------------------

extern uint16_t score;
extern uint16_t minHighscore;
extern int16_t  misses;
extern int16_t  moveTick;
extern int16_t  spawnDelay;
extern int8_t   spawnCount;
extern int8_t   speedMax;
extern uint8_t  parachuteLaunchCount;
extern int8_t   parachutes[10];
extern int8_t   helicopterAnimation;
extern int8_t   sharkAnimation;
extern int8_t   floodedAnimation;
extern uint8_t  speedBlades;
extern int16_t  gameOverDelay;   // tempo d'affichage des 3 requins avant l'ecran game over

enum class GameState : uint8_t {
  home,
  run,
  gameOver,
  pauseScreen
};

extern GameState paraState;

struct Player {
  uint8_t spriteIndex;
};

extern Player boat;

#endif
