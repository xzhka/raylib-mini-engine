#include "core/ConfigLoader.h"

#include "core/ParameterRegistry.h"
#include "core/StringUtils.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <set>

namespace 
{
    std::string CleanLine(std::string line) 
    {
        for (const char* marker : { "#", "//" }) 
        {
            const auto pos = line.find(marker);
            if (pos != std::string::npos) line.erase(pos);
        }
        std::replace(line.begin(), line.end(), '=', ' ');
        return StringUtils::Trim(line);
    }
}

std::string ConfigLoadResult::Summary() const 
{
    if (!fileFound) 
    {
        return "Cannot open config file: " + path;
    }
    if (!errors.empty()) 
    {
        std::string text = "Config rejected, nothing was changed (" + path + "):";
        for (const std::string& e : errors) text += "\n  " + e;
        return text;
    }
    std::string text = "Config loaded from " + path + ": " + std::to_string(applied) + " parameter(s) applied";
    for (const std::string& m : missing) text += "\n  note: '" + m + "' not in file, keeping current value";
    return text;
}

ConfigLoadResult ConfigLoader::Load(const std::string& path, SimulationParams& params) 
{
    ConfigLoadResult result;
    result.path = path;

    std::ifstream file{ std::filesystem::u8path(path) }; // u8path: non-ASCII Windows paths work too
    if (!file) return result;
    result.fileFound = true;

    SimulationParams candidate = params;
    ParameterRegistry registry(candidate);
    std::set<std::string> seen;

    std::string raw;
    int lineNo = 0;
    while (std::getline(file, raw)) 
    {
        ++lineNo;
        if (lineNo == 1 && raw.compare(0, 3, "\xEF\xBB\xBF") == 0) raw.erase(0, 3); // UTF-8 BOM

        const std::string line = CleanLine(raw);
        if (line.empty()) continue;

        const std::vector<std::string> tokens = StringUtils::SplitWhitespace(line);
        const std::string where = "line " + std::to_string(lineNo) + ": ";
        if (tokens.size() != 2) 
        {
            result.errors.push_back(where + "expected '<name> <value>'");
            continue;
        }

        std::string error;
        if (!registry.Set(tokens[0], tokens[1], error)) 
        {
            result.errors.push_back(where + error);
            continue;
        }
        seen.insert(registry.Find(tokens[0])->name);
    }

    if (!result.errors.empty()) return result;

    for (const auto& p : registry.All()) 
    {
        if (!seen.count(p.name)) result.missing.push_back(p.name);
    }
    params = candidate;
    result.applied = static_cast<int>(seen.size());
    return result;
}
