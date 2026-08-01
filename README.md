# AKA — Collection Game & Watch

Portage de plusieurs Game & Watch sur la console **AKA** (ESP32-S3, ESP-IDF),
réunis dans une seule application avec écran de sélection façon carrousel.

---

## 1. Vue d'ensemble

- **Shell multi-jeux** : écran de sélection (carrousel horizontal + icônes),
  chaque jeu est un `GameModule { name, enter, update, render, icon, save_id,
  name_l10n, rules_l10n, controls_l10n }` déclaré dans `main/games.cpp`.
- **Features globales** (dans `main/app_main.cpp`, valables pour tous les jeux) :
  menu options, capture d'écran, retour au sélecteur, retour au loader,
  calibrage du joystick, multilingue, sauvegarde SD.
- **Couche partagée `main/gnw/`** (capitalisation) : fait tourner n'importe quel
  Game & Watch « META HD » (patron Steph/jicehel) sans toucher à sa logique.

Ajouter un jeu = écrire un `GameModule` (enter/update/render sur `gb_graphics`)
et l'ajouter à `GAME_LIST` dans `games.cpp` ; le sélecteur et les features
globales suivent automatiquement.

---

## 2. Les jeux

| Jeu            | État                    | Rendu                         | Son            | Score SD        |
|----------------|-------------------------|-------------------------------|----------------|-----------------|
| Parachute      | complet                 | assets HD (a27ds), plein écran| WAV G&W (SD)   | nominatif       |
| Donkey Kong Jr | porté (valider au build)| moteur Steph via `gnw`        | tons           | via `gb.save`→SD|
| Octopus        | porté (valider au build)| moteur header-only via `gnw`  | tons           | nominatif       |
| Formula 1      | porté (valider au build)| cœur Tufty (framebuffer)      | tons           | nominatif       |
| Fire!          | 1er jet (build à valider)| firebuino via shim `gb.display`| tons           | via `gb.save`→SD|
| Nu, pogodi!    | reconstruit (build à valider)| art MIT rasterisé (SVG)   | tons           | nominatif       |

### Descriptif, règles et commandes

**Parachute** — Rattrape les parachutistes dans ton bateau ; en laisser tomber
un (eau / requin) coûte une vie ; 3 ratés = fin de partie.
Commandes : `GAUCHE/DROITE` déplacer le bateau · `A` démarrer · `RUN` pause.

**Donkey Kong Jr** — Aide Junior à grimper aux lianes, évite oiseaux et
mâchoires, récupère la clé pour libérer Papa.
Commandes : `GAUCHE/DROITE` se déplacer · `A`/`HAUT` sauter/grimper · `BAS`
descendre · `B` fruit · `A` démarrer.

**Octopus** — Plonge chercher le trésor et remonte-le au bateau, évite les
tentacules de la pieuvre ; une prise = une vie perdue.
Commandes : `GAUCHE` avancer/plonger · `DROITE` reculer · `RUN` pause.

**Formula 1** — Évite les voitures adverses en changeant de voie ; la vitesse
augmente avec la distance.
Commandes : `HAUT/BAS` changer de voie · `A` démarrer.

Les textes (nom, règles, commandes) sont localisés dans `main/gametexts.cpp`
(**FR + EN** rédigés ; DE/ES/IT repliés sur EN, à compléter). En jeu, les
mêmes commandes globales s'appliquent partout :
`MENU` (court) options · `MENU` (long) capture · `RUN`+`MENU` (500 ms) loader ·
au sélecteur, `A` jouer · `B` infos/règles/scores · `MENU` options.

---

## 3. Emplacement des sources

### Dépôts d'origine (portés)
- Parachute (META, jicehel) : `github.com/jicehel/Parachute_Meta_V2`
  - sprites couleur HD : `github.com/a27ds/Parachute` (`Assets/_Game/Sprites`)
  - sons WAV : `github.com/delphiunderground/parachute_sdl` (`Sound`)
- Donkey Kong Jr (META, Steph) : `github.com/m1cr0lab-gamebuino/gb-donkey-kong-jr`
- Octopus (META, Frakasss/jicehel) : `github.com/Frakasss/Octopus_HD`
- Formula 1 (Tufty2350, joyrider3774) : `github.com/joyrider3774/formula1_tufty2350`
- Squelette / lib AKA : `AKAsseBricks` / `pAKAman` (composant `gamebuino` + couche `core/`+`ui/`)

### Dans le projet (`main/`)
- Shell : `app_main.cpp`, `game_module.h`, `games.cpp`, `select_screen.*`
- Couche partagée : `gnw/` (`gnw_present`, `Gamebuino-Meta` shim, `gnw_l10n`, `gnw_scores`)
- Infos/scores : `info_screen.*`, `gametexts.*`, `icons.*`
- Parachute : `parachute.cpp`, `render.*`, `sfx.*`, `assets_hd.*`, `Global.h`,
  `Animate.h`, `globals.cpp`, `arduino_compat.h`
- Donkey Kong Jr : `dkjr/dkjr.cpp` + moteur d'origine dans `dkjr/src_meta/`
- Octopus : `octopus.cpp` + moteur dans `octopus/`
- Formula 1 : `f1_module.cpp`, `f1_aka_sound.cpp`, `f1_aka_savestate.cpp`,
  cœur dans `f1/src/`, shim Pico dans `f1/compat/`
