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
    Graphics_ChipScreen *screen = malloc( sizeof( ChipScreen ) );
    
    screen->screen[31] = 1;
    
    printf( "%llu", screen->screen[31] );
    
    free( screen );
    return 0;
}

