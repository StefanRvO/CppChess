#pragma once
#include <sys/time.h>
#include <unistd.h>
#include "RingBuffer.h"
#define AVGLENGHT 30
class Timer
{
    private:
    timeval last;
    timeval slast;
    int framerate;
    RingBuffer<float,AVGLENGHT> *buf;
    public:
    Timer(int fps)
    {
        framerate=fps;
        gettimeofday(&last,NULL);
        gettimeofday(&slast,NULL);
        buf=new RingBuffer<float,AVGLENGHT>;
        for(int i=0;i<AVGLENGHT; i++) buf->push_back(0);
    }
    void tick()
    {
        timeval tv;
        gettimeofday(&tv,NULL);
        long long goal=last.tv_sec*1000000+last.tv_usec+(1000000./framerate);
        long long cur=tv.tv_sec*1000000+tv.tv_usec;
        if(goal>cur)
        {
            long long diff=goal-cur;
            usleep(diff);
        }
        slast=last;
        last=tv;
        long long lastT=last.tv_sec*1000000+last.tv_usec;
        long long slastT=slast.tv_sec*1000000+slast.tv_usec;
        long diff=lastT-slastT;
        float fps=(float)(1000000./diff);
        buf->push_back(fps);
    }
    void presisionTick() //use more cpu while waiting
    {
        timeval tv;
        gettimeofday(&tv,NULL);
        long long goal=last.tv_sec*1000000+last.tv_usec+(1000000./framerate);
        long long cur=tv.tv_sec*1000000+tv.tv_usec;
        while(goal>cur)
        {
            usleep((goal-cur)/10);
            gettimeofday(&tv,NULL);
            cur=tv.tv_sec*1000000+tv.tv_usec;
        }
        slast=last;
        last=tv;
        long long lastT=last.tv_sec*1000000+last.tv_usec;
        long long slastT=slast.tv_sec*1000000+slast.tv_usec;
        long diff=lastT-slastT;
        float fps=(float)(1000000./diff);
        buf->push_back(fps);
    }

    void highPresisionTick() //use 100% cpu while waiting
    {
        timeval tv;
        gettimeofday(&tv,NULL);
        long long goal=last.tv_sec*1000000+last.tv_usec+(1000000./framerate);
        long long cur=tv.tv_sec*1000000+tv.tv_usec;
        while(goal>cur)
        {
            gettimeofday(&tv,NULL);
            cur=tv.tv_sec*1000000+tv.tv_usec;
        }
        slast=last;
        last=tv;
        long long lastT=last.tv_sec*1000000+last.tv_usec;
        long long slastT=slast.tv_sec*1000000+slast.tv_usec;
        long diff=lastT-slastT;
        float fps=(float)(1000000./diff);
        buf->push_back(fps);
    }
    void setFPS(int fps)
    {
        framerate=fps;
    }
    int getFPS()
    {
        return buf->at(AVGLENGHT-1);
    }
    int getAvgFPS()
    {
        return buf->getAvg();
    }

};
