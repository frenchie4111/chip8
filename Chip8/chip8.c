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
        chip->pc = 0x200;
        chip->sp = 0;
        
        chip->waiting_key = 0;
        chip->redraw = 0;
        chip->screen = screen;
    }
    
    Chip8_InitializeFont( chip );
    
    return chip;
}

void Chip8_ProcessCommand( Chip8 *chip ) {    
    uint16_t opcode = ((( uint16_t )chip->memory[ chip->pc ] ) << 8 ) | (( uint16_t )chip->memory[ chip->pc + 1 ] );
    chip->delay_timer--;
    chip->sound_timer--;
//    printf( "%x\n", opcode );
    
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
            int collision = Graphics_DrawSprite( chip->screen, chip->V[ ( opcode & 0x0F00 ) >> 8 ], chip->V[ ( opcode & 0x00F0 ) >> 4 ], sprite, opcode & 0x000F );
            chip->V[ 0xF ] = collision;
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
                    chip->waiting_key = 1;
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
                case 0xF029: { // Sets i to character VX
                    uint16_t x = chip->V[ ( opcode & 0x0F00 ) >> 8 ];
                    chip->i = ( x * 0x5 );
                    break;
                }
                case 0xF033:
                    chip->memory[ chip->i + 2 ] = chip->V[ ( opcode & 0x0F00 ) >> 8 ] % 10;
                    chip->memory[ chip->i + 1 ] = ( chip->V[ ( opcode & 0x0F00 ) >> 8 ] / 10 ) % 10;
                    chip->memory[ chip->i ] = chip->V[ ( opcode & 0x0F00 ) >> 8 ] / 100;
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
    FILE *file = fopen( romPath, "rb" );
    
    fseek( file, 0, SEEK_END ); // seek to end of file
    long size = ftell( file ); // get current file pointer
    fseek(file, 0, SEEK_SET); // seek back to beginning of file
    
    unsigned char buffer; // note: 1 byte
    
    for( int i = 0; i < size; i++ ) {
        fread(&buffer, 1, 1, file);
        chip->memory[ 0x200 + i ] = buffer;
    }
    
    fclose( file );
}

void Chip8_InitializeFont( Chip8 *chip ) {
    chip->memory[ 0x000 ] = 0xF0;
    chip->memory[ 0x001 ] = 0x90;
    chip->memory[ 0x002 ] = 0x90;
    chip->memory[ 0x003 ] = 0x90;
    chip->memory[ 0x004 ] = 0xF0;
    
    chip->memory[ 0x005 ] = 0x20;
    chip->memory[ 0x006 ] = 0x60;
    chip->memory[ 0x007 ] = 0x20;
    chip->memory[ 0x008 ] = 0x20;
    chip->memory[ 0x009 ] = 0x70;
    
    chip->memory[ 0x00A ] = 0xF0;
    chip->memory[ 0x00B ] = 0x10;
    chip->memory[ 0x00C ] = 0xF0;
    chip->memory[ 0x00D ] = 0x80;
    chip->memory[ 0x00E ] = 0xF0;
    
    chip->memory[ 0x00F ] = 0xF0;
    chip->memory[ 0x010 ] = 0x10;
    chip->memory[ 0x011 ] = 0xF0;
    chip->memory[ 0x012 ] = 0x10;
    chip->memory[ 0x013 ] = 0xF0;
    
    chip->memory[ 0x014 ] = 0x90;
    chip->memory[ 0x015 ] = 0x90;
    chip->memory[ 0x016 ] = 0xF0;
    chip->memory[ 0x017 ] = 0x10;
    chip->memory[ 0x018 ] = 0x10;
    
    chip->memory[ 0x019 ] = 0xF0;
    chip->memory[ 0x01A ] = 0x80;
    chip->memory[ 0x01B ] = 0xF0;
    chip->memory[ 0x01C ] = 0x10;
    chip->memory[ 0x01D ] = 0xF0;
    
    chip->memory[ 0x01E ] = 0xF0;
    chip->memory[ 0x01F ] = 0x80;
    chip->memory[ 0x020 ] = 0xF0;
    chip->memory[ 0x021 ] = 0x90;
    chip->memory[ 0x022 ] = 0xF0;
    
    chip->memory[ 0x023 ] = 0xF0;
    chip->memory[ 0x024 ] = 0x10;
    chip->memory[ 0x025 ] = 0x20;
    chip->memory[ 0x026 ] = 0x40;
    chip->memory[ 0x027 ] = 0x40;
    
    chip->memory[ 0x028 ] = 0xF0;
    chip->memory[ 0x029 ] = 0x90;
    chip->memory[ 0x02A ] = 0xF0;
    chip->memory[ 0x02B ] = 0x90;
    chip->memory[ 0x02C ] = 0xF0;
    
    chip->memory[ 0x02D ] = 0xF0;
    chip->memory[ 0x02E ] = 0x90;
    chip->memory[ 0x02F ] = 0xF0;
    chip->memory[ 0x030 ] = 0x10;
    chip->memory[ 0x031 ] = 0xF0;
    
    chip->memory[ 0x032 ] = 0xF0;
    chip->memory[ 0x033 ] = 0x90;
    chip->memory[ 0x034 ] = 0xF0;
    chip->memory[ 0x035 ] = 0x90;
    chip->memory[ 0x036 ] = 0x90;
    
    chip->memory[ 0x037 ] = 0xE0;
    chip->memory[ 0x038 ] = 0x90;
    chip->memory[ 0x039 ] = 0xE0;
    chip->memory[ 0x03A ] = 0x90;
    chip->memory[ 0x03B ] = 0xE0;
    
    chip->memory[ 0x03C ] = 0xF0;
    chip->memory[ 0x03D ] = 0x80;
    chip->memory[ 0x03E ] = 0x80;
    chip->memory[ 0x03F ] = 0x80;
    chip->memory[ 0x040 ] = 0xF0;
    
    chip->memory[ 0x041 ] = 0xE0;
    chip->memory[ 0x042 ] = 0x90;
    chip->memory[ 0x043 ] = 0x90;
    chip->memory[ 0x044 ] = 0x90;
    chip->memory[ 0x045 ] = 0xE0;
    
    chip->memory[ 0x046 ] = 0xF0;
    chip->memory[ 0x047 ] = 0x80;
    chip->memory[ 0x048 ] = 0xF0;
    chip->memory[ 0x049 ] = 0x80;
    chip->memory[ 0x04A ] = 0xF0;
    
    chip->memory[ 0x04B ] = 0xF0;
    chip->memory[ 0x04C ] = 0x80;
    chip->memory[ 0x04D ] = 0xF0;
    chip->memory[ 0x04E ] = 0x80;
    chip->memory[ 0x04F ] = 0x80;
}
