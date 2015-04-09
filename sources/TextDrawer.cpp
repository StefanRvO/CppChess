#include "../headers/TextDrawer.h"
TextDrawer::TextDrawer(const char* fontpath,int fontsize)
{
    font=TTF_OpenFont(fontpath,fontsize);
}
TextDrawer::~TextDrawer()
{
    TTF_CloseFont(font);
}
void TextDrawer::DrawText(SDL_Renderer *render,const char* text,int x,int y,int r,int g,int b, int a)
{
    SDL_Color color;
    color.r=r;
    color.g=g;
    color.b=b;
    color.a=a;
    SDL_Rect rect;
    rect.x=x;
    rect.y=y;
    TTF_SizeUTF8(font,text,&rect.w,&rect.h);
    SDL_Surface *textSurface=TTF_RenderUTF8_Blended(font, text, color);
    SDL_Texture *textTexture=SDL_CreateTextureFromSurface(render, textSurface);
    SDL_RenderCopy(render,textTexture, NULL, &rect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}
