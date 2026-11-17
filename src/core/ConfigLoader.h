#pragma once

#include "core/Config.h"

#include <string>
#include <vector>

struct ConfigLoadResult 
{
    std::string path;
    bool fileFound = false;
    int applied = 0;
    std::vector<std::string> errors; // any error => nothing was applied
    std::vector<std::string> missing; // parameters absent from the file (kept as they were)

    bool Success() const { return fileFound && errors.empty(); }
    std::string Summary() const; // human readable, multi-line
};

// Loads "<name> <value>" lines (comments: # or //, optional '=') into SimulationParams.
// The load is ATOMIC: the file is parsed into a copy, and the copy replaces the live
// parameters only if the whole file is valid. A typo therefore can never leave the
// running simulation with half-applied settings.
class ConfigLoader 
{
    public:
    static ConfigLoadResult Load(const std::string& path, SimulationParams& params);
};
