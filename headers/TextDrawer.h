#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
class TextDrawer
{
    private:
    TTF_Font *font;
    public:
    TextDrawer(const char* fontpath,int fontsize);
    ~TextDrawer();
    void DrawText(SDL_Renderer *render,const char* text,int x,int y,int r,int g,int b, int a);
};
