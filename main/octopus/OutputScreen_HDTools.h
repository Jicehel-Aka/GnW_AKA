#include "../gnw/gnw_present.h"
#include <cstddef>   // size_t
#include <cstring>   // memcpy

#ifndef HD_TOOLS
#define HD_TOOLS

// -------------------------------------------------------------------------
// Initialization related to the DMA controller
// -------------------------------------------------------------------------
static constexpr uint8_t DigitWidth      = 5; // width of each digit's sprite
static constexpr uint8_t DigitHeight     = 9; // height of each digit's sprite
static constexpr uint8_t DigitOffset     = 2; // offset in pixels between two digits
static constexpr uint8_t DigitSheetWidth = 10 * DigitWidth; // total width of the digits spritesheet

// rendering buffers
uint16_t buffer1[screenWidth * sliceHeight];
uint16_t buffer2[screenWidth * sliceHeight];

// flag for an ongoing data transfer
bool drawPending = false;

// -------------------------------------------------------------------------
// Memory transfer to DMA controller
// -------------------------------------------------------------------------
// initiates memory forwarding to the DMA controller....
void customDrawBuffer(uint8_t x, uint8_t y, uint16_t* buffer, uint8_t w, uint8_t h) {
  ::gnw_present_slice_bswap(y, h, buffer, w);   // Octopus (Frakasss) est en big-endian
  if (y + h >= screenHeight) ::gnw_present_flush();
}

// waits for the memory transfer to be completed
// and close the transaction with the DMA controller
void waitForPreviousDraw() {}

template< size_t size >
void extractDigits(uint8_t (&buffer)[size], uint16_t value) {
  for (uint8_t i = 0; i < size; ++i) {
    buffer[i] = value % 10;
    value /= 10;
  }
}

//###################################################################################################
//###################################################################################################
void drawSprite(Sprite sprite, uint8_t sliceY, uint16_t* buffer) {
  // Check if sprite has one part to show on the current slice
  if (sliceY < sprite.y + sprite.h && sprite.y < sliceY + sliceHeight) {
    // Determines the boundaries of the sprite surface within the current slice
    uint8_t  xMin = sprite.x;
    uint8_t  xMax = sprite.x + sprite.w - 1;
    uint8_t  yMin = sprite.y < sliceY ? sliceY : sprite.y;
    uint8_t  yMax = sprite.y + sprite.h >= sliceY + sliceHeight ? sliceY + sliceHeight - 1 : sprite.y + sprite.h - 1;
    uint8_t  px, py;
    uint16_t color;
    // Display the sprite pixels to be drawn
    for (py = yMin; py <= yMax; py++) {
      for (px = xMin; px <= xMax; px++) {
        // Picks the pixel color from the spritesheet
        color = spritesheet[px + py * screenWidth];
        // If color is the color selected for the Sprite
        if (color == sprite.color) {
          // Copies the color code into the rendering buffer  
          buffer[px + (py - sliceY) * screenWidth] = sBlack;
        }
      }
    }
  }
}

//###################################################################################################
//###################################################################################################
void drawItem(Sprite sprite, uint8_t sliceY, uint16_t* buffer, uint16_t xx, uint16_t yy) {
  // Check if sprite has one part to show on the current slice
  if (sliceY < (sprite.y+yy) + sprite.h && (sprite.y+yy) < sliceY + sliceHeight) {
    // Determines the boundaries of the sprite surface within the current slice
    uint16_t  xMin = sprite.x;
    uint16_t  xMax = sprite.x + sprite.w - 1;
    uint16_t  yMin = (sprite.y+yy) < sliceY ? sliceY : (sprite.y+yy);
    uint16_t  yMax = (sprite.y+yy) + sprite.h >= sliceY + sliceHeight ? sliceY + sliceHeight - 1 : (sprite.y+yy) + sprite.h - 1;
    uint16_t  px, py;
    uint16_t color;
    // Display the sprite pixels to be drawn
    for (py = yMin; py <= yMax; py++) {
      for (px = xMin; px <= xMax; px++) {
        // Picks the pixel color from the spritesheet
        color = IMAGE_ITEMS[px + (py-yy) * 355];
        // If color is the color selected for the Sprite
        if (color == sprite.color) {
          // Copies the color code into the rendering buffer 
          buffer[(px-xMin+xx) + (py - sliceY) * screenWidth] = sBlack;
          //buffer[(px-xMin+xx) + (py-yMin+yy - sliceY) * screenWidth] = sBlack;
        }
      }
    }
  }
}

