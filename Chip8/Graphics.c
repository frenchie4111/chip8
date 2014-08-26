//
//  Graphics.c
//  Chip8
//
//  Created by Mike Lyons on 8/23/14.
//  Copyright (c) 2014 Mike Lyons. All rights reserved.
//

#include "Graphics.h"

static void Graphics_NewTexture( Graphics_ChipScreen *screen ) {
    screen->texture = SDL_CreateTexture(screen->renderer,
                                        SDL_PIXELFORMAT_RGB888,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        64,
                                        32);
}

Graphics_ChipScreen *Graphics_InitGraphics() {
    Graphics_ChipScreen *new_screen = malloc( sizeof( Graphics_ChipScreen ) );
    
    SDL_Init( SDL_INIT_EVERYTHING );
    
    SDL_CreateWindowAndRenderer( 64,
                                 32,
                                 SDL_WINDOW_FULLSCREEN_DESKTOP,
                                 &new_screen->window,
                                 &new_screen->renderer);
    
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_RenderSetLogicalSize(new_screen->renderer, 64, 32);
    
    Graphics_NewTexture( new_screen );
    
    new_screen->cleared = 0;
    new_screen->updated = 0;
    
    return new_screen;
}

void Graphics_DrawGraphics( Graphics_ChipScreen *screen ) {
    if( screen->cleared ) {
        SDL_RenderClear(screen->renderer);
    }
    if( screen->updated ) {
        SDL_RenderCopy(screen->renderer, screen->texture, NULL, NULL);
    }
    SDL_RenderPresent(screen->renderer);
    
    screen->cleared = 0;
    screen->updated = 0;
}

void Graphics_ClearScreen( Graphics_ChipScreen *screen ) {
    screen->cleared = 1;
    SDL_DestroyTexture(screen->texture);
    Graphics_NewTexture( screen );
}

int Graphics_DrawSprite( Graphics_ChipScreen *screen,  int x, int y, uint8_t sprite[], int h ) {
    void *pixels;
    int pitch;
    SDL_Rect dimen;
    dimen.x = x;
    dimen.y = y;
    dimen.w = 8;
    dimen.h = h;
    
    char collision = 0;
    SDL_LockTexture(screen->texture, &dimen, &pixels, &pitch);
    Uint32 * image_pixels = (Uint32 *) pixels;
    pitch /= 4; // We increased the size x4 so decrease the pitch

    for( int y = 0; y < h; y ++ ) {
        for( int x = 0; x < 8; x ++ ) {
            uint32_t image_pixel = image_pixels[ x + ( y * pitch ) ];
            uint32_t sprite_pixel = sprite[ y ] & ( 0x80 >> x );
            
            if( sprite_pixel != 0 ) {
                if( image_pixel != 0 ) {
                    collision = 1;
                    image_pixels[ x + ( y * pitch ) ] = 0x000000;
                } else {
                    image_pixels[ x + ( y * pitch ) ] = 0xFFFFFF;
                }
            }
        }
    }
    
    SDL_UnlockTexture(screen->texture);
    
    screen->updated = 1;
    return collision;
}

void Graphics_Quit( Graphics_ChipScreen *screen ) {
    SDL_DestroyRenderer( screen->renderer );
    SDL_DestroyTexture( screen->texture );
    SDL_DestroyWindow( screen->window );
    SDL_Quit();
}
