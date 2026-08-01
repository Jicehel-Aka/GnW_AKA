#include <stdint.h>
#include "commonvars.h"
#include "framebuffer.h"

uint8_t gameState, debugMode;
char* verifyCode = NULL;
bool EnemyStates[3][3];
bool PlayerStates[3];
uint8_t HitPosition, LivesLost;
uint16_t Teller, FlashesDelay, Flashes, Delay;
uint32_t Score;
bool CanMove, CrashSoundPlayed;
uint32_t frames;
uint8_t alternateSource;
uint8_t buttons, prevbuttons = 0;
Framebuffer fb;
