#include "gametexts.h"
#include "gnw/gnw_l10n.h"
#include <cstddef>

// Order: FR, EN, DE, ES, IT. nullptr -> English fallback (handled by L()).

// ---- Parachute ----
const char* const PARA_NAME[LANG_COUNT]  = { "Parachute", "Parachute", nullptr, nullptr, nullptr };
const char* const PARA_CTRL[LANG_COUNT]  = {
    "GAUCHE/DROITE : deplacer le bateau\nA : demarrer   RUN : pause",
    "LEFT/RIGHT: move the boat\nA: start   RUN: pause",
    nullptr, nullptr, nullptr };
const char* const PARA_RULES[LANG_COUNT] = {
    "Rattrape les parachutistes dans ton bateau.\nEn laisser tomber un (eau / requin) coute une vie.\n3 rates = fin de partie.",
    "Catch the falling parachutists in your boat.\nMissing one (water / shark) costs a life.\n3 misses = game over.",
    nullptr, nullptr, nullptr };

// ---- Donkey Kong Jr ----
const char* const DKJR_NAME[LANG_COUNT]  = { "Donkey Kong Jr", "Donkey Kong Jr", nullptr, nullptr, nullptr };
const char* const DKJR_CTRL[LANG_COUNT]  = {
    "GAUCHE/DROITE : se deplacer\nA ou HAUT : sauter/grimper   BAS : descendre\nB : fruit    A : demarrer",
    "LEFT/RIGHT: move\nA or UP: jump/climb   DOWN: descend\nB: fruit    A: start",
    nullptr, nullptr, nullptr };
const char* const DKJR_RULES[LANG_COUNT] = {
    "Aide Junior a grimper aux lianes.\nEvite les oiseaux et les machoires.\nRecupere la cle pour liberer Papa.",
    "Help Junior climb the vines.\nAvoid the birds and the snapjaws.\nGrab the key to free Papa.",
    nullptr, nullptr, nullptr };

// ---- Octopus ----
const char* const OCTO_NAME[LANG_COUNT]  = { "Octopus", "Octopus", nullptr, nullptr, nullptr };
const char* const OCTO_CTRL[LANG_COUNT]  = {
    "GAUCHE : avancer/plonger   DROITE : reculer\nRUN : pause",
    "LEFT: advance/dive   RIGHT: retreat\nRUN: pause",
    nullptr, nullptr, nullptr };
const char* const OCTO_RULES[LANG_COUNT] = {
    "Plonge chercher le tresor et remonte-le au bateau.\nEvite les tentacules de la pieuvre.\nUne prise = une vie perdue.",
    "Dive for the treasure and bring it back to the boat.\nAvoid the octopus tentacles.\nGetting caught costs a life.",
    nullptr, nullptr, nullptr };

// ---- Formula 1 ----
const char* const F1_NAME[LANG_COUNT]  = { "Formula 1", "Formula 1", nullptr, nullptr, nullptr };
const char* const F1_CTRL[LANG_COUNT]  = {
    "GAUCHE/DROITE : changer de voie\nA : demarrer",
    "LEFT/RIGHT: change lane\nA: start",
    nullptr, nullptr, nullptr };
const char* const F1_RULES[LANG_COUNT] = {
    "Evite les voitures adverses en changeant de voie.\nLa vitesse augmente avec la distance.",
    "Dodge the other cars by switching lanes.\nSpeed rises the further you go.",
    nullptr, nullptr, nullptr };

// ---- Fire! (firebuino) ----
const char* const FIRE_NAME[LANG_COUNT]  = { "Fire!", "Fire!", nullptr, nullptr, nullptr };
const char* const FIRE_CTRL[LANG_COUNT]  = {
    "GAUCHE/DROITE : deplacer les sauveteurs\nA : demarrer",
    "LEFT/RIGHT: move the rescuers\nA: start",
    nullptr, nullptr, nullptr };
const char* const FIRE_RULES[LANG_COUNT] = {
    "Avec le trampoline, rattrape les gens qui sautent de\nl'immeuble en feu et renvoie-les vers l'ambulance.\nEn laisser tomber un coute une vie.",
    "Bounce the people jumping\nfrom the burning building\ntoward the ambulance.\nDropping one costs a life.",
    nullptr, nullptr, nullptr };

// ---- Nu, pogodi! (art MIT (c) 2014 Pawel Winiecki) ----
const char* const NUPO_NAME[LANG_COUNT]  = { "Nu, pogodi!", "Nu, pogodi!", nullptr, nullptr, nullptr };
const char* const NUPO_CTRL[LANG_COUNT]  = {
    "FLECHES : placer le panier aux 4 coins\nA : demarrer",
    "ARROWS: move the basket to the 4 corners\nA: start",
    nullptr, nullptr, nullptr };
const char* const NUPO_RULES[LANG_COUNT] = {
    "Le loup rattrape dans son panier les oeufs qui\ndevalent les 4 goulottes. Un oeuf casse = une vie.",
    "The wolf catches the eggs rolling down the 4 chutes\nin his basket. A broken egg costs a life.",
    nullptr, nullptr, nullptr };

// ---- Manhole (sprites IA + decor original) ----
const char* const MANHOLE_NAME[LANG_COUNT]  = { "Manhole", "Manhole", nullptr, nullptr, nullptr };
const char* const MANHOLE_CTRL[LANG_COUNT]  = {
    "FLECHES : glisser la plaque sous l'un des 4 trous\nA : demarrer",
    "ARROWS: slide the cover under one of the 4 holes\nA: start",
    nullptr, nullptr, nullptr };
const char* const MANHOLE_RULES[LANG_COUNT] = {
    "Glisse la plaque sous les pietons pour boucher le trou.\nUn pieton qui tombe coute une vie.",
    "Slide the cover under the pedestrians to bridge the hole.\nA pedestrian who falls costs a life.",
    nullptr, nullptr, nullptr };
