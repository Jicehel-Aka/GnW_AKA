

void game_initGame(){
  gameState = TITLESCREEN;
 
  animFrame = 0;
  catchGold = false;
  inGold = false;
  score = 0;
  getPoint = 0;
  life = 3;
  subGold = false;
  subMove = 0;
  barcaState = 0;
  moveSubButton = true;
  dieFrame = 0;
  endGame = false;
  hited = false;
}

//#######################################
  void game_checkGameOver(){
    if (life == 0) {
      gameState = GAMEOVER;
      //game_recordScore();
    }
  }

//#######################################
  void game_recordScore(){
    if (score > highScore) {
      highScore = score;
    }
  }

//#######################################
  void game_checkGoldInBarca(){
    //Character come back to barca at least 1 time => gold saved
    if (subMove == 0 && subGold == true && inGold == false) {
      inGold = true;
    } else {
      inGold = false;
    }

    //Define how many Character need to be displayed in Barca + goldYN
    if(subGold == true){
      if (subMove == 0 && life == 3) {barcaState = 2;}
      if (subMove == 0 && life == 2) {barcaState = 5;} 
      if (subMove == 0 && life == 1) {barcaState = 8;}       
    }else{
      if (subMove > 0 && life == 3) {barcaState = 1;}
      if (subMove > 0 && life == 2) {barcaState = 4;}
      if (subMove > 0 && life == 1) {barcaState = 7;}
    }
  }

//#######################################
byte animBarca = 0;

void Barca() {
  if (barcaState == 0 && hited == false) {
    showSpriteDiver1 = true;
    showSpriteDiver1Arm1 = true;
    showSpriteDiver1Arm2 = false;
    showSpriteDiver2 = true;
    showSpriteDiver3 = true;
  } else if (barcaState == 1 && hited == false) {
    showSpriteDiver1 = false;
    showSpriteDiver1Arm1 = false;
    showSpriteDiver1Arm2 = false;
    showSpriteDiver2 = true;
    showSpriteDiver3 = true;
  } else if (barcaState == 2 && hited == false) {
    if (animBarca == 0 || animBarca == 2 ) {
      showSpriteDiver1 = true;
      showSpriteDiver1Arm1 = true;
      showSpriteDiver1Arm2 = false;
      showSpriteDiver2 = true;
      showSpriteDiver3 = true;
    } else if (animBarca == 1 || animBarca == 3) {
      showSpriteDiver1 = true;
      showSpriteDiver1Arm1 = false;
      showSpriteDiver1Arm2 = true;
      showSpriteDiver2 = true;
      showSpriteDiver3 = true;
    } else if (animBarca == 4) {
      score = score + getPoint;
      subGold = false;
      animBarca = 0;
      barcaState = 0;
    }
    if  ((moveTick % speedAnim) == 0) {
      animBarca++;
      //outputSound_tick();
    }
  } else if (barcaState == 3 && hited == false) {
    showSpriteDiver1 = true;
    showSpriteDiver1Arm1 = true;
    showSpriteDiver1Arm2 = false;
    showSpriteDiver2 = true;
    showSpriteDiver3 = false;
  } else if (barcaState == 4 && hited == false) {
    showSpriteDiver1 = false;
    showSpriteDiver1Arm1 = false;
    showSpriteDiver1Arm2 = false;
    showSpriteDiver2 = true;
    showSpriteDiver3 = false;
  } else if (barcaState == 5 && hited == false) {
    if (animBarca == 0 || animBarca == 2) {
      showSpriteDiver1 = true;
      showSpriteDiver1Arm1 = true;
      showSpriteDiver1Arm2 = false;
      showSpriteDiver2 = true;
      showSpriteDiver3 = false;
    }
    if (animBarca == 1 || animBarca == 3) {
      showSpriteDiver1 = true;
      showSpriteDiver1Arm1 = false;
      showSpriteDiver1Arm2 = true;
      showSpriteDiver2 = true;
      showSpriteDiver3 = false;
    }
    if (animBarca == 4) {
      score = score + getPoint;
      subGold = false;
      animBarca = 0;
      barcaState = 3;
    }
    if ((moveTick % speedAnim) == 0) {
      animBarca++;
      //outputSound_tick();
    }
  } else if (barcaState == 6 && hited == false) {
    showSpriteDiver1 = true;
    showSpriteDiver1Arm1 = true;
    showSpriteDiver1Arm2 = false;
    showSpriteDiver2 = false;
    showSpriteDiver3 = false;
  } else if (barcaState == 7 && hited == false) {
    showSpriteDiver1 = false;
    showSpriteDiver1Arm1 = false;
    showSpriteDiver1Arm2 = false;
    showSpriteDiver2 = false;
    showSpriteDiver3 = false;
  } else if (barcaState == 8 && hited == false) {
    if (animBarca == 0 || animBarca == 2) {
      showSpriteDiver1 = true;
      showSpriteDiver1Arm1 = true;
      showSpriteDiver1Arm2 = false;
      showSpriteDiver2 = false;
      showSpriteDiver3 = false;
    }
    if (animBarca == 1 || animBarca == 3) {
      showSpriteDiver1 = true;
      showSpriteDiver1Arm1 = false;
      showSpriteDiver1Arm2 = true;
      showSpriteDiver2 = false;
      showSpriteDiver3 = false;
    }
    if (animBarca == 4) {
      score = score + getPoint;
      subGold = false;
      animBarca = 0;
      barcaState = 9;
    }
    if ((moveTick % speedAnim) == 0) {
      animBarca++;
      //outputSound_tick();
    }
  } else if (barcaState == 9 && hited == false) {
    showSpriteDiver1 = true;
    showSpriteDiver1Arm1 = true;
    showSpriteDiver1Arm2 = false;
    showSpriteDiver2 = false;
    showSpriteDiver3 = false;
  } else if (barcaState == 10) {
    showSpriteDiver1 = false;
    showSpriteDiver1Arm1 = false;
    showSpriteDiver1Arm2 = false;
    showSpriteDiver2 = false;
    showSpriteDiver3 = false;
  }
}



