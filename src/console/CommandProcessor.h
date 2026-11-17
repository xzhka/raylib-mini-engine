#pragma once

#include <functional>
#include <string>
#include <vector>

class ParameterRegistry;

struct CommandResult 
{
    bool ok = true;
    std::string text;
};

// Parses and executes one console line. Knows nothing about drawing or keyboards.
//   <command> [args...] registered commands (ReloadConfig, help, clear, ...)
//   <parameter> <value> sets a parameter from the parameter table
//   <parameter> prints the current value
class CommandProcessor 
{
    public:
    using Args = std::vector<std::string>;
    using Handler = std::function<CommandResult(const Args&)>;

    explicit CommandProcessor(ParameterRegistry& registry);

    void Register(const std::string& name, const std::string& help, Handler handler);
    CommandResult Execute(const std::string& line);

    private:
    struct Command 
    {
        std::string name;
        std::string help;
        Handler handler;
    };

    CommandResult Help() const;

    ParameterRegistry& Registry;
    std::vector<Command> Commands;
};
