#pragma once

#include <deque>
#include <string>

struct SimulationParams;


class EventLog 
{
    public:
    explicit EventLog(const SimulationParams& params) : Params(params) {}

    void Log(const std::string& text);
    void Update(); // drops expired entries
    void Draw(int x, int y) const; // translucent window, top -> bottom

    private:
    struct Entry 
    {
        std::string text;
        double createdAtMs;
    };

    const SimulationParams& Params;
    std::deque<Entry> Entries;
};
