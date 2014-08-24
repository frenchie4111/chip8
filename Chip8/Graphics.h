//
//  Graphics.h
//  Chip8
//
//  Created by Mike Lyons on 8/23/14.
//  Copyright (c) 2014 Mike Lyons. All rights reserved.
//

#include <stdio.h>

#include <SDL2/SDL.h>

#ifndef Chip8_Graphics_h
#define Chip8_Graphics_h

typedef struct Graphics_ChipScreen {
    char screen[ 64 * 32 ];
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} Graphics_ChipScreen;

// Initializes SDL, and returns a ChipScreen
Graphics_ChipScreen* Graphics_InitGraphics( int scale );

// Draws the given chip screen
void Graphics_DrawGraphics( Graphics_ChipScreen *screen );

// Safely quits
void Graphics_Quit( Graphics_ChipScreen *screen );

#endif
