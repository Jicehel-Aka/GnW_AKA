#include "OutputScreen_Sprites.h"
#include "OutputScreen_HDTools.h"

//################################################################################
void outputScreen_titleScreen(){
  drawBackground(titleImage, fullScreen, 0, 0, false);
}

//################################################################################
void outputScreen_menu(){
  drawMenu();
}

//################################################################################
void outputScreen_credits(){
  drawCredits();
}

//################################################################################
void outputScreen_controls(){
  drawBackground(IMAGE_CONTROLS, fullScreen, 0, 0, false);
}

//################################################################################
void outputScreen_gameOver(){
  drawBackground(background, spriteGameOver, 53, 60, true);
}

//################################################################################
void outputScreen_pause(){
  drawBackground(background, spritePauseScreen, 66, 60, true);
}

//################################################################################
void outputScreen_run(){
  draw();
}
