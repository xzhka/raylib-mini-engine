#include "tools/EventLog.h"

#include "core/Clock.h"
#include "core/Config.h"
#include <raylib.h>
#include <algorithm>

namespace 
{
    constexpr int kWidth = 340;
    constexpr int kMaxLines = 20;
    constexpr int kLineH = 18;
    constexpr int kHeaderH = 26;
    constexpr int kFont = 16;
}

void EventLog::Log(const std::string& text) 
{
    Entries.push_back({ text, Clock::NowMs() });
}

void EventLog::Update() 
{
    const double now = Clock::NowMs();
    const double lifetimeMs = Params.logEntryLifetimeSec * 1000.0;
    while (!Entries.empty() && now - Entries.front().createdAtMs >= lifetimeMs) 
    {
        Entries.pop_front();
    }
}

void EventLog::Draw(int x, int y) const 
{
    DrawRectangle(x, y, kWidth, kHeaderH + kMaxLines * kLineH + 6, Fade(BLACK, 0.65f));
    DrawText("Event Log", x + 8, y + 5, kFont, SKYBLUE);

    // If more lines than fit, show the newest ones.
    const std::size_t total = Entries.size();
    const std::size_t first = total > kMaxLines ? total - kMaxLines : 0;

    int lineY = y + kHeaderH;
    for (std::size_t i = first; i < total; ++i) 
    {
        DrawText(Entries[i].text.c_str(), x + 8, lineY, kFont, RAYWHITE);
        lineY += kLineH;
    }
}
