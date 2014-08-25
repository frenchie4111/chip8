//
//  test.c
//  Chip8
//
//  Created by Michael Lyons on 8/25/14.
//  Copyright (c) 2014 Mike Lyons. All rights reserved.
//

#include <stdio.h>

#include "chip8.h"

void assert( int assertion, char *message ) {
    if( !assertion ) {
        printf( "\n%s\n", message );
        exit( EXIT_FAILURE );
    } else {
        printf(".");
    }
}

void test_Chip8_SetInstruction( Chip8 *chip, uint16_t instruction ) {
    uint8_t first = instruction >> 8;
    uint8_t second = instruction;
    
    chip->memory[0] = first;
    chip->memory[1] = second;
    chip->pc = 0;
}

// Clears the screen
void test_Chip8_00E0( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0x00E0 );
    Chip8_ProcessCommand( chip );
    
    assert( chip->screen->cleared, "Boolean for screen clear not set" );
    assert( chip->redraw, "Boolean for redraw not set" );
}

// Return from subroutine
void test_Chip8_00EE( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0x00EE );
    chip->stack[ 0 ] = 0xFFFF;
    chip->sp = 1;
    Chip8_ProcessCommand( chip );
    
    assert( chip->sp == 0, "Stack pointer didn't decrement" );
    assert( chip->pc == 0xFFFF, "Pc didn't change" );
}

// Jump to NNN
void test_Chip8_1NNN( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0x1123 );
    Chip8_ProcessCommand( chip );
    
    assert( chip->pc == 0x123, "Pc didn't change to 123" );
}

void test_Chip8() {
    Graphics_ChipScreen *screen = Graphics_InitGraphics();
    Chip8 *chip = Chip8_Initialize( screen );
    assert( chip != NULL, "Chip failed to init" );
    assert( chip->screen->cleared == 0, "Chip screen didn't start cleared" );
    
    test_Chip8_00E0( chip );
    test_Chip8_00EE( chip );
    
    
    printf( "\n" );
}

int main() {
    test_Chip8();
    
    return 1;
}