//
//  main.cpp
//  chipeit
//
//  Created by Stephanos on 8/18/23.
//

#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>
#include "chip8.h"
//#include "display.hpp"
#include <SDL2/SDL.h>

int main(int argc, const char * argv[]) {
//    Display display{};
    Chip8 chip8{};
//    chip8.load("1-chip8-logo.ch8");
//    chip8.load("ibm.ch8");
    chip8.load("maze-demo.ch8");
//    chip8.load("particle-demo.ch8");
//    chip8.load("zero-demo.ch8");
    
    SDL_Init(SDL_INIT_EVERYTHING);
    // Create a window
    SDL_Window* window = SDL_CreateWindow("Demo Game",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          64 * 10,
                                          32 * 10,
                                          SDL_WINDOW_OPENGL);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetScale(renderer, 10, 10);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Texture * texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 64, 32);
    std::array<uint32_t, 2048> pixels;
    pixels.fill(0);
    while(true) {
        
        SDL_Event event;
        if(SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT){
                break;
            }
        }
        chip8.emulateCycle();
        auto gfx = chip8.get_gfx();
        
//        using namespace std::literals::chrono_literals;
//        std::this_thread::sleep_for(10ms);
        
        std::transform(gfx.begin(), gfx.end(), pixels.begin(), [](auto pix){
            return pix ? std::numeric_limits<uint32_t>::max() : 0;
        });
        
        SDL_UpdateTexture(texture, NULL, pixels.data(), 64 * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

//        auto gfx = chip8.get_gfx();
//        display.draw(gfx);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
