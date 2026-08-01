// Fire! (firebuino, Luis Dominguez / makerSquirrel, art Erico P. Monteiro, LGPL)
// Module AKA. firebuino possede sa boucle (while(true){ if(!gb.update())... }),
// on l'execute donc directement et gb.update() (dans le shim) gere frame/entree/
// present ; la sortie vers le selecteur se fait par longjmp depuis gb.update().
#include "game_module.h"
#include "gnw/Gamebuino-Meta.h"    // gb, Image, colors, gbc_exit_ctx
#include "gametexts.h"
#include "icons.h"
#include "gnw/gnw_scores.h"   // scores_submit_named (declaration globale)
#include "fire_hd/fire_hd.h"  // mode "AKA HD" (rendu natif 320x240)
#include "gb_graphics.h"      // vrai type gb_graphics (SDK), pour l'ecran de choix
#include <cstddef>   // NULL
#include <cstring>   // strcpy

namespace fire_impl {
    #include "fire/globalStuff.h"
    #include "fire/graphics.h"
    #include "fire/soundfx.h"
    #include "fire/highscore.h"
    // prototypes (les .ino Arduino sont auto-prototypes ; pas nous)
    void moveSurvivors(); void menuLoop(); void gameLoop();
    #include "fire/firebuino.inc"   // defines setup()/loop() + globals (isolated here)
}

static bool s_setup = false;
enum class FireMode { CHOOSING, META, AKA_HD };
static FireMode s_mode = FireMode::CHOOSING;

static void fire_enter() { s_setup = false; s_mode = FireMode::CHOOSING; }

extern gb_graphics gfx;   // pour l'ecran de choix (avant tout moteur)

static void fire_choose_render() {
    gfx.setColor(gfx.makeColor(0, 0, 0)); gfx.clear();
    gfx.setColor(gfx.makeColor(255, 220, 60));
    gfx.move_cursor(70, 90);  gfx.print_str("FIRE!");
    gfx.setColor(gfx.makeColor(255, 255, 255));
    gfx.move_cursor(40, 120); gfx.print_str("A : mode META original (80x64)");
    gfx.move_cursor(40, 140); gfx.print_str("B : mode AKA HD (320x240)");
    gfx.update();
}

static void fire_update(const Keys& k) {
    if (s_mode == FireMode::CHOOSING) {
        fire_choose_render();
        if (k.pressed & GB_KEY_A) { s_mode = FireMode::META;   }
        if (k.pressed & GB_KEY_B) { s_mode = FireMode::AKA_HD; fire_hd::enter(); }
        return;
    }
    if (s_mode == FireMode::AKA_HD) { fire_hd::update(k); return; }

    // --- mode META (moteur firebuino d'origine, inchange) ---
    // Run firebuino until it asks to leave (MENU -> longjmp out of its loop).
    if (setjmp(gbc_exit_ctx) == 0) {
        if (!s_setup) { fire_impl::setup(); s_setup = true; }
        fire_impl::loop();            // blocks; presents via gb.update() each frame
    }
    // returned via longjmp: leave to the selector
    extern bool g_exit_to_menu; g_exit_to_menu = true;
    s_setup = false;
    s_mode = FireMode::CHOOSING;
}

static void fire_render() {
    if (s_mode == FireMode::AKA_HD) fire_hd::render();
    // META : le rendu se fait dans gb.update() (shim), rien a faire ici.
}

extern const GameModule fire_module = { "Fire!", fire_enter, fire_update, fire_render,
                                 FIRE_ICON, ICON_W, ICON_H, "FIRE", FIRE_NAME,
                                 FIRE_RULES, FIRE_CTRL };
