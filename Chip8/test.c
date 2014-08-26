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
    
    chip->memory[0x200] = first;
    chip->memory[0x201] = second;
    chip->pc = 0x200;
}

// Clears the screen
void test_Chip8_00E0( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0x00E0 );
    Chip8_ProcessCommand( chip );
    
    assert( chip->screen->cleared, "Boolean for screen clear not set" );
    assert( chip->redraw, "Boolean for redraw not set" );
    assert( chip->pc == 0x202, "Pc not incremented" );
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

// Processes subroutine NNN
void test_Chip8_2NNN( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0x2321 );
    chip->sp = 0;
    chip->stack[ 0 ] = 0;
    Chip8_ProcessCommand( chip );
    
    assert( chip->sp == 1, "Stack pointer didn't move" );
    assert( chip->stack[0] == 0x202, "Stack didn't store jump point" );
    assert( chip->pc == 0x321, "Pc didn't change to 321" );
}

// Skip next instruction if VX == NN
void test_Chip8_3XNN( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0x3011 );
    chip->V[ 0 ] = 0x11;
    Chip8_ProcessCommand( chip );
    
    assert( chip->pc == 0x204, "3XNN Didn't skip when equal" );
    
    test_Chip8_SetInstruction( chip, 0x3011 );
    chip->V[ 0 ] = 0x12;
    Chip8_ProcessCommand( chip );
    
    assert( chip->pc == 0x202, "3XNN Skipped when not equal" );
}

// Skip next instruction if VX != NN
void test_Chip8_4XNN( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0x4011 );
    chip->V[ 0 ] = 0x12;
    Chip8_ProcessCommand( chip );
    
    assert( chip->pc == 0x204, "4XNN Didn't skip when not equal" );
    
    test_Chip8_SetInstruction( chip, 0x4011 );
    chip->V[ 0 ] = 0x11;
    Chip8_ProcessCommand( chip );
    
    assert( chip->pc == 0x202, "4XNN Skipped when equal" );
}

// Skip next instruction if VX == VY
void test_Chip8_5XY0( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0x5010 );
    chip->V[ 0 ] = 0x12;
    chip->V[ 1 ] = 0x12;
    Chip8_ProcessCommand( chip );
    
    assert( chip->pc == 0x204, "5XYN Didn't skip when equal" );
    
    test_Chip8_SetInstruction( chip, 0x5010 );
    chip->V[ 0 ] = 0x11;
    chip->V[ 1 ] = 0x12;
    Chip8_ProcessCommand( chip );
    
    assert( chip->pc == 0x202, "5XYN Skipped when not equal" );
}

// Set VX to NN
void test_Chip8_6XNN( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0x60FF );
    chip->V[ 0 ] = 0;
    Chip8_ProcessCommand( chip );
    
    assert( chip->V[ 0 ] == 0xFF, "Didn't change V0 to FF" );
    assert( chip->pc == 0x202, "Didn't progress Pc" );
}

// Adds NN to VX
void test_Chip8_7XNN( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0x7001 );
    chip->V[ 0 ] = 0x01;
    Chip8_ProcessCommand( chip );
    
    assert( chip->V[ 0 ] == 0x02, "Didn't add 01 to V0" );
    assert( chip->pc == 0x202, "7XNN Didn't progress Pc" );
}

// Sets VX to VY
void test_Chip8_8XY0( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0x8120 );
    chip->V[ 1 ] = 0x00;
    chip->V[ 2 ] = 0x12;
    Chip8_ProcessCommand( chip );
    
    assert( chip->V[ 1 ] == 0x12, "Didn't change v1 to 12" );
    assert( chip->V[ 2 ] == 0x12, "Didn't leave v2 as 12" );
    assert( chip->pc == 0x202, "8XY0 Didn't progress Pc" );
}

// VX = VX | VY
void test_Chip8_8XY1( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0x8121 );
    chip->V[ 1 ] = 0x34;
    chip->V[ 2 ] = 0x12;
    Chip8_ProcessCommand( chip );
    
    assert( chip->V[ 1 ] == (0x12 | 0x34), "Didn't or VX to VX VY" );
    assert( chip->V[ 2 ] == 0x12, "Didn't leave v2 as 12" );
    assert( chip->pc == 0x202, "8XY1 Didn't progress Pc" );
}

