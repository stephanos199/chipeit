//
//  chip8.h
//  chipeit
//
//  Created by Stephanos on 8/18/23.
//

#ifndef chip8_h
#define chip8_h

#include <string>
#include <array>

class Chip8 {
    unsigned short opcode; // Current opcode
    
    /*
     0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
     0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
     0x200-0xFFF - Program ROM and work RAM
     */
    std::array<unsigned short, 4096> memory; // Memory
    
    std::array<unsigned char, 16> V; // Registers V0 - VE
    unsigned int I; // Index register
    unsigned int pc; // Program counter
    std::array<unsigned char, 64 * 32> gfx; // Graphics (2048 pixels, 0 or 1 for black or white)
    unsigned char delay_timer; // When set above zero, timers count to 0
    unsigned char sound_timer; // When set above zero, timers count to 0. When reaching 0 buzz
    std::array<unsigned short, 16> stack; // Instruction stack
    unsigned short sp; // Stack pointer
    std::array<unsigned char, 16> key; // Hex-based keypad
    
public:
    Chip8();
    std::array<unsigned char, 64 * 32>& get_gfx();
    void load(std::string const& file);
    void emulateCycle();
    
private:
    void debug_unknown_opcode(unsigned short opcode);
    void debug_opcode(unsigned short opcode);
};

#endif /* chip8_h */
