// Parachute wrapped as a shell GameModule. Logic identical to the standalone
// port; only the entry points change (enter/update/render dispatchers).
#include "game_module.h"
#include "gametexts.h"
#include "gnw/gnw_scores.h"
#include "icons.h"
#include "gb_common.h"
#include "Global.h"
#include "arduino_compat.h"
#include "sfx.h"
#include "render.h"
#include "Animate.h"
#include "ui/highscores.h"

static void init_game() {
    paraState = GameState::run;
    sfx::tick();
    misses = 0; score = 0; boat.spriteIndex = 0;
    spawnDelay = 4; sharkAnimation = -2; floodedAnimation = -2;
    speedMax = 25; helicopterAnimation = 0; speedBlades = 5; gameOverDelay = 0;
    moveTick = speedMax; spawnCount = spawnDelay; parachuteLaunchCount = 0;
    for (auto& v : parachutes) v = -1;
}

static void update_home(const Keys& k) {
    if (k.pressed & GB_KEY_A) init_game();
}
static void update_run(const Keys& k) {
    if (misses >= 3) {
        // (3) garder la scene avec les 3 requins + jouer le game over, puis effacer
        if (gameOverDelay == 0) { gameOverDelay = 60; sfx::wav("gameOver.wav"); }
        if (--gameOverDelay <= 0) paraState = GameState::gameOver;
        return;   // jeu fige : render_run continue d'afficher les 3 requins
    }
    if ((k.pressed & GB_KEY_LEFT) && boat.spriteIndex > 0)  { --boat.spriteIndex; sfx::tick(); }
    else if ((k.pressed & GB_KEY_RIGHT) && boat.spriteIndex < 2) { ++boat.spriteIndex; sfx::tick(); }
    if (k.pressed & GB_KEY_RUN) { paraState = GameState::pauseScreen; return; }
    animateShark(); animateHelicopter(); animateParatrooper();
    if (floodedAnimation > -2) animateFlooded();
    if (moveTick > 0) { --moveTick; }
    else {
        --spawnCount;
        if ((spawnCount < 1) && (random(6 - (score / 200)) < 4)) {
            if (parachuteLaunchCount < 9) {
                size_t col = random(0, 3);
                parachutes[parachuteLaunchCount] = firstSpriteColumn[col];
                ++parachuteLaunchCount;
                spawnCount = (spawnDelay - int(score / 60));
                if (spawnCount < 2) spawnCount = 2;
            }
        }
        moveTick = speedMax - (score / 75);
    }
    if ((score > 0) && ((score % 500) == 0) && misses > 0) { --misses; sfx::wav("Chance.wav"); }
}
static void update_pause(const Keys& k)    { if (k.pressed & GB_KEY_A) paraState = GameState::run; }
static void update_gameover(const Keys& k) {
    if (k.pressed & GB_KEY_A) { scores_submit_named("PARACHUTE", (uint32_t)score); paraState = GameState::home; }
}

static void parachute_enter() { paraState = GameState::home; }
static void parachute_update(const Keys& k) {
    switch (paraState) {
        case GameState::home:        update_home(k);     break;
        case GameState::run:         update_run(k);      break;
        case GameState::gameOver:    update_gameover(k); break;
        case GameState::pauseScreen: update_pause(k);    break;
    }
}
static void parachute_render() {
    switch (paraState) {
        case GameState::home:        render_home();     break;
        case GameState::run:         render_run();      break;
        case GameState::gameOver:    render_gameover(); break;
        case GameState::pauseScreen: render_pause();    break;
    }
}
extern const GameModule parachute_module = { "Parachute", parachute_enter, parachute_update, parachute_render, PARACHUTE_ICON, ICON_W, ICON_H,
  "PARACHUTE", PARA_NAME, PARA_RULES, PARA_CTRL };
