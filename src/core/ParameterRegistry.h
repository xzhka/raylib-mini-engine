#pragma once

#include "core/Config.h"

#include <string>
#include <vector>

// Single place that knows every tunable parameter: its name, valid range and where
// it lives inside SimulationParams. Both the F5 config loader and the console go
// through Set(), so parsing and validation exist exactly once.
class ParameterRegistry 
{
    public:
    struct Parameter 
    {
        std::string name;
        std::string description;
        double minValue;
        double maxValue;
        float* floatValue;
        int* intValue;
    };

    explicit ParameterRegistry(SimulationParams& params);
    ParameterRegistry(const ParameterRegistry&) = delete; // holds pointers into `params`
    ParameterRegistry& operator=(const ParameterRegistry&) = delete;

    const Parameter* Find(const std::string& name) const; // case-insensitive
    bool Set(const std::string& name, const std::string& valueText, std::string& error);
    std::string ValueToString(const Parameter& parameter) const;
    const std::vector<Parameter>& All() const { return Parameters; }

    private:
    std::vector<Parameter> Parameters;
};
