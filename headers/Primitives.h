#pragma once
#include <SDL2/SDL.h>
void drawFilledCircle(SDL_Renderer* renderer, int x0, int y0, int radius);
void SDL_RenderDrawThickLine(SDL_Renderer* renderer,
                       int           x1,
                       int           y1,
                       int           x2,
                       int           y2,
                       int           width);
void drawCircle(SDL_Renderer* renderer, int x0, int y0, int radius);
