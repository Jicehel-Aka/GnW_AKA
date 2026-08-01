#include "sfx.h"
#include <string.h>
#include "gb_common.h"            // MOUNT_POINT
#include "core/audio.h"           // player + g_sfx (audio du squelette, reutilise)
#include "gb_audio_track_wav.h"

// Parachute n'a pas de musique PMF : la 4e voix du player est libre -> on y met
// une voix WAV pour les samples G&W authentiques. Le "tick" (tres frequent)
// reste un ton via g_sfx pour eviter des acces fichier a chaque deplacement.
#define SND(f) MOUNT_POINT "/GnW_AKA/PARACHUTE/" f
static gb_audio_track_wav s_wav;
static int s_miss = 0;

void sfx::init()   { player.add_track(&s_wav, 1.0f); }
void sfx::tick()   { g_sfx.play_tone(1200.0f, 18, 0.35f); }
void sfx::ok()     { s_wav.play_wav(SND("GOT.WAV")); }
void sfx::cancel() {
    static const char* m[3] = { SND("MISS1.WAV"), SND("MISS2.WAV"), SND("MISS3.WAV") };
    s_wav.play_wav(m[s_miss]); s_miss = (s_miss + 1) % 3;
}
void sfx::wav(const char* n) {
    if      (!strcmp(n, "Chance.wav"))   s_wav.play_wav(SND("CHANCE.WAV"));
    else if (!strcmp(n, "gameOver.wav")) s_wav.play_wav(SND("ALARM.WAV"));
    else                                 g_sfx.play_tone(440.0f, 60, 0.4f);  // pause
}
