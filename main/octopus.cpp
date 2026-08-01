// Octopus (Jean-Charles Lebeau, structure G&W HD de Steph) — module shell.
// Le moteur est "tout-en-headers" : on l'isole dans un namespace anonyme pour
// eviter tout conflit de globals avec les autres jeux, et on ne redirige que
// le transfert (OutputScreen_HDTools.h -> gnw_present, deja patche).
#include "gnw/Gamebuino-Meta.h"   // gb (global) + gnw_gb_update
#include "game_module.h"
#include "gametexts.h"
#include "gnw/gnw_scores.h"
#include "icons.h"

// En-tetes standard AU SCOPE GLOBAL (sinon tires dans le namespace anonyme -> casse
// la lib standard et rend gnw_present_* introuvables a l'edition de liens).
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include "gnw/gnw_present.h"

namespace {
    #include "octopus/Global.h"
    #include "octopus/OutputScreen.h"
    #include "octopus/OutputSound.h"
    #include "octopus/OutputLight.h"
    #include "octopus/_Game.h"
    #include "octopus/_Buttons.h"
    #include "octopus/_Ennemy.h"

    void oct_logic() {
        switch (gameState) {
            case RUN:
                checkButton();
                Barca();               // anime/positionne la barque
                Sub();                 // anime/deplace les plongeurs
                game_checkGoldInBarca();
                ennemy_octopus();
                ennemy_OctopusCollision();   // la pieuvre attrape le plongeur
                game_checkGameOver();
                if (gameState == GAMEOVER) { static bool done=false;
                    if(!done){ ::scores_submit_named("OCTOPUS",(uint32_t)score); done=true; } }
                moveTick = (moveTick + 1) % maxTick;
                break;
            default:               // TITLESCREEN / MENU / PAUSE / GAMEOVER
                checkButton();
                break;
        }
    }
    void oct_draw() {
        switch (gameState) {
            case TITLESCREEN: outputScreen_titleScreen(); break;
            case MENU:        outputScreen_menu();        break;
            case RUN:         outputScreen_run();         break;
            case PAUSE:       outputScreen_pause();       break;
            case GAMEOVER:    outputScreen_gameOver();    break;
            case CONTROLS:    outputScreen_controls();    break;
            case CREDITS:     outputScreen_credits();     break;
        }
    }
}

static void octopus_enter()  { gameState = TITLESCREEN; outputLight_clear(); outputSound_tick(); }
static void octopus_update(const Keys& k) { gnw_gb_update(k); oct_logic(); }
static void octopus_render() { oct_draw(); }

extern const GameModule octopus_module = { "Octopus", octopus_enter, octopus_update, octopus_render, OCTOPUS_ICON, ICON_W, ICON_H,
  "OCTOPUS", OCTO_NAME, OCTO_RULES, OCTO_CTRL };
