//
//  Graphics.h
//  Chip8
//
//  Created by Mike Lyons on 8/23/14.
//  Copyright (c) 2014 Mike Lyons. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#ifndef Chip8_Graphics_h
#define Chip8_Graphics_h

typedef struct Graphics_ChipScreen {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    char updated;
    char cleared;
} Graphics_ChipScreen;

// Initializes SDL, and returns a ChipScreen
Graphics_ChipScreen* Graphics_InitGraphics();

// Draws the given chip screen
void Graphics_DrawGraphics( Graphics_ChipScreen *screen );

// Draws a sprite onto the screen, doesn't update graphics
int Graphics_DrawSprite( Graphics_ChipScreen *screen,  int x, int y, uint32_t sprite[], int h );

void Graphics_ClearScreen( Graphics_ChipScreen *screen );

// Safely quits
void Graphics_Quit( Graphics_ChipScreen *screen );

#endif
