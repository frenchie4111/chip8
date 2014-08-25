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

int main(int argc, const char * argv[])
{
    Graphics_ChipScreen *screen = Graphics_InitGraphics( 1 );
    
    uint32_t sprite[ 24 ] = { 1, 1, 1, 1, 1, 1, 1, 1,
                              1, 0, 0, 0, 0, 0, 0, 1,
                              1, 1, 1, 1, 1, 1, 1, 1 };
    
    int col = Graphics_DrawSprite(screen, 5, 5, sprite, 3);
    Graphics_DrawGraphics(screen);
    
    col = Graphics_DrawSprite(screen, 7, 5, sprite, 3);
    Graphics_DrawGraphics(screen);
    
    while( 1 );
    
    Graphics_Quit( screen );
    
    return 0;
}

