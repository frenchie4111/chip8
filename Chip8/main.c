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
    
    while( 1 );
    
    Graphics_Quit( screen );
    
    return 0;
}

