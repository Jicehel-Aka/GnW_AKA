#ifndef OCTOPUS_SOUNDS
#define OCTOPUS_SOUNDS

void outputSound_tick(){
  gb.sound.playTick();
}

void outputSound_gameOver(){
  gb.sound.play("gameOver.wav");
}

void outputSound_pause(){
  gb.sound.playCancel();
  //gb.sound.play("pauseScreen.wav");
}

#endif
