//
//  chip8.cpp
//  chipeit
//
//  Created by Stephanos on 8/18/23.
//

#include "chip8.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <vector>
#include <filesystem>
#include <random>
#include <iostream>

Chip8::Chip8() {
    // Initialize registers and memory
    pc = 0x200; // Program counter starts at 0x200 (512)
    
    // Clear display
    gfx.fill(0);
    
    // Clear stack
    stack.fill(0);
    
    // Clear registers V0-VF
    V.fill(0);
    
    // Clear memory
    memory.fill(0);
    
    // Load font
    std::array<uint8_t, 80> chip8_fontset =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, //0
        0x20, 0x60, 0x20, 0x20, 0x70, //1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
        0x90, 0x90, 0xF0, 0x10, 0x10, //4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
        0xF0, 0x10, 0x20, 0x40, 0x40, //7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
        0xF0, 0x90, 0xF0, 0x90, 0x90, //A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
        0xF0, 0x80, 0x80, 0x80, 0xF0, //C
        0xE0, 0x90, 0x90, 0x90, 0xE0, //D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
        0xF0, 0x80, 0xF0, 0x80, 0x80  //F
    };
    std::copy(chip8_fontset.begin(), chip8_fontset.end(), memory.begin());
    
    // Reset timers
    delay_timer = 60;
    sound_timer = 0;
}

std::array<uint8_t, 64 * 32>& Chip8::get_gfx() {
    return gfx;
};

void Chip8::load(std::string const& file) {
    // Read ROM file content into buffer
    std::ifstream input(file, std::ios::in | std::ios::binary);
    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    
    // Copy buffer into memory starting at 0x200 (512)
    std::copy(buffer.begin(), buffer.end(), memory.begin() + 512);
}

void Chip8::debug_unknown_opcode(unsigned short opcode) {
    std::cout << "Unknown opcode: " << std::hex << opcode << std::dec << std::endl;
}

void Chip8::debug_opcode(unsigned short opcode) {
    std::cout << "Executing opcode: " << std::hex << opcode << std::dec << std::endl;
}

