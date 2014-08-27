//
//  main.c
//  Chip8
//
//  Created by Mike Lyons on 8/23/14.
//  Copyright (c) 2014 Mike Lyons. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "Graphics.h"
#include "chip8.h"

int main(int argc, const char * argv[])
{
    Graphics_ChipScreen *screen = Graphics_InitGraphics( 1 );
    Chip8 *chip = Chip8_Initialize( screen );
    Chip8_LoadRom( chip, "MAZE" );
    
    while( 1 ) {
        Chip8_ProcessCommand( chip );
        
        if( chip->redraw ) {
            Graphics_DrawGraphics( chip->screen );
            chip->redraw = 0;
        }
    }
    
    return 0;
}

