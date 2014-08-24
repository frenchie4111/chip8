//
//  Graphics.c
//  Chip8
//
//  Created by Mike Lyons on 8/23/14.
//  Copyright (c) 2014 Mike Lyons. All rights reserved.
//

#include "Graphics.h"

Graphics_ChipScreen *Graphics_InitGraphics( int scale ) {
    Graphics_ChipScreen *new_screen = malloc( sizeof( Graphics_ChipScreen ) );
    
    SDL_Init( SDL_INIT_EVERYTHING );
    
    SDL_CreateWindowAndRenderer( 64 * scale,
                                32 * scale,
                                SDL_WINDOW_SHOWN,
                                &new_screen->window,
                                &new_screen->renderer);
    
    new_screen->texture = SDL_CreateTexture(new_screen->renderer,
                                            SDL_PIXELFORMAT_RGB888,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            64 * scale,
                                            32 * scale);
    
    void *pixels;
    
    return new_screen;
}

