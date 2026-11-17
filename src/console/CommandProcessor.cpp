#include "console/CommandProcessor.h"

#include "core/ParameterRegistry.h"
#include "core/StringUtils.h"

#include <cstdio>

CommandProcessor::CommandProcessor(ParameterRegistry& registry) : Registry(registry) 
{
    Register("help", "List commands and parameters", [this](const Args&) { return Help(); });
}

void CommandProcessor::Register(const std::string& name, const std::string& help, Handler handler) 
{
    Commands.push_back({ name, help, std::move(handler) });
}

CommandResult CommandProcessor::Execute(const std::string& line) 
{
    const std::vector<std::string> tokens = StringUtils::SplitWhitespace(line);
    if (tokens.empty()) return {};

    const std::string& name = tokens.front();
    const Args args(tokens.begin() + 1, tokens.end());

    for (const Command& command : Commands) 
    {
        if (StringUtils::EqualsIgnoreCase(command.name, name)) return command.handler(args);
    }

    if (const ParameterRegistry::Parameter* p = Registry.Find(name)) 
    {
        if (args.empty()) 
        {
            return { true, p->name + " = " + Registry.ValueToString(*p) };
        }
        if (args.size() > 1) 
        {
            return { false, "Usage: " + p->name + " <value>" };
        }
        std::string error;
        if (!Registry.Set(name, args[0], error)) 
        {
            return { false, error };
        }
        return { true, p->name + " = " + Registry.ValueToString(*p) };
    }

    return { false, "Unknown command or parameter '" + name + "'. Type 'help'." };
}

CommandResult CommandProcessor::Help() const 
{
    char buffer[200];
    std::string text = "Commands:";
    for (const Command& c : Commands) 
    {
        std::snprintf(buffer, sizeof(buffer), "\n  %-14s %s", c.name.c_str(), c.help.c_str());
        text += buffer;
    }

    text += "\nParameters ('<name> <value>' to set, '<name>' to read):";
    for (const auto& p : Registry.All()) 
    {
        std::snprintf(buffer, sizeof(buffer), "\n  %-14s = %-6s [%g .. %g]  %s",
                      p.name.c_str(), Registry.ValueToString(p).c_str(),
                      p.minValue, p.maxValue, p.description.c_str());
        text += buffer;
    }
    return { true, text };
}
