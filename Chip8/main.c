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

int handleEvent( Chip8 *chip, SDL_Event *event ) {
    switch ( event->type ) {
        case SDL_QUIT:
            return 0;
        case SDL_KEYDOWN:
            switch ( event->key.keysym.sym ) {
                case SDLK_ESCAPE:
                    return 0;
            }
            return 1;
        default:
            return -1;
    }
}

int main(int argc, const char * argv[])
{
    Graphics_ChipScreen *screen = Graphics_InitGraphics( 1 );
    Chip8 *chip = Chip8_Initialize( screen );
    Chip8_LoadRom( chip, "MERLIN" );
    SDL_Event event;
    
    while( 1 ) {
        
        while( chip->waiting_key ) {
            SDL_WaitEvent( &event );
            if( handleEvent( chip, &event ) == 1 ) {
                chip->waiting_key = 0;
            }
        }
        
        while( SDL_PollEvent( &event ) ) {
            if( !handleEvent( chip, &event ) == 0 ) {
                break;
            }
        }
        
        Chip8_ProcessCommand( chip );
        
        if( chip->redraw ) {
            Graphics_DrawGraphics( chip->screen );
            chip->redraw = 0;
        }
    }
    
    Graphics_Quit( chip->screen );
    return 0;
}

