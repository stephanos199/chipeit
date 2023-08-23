//
//  display.cpp
//  chipeit
//
//  Created by Stephanos on 8/18/23.
//

#include <SDL2/SDL.h>
#include "display.hpp"

Display::Display() {
    SDL_Init(SDL_INIT_VIDEO);
    
    window = SDL_CreateWindow("CHIP8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, SDL_WINDOW_BORDERLESS);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetScale(renderer, 10, 10);
}

void Display::draw(std::array<unsigned char, 2048>& gfx) {
    int t_length = 64;
    int t_width = 32;
    
    uint8_t* pixels = gfx.data();
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    SDL_RenderDrawPoint(renderer, 640 / 2, 320 / 2);
    SDL_RenderPresent(renderer);
}

Display::~Display() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