//###################################################################################################
void drawDigitOnSlice(uint8_t digit, uint8_t index, uint8_t sliceY, uint16_t * buffer) {
  // calculates the coordinates of the sprite
  uint8_t dx = 119 - index * (DigitWidth + DigitOffset);
  uint8_t dy = 6;

  // we check first of all that the intersection between
  // the sprite and the current slice is not empty
  if (sliceY < dy + DigitHeight && dy < sliceY + sliceHeight) {
    // determines the boundaries of the sprite surface within the current slice
    uint8_t ymin = dy < sliceY ? sliceY : dy;
    uint8_t ymax = dy + DigitHeight >= sliceY + sliceHeight ? sliceY + sliceHeight - 1 : dy + DigitHeight - 1;
    // determines the memory address of the byte block to be copied
    uint16_t * source = (uint16_t *)DIGIT_COLORMAP + digit * DigitWidth;
    // determines the destination memory address in the framebuffer
    uint16_t * dest = buffer + dx;
    // the 16-bit color codes are copied byte by byte,
    // so a factor of 2 is required here
    uint8_t size = DigitWidth << 1;
    // then each memory block is scanned
    for (uint8_t py = ymin; py <= ymax; py++) {
      // and copied to the framebuffer
      memcpy(dest + (py - sliceY) * screenWidth, source + (py - dy) * DigitSheetWidth, size);
    }
  }
}

//###################################################################################################
void drawText(Sprite sprite, uint8_t sliceY, uint16_t* buffer, uint8_t x, uint8_t y) {
  if ((sliceY < (y + sprite.h)) && (y < (sliceY + sliceHeight))) {
    uint8_t xMin = x;
    uint8_t xMax = x + sprite.w - 1;
    uint8_t yMin = y < sliceY ? sliceY : y;
    uint8_t yMax = y + sprite.h >= sliceY + sliceHeight ? sliceY + sliceHeight - 1 : y + sprite.h - 1;

    uint8_t  px, py, sx, sy;
    uint16_t color;

    for (py = yMin; py <= yMax; ++py) {
      sy = py - y + sprite.y;
      for (px = xMin; px <= xMax; ++px) {
        sx = px - xMin + sprite.x;
        color = spritesheet[sx + sy * screenWidth];
        if (color == sRed) {
          buffer[px + (py - sliceY) * screenWidth] = sBlack;
        }
      }
    }
  }
}

//###################################################################################################
void drawBackground(const uint16_t * background, const Sprite spriteToDisplay, uint8_t spriteX, uint8_t spriteY, boolean displaySprite){
  constexpr size_t bufferSize = (sizeof(uint16_t) * screenWidth * sliceHeight);
  for (uint8_t sliceIndex = 0; sliceIndex < slices; sliceIndex++){
    uint16_t * buffer = (sliceIndex % 2 == 0) ? buffer1 : buffer2;                          // buffers are switched according to the parity of sliceIndex
    uint8_t sliceY = sliceIndex * sliceHeight;                                              // the top border of the current slice is calculated
    size_t backgroundOffset = (sliceY * screenWidth);
    memcpy(buffer, &background[backgroundOffset], bufferSize);                              // starts by drawing the background
    //if (displaySprite == true) {drawText(spriteToDisplay, sliceY, buffer, spriteX, spriteY);} // and finally draws the sprite if needed
    if (sliceIndex != 0){                                                                   // then we make sure that the sending of the previous buffer to the DMA controller has taken place
      waitForPreviousDraw();
    }
    customDrawBuffer(0, sliceY, buffer, screenWidth, sliceHeight);                          // after which we can then send the current buffer
  }
  waitForPreviousDraw();                                                                    // always wait until the DMA transfer is completed for the last slice before entering the next cycle 
}



