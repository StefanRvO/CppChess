//Implements a ringbuffer on top of the array class
//You put things in the buffer with push_back(T) and get them off with pop_front()
#pragma once
#include <stdexcept>
#include <exception>
#include <array>
#include <mutex>
#include <cmath>
#include <limits>
template <class T,std::size_t _size>
class RingBuffer
{
    private:
        std::array<T,_size> *Buffer;
        size_t cap;
        unsigned long long HEAD;
        unsigned long long TAIL;
        unsigned long long ioTotal;
        std::mutex lock;
    public:
        RingBuffer()
        {
            Buffer=new std::array<T,_size>;
            HEAD=0;
            TAIL=0;
            ioTotal=0;
            cap=_size;
        }
        T at(size_t index) //return (reference to) element at index
        {
            return *(Buffer->data()+(TAIL+index)%capacity());
        }
        size_t size()
        {
            return ioTotal;
        }

        size_t capacity()
        {
            return cap;
        }
        void eraseElements(long count) //erase this amount of elements from buffer
        {
            lock.lock();
            if (count>ioTotal) count=ioTotal;
            TAIL=(TAIL+count)%capacity();
            ioTotal-=count;
            lock.unlock();
        }
        bool empty()
        {
            return ioTotal==0;
        }

        bool full()
        {
            return ioTotal>=capacity();
        }

        void push_back(T element)
        {
            if (full())
            {
                lock.lock();
                *(Buffer->data()+HEAD)=element;
                HEAD=(HEAD+1)%capacity();
                TAIL=(TAIL+1)%capacity();
                lock.unlock();
                return;
            }
            lock.lock();
            *(Buffer->data()+HEAD)=element;
            HEAD=(HEAD+1)%capacity();
            ioTotal++;
            lock.unlock();
        }

        T pop_front()
        {
            if (empty()) throw(std::range_error("Buffer is empty"));
            lock.lock();
            T returnval=*(Buffer->data()+TAIL);
            TAIL=(TAIL+1)%capacity();
            ioTotal--;
            lock.unlock();
            return returnval;
        }
        void clear()
        {
            lock.lock();
            HEAD=0;
            TAIL=0;
            ioTotal=0;
            lock.unlock();
        }
        double getAvg()
        {
            size_t tmpsize=size();
            long long tmpTAIL=TAIL;
            double avg=0;
            for(unsigned int i=0; i<tmpsize;i++)
            {
                avg+=*(Buffer->data()+((tmpTAIL+i)%capacity()));
            }
            return avg/tmpsize;
        }
        double getStandDiv()
        {
            size_t tmpsize=size();
            double mean= getAvg();
            long long tmpTAIL=TAIL;
            double avg=0;
            for(size_t i=0; i<tmpsize;i++)
            {
                avg+=pow(*(Buffer->data()+((tmpTAIL+i)%capacity()))-mean,2);
            }
            avg/=tmpsize;
            return sqrt(avg);
        }
        double getRelativeStandDiv()
        {
            size_t tmpsize=size();
            double mean= getAvg();
            long long tmpTAIL=TAIL;
            double avg=0;
            for(size_t i=0; i<tmpsize;i++)
            {
                avg+=pow(*(Buffer->data()+((tmpTAIL+i)%capacity()))-mean,2);
            }
            avg/=tmpsize;
            return sqrt(avg)/mean;
        }
        double getMaxDiff()
        {
            size_t tmpsize=size();
            long long tmpTAIL=TAIL;
            double max=-std::numeric_limits<double>::infinity();
            double min=std::numeric_limits<double>::infinity();
            for(size_t i=0; i<tmpsize;i++)
            {
                if(*(Buffer->data()+((tmpTAIL+i)%capacity()))>max) max=*(Buffer->data()+((tmpTAIL+i)%capacity()));
                if(*(Buffer->data()+((tmpTAIL+i)%capacity()))<min) min=*(Buffer->data()+((tmpTAIL+i)%capacity()));
            }
            return max-min;
        }
        double getRealativeMaxDiff()
        {
            size_t tmpsize=size();
            double mean= getAvg();
            long long tmpTAIL=TAIL;
            double max=-std::numeric_limits<double>::infinity();
            double min=std::numeric_limits<double>::infinity();
            for(size_t i=0; i<tmpsize;i++)
            {
                if(*(Buffer->data()+((tmpTAIL+i)%capacity()))>max) max=*(Buffer->data()+((tmpTAIL+i)%capacity()));
                if(*(Buffer->data()+((tmpTAIL+i)%capacity()))<min) min=*(Buffer->data()+((tmpTAIL+i)%capacity()));
            }
            return (max-min)/mean;
        }

};
