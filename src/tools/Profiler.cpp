#include "tools/Profiler.h"

Profiler& Profiler::Instance() 
{
    static Profiler instance;
    return instance;
}

void Profiler::BeginFrame() 
{
    LastShowedFrame.swap(Current);
    Current.clear();
    Depth = 0;
}

std::size_t Profiler::OpenMarker(const std::string& name, const std::string& category) 
{
    Current.push_back({ name, category, 0.0, Depth });
    ++Depth;
    return Current.size() - 1;
}

void Profiler::CloseMarker(std::size_t slot, double durationMs) 
{
    --Depth;
    if (slot < Current.size()) {
        Current[slot].durationMs = durationMs;
    }
}
