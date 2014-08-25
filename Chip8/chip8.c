//
//  chip8.c
//  Chip8
//
//  Created by Michael Lyons on 8/25/14.
//  Copyright (c) 2014 Mike Lyons. All rights reserved.
//

#include "chip8.h"

Chip8 *Chip8_Initialize( Graphics_ChipScreen *screen ) {
    Chip8 *chip = malloc( sizeof( Chip8 ) );
    
    if( chip ) {
        chip->i = 0;
        chip->pc = 0;
        chip->sp = 0;
        
        chip->redraw = 0;
        chip->screen = screen;
    }
    
    return chip;
}

void Chip8_ProcessCommand( Chip8 *chip ) {
    uint16_t opcode = ((( uint16_t )chip->memory[ chip->pc ] ) << 8 ) | (( uint16_t )chip->memory[ chip->pc + 1 ] );
    
    switch ( opcode & 0xF000 ) {
        case 0x0000:
            switch ( opcode )
                case 0x00E0: // Clear Screen
                    Graphics_ClearScreen( chip->screen );
                    chip->redraw = 1;
                    break;
                case 0x00EE: // Return from subroutine
                    chip->pc = chip->stack[ chip->sp - 1 ];
                    chip->sp--;
                    break;
                default: // Run RCA1802 command
                    // NOOP
                    break;
            break;
        case 0x1000: // Jump to address NNN
            chip->pc = ( ( opcode << 4 ) >> 4 );
            return; // Don't increment pc
            break;
    }
}

void Chip8_LoadRom( Chip8 *chip, char *romPath ) {
    chip->memory[ 0 ] = 0xFF;
    chip->memory[ 1 ] = 0x22;
}
