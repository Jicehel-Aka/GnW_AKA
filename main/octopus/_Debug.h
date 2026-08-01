//#######################################
  void debug(){
    if (subMove > 0) {
      // SerialUSB.print("11111 - Avant --");
      // SerialUSB.print("subMove = ");
      // SerialUSB.println(subMove);
      // SerialUSB.print("diverToShow = ");
      // SerialUSB.println(diverToShow);
      // oldVal = subMove;
      SerialUSB.print("MoveTick :");
      SerialUSB.print(moveTick);
      SerialUSB.print("  -  ");
      SerialUSB.print("MaxTick :");
      SerialUSB.print(maxTick);
      SerialUSB.print("  -  ");
      SerialUSB.print("animFrame :");
      SerialUSB.println(animFrame);
    }
  }
