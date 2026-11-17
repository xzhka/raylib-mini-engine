#include "tools/ScopeMarker.h"

#include "core/Clock.h"
#include "tools/Profiler.h"

ScopeMarker::ScopeMarker(const char* name, const char* category)
    : Slot(Profiler::Instance().OpenMarker(name, category)), StartMs(Clock::NowMs()) {}

ScopeMarker::~ScopeMarker() 
{
    const double durationMs = Clock::NowMs() - StartMs;
    Profiler::Instance().CloseMarker(Slot, durationMs);
}