- Couche globale à copier du squelette : `core/`, `ui/` (cf. §5)
- Extension de la lib : `../lib_extension/` (`gb_graphics_image.cpp` + déclarations)
- Sons sur SD : `../sdcard/PARACHUTE/`
- Outil : `tools/conv_sprites.py` (génération d'assets)

---

## 4. Montage & build

1. Récupérer le composant `components/gamebuino/` du squelette (AKAsseBricks/pAKAman)
   + `sdkconfig`.
2. **Étendre la lib** (une fois) : coller les déclarations de
   `lib_extension/gb_graphics.h.additions.txt` dans `gb_graphics.h`, et ajouter
   `gb_graphics_image.cpp` aux `SRCS` du composant (`drawImage` + `drawImageScaled`).
3. Copier `sdcard/PARACHUTE/` sur la carte SD.
4. `idf.py set-target esp32s3 && idf.py build flash monitor`.

---

## 4bis. Shim `gb.display` (jeux META « standard »)

`gnw/gb_display.*` ajoute un `gb.display` + `Image` indexée (80×64) rendu dans un
back-buffer puis mis à l'échelle 320×240. Il débloque les jeux META qui utilisent
l'API standard (pas le patron HD par tranches) — **Fire!** est le premier. Les
jeux qui *possèdent leur boucle* (`while(true){ if(!gb.update())… }`) sont pilotés
via `gb.update()` + un pont `setjmp/longjmp` pour ressortir au sélecteur.

À VALIDER au build : la palette exacte `INDEX_GREEN`, l'entête/paquetage des
images indexées (`IMG_HDR` dans `gb_display.cpp`), et le pont de boucle.

## 5. Couche globale à copier du squelette (`main/core/`, `main/ui/`)

Réutilisée telle quelle (comme le composant `gamebuino`) :
`core/input`, `core/graphics` (capture BMP), `core/audio` (player, `g_sfx`,
`audio_game_init`, `audio_set_volume`), `core/i18n`, `core/settings`,
`core/sdcard`, `ui/menu`, `ui/highscores`.

**3 retouches** (chemins spécifiques au jeu) : `core/settings.cpp`,
`ui/highscores.cpp`, `core/graphics.cpp` → remplacer `PAKAMAN` par le dossier
`PARACHUTE`/collection.

---

## 6. Ce qui est fait

- Shell carrousel (icônes réelles Parachute/DKJr/Octopus, points de pagination,
  défilement fluide) + champ `icon` optionnel par jeu.
- Parachute : portage complet (assets HD a27ds plein écran, conversion
  RGB565→BGR565, sons WAV rééchantillonnés 44,1 kHz sur SD).
- DKJr : moteur de Steph réutilisé tel quel, transfert `Renderer` redirigé vers
  `gnw_present` ; `gb.save` persiste sur SD.
- Octopus : moteur header-only réutilisé, transfert redirigé, isolé en namespace.
- Formula 1 : cœur portable compilé, colle AKA (présent BE→BGR565, boutons, son,
  savestate) + shim Pico minimal (`time_us_32`).
- Features globales : menu options, capture, retour sélecteur, retour loader,
  calibrage.
- Multilingue : `gnw_l10n` (langue suivie depuis les settings) ; noms, règles,
  commandes et page infos localisés (FR/EN).
- Scores : `gnw_scores` (top 5 par jeu sur SD) + **saisie de nom 1–8 caractères**
  quand on entre dans le top 5 ; page infos (touche `B`) affiche rang · nom · score.
- Langue sélectionnable depuis le menu du sélecteur, persistée dans les settings.

---

## 7. Ce qu'il reste à faire

- Compléter les traductions **DE / ES / IT** dans `gametexts.cpp`.
- Fournir des **icônes** pour Formula 1 (et affiner celles existantes si besoin).
- Sprites couleur HD pour DKJr / Octopus (à déposer directement, les URLs
  externes ne sont pas récupérables) puis même pipeline que Parachute.
- Roadmap : OilPanic / Kong / FirePanic (Press-Play-On-Tape, Arduboy 1-bit) —
  logique réutilisable, rendu à coloriser, même shell.

---

## 8. Points à vérifier au premier build (pas de compilateur ici)

- **SD montée** (`sd_init` du squelette) : requise pour scores, capture, `gb.save`.
- **Langue** : `app_main` définit `language` (comme `volume`) et le synchronise
  vers `gnw_lang_index` ; si le squelette nomme la langue autrement, pointer la
  synchro dessus. Ordre supposé FR, EN, DE, ES, IT.
- **DKJr** : compléter le shim `gnw/Gamebuino-Meta.h` si un symbole Gamebuino
  manque au build ; vérifier le masque couleur des sprites au rendu.
- **Octopus** : vérifier le nom exact du global `score`.
- **Formula 1** : si les couleurs sortent inversées, ajuster le `present`
  (`bswap16` / `swap_rb`) dans `f1_module.cpp` ; vérifier que le cœur ne tire pas
  d'autre symbole Pico que `time_us_32` (déjà shimé).
- **Symboles** : le `gameState` global de Parachute a été renommé `paraState`
  pour ne pas heurter celui de F1.

## Attributions
- Nu, pogodi! : art vectoriel (c) 2014 Paweł Winiecki, licence MIT
  (pawel-winiecki/nu-pogodi-html5-game). Notice MIT à conserver.
- Fire! : firebuino (makerSquirrel), art Erico P. Monteiro, LGPL v3.
- Parachute : sprites a27ds ; sons delphiunderground/parachute_sdl.
- Moteurs : DKJr (m1cr0lab/Steph), Octopus (Frakasss), F1 (joyrider3774).