// VX = VX & VY
void test_Chip8_8XY2( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0x8122 );
    chip->V[ 1 ] = 0x34;
    chip->V[ 2 ] = 0x12;
    Chip8_ProcessCommand( chip );
    
    assert( chip->V[ 1 ] == (0x12 & 0x34), "Didn't and VX to VX VY" );
    assert( chip->V[ 2 ] == 0x12, "Didn't leave v2 as 12" );
    assert( chip->pc == 0x202, "8XY2 Didn't progress Pc" );
}

// VX = VX ^ VY
void test_Chip8_8XY3( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0x8123 );
    chip->V[ 1 ] = 0x34;
    chip->V[ 2 ] = 0x12;
    Chip8_ProcessCommand( chip );
    
    assert( chip->V[ 1 ] == (0x12 ^ 0x34), "Didn't xor VX to VX VY" );
    assert( chip->V[ 2 ] == 0x12, "Didn't leave v2 as 12" );
    assert( chip->pc == 0x202, "Didn't progress Pc" );
}

// If VX = VX + VY has a carry, then set the carry bit
void test_Chip8_8XY4( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0x8124 );
    chip->V[ 1 ] = 0x11;
    chip->V[ 2 ] = 0x12;
    Chip8_ProcessCommand( chip );
    
    assert( chip->V[ 1 ] == 0x11 + 0x12, "Didn't add values" );
    
    test_Chip8_SetInstruction( chip, 0x8124 );
    chip->V[ 1 ] = 0xFF;
    chip->V[ 2 ] = 0x12;
    Chip8_ProcessCommand( chip );
    
    assert( chip->V[ 1 ] == (uint8_t)(0xFF + 0x12), "Didn't add values" );
    assert( chip->V[ 0xF ] == 1, "Didn't set carry flag" );
    assert( chip->pc == 0x202, "Didn't progress Pc" );
}


// If VX = VX - VY has a borrow, then set the borrow bit
void test_Chip8_8XY5( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0x8125 );
    chip->V[ 1 ] = 0x12;
    chip->V[ 2 ] = 0x11;
    Chip8_ProcessCommand( chip );
    
    assert( chip->V[ 1 ] == 0x12 - 0x11, "8XY5 Didn't subtract values" );
    assert( chip->V[ 0xF ] == 1, "8XY5 Didn't set carry flag" );
    
    test_Chip8_SetInstruction( chip, 0x8125 );
    chip->V[ 1 ] = 0x11;
    chip->V[ 2 ] = 0x12;
    Chip8_ProcessCommand( chip );
    
    assert( chip->V[ 1 ] == (uint8_t)(0x11 - 0x12), "8XY5 Carry didn't subtract values" );
    assert( chip->V[ 0xF ] == 0, "8XY5 Didn't set carry flag" );
    assert( chip->pc == 0x202, "8XY5 Didn't progress Pc" );
}

// >>
void test_Chip8_8X06( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0x8106 );
    chip->V[ 1 ] = 0x03;
    Chip8_ProcessCommand( chip );
    
    assert( chip->V[ 1 ] == 0x1, "Didn't shift to right" );
    assert( chip->V[ 0xF ] == 0x1, "Didn't set VF to lsb" );
    assert( chip->pc == 0x202, "Didn't progress Pc" );
}

// If VX = VY - VX has a borrow, then set the borrow bit
void test_Chip8_8XY7( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0x8127 );
    chip->V[ 1 ] = 0x11;
    chip->V[ 2 ] = 0x12;
    Chip8_ProcessCommand( chip );
    
    assert( chip->V[ 1 ] == 0x12 - 0x11, "8XY7 Didn't subtract values" );
    assert( chip->V[ 0xF ] == 1, "8XY7 Didn't set carry flag" );
    
    test_Chip8_SetInstruction( chip, 0x8127 );
    chip->V[ 1 ] = 0x12;
    chip->V[ 2 ] = 0x11;
    Chip8_ProcessCommand( chip );
    
    assert( chip->V[ 1 ] == (uint8_t)(0x11 - 0x12), "8XY7 Carry didn't subtract values" );
    assert( chip->V[ 0xF ] == 0, "8XY7 Didn't set carry flag" );
    assert( chip->pc == 0x202, "8XY7 Didn't progress Pc" );
}

