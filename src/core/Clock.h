#pragma once

// Single time source for the whole project (profiler, event log, ...).
//
// Backends:
//   Windows : WinAPI QueryPerformanceCounter / QueryPerformanceFrequency (no std::chrono)
//   other   : std::chrono::steady_clock (so the project still builds on Linux/macOS)
class Clock 
{
    public:
    // Milliseconds since the first call (monotonic).
    static double NowMs();

    // Name of the active backend, shown in the profiler overlay.
    static const char* BackendName();
};
