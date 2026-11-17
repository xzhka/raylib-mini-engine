#pragma once

#include <cstddef>

// RAII timer constructor starts the block, destructor ends the block.
// Usage: { PROFILE_SCOPE("Physics", "Logic"); ... }
class ScopeMarker 
{
    public:
    ScopeMarker(const char* name, const char* category);
    ~ScopeMarker();

    ScopeMarker(const ScopeMarker&) = delete;
    ScopeMarker& operator=(const ScopeMarker&) = delete;

    private:
    std::size_t Slot; // index of our record in the profiler (keeps pre-order)
    double StartMs;
};

#define PROFILE_CONCAT_INNER(a, b) a##b
#define PROFILE_CONCAT(a, b) PROFILE_CONCAT_INNER(a, b)
#define PROFILE_SCOPE(name, category) \
    ScopeMarker PROFILE_CONCAT(profileScope_, __LINE__)(name, category)
