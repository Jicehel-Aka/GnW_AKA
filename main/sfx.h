#pragma once
// Sound facade: maps the META gb.sound.* calls used by the game to the AKA
// high-level audio API (gb_audio_player + gb_audio_track_tone).
namespace sfx {
    void init();                     // call once at boot
    void tick();                     // gb.sound.playTick()
    void ok();                       // gb.sound.playOK()
    void cancel();                   // gb.sound.playCancel()
    void wav(const char* name);      // gb.sound.play("pauseScreen.wav" | "Chance.wav" | "gameOver.wav")
}