// <<
void test_Chip8_8X08( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0x8108 );
    chip->V[ 1 ] = 0x03;
    Chip8_ProcessCommand( chip );
    
    assert( chip->V[ 1 ] == 0x6, "Didn't shift to right" );
    assert( chip->V[ 0xF ] == 0x0, "Didn't set VF to lsb" );
    assert( chip->pc == 0x202, "Didn't progress Pc" );
}

// Skip next instruction if VX != VY
void test_Chip8_9XY0( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0x9010 );
    chip->V[ 0 ] = 0x11;
    chip->V[ 1 ] = 0x12;
    Chip8_ProcessCommand( chip );
    
    assert( chip->pc == 0x204, "9XY0 Didn't skip when not equal" );
    
    test_Chip8_SetInstruction( chip, 0x9010 );
    chip->V[ 0 ] = 0x11;
    chip->V[ 1 ] = 0x11;
    Chip8_ProcessCommand( chip );
    
    assert( chip->pc == 0x202, "9XY0 Skipped when equal" );
}

void test_Chip8_ANNN( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0xA123 );
    Chip8_ProcessCommand( chip );
    
    assert( chip->i == 0x123, "I wasn't set to 123" );
}

void test_Chip8_BNNN( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0xB123 );
    chip->V[ 0 ] = 1;
    Chip8_ProcessCommand( chip );
    
    assert( chip->pc == 0x124, "Pc wasn't set to 124" );
}

// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
void test_Chip8_DXYN( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0xD123 );
    chip->V[ 1 ] = 1;
    chip->V[ 2 ] = 1;
    chip->i = 0x202;
    chip->memory[ 0x202 ] = 0xF1;
    chip->memory[ 0x203 ] = 0xF1;
    chip->memory[ 0x204 ] = 0xF1;
    Chip8_ProcessCommand( chip );
    
    Graphics_DrawGraphics( chip->screen );
    SDL_Delay( 1000 );
}

void test_Chip8_EX9E( Chip8 *chip ) { // Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.
    test_Chip8_SetInstruction( chip, 0xE19E );
    chip->V[ 1 ] = 0x2;
    chip->keypad[ 0x2 ] = 1;
    Chip8_ProcessCommand( chip );
    
    assert( chip->pc = 0x204, "Button didn't cause skip" );
}

void test_Chip8_EXA1( Chip8 *chip ) {
    test_Chip8_SetInstruction( chip, 0xE19E );
    chip->V[ 1 ] = 0x2;
    chip->keypad[ 0x2 ] = 0;
    Chip8_ProcessCommand( chip );
    
    assert( chip->pc = 0x204, "No button didn't cause skip" );
}

void test_Chip8() {
    Graphics_ChipScreen *screen = Graphics_InitGraphics();
    Chip8 *chip = Chip8_Initialize( screen );
    assert( chip != NULL, "Chip failed to init" );
    assert( chip->screen->cleared == 0, "Chip screen didn't start cleared" );
    
    test_Chip8_00E0( chip );
    test_Chip8_00EE( chip );
    test_Chip8_1NNN( chip );
    test_Chip8_2NNN( chip );
    test_Chip8_3XNN( chip );
    test_Chip8_4XNN( chip );
    test_Chip8_5XY0( chip );
    test_Chip8_6XNN( chip );
    test_Chip8_7XNN( chip );
    test_Chip8_8XY0( chip );
    test_Chip8_8XY1( chip );
    test_Chip8_8XY2( chip );
    test_Chip8_8XY3( chip );
    test_Chip8_8XY4( chip );
    test_Chip8_8XY5( chip );
    test_Chip8_8X06( chip );
    test_Chip8_8XY7( chip );
    test_Chip8_8X08( chip );
    test_Chip8_9XY0( chip );
    test_Chip8_ANNN( chip );
    test_Chip8_BNNN( chip );
    test_Chip8_DXYN( chip );
    test_Chip8_EX9E( chip );
    test_Chip8_EXA1( chip );
    
    printf( "\n" );
}

int main() {
    test_Chip8();
    
    return 1;
}