void Chip8::emulateCycle() {
    // Fetch Opcode (first 8 bits offset then next 8 bits for a 2-byte instruction)
    opcode = memory[pc] << 8 | memory[pc + 1];
    
    debug_opcode(opcode);
    
    // Increment pc
    pc += 2;
    
    // Instruction sections
    unsigned char X = (opcode >> 8) & 0x0F;
    unsigned char Y = (opcode >> 4) & 0x0F;
    auto NNN = opcode & 0x0FFF;
    auto NN = opcode & 0x00FF;
    auto N = opcode & 0x000F;
    
    // Decode Opcode
    switch(opcode & 0xF000) { // Read first 4 bits
            
        case 0x0000: // Could be 00E0 or 00EE
            switch(opcode & 0x000F) {
                case 0x0000: // 00E0: Clears display
                    // Clear display
                    gfx.fill(0);
                    break;
                case 0x000E: // 00EE: Return from subroutine
                    // Pop from the stack into pc
                    pc = stack[--sp];
                    break;
                default:
                    debug_unknown_opcode(opcode);
                    break;
            }
            break;
            
        case 0x1000: // 1NNN: Jump to NNN
            pc = NNN;
            break;
            
        case 0x2000: // 2NNN: Calls subroutine at NNN
            stack[sp] = pc;
            sp++;
            pc = NNN;
            break;
            
        case 0x3000: // 3XNN: Skip next instruction if Vx == NN
            if(V[X] == NN){
                pc += 2;
            }
            break;
            
        case 0x4000: // 4XNN: Skip next instruction if Vx != NN
            if(V[X] != NN){
                pc += 2;
            }
            break;
            
        case 0x5000: // 5XY0: Skip next instruction if Vx == Vy
            if(V[X] == V[Y]){
                pc += 2;
            }
            break;
            
        case 0x6000: // 6XNN: Set register Vx to NN
            V[X] = NN;
            break;
            
        case 0x7000: // 7XNN: Adds NN to Vx (carry flag unchanged)
            V[X] += NN;
            break;
            
        case 0x8000: // 8XY-: BitOps and Math
            switch(opcode & 0x000F) {
                case 0x0000: // 8XY0: Sets Vx to the value of Vy
                    V[X] = V[Y];
                    break;
                case 0x0001: // 8XY1: Sets Vx to Vx OR Vy
                    V[X] |= V[Y];
                    break;
                case 0x0002: // 8XY2: Sets Vx to Vx AND Vy
                    V[X] &= V[Y];
                    break;
                case 0x0003: // 8XY1: Sets Vx to Vx XOR Vy
                    V[X] ^= V[Y];
                    break;
                case 0x0004: { // 8XY4: Sets Vx to Vx + Vy (VF to 1 if there's a carry)
                    uint16_t result = V[X] + V[Y]; // Use a larger data type to add
                    if(result >> 8) { // If after removing first 8 bits value is not 0
                        V[0xF] = 1; // Set carry flag
                    }
                    V[X] = result & 0xFF; // Store first 8 bits in Vx
                    break;
                }
                case 0x0006: // 8XY6: Stores least significant bit of Vx in VF then shifts VX to the right by 1
                    V[0xF] = V[X] & 0x01; // Isolate LSB and store in VF
                    V[X] >>= 1;
                    break;
                case 0x000E: // 8XYE: Stores most significant bit of Vx in VF then shifts Vx to the left by 1
                    V[0xF] = V[X] >> 15; // Isolate MSB and store in VF
                    V[X] <<= 1;
                    break;
                default:
                    debug_unknown_opcode(opcode);
                    break;
            }
            break;
            
        case 0x9000: // 9XY0: Skips next instruction if Vx != Vy
            if(V[X] != V[Y]) {
                pc += 2;
            }
            break;
            
        case 0xA000: // ANNN: Sets I to the address of NNN
            I = NNN;
            break;
            
        case 0xB000: // BNNN: Jump to adress NNN + V0
            pc = NNN + V[0];
            break;
            
        case 0xC000: // CXNN: Set Vx to rand() & NN
            V[X] = (std::rand() % 0xFF) & NN;
            break;
            
        case 0xD000: { // DXYN: draw sprite at coordinate Vx, Vy with width of 8 and height of N;
            auto x = V[X] % 64;
            auto y = V[Y] % 32;
            auto height = N;
            unsigned short pixel;
            
            V[0xF] = 0;
            for(auto yline = 0; yline < height; yline++) {
                pixel = memory[I + yline];
                for(auto xline = 0; xline < 8; xline++){ // Scan bit by bit
                    if((pixel & 0x80 >> xline) != 0) { // If this bit is set
                        if(gfx[x + xline + ((y + yline) * 64)] == 1){ // If display bit set then set VF to 1 for collision
                            V[0xF] = 1;
                        }
                        gfx[x + xline + ((y + yline) * 64)] ^= 1; // Set pixel by XOR
                    }
                }
            }
            break;
        }
            
        case 0xF000: // FX--
            switch(opcode & 0x00FF) {
                case 0x001E: // FX1E: Adds Vx to I. VF not affected
                    I += V[X];
                    break;
                    
                case 0x0007: // FX07: sets Vx to the value of delay timer
                    V[X] = delay_timer;
                    break;
                    
                case 0x0015: // FX15: Sets the delay timer to Vx
                    delay_timer = V[X];
                    break;
                    
                case 0x0018: // FX15: Sets the sound timer to Vx
                    sound_timer = V[X];
                    break;
                    
                case 0x0055: // FX55: Stores from V0 to Vx (inclusive) into memory starting at I (reg_dump)
                    for (uint8_t index = 0; index <= X; index++) {
                        memory[I + index] = V[index];
                    }
                    break;
                case 0x0065: // FX65: Fills from V0 to Vx (inclusive) with values from memory starting at I (reg_load)
                    for (uint8_t index = 0; index <= X; index++) {
                        V[index] = memory[I + index];
                    }
                    break;
                default:
                    debug_unknown_opcode(opcode);
                    break;
            }
            break;
        default:
            debug_unknown_opcode(opcode);
            break;
    }
    
    // Execute Opcode
    // Update timers
    
    if (delay_timer > 0) {
        delay_timer--;
    }
    
    if (sound_timer > 0) {
        // TODO: Beep
        std::cout << "BEEEEEEEEEEP" << std::endl;
        sound_timer--;
    }
}
