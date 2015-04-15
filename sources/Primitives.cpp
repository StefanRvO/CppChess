#include "../headers/Primitives.h"
#include <cmath>
void SDL_Draw_Arrow(SDL_Renderer* renderer, //crude hack to avoid thickLineRGBA
                       int           x1,
                       int           y1,
                       int           x2,
                       int           y2,
                       int           arrowHeight,
                       int           arrowWidth,
                       int           width)
{
    auto angle = atan2(float((y2-y1)),float(x2-x1));
    //Draw base line
    SDL_RenderDrawThickLine(renderer, x1, y1, x2, y2, width);

    //Draw first arrow part
    SDL_RenderDrawThickLine(renderer, x2,y2,x2-arrowHeight*cos(angle)+arrowWidth*sin(angle),
      y2-arrowHeight*sin(angle)-arrowWidth*cos(angle), width);
    //Draw second arrow part
    SDL_RenderDrawThickLine(renderer, x2,y2,x2-arrowHeight*cos(angle)-arrowWidth*sin(angle),
      y2-arrowHeight*sin(angle)+arrowWidth*cos(angle), width);
}

void drawFilledCircle(SDL_Renderer* renderer, int x0, int y0, int radius)
{ //Uses the midpoint circle algorithm to draw a filled circle
  //https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
    int x=radius;
    int y=0;
    int radiusError=1-x;
    while(x>=y)
    {
        SDL_RenderDrawLine(renderer,x+x0,y+y0,-x+x0,y+y0);
        SDL_RenderDrawLine(renderer,y+x0,x+y0,-y+x0,x+y0);
        SDL_RenderDrawLine(renderer,-x+x0,-y+y0,x+x0,-y+y0);
        SDL_RenderDrawLine(renderer,-y+x0,-x+y0,y+x0,-x+y0);
        y++;
        if(radiusError<0) radiusError+=2*y+1;
        else
        {
            x--;
            radiusError+=2*(y-x+1);
        }
    }
}
void drawCircle(SDL_Renderer* renderer, int x0, int y0, int radius)
{ //Uses the midpoint circle algorithm to draw a filled circle
  //https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
    int x=radius;
    int y=0;
    int radiusError=1-x;
    while(x>=y)
    {
        SDL_RenderDrawPoint(renderer,x+x0,y+y0);
        SDL_RenderDrawPoint(renderer,y+x0,x+y0);
        SDL_RenderDrawPoint(renderer,-x+x0,y+y0);
        SDL_RenderDrawPoint(renderer,-y+x0,x+y0);
        SDL_RenderDrawPoint(renderer,-x+x0,-y+y0);
        SDL_RenderDrawPoint(renderer,-y+x0,-x+y0);
        SDL_RenderDrawPoint(renderer,x+x0,-y+y0);
        SDL_RenderDrawPoint(renderer,y+x0,-x+y0);
        y++;
        if(radiusError<0) radiusError+=2*y+1;
        else
        {
            x--;
            radiusError+=2*(y-x+1);
        }
    }
}

void SDL_RenderDrawThickLine(SDL_Renderer* renderer, //crude hack to avoid thickLineRGBA
                       int           x1,
                       int           y1,
                       int           x2,
                       int           y2,
                       int           width)
{
    float offset=width/2.;
    for(float i=-offset; i<offset; i++)
    {
      SDL_RenderDrawLine(renderer, x1+i,y1,x2+i,y2);
      SDL_RenderDrawLine(renderer, x1,y1+i,x2,y2+i);
    }

}
