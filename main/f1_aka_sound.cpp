#include "sound.h"
#include "core/audio.h"    // g_sfx
void initSound(void)         {}
void playTickSound(void)     { g_sfx.play_tone(1000.0f, 14, 0.30f); }
void playCrashSound(void)    { g_sfx.play_tone( 150.0f,140, 0.50f); }
void playGameOverSound(void) { g_sfx.play_tone( 400.0f,300, 0.50f); }
