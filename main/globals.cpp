// Single definition point for the globals declared extern in Global.h.
#include "Global.h"

uint16_t score;
uint16_t minHighscore;
int16_t  misses;
int16_t  moveTick;
int16_t  spawnDelay;
int8_t   spawnCount;
int8_t   speedMax;
uint8_t  parachuteLaunchCount;
int8_t   parachutes[10];
int8_t   helicopterAnimation;
int8_t   sharkAnimation;
int8_t   floodedAnimation;
uint8_t  speedBlades;
int16_t  gameOverDelay { 0 };

GameState paraState { GameState::home };
Player    boat    { 0 };
