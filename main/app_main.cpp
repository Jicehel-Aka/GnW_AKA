/*
 * AKA — collection Game & Watch (ESP32-S3 / ESP-IDF).
 * Shell : ecran de selection -> execution du module choisi, avec les features
 * globales (menu, capture, reglages SD, multilangue, retour loader).
 */
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"

#include "gb_core.h"
#include "gb_graphics.h"
#include "gb_common.h"

#include "core/sdcard.h"
#include "core/input.h"
#include "core/graphics.h"
#include "core/audio.h"
#include "core/i18n.h"
#include "core/settings.h"
#include "ui/menu.h"
#include "ui/highscores.h"

#include "game_module.h"
#include "select_screen.h"
#include "sfx.h"
#include "gnw/gnw_scores.h"
#include "gnw/gnw_l10n.h"

int         volume = 200;
int         language = LANG_EN;   // 0..LANG_COUNT-1 (persisted by settings)
gb_core     g_core;
gb_graphics gfx;

static void checkReturnToLoader(bool run_held, bool menu_held) {
    static uint32_t combo_start = 0;
    uint32_t now = esp_timer_get_time() / 1000;
    if (run_held && menu_held) {
        if (!combo_start) combo_start = now;
        else if (now - combo_start >= 500) {
            const esp_partition_t* loader = esp_partition_find_first(
                ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_1, nullptr);
            if (loader) { esp_ota_set_boot_partition(loader); esp_restart(); }
        }
    } else combo_start = 0;
}

extern "C" void app_main(void) {
    g_core.init();
    gfx_init();
    g_core.joystick.calibrate_center();
    sd_init();
    settings_load();
    audio_game_init();
    audio_set_volume(volume);
    sfx::init();
    input_init();
    highscores_init();
    scores_init();
    gnw_lang_index = language;
    srand((unsigned)esp_timer_get_time());

    const int64_t FRAME_US = 25000;   // 40 FPS

    while (true) {
        // 1) game-select screen
        int idx = select_screen_run();
        const GameModule* game = GAME_LIST[idx];
        game->enter();
        g_exit_to_menu = false;

        // 2) run the chosen game with the global features
        uint32_t menu_press_start = 0;
        bool     menu_shot_done   = false;
        int64_t  last = esp_timer_get_time();

        while (!g_exit_to_menu) {
            int64_t now = esp_timer_get_time();
            if (now - last < FRAME_US) { vTaskDelay(1); continue; }
            last = now;

            gnw_lang_index = language;
            Keys k; input_poll(k);
            uint32_t s = g_core.buttons.state();
            checkReturnToLoader(s & GB_KEY_RUN, s & GB_KEY_MENU);

            if (k.MENU && !k.RUN) {                       // MENU long -> capture
                uint32_t ms = esp_timer_get_time() / 1000;
                if (!menu_press_start) { menu_press_start = ms; menu_shot_done = false; }
                else if (!menu_shot_done && ms - menu_press_start >= 500) {
                    menu_shot_done = true;
                    char shot[64];
                    if (gfx_save_screenshot_bmp(shot, sizeof shot)) {
                        gfx_text_center(115, i18n::T(i18n::STR_SHOT_SAVED), color_yellow);
                        gfx_flush(); vTaskDelay(pdMS_TO_TICKS(900));
                    }
                }
            } else {
                if (menu_press_start && !menu_shot_done) {    // MENU court -> menu modal
                    MenuAction act = menu_open(true);
                    if (act == MenuAction::ReturnTitle) g_exit_to_menu = true;   // -> selecteur
                    else if (act == MenuAction::StartGame) game->enter();        // relancer
                }
                menu_press_start = 0; menu_shot_done = false;
            }

            game->update(k);
            game->render();
        }
    }
}
