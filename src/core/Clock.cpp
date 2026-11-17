#include "core/Clock.h"

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

double Clock::NowMs() 
{
    static const LONGLONG frequency = [] 
    {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f.QuadPart;
    }();
    static const LONGLONG origin = [] 
    {
        LARGE_INTEGER c;
        QueryPerformanceCounter(&c);
        return c.QuadPart;
    }();

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return static_cast<double>(now.QuadPart - origin) * 1000.0 / static_cast<double>(frequency);
}

const char* Clock::BackendName() { return "QueryPerformanceCounter"; }

#else

#include <chrono>

double Clock::NowMs() 
{
    using namespace std::chrono;
    static const auto origin = steady_clock::now();
    return duration<double, std::milli>(steady_clock::now() - origin).count();
}

const char* Clock::BackendName() { return "std::chrono"; }

#endif
