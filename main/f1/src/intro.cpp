#include <stdint.h>
#include "commonvars.h"
#include "printfuncs.h"
#include "intro.h"
#include "images/bigfont_RGB565_BE.h"
#include "framebuffer.h"

#define FRAMEDELAY 60


int ay;

void initIntro(void)
{
    ay = fb.height;
}

void intro(void)
{
    if (gameState == gsInitIntro)
    {
        initIntro();
        gameState -= gsInitDiff;
    }
    
    frames++;
    bufferFillRect(&fb,0,0,fb.width,fb.height, 0x0000);
    if (frames < FRAMEDELAY)
    {
        printMessage(bigfont_data, (14 - 12) >> 1, 4, 24, "WILLEMS DAVY");
    }
    else
    {
        if (frames < FRAMEDELAY *2)
        {
            printMessage(bigfont_data, (14-8) >> 1, 4, 24, "PRESENTS");
        }
        else
        {
            if(ay > 0)
            {
                ay -= 10;
            }
            else
            {
                gameState = gsInitGameIntro;
            }
        }
    }
        
    if ((buttons & BUTTON_C_MASK) && !(prevbuttons & BUTTON_C_MASK)|| 
		((buttons & BUTTON_A_MASK) && !(prevbuttons & BUTTON_A_MASK))||
		((buttons & BUTTON_B_MASK) && !(prevbuttons & BUTTON_B_MASK)))
    {            
        gameState = gsInitGameIntro;
    }

    if (buttons & BUTTON_UP_MASK)
    {
        debugMode = 1;
    }
}
