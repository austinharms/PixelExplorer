// modified from https://gist.github.com/gongzhitaao/7062087

#include <chrono>

#ifndef PIXELEXPLORER_TIMER_H_
#define PIXELEXPLORER_TIMER_H_
namespace pixelexplorer {
    class Timer
    {
    public:
        inline Timer() : _startTime(ClockType::now()) {}
        inline void reset() { _startTime = ClockType::now(); }
        double elapsed() const {
            return std::chrono::duration_cast<SecondDuration>(ClockType::now() - _startTime).count();
        }

    private:
        typedef std::chrono::high_resolution_clock ClockType;
        typedef std::chrono::duration<double, std::ratio<1>> SecondDuration;
        std::chrono::time_point<ClockType> _startTime;
    };
}
#endif // !PIXELEXPLORER_TIMER_H_
