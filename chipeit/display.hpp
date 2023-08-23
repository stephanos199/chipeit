//
//  display.hpp
//  chipeit
//
//  Created by Stephanos on 8/18/23.
//

#ifndef display_hpp
#define display_hpp

#include <SDL2/SDL.h>
#include <array>

class Display {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
public:
    Display();
    ~Display();
    void draw(std::array<unsigned char, 2048>& gfx);
};

#endif /* display_hpp */
