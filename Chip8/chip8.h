//
//  chip8.h
//  Chip8
//
//  Created by Michael Lyons on 8/25/14.
//  Copyright (c) 2014 Mike Lyons. All rights reserved.
//

#ifndef __Chip8__chip8__
#define __Chip8__chip8__

#include <stdio.h>
#include <time.h>

#include "Graphics.h"

typedef struct Chip8 {
    uint8_t memory[ 4096 ];
    uint8_t V[ 16 ];
    uint16_t i;
    uint16_t pc;
    
    uint8_t delay_timer;
    uint8_t sound_timer;
    
    uint16_t stack[ 16 ];
    uint16_t sp;
    
    uint8_t keypad[ 16 ];
    char waiting_key;
    
    char redraw;
    Graphics_ChipScreen *screen;
} Chip8;

Chip8 *Chip8_Initialize( Graphics_ChipScreen *screen );

void Chip8_InitializeFont( Chip8 *chip );

void Chip8_ProcessCommand( Chip8 *chip );

void Chip8_LoadRom( Chip8 *chip, char *romPath );

#endif /* defined(__Chip8__chip8__) */
