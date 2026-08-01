#include "ui/menu.h"      // menu_open / MenuAction
#include "core/graphics.h" // gfx_save_screenshot_bmp
#include "Gamebuino-Meta.h"
#include "core/audio.h"     // g_sfx (skeleton tone bus)
#include "gnw_scores.h"
#include "gb_common.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cstddef>
#include "core/input.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
namespace gbc { void present(); }

// gb.save : small int32 store persisted on the SD (faithful for DKJr's Score),
// and high values mirrored into the per-game score table so the info page shows
// DKJr high scores too.
static int32_t s_save[16];
static bool    s_save_loaded = false;
static void save_path(char* p, size_t n){ snprintf(p,n, MOUNT_POINT "/GnW_AKA/GBSAVE.DAT"); }
static void save_load(){ if(s_save_loaded) return; s_save_loaded=true;
    char p[96]; save_path(p,sizeof p); FILE* f=fopen(p,"rb");
    if(f){ fread(s_save,sizeof(int32_t),16,f); fclose(f);} }
static void save_flush(){ char p[96]; save_path(p,sizeof p);
    FILE* f=fopen(p,"wb"); if(f){ fwrite(s_save,sizeof(int32_t),16,f); fclose(f);} }

int32_t GbSave::get(int index){ save_load(); return (index>=0&&index<16)? s_save[index]:0; }
void    GbSave::set(int index, int32_t v){ save_load();
    if(index>=0&&index<16){ s_save[index]=v; save_flush(); }
    if(v>0) scores_add("DKJR","DK",(uint32_t)v); }


Gb gb;

static uint16_t key_bit(Button b) {
    switch (b) {
        case BUTTON_LEFT:  return GB_KEY_LEFT;
        case BUTTON_RIGHT: return GB_KEY_RIGHT;
        case BUTTON_UP:    return GB_KEY_UP;
        case BUTTON_DOWN:  return GB_KEY_DOWN;
        case BUTTON_A:     return GB_KEY_A;
        case BUTTON_B:     return GB_KEY_B;
        case BUTTON_MENU:  return GB_KEY_MENU;
        default:           return GB_KEY_RUN;   // HOME ~ RUN
    }
}
bool GbButtons::held(Button b)    const { return st & key_bit(b); }
bool GbButtons::pressed(Button b) const { uint16_t m = key_bit(b); return (st & m) && !(prev & m); }

// Map the engine's procedural FX to a short tone on the shared bus.
void GbSound::fx(const Gamebuino_Meta::Sound_FX* f) {
    using W = Gamebuino_Meta::Sound_FX_Wave;
    float freq = (f && f->type == W::NOISE) ? 220.0f : 880.0f;
    g_sfx.play_tone(freq, 30, 0.4f);
}

void GbSound::playTick()   { g_sfx.play_tone(1200.0f, 16, 0.30f); }
void GbSound::playOK()     { g_sfx.play_tone( 880.0f, 60, 0.45f); }
void GbSound::playCancel() { g_sfx.play_tone( 160.0f,120, 0.45f); }
void GbSound::play(const char*) { g_sfx.play_tone(440.0f, 60, 0.40f); }

void gnw_gb_update(const Keys& k) {
    gb.frameCount++;
    gb.buttons.set((uint16_t)k.raw);   // raw expander bitmask (EXPANDER_KEY_* == GB_KEY_*)
}


// ---- loop-ownership bridge + gb.update() ---------------------------------
jmp_buf       gbc_exit_ctx;
volatile bool gbc_want_exit = false;

void Gb::pickRandomSeed() { srand((unsigned)esp_timer_get_time()); }

bool Gb::update() {
    static int64_t last = 0;
    const int64_t FRAME_US = 1000000 / 25;         // ~firebuino FPS
    int64_t now = esp_timer_get_time();
    if (now - last < FRAME_US) { vTaskDelay(1); return false; }   // not yet -> firebuino `continue`s
    last = now;

    Keys k; input_poll(k);
    gnw_gb_update(k);                              // refresh gb.buttons/frameCount

    // exit to the game selector on MENU (loop-owning game can't be driven frame
    // by frame by the shell, so we break out of its while(true) via longjmp).
    // MENU court -> menu d'options (comme les autres jeux) ; MENU long -> capture d'ecran
    static uint32_t s_menu_start = 0; static bool s_shot_done = false;
    uint32_t ms = (uint32_t)(now / 1000);
    if (k.raw & GB_KEY_MENU) {
        if (!s_menu_start) { s_menu_start = ms; s_shot_done = false; }
        else if (!s_shot_done && ms - s_menu_start >= 500) {
            s_shot_done = true;
            char shot[64]; gfx_save_screenshot_bmp(shot, sizeof shot);   // SHOTxxxx.BMP
        }
    } else {
        if (s_menu_start && !s_shot_done) {                             // appui court -> menu
            MenuAction act = menu_open(true);
            if (act == MenuAction::ReturnTitle) gbc_want_exit = true;   // "retour au selecteur"
        }
        s_menu_start = 0; s_shot_done = false;
    }
    gbc::present();                               // scale 80x64 -> screen + text
    if (gbc_want_exit) { gbc_want_exit = false; longjmp(gbc_exit_ctx, 1); }
    return true;                                  // one frame elapsed
}
