void ennemy_octopus() {

  for (size_t indexLeg = 0; indexLeg < 4; indexLeg++) {
    if (moveTick == octopusLegFrame[indexLeg][0] || moveTick == octopusLegFrame[indexLeg][1] || moveTick == octopusLegFrame[indexLeg][2] || moveTick == octopusLegFrame[indexLeg][3]) {
      if(indexLeg==0){
        if(octopusLegReverse[indexLeg] == false){
          octopusCurrentLeg1Length=octopusCurrentLeg1Length+1;
          if(octopusCurrentLeg1Length==octopusLegLength[indexLeg]){
            octopusLegReverse[indexLeg]=true;
          }
        }else{
          octopusCurrentLeg1Length=octopusCurrentLeg1Length-1;
          if(octopusCurrentLeg1Length==0){
            octopusLegReverse[indexLeg]=false;
          }
        }
      }

      if(indexLeg==1){
        if(octopusLegReverse[indexLeg] == false){
          octopusCurrentLeg2Length=octopusCurrentLeg2Length+1;
          if(octopusCurrentLeg2Length==octopusLegLength[indexLeg]){
            octopusLegReverse[indexLeg]=true;
          }
        }else{
          octopusCurrentLeg2Length=octopusCurrentLeg2Length-1;
          if(octopusCurrentLeg2Length==0){
            octopusLegReverse[indexLeg]=false;
          }
        }
      }

      if(indexLeg==2){
        if(octopusLegReverse[indexLeg] == false){
          octopusCurrentLeg3Length=octopusCurrentLeg3Length+1;
          if(octopusCurrentLeg3Length==octopusLegLength[indexLeg]){
            octopusLegReverse[indexLeg]=true;
          }
        }else{
          octopusCurrentLeg3Length=octopusCurrentLeg3Length-1;
          if(octopusCurrentLeg3Length==0){
            octopusLegReverse[indexLeg]=false;
          }
        }
      }

      if(indexLeg==3){
        if(octopusLegReverse[indexLeg] == false){
          octopusCurrentLeg4Length=octopusCurrentLeg4Length+1;
          if(octopusCurrentLeg4Length==octopusLegLength[indexLeg]){
            octopusLegReverse[indexLeg]=true;
          }
        }else{
          octopusCurrentLeg4Length=octopusCurrentLeg4Length-1;
          if(octopusCurrentLeg4Length==0){
            octopusLegReverse[indexLeg]=false;
          }
        }
      }
      /*
      //gb.sound.playTick();
      if(octopusLegReverse[indexLeg] == false){
        octopusCurrentLegLength[indexLeg]=octopusCurrentLegLength[indexLeg]+1;
        if(octopusCurrentLegLength[indexLeg]==octopusLegLength[indexLeg]){
          octopusLegReverse[indexLeg]=true;
        }
      }else{
        octopusCurrentLegLength[indexLeg]=octopusCurrentLegLength[indexLeg]-1;
        if(octopusCurrentLegLength[indexLeg]==0){
          octopusLegReverse[indexLeg]=false;
        }
      }*/
    }
  }
}


void ennemy_OctopusCollision() {
  // Utilise les VRAIES longueurs de patte (mises a jour par ennemy_octopus),
  // pas octopusLegAnim[] qui n'est jamais modifie.
  const uint8_t curLen[4] = { octopusCurrentLeg1Length, octopusCurrentLeg2Length,
                              octopusCurrentLeg3Length, octopusCurrentLeg4Length };
  for (size_t indexLeg = 0; indexLeg < 4; ++indexLeg) {
    if ((curLen[indexLeg] == octopusLegLength[indexLeg]) && (subMove == 2 + indexLeg)) {
      getPoint = 0;
      hited = true;
    }
  }
}
