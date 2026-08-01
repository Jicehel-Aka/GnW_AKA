#pragma once
#ifndef OCTOPUS_GLOBAL
#define OCTOPUS_GLOBAL

#define TITLESCREEN 0
#define MENU 2
#define RUN 3
#define PAUSE 4
#define GAMEOVER 5
#define CONTROLS 6
#define CREDITS 7

// -------------------------------------------------------------------------
// Global constants
// -------------------------------------------------------------------------

uint8_t gameState = 0;

constexpr uint8_t  screenWidth  = 160;
constexpr uint8_t  screenHeight = 128;
constexpr uint8_t  sliceHeight  = 8;                    // Redefined the size of the buffer to take less memory
constexpr uint8_t  slices = screenHeight / sliceHeight; // Number of horizontal slices to be cut is calculated
constexpr uint16_t transColor   = 0xdfff;
constexpr uint16_t sRed   = 0x00f8;
constexpr uint16_t sGreen = 0xe007;
constexpr uint16_t sPink  = 0x1ff8;
constexpr uint16_t sBlack = 0x0000;

// -------------------------------------------------------------------------
// Define global variables
// -------------------------------------------------------------------------

int16_t  moveTick;
int16_t  maxTick = 130;   // +30% de temps de reaction (etait 100)
uint16_t minHighscore;
uint8_t animFrame = 0;
bool catchGold = false;
bool hited = false;
bool inGold = false;
uint16_t score = 0;
uint16_t getPoint = 0;
uint8_t life = 3;
bool subGold = false;
uint8_t subMove = 0;
uint8_t barcaState = 0;
bool moveSubButton = true;
uint8_t dieFrame = 0;
bool endGame = false;
uint16_t highScore = 0;
uint8_t diverToShow = 0;
uint8_t diverArmToShow = 0;
uint8_t diverBagToShow = 0;
bool debug = false;
// just for debug
uint16_t oldVal;
uint8_t speedAnim = 4;
uint8_t menuOption = 1;
// -------------------------------------------------------------------------
// Sprite visibility
// -------------------------------------------------------------------------

bool showSpriteDiver1 = false;
bool showSpriteDiver1Arm1 = false;
bool showSpriteDiver1Arm2 = false;
bool showSpriteDiver2 = false;
bool showSpriteDiver3 = false;

// legs animation markers
uint8_t octopusLeg[4] = {1,1,1,1};
uint8_t octopusMoveLeg[4] = {1,1,1,1};

// legs lengths
const uint8_t octopusLegLength[4] = {4,5,4,3};
uint8_t octopusCurrentLegLength[4]= {0,0,0,0};
uint8_t octopusCurrentLeg1Length=0;
uint8_t octopusCurrentLeg2Length=0;
uint8_t octopusCurrentLeg3Length=0;
uint8_t octopusCurrentLeg4Length=0;


uint8_t octopusLegAnim[4] = {1, 1, 1, 1};
bool octopusLegReverse[4] = {false, false, false,false};
const uint16_t octopusLegFrame[4][4] {
  { 30,  60,  90, 300},
  { 25,  50,  75, 100},
  { 50,  90, 300, 300},
  { 10,  35,  55,  85},
};
#endif
