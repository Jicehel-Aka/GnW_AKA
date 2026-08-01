idf_component_register(
    SRCS
        # shell
        "app_main.cpp" "games.cpp" "select_screen.cpp"
        # helpers partages (utilises par le selecteur / la page infos / Parachute)
        "gnw/gnw_l10n.cpp" "gnw/gnw_scores.cpp"
        "info_screen.cpp" "gametexts.cpp"
        # game: Parachute
        "parachute.cpp" "icons.cpp" "globals.cpp" "render.cpp" "sfx.cpp" "assets_hd.cpp"
        # couche META partagee (Octopus)
        "gnw/Gamebuino-Meta.cpp" "gnw/gb_display.cpp" "gnw/gnw_present.cpp"
        # game: Octopus (moteur header-only isole en namespace)
        "octopus.cpp"
        # game: Formula 1 (coeur portable f1/src + colle AKA)
        "f1_module.cpp" "f1_aka_sound.cpp" "f1_aka_savestate.cpp" "icon_f1.cpp" "fire.cpp" "icon_fire.cpp" "fire_hd/fire_hd.cpp" "fire_hd/assets_fire_hd.cpp" "dkjr/dkjr.cpp" "dkjr/src_meta/engine/Game.cpp" "dkjr/src_meta/engine/Creature.cpp" "dkjr/src_meta/engine/Fruit.cpp" "dkjr/src_meta/player/Player.cpp" "dkjr/src_meta/graphics/Renderer.cpp" "dkjr/src_meta/data/Score.cpp" "dkjr/src_meta/sounds/Buzzer.cpp"
        "f1/src/framebuffer.cpp" "f1/src/game.cpp" "f1/src/intro.cpp"
        "f1/src/commonvars.cpp" "f1/src/printfuncs.cpp"
        # couche globale (squelette)
        "core/input.cpp" "core/graphics.cpp" "core/audio.cpp"
        "core/i18n.cpp"  "core/settings.cpp" "core/sdcard.cpp"
        "ui/menu.cpp"    "ui/highscores.cpp"
    INCLUDE_DIRS "." "gnw" "f1/src" "f1/compat"
    REQUIRES gamebuino
    PRIV_REQUIRES app_update)

# L'AKA compile en -Werror : on relegue en simples avertissements les points
# non bloquants du code d'origine.
target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wno-error=reorder -Wno-error=switch -Wno-error=unused-variable
    -Wno-error=type-limits -Wno-error=narrowing -Wno-error=parentheses -Wno-error=maybe-uninitialized)
