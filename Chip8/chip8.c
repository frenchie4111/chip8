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
            switch ( opcode ) {
                case 0x00E0: // Clear Screen
                    Graphics_ClearScreen( chip->screen );
                    chip->redraw = 1;
                    break;
                case 0x00EE: // Return from subroutine
                    chip->pc = chip->stack[ chip->sp - 1 ];
                    chip->sp--;
                    return;
                    break;
                default: // Run RCA1802 command
                    // NOOP
                    break;
            }
            break;
        case 0x1000: // Jump to address NNN
            chip->pc = opcode & 0x0FFF;
            return; // Don't increment pc
        case 0x2000: // Enter subroutine
            chip->stack[ chip->sp++ ] = chip->pc + 2;
            chip->pc = opcode & 0x0FFF;
            return;
        case 0x3000: // Skip when V == NN
            if( chip->V[ ( opcode & 0x0F00 ) >> 8 ] == ( opcode & 0x00FF ) ) {
                chip->pc += 2;
            }
            break;
        case 0x4000: // Skip when V != NN
            if( chip->V[ ( opcode & 0x0F00 ) >> 8 ] != ( opcode & 0x00FF ) ) {
                chip->pc += 2;
            }
            break;
        case 0x5000: // Skip if VX == VY
            if( chip->V[ ( opcode & 0x0F00 ) >> 8 ] == chip->V[ ( opcode & 0x00F0 ) >> 4 ] ) {
                chip->pc += 2;
            }
            break;
        case 0x6000: // Set VX to NN
            chip->V[ ( opcode & 0x0F00 ) >> 8 ] = ( opcode & 0x00FF );
            break;
        case 0x7000: // Add NN to VX
            chip->V[ ( opcode & 0x0F00 ) >> 8 ] += ( opcode & 0x00FF );
            break;
        case 0x8000:
            switch ( opcode & 0xF00F ) {
                case 0x8000: // Set the value of VX to VY
                    chip->V[ ( opcode & 0x0F00 ) >> 8 ] = chip->V[ ( opcode & 0x00F0 ) >> 4 ];
                    break;
                case 0x8001: // VX = VX | VY
                    chip->V[ ( opcode & 0x0F00 ) >> 8 ] |= chip->V[ ( opcode & 0x00F0 ) >> 4 ];
                    break;
                case 0x8002: // VX = VX & VY
                    chip->V[ ( opcode & 0x0F00 ) >> 8 ] &= chip->V[ ( opcode & 0x00F0 ) >> 4 ];
                    break;
                case 0x8003: // VX = VX ^ VY
                    chip->V[ ( opcode & 0x0F00 ) >> 8 ] ^= chip->V[ ( opcode & 0x00F0 ) >> 4 ];
                    break;
                case 0x8004: { // Adds VX and VY, VF = carry
                    uint16_t sum;
                    uint16_t vx = chip->V[ ( opcode & 0x0F00 ) >> 8 ];
                    uint16_t vy = chip->V[ ( opcode & 0x00F0 ) >> 4 ];
                    sum = vx + vy;
                    if( sum != ( uint8_t ) sum ) {
                        chip->V[ 0xF ] = 1;
                    } else {
                        chip->V[ 0xF ] = 0;
                    }
                    chip->V[ ( opcode & 0x0F00 ) >> 8 ] = ( uint8_t ) sum;
                   break;
                }
                case 0x8005: { // Subtracts Y from X
                    uint8_t vx = chip->V[ ( opcode & 0x0F00 ) >> 8 ];
                    uint8_t vy = chip->V[ ( opcode & 0x00F0 ) >> 4 ];
                    chip->V[ 0xF ] = ( vx > vy )? 1 : 0;
                    chip->V[ ( opcode & 0x0F00 ) >> 8 ] = vx - vy;
                    break;
                }
                case 0x8006: // Shifts right, sets VF to lsb
                    chip->V[ 0xF ] = chip->V[ ( opcode & 0x0F00 ) >> 8 ] & 0x0001;
                    chip->V[ ( opcode & 0x0F00 ) >> 8 ] >>= 1;
                    break;
                case 0x8007: { // Subtracts X from Y
                    uint8_t vx = chip->V[ ( opcode & 0x0F00 ) >> 8 ];
                    uint8_t vy = chip->V[ ( opcode & 0x00F0 ) >> 4 ];
                    chip->V[ 0xF ] = ( vy > vx )? 1 : 0;
                    chip->V[ ( opcode & 0x0F00 ) >> 8 ] = vy - vx;
                    break;
                }
                case 0x8008: { // Shifts left, sets VF to msb
                    chip->V[ 0xF ] = ( chip->V[ ( opcode & 0x0F00 ) >> 8 ] & 0x8000 ) >> 15;
                    chip->V[ ( opcode & 0x0F00 ) >> 8 ] <<= 1;
                    break;
                }
            }
            break;
        case 0x9000: // Skips next if X != y
            if( chip->V[ ( opcode & 0x0F00 ) >> 8 ] != chip->V[ ( opcode & 0x00F0 ) >> 4 ] ) {
                chip->pc += 2;
            }
            break;
        case 0xA000: // Sets I to NNN
            chip->i = opcode & 0x0FFF;
            break;
        case 0xB000: // Pc = V0 + NNN
            chip->pc = ( opcode & 0x0FFF ) + chip->V[ 0 ];
            return;
        case 0xC000: // X = Random & NN
            srand((unsigned int)time(NULL));
            int r = rand() % 256;
            chip->V[ ( opcode & 0x0F00 ) >> 8 ] = r & ( opcode & 0x00FF );
            break;
        case 0xD000: { // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
            uint8_t *sprite = (uint8_t *) ( (chip->memory) + ( chip->i ) );
            Graphics_DrawSprite( chip->screen, chip->V[ ( opcode & 0x0F00 ) >> 8 ], chip->V[ ( opcode & 0x00F0 ) >> 4 ], sprite, opcode & 0x000F );
            chip->redraw = 1;
            break;
        }
        case 0xE000:
            switch( opcode & 0xF0FF ) { // If VX key is pressed, skip
                case 0xE09E:
                    if( chip->keypad[ chip->V[ ( opcode & 0x0F00 ) >> 8 ] ] ) {
                        chip->pc += 2;
                    }
                    break;
                case 0xE0A1:
                    if( !chip->keypad[ chip->V[ ( opcode & 0x0F00 ) >> 8 ] ] ) {
                        chip->pc += 2;
                    }
                    break;
            }
            break;
        case 0xF000:
            switch ( opcode & 0xF0FF ) {
                case 0xF007: // FX07 Sets VX to the value of the delay timer.
                    chip->V[ ( opcode & 0x0F00 ) >> 8 ] = chip->delay_timer;
                    break;
                case 0xF00A:
                    // Wait for button press
                    break;
                case 0xF015: // Set delay timer
                    chip->delay_timer = chip->V[ ( opcode & 0x0F00 ) >> 8 ];
                    break;
                case 0xF018: // Set delay timer
                    chip->sound_timer = chip->V[ ( opcode & 0x0F00 ) >> 8 ];
                    break;
                case 0xF01E: // Adds VX to i
                    chip->i += chip->V[ ( opcode & 0x0F00 ) >> 8 ];
                    break;
                case 0xF029:
                    // Sets i to character VX
                    break;
                case 0xF033:
                    // Binary-Coded Decimal
                    break;
                case 0xF055: { // The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
                    uint16_t x = chip->V[ ( opcode & 0x0F00 ) >> 8 ];
                    for( int i = 0; i < x; i++ ) {
                        chip->memory[ i + chip->i ] = chip->V[ i ];
                    }
                    break;
                }
                case 0xF065: { // The interpreter reads values from memory starting at location I into registers V0 through Vx.
                    uint16_t x = chip->V[ ( opcode & 0x0F00 ) >> 8 ];
                    for( int i = 0; i < x; i++ ) {
                        chip->memory[ i + chip->i ] = chip->V[ i ];
                    }
                    break;
                }
            }
            break;
    }
    
    chip->pc += 2; // Add two, as each command is 2 bytes
}

void Chip8_LoadRom( Chip8 *chip, char *romPath ) {
    chip->memory[ 0 ] = 0xFF;
    chip->memory[ 1 ] = 0x22;
}