//###################################################################################################
void draw() {
  // Declares a pointer that will alternate between the two memory buffers
  uint16_t* buffer;
  // Declares the top border of current slice
  uint8_t sliceY;
  // Go through each slice one by one
  for (uint8_t sliceIndex = 0; sliceIndex < slices; sliceIndex++) {
    // Buffers are switched according to the parity of sliceIndex
    buffer = sliceIndex % 2 == 0 ? buffer1 : buffer2;
    // Top border of the current slice is calculated
    sliceY = sliceIndex * sliceHeight;

    // Starts by drawing the background
    memcpy(buffer, background + sliceY * screenWidth, 2 * screenWidth * sliceHeight);


    // draw divers on barca
    if (showSpriteDiver1)drawSprite(spriteDiver1, sliceY, buffer);
    if (showSpriteDiver1Arm1)drawSprite(spriteDiver1Arm1, sliceY, buffer);
    if (showSpriteDiver1Arm2)drawSprite(spriteDiver1Arm2, sliceY, buffer);
    if (showSpriteDiver2)drawSprite(spriteDiver2, sliceY, buffer);
    if (showSpriteDiver3)drawSprite(spriteDiver3, sliceY, buffer);


    // draw the 3 parts of the diver if needed
    if (diverToShow > 0) drawSprite(diver[diverToShow - 1], sliceY, buffer);
    if (diverArmToShow > 0) drawSprite(diverArm[diverArmToShow - 1], sliceY, buffer);
    if (diverBagToShow > 0) drawSprite(diverBag[diverBagToShow - 1], sliceY, buffer);


    // draw octopus legs

    for(size_t index = 0; index < octopusCurrentLeg1Length; ++index){
      drawSprite(spriteOctopusLeg1[index], sliceY, buffer);
    }
   
  
    for(size_t index = 0; index < octopusCurrentLeg2Length; ++index){
      drawSprite(spriteOctopusLeg2[index], sliceY, buffer);
    }

    for(size_t index = 0; index < octopusCurrentLeg3Length; ++index){
      drawSprite(spriteOctopusLeg3[index], sliceY, buffer);
    }

    for(size_t index = 0; index < octopusCurrentLeg4Length; ++index){
      drawSprite(spriteOctopusLeg4[index], sliceY, buffer);
    }


    // Draw score
    const uint8_t nbDigits = 6;
    uint8_t digits[nbDigits] = {};
    extractDigits(digits, score);
    for (uint8_t i = 0; i < nbDigits; i++)
      drawDigitOnSlice(digits[nbDigits - i], nbDigits - i - 1, sliceY, buffer);


    // Verify that previous buffer has been sent to the DMA controller
    if (sliceIndex != 0) waitForPreviousDraw();
    // Then send the current buffer
    customDrawBuffer(0, sliceY, buffer, screenWidth, sliceHeight);
  }

  // Wait until the DMA transfer is completed before entering the next cycle
  waitForPreviousDraw();
}

//###################################################################################################
void drawMenu() { 
  constexpr size_t bufferSize = (sizeof(uint16_t) * screenWidth * sliceHeight);
  for (uint8_t sliceIndex = 0; sliceIndex < slices; sliceIndex++){
    uint16_t * buffer = (sliceIndex % 2 == 0) ? buffer1 : buffer2;                          // buffers are switched according to the parity of sliceIndex
    uint8_t sliceY = sliceIndex * sliceHeight;                                              // the top border of the current slice is calculated
    memcpy(buffer, background + sliceY * screenWidth, 2 * screenWidth * sliceHeight);

    drawItem(item_Menu, sliceY, buffer, 60, 5);
    drawItem(item_Game, sliceY, buffer, 40, 45);
    drawItem(item_Controls, sliceY, buffer, 40, 61);
    drawItem(item_Credits, sliceY, buffer, 40, 77);

    drawItem(item_Cursor, sliceY, buffer, 17, 29+(menuOption*16));
    
    if (sliceIndex != 0){                                                                   // then we make sure that the sending of the previous buffer to the DMA controller has taken place
      waitForPreviousDraw();
    }
    customDrawBuffer(0, sliceY, buffer, screenWidth, sliceHeight);                          // after which we can then send the current buffer
  }
  waitForPreviousDraw();                                                                    // always wait until the DMA transfer is completed for the last slice before entering the next cycle 
}

//###################################################################################################
void drawCredits(){
  constexpr size_t bufferSize = (sizeof(uint16_t) * screenWidth * sliceHeight);
  for (uint8_t sliceIndex = 0; sliceIndex < slices; sliceIndex++){
    uint16_t * buffer = (sliceIndex % 2 == 0) ? buffer1 : buffer2;                          // buffers are switched according to the parity of sliceIndex
    uint8_t sliceY = sliceIndex * sliceHeight;                                              // the top border of the current slice is calculated
    memcpy(buffer, background + sliceY * screenWidth, 2 * screenWidth * sliceHeight);

    drawItem(item_Credits, sliceY, buffer, 50, 5);
    
    drawItem(item_Jicehel, sliceY, buffer, 40, 45);
    drawItem(item_Steph, sliceY, buffer, 40, 61);
    drawItem(item_Frakasss, sliceY, buffer, 40, 77);
   
    if (sliceIndex != 0){                                                                   // then we make sure that the sending of the previous buffer to the DMA controller has taken place
      waitForPreviousDraw();
    }
    customDrawBuffer(0, sliceY, buffer, screenWidth, sliceHeight);                          // after which we can then send the current buffer
  }
  waitForPreviousDraw();                                                                    // always wait until the DMA transfer is completed for the last slice before entering the next cycle 
}
#endif
