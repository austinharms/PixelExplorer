// modified from https://gist.github.com/gongzhitaao/7062087

#include <chrono>

#ifndef PIXELEXPLORER_TIMER_H_
#define PIXELEXPLORER_TIMER_H_
namespace pixelexplorer {
    class Timer
    {
    public:
        inline Timer();
        inline void reset();
        inline double elapsed() const;

    private:
        typedef std::chrono::high_resolution_clock ClockType;
        typedef std::chrono::duration<double, std::ratio<1>> SecondDuration;
        std::chrono::time_point<ClockType> _startTime;
    };
}

#include "Timer.inl"
#endif // !PIXELEXPLORER_TIMER_H_
