#pragma once

#include <cstddef>
#include <string>
#include <vector>

// One finished measurement.
struct MarkerRecord 
{
    std::string name;
    std::string category;
    double durationMs = 0.0;
    int depth = 0;      // nesting level (0 = root)
};

// Frame-based instrumentation profiler (data side only; drawing is ProfilerOverlay).
//
// Records are stored in the order the scopes were OPENED (pre-order), so the flat
// vector together with `depth` is exactly the call tree - no tree structure needed.
// Two buffers: the frame being recorded and the last completed one, which the
// overlay shows (a frame can't display its own, still unfinished, timings).
class Profiler 
{
    public:
    static Profiler& Instance();

    // Call once at the very start of every frame: publishes the finished
    // frame's markers and clears the buffer for the new one.
    void BeginFrame();

    // Used by ScopeMarker only.
    std::size_t OpenMarker(const std::string& name, const std::string& category);
    void CloseMarker(std::size_t slot, double durationMs);

    const std::vector<MarkerRecord>& LastFrame() const { return LastShowedFrame; }

    private:
    Profiler() = default;

    std::vector<MarkerRecord> Current;
    std::vector<MarkerRecord> LastShowedFrame;
    int Depth = 0;
};
