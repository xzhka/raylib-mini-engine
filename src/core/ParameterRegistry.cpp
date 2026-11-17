#include "core/ParameterRegistry.h"

#include "core/StringUtils.h"

#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>

ParameterRegistry::ParameterRegistry(SimulationParams& p) 
{
    Parameters = {
        { "spawnPeriod",   "Seconds between enemy spawns", 0.01, 3600.0, &p.spawnPeriodSec, nullptr },
        { "minEnemySpeed", "Minimum enemy speed (units/s)", 0.01, 1000.0, &p.minEnemySpeed, nullptr },
        { "maxEnemySpeed", "Maximum enemy speed (units/s)", 0.01, 1000.0, &p.maxEnemySpeed, nullptr },
        { "maxEnemies",    "Max enemies alive at once", 1.0, 10000.0, nullptr, &p.maxEnemies },
        { "logLifetime",   "Seconds an event log line stays", 0.1, 3600.0, &p.logEntryLifetimeSec, nullptr },
    };
}

const ParameterRegistry::Parameter* ParameterRegistry::Find(const std::string& name) const 
{
    for (const Parameter& p : Parameters) 
    {
        if (StringUtils::EqualsIgnoreCase(p.name, name)) return &p;
    }
    return nullptr;
}

bool ParameterRegistry::Set(const std::string& name, const std::string& valueText, std::string& error) 
{
    const Parameter* p = Find(name);
    if (!p) 
    {
        error = "Unknown parameter '" + name + "'";
        return false;
    }

    const char* begin = valueText.c_str();
    char* end = nullptr;
    errno = 0;
    double value = 0.0;

    if (p->intValue) 
    {
        const long parsed = std::strtol(begin, &end, 10);
        if (end == begin || *end != '\0' || errno == ERANGE) 
        {
            error = p->name + " expects an integer";
            return false;
        }
        value = static_cast<double>(parsed);
    }
    else 
    {
        value = std::strtod(begin, &end);
        if (end == begin || *end != '\0' || errno == ERANGE || !std::isfinite(value)) 
        {
            error = p->name + " expects a number (use '.' as decimal separator)";
            return false;
        }
    }

    if (value < p->minValue || value > p->maxValue) 
    {
        char buffer[128];
        std::snprintf(buffer, sizeof(buffer), "%s must be in [%g, %g]", p->name.c_str(), p->minValue, p->maxValue);
        error = buffer;
        return false;
    }

    if (p->intValue) *p->intValue = static_cast<int>(value);
    else *p->floatValue = static_cast<float>(value);
    return true;
}

std::string ParameterRegistry::ValueToString(const Parameter& p) const 
{
    char buffer[32];
    if (p.intValue) std::snprintf(buffer, sizeof(buffer), "%d", *p.intValue);
    else std::snprintf(buffer, sizeof(buffer), "%g", static_cast<double>(*p.floatValue));
    return buffer;
}