byte frameSub = 0;

void die() {
  moveSubButton = false;
  for (size_t index = 0; index < 4; ++index)
    //showOctopusLeg3[index] = true;
  diverToShow = 6;
  if ((frameSub == 0) || (frameSub == 2) || (frameSub == 4) || (frameSub == 6)) {
    diverArmToShow = 4;
    diverBagToShow = 6;
  }
  if (( frameSub == 1) || (frameSub == 3) || (frameSub == 5) || (frameSub == 7)) {
    diverArmToShow = 5;
    diverBagToShow = 7;
  }
}

void animSubGold() {
  if (subGold == false) {
    if (animFrame == 0) {
      diverToShow = 5;
      diverArmToShow = 1;
      diverBagToShow = 0;
    }
    if (animFrame == 1) {
      diverToShow = 5;
      diverArmToShow = 2;
      diverBagToShow = 0;
    }
    if (animFrame == 2) {
      diverToShow = 5;
      diverArmToShow = 3;
      diverBagToShow = 0;
    }
    if (animFrame == 3) {
      ++score;
      ++getPoint;
      // gb.sound.play("OR.WAV");
      gb.sound.playOK();
      subGold = true;
      subMove = 5;
      animFrame = 1;
      catchGold = false;
    }
  } else if (subGold == true) {
    if (animFrame == 0) {
      diverToShow = 5;
      diverArmToShow = 1;
      diverBagToShow = 5 ;
    }
    if (animFrame == 1) {
      diverToShow = 5;
      diverArmToShow = 2;
      diverBagToShow = 5 ;
    }
    if (animFrame == 2) {
      diverToShow = 5;
      diverArmToShow = 3;
      diverBagToShow = 5 ;
    }
    if (animFrame == 3) {
      ++score;
      ++getPoint;
      // gb.sound.play("OR.WAV");
      gb.sound.playOK();
      subGold = true;
      subMove = 5;
      animFrame = 0;
      catchGold = false;
    }
  }
  if ((moveTick % speedAnim) == 0) {
    animFrame++;
  }
}

void moveSub(){
  // If diger hasn't been hited
  if (hited == false) {
    if (subGold == false) {
      // if diver has no bag
      diverToShow = subMove;
      diverBagToShow = 0;
      if (subMove == 0) diverArmToShow = 0;
    } else {
      // if diver has a gold bag
      if (subMove == 0) {
        diverToShow = 0;
        diverBagToShow = 0;
        diverArmToShow = 0;
      } else {
        diverToShow = subMove;
        diverBagToShow = 5 - subMove;
      }
    }
  }
  if (hited == true && life == 3) {
    animBarca = 0;
    if (animBarca == 0) {
      showSpriteDiver1 = true;
      showSpriteDiver1Arm1 = true;
      showSpriteDiver1Arm2 = false;
      showSpriteDiver2 = false;
      showSpriteDiver3 = true;
    }

    if (animBarca == 1) {
      showSpriteDiver1 = true;
      showSpriteDiver1Arm1 = true;
      showSpriteDiver1Arm2 = false;
      showSpriteDiver2 = true;
      showSpriteDiver3 = false;
    }
    if (moveTick % speedAnim == 0) {
      animBarca++;
    }
    die();
    if (frameSub == 8) {
      barcaState = 3;
      hited = false;
      subMove = 0;
      inGold = false;
      subGold = false;
      --life;
      frameSub = 0;
      moveSubButton = true;
    }
    if ((moveTick % speedAnim) == 0) {
      frameSub++;
      gb.sound.playTick();
    }
  } else if (hited == true && life == 2) {
    animBarca = 0;
    if (animBarca == 0) {
      showSpriteDiver1 = false;
      showSpriteDiver1Arm1 = false;
      showSpriteDiver1Arm2 = true;
      showSpriteDiver2 = false;
      showSpriteDiver3 = false;
    }
    if (animBarca == 1) {
      showSpriteDiver1 = true;
      showSpriteDiver1Arm1 = true;
      showSpriteDiver1Arm2 = false;
      showSpriteDiver2 = false;
      showSpriteDiver3 = false;
    }
    if ((moveTick % speedAnim) == 0) {
      animBarca++;
    }
    die();
    if (frameSub == 8) {
      barcaState = 6;
      hited = false;
      subMove = 0;
      inGold = false;
      subGold = false;
      --life;
      frameSub = 0;
      moveSubButton = true;
    }
    if ((moveTick % speedAnim) == 0) {
      frameSub++;
      gb.sound.playTick();
    }
  } else if (hited == true && life == 1) {
    moveSubButton = false;
    showSpriteDiver1 = false;
    showSpriteDiver1Arm1 = false;
    showSpriteDiver1Arm2 = false;
    showSpriteDiver2 = false;
    showSpriteDiver3 = false;
    die();
    if (frameSub == 8) {
      barcaState = 10;
      hited = false;
      subMove = 0;
      inGold = false;
      subGold = false;
      --life;
      frameSub = 0;
      moveSubButton = true;
    }
    if ((moveTick % speedAnim) == 0) {
      frameSub++;
      gb.sound.playTick();
    }
  }
}

void Sub(){      
  if (subMove < 6) {
    moveSub();
  } else {
    animSubGold();
  }
}
