#include "tools/ProfilerOverlay.h"

#include "core/Clock.h"
#include "tools/Profiler.h"

#include <raylib.h>

#include <functional>
#include <string>

namespace 
{
    constexpr int kWidth   = 470;
    constexpr int kRowH    = 16;
    constexpr int kHeaderH = 26;
    constexpr int kFont    = 14;
    constexpr int kIndent  = 14;

    Color CategoryColor(const std::string& category) 
    {
        const float hue = static_cast<float>(std::hash<std::string>{}(category) % 360);
        return ColorFromHSV(hue, 0.45f, 1.0f);
    }
}

int ProfilerOverlay::Width() { return kWidth; }

void ProfilerOverlay::Draw(int x, int y) const 
{
    const auto& records = Profiler::Instance().LastFrame();
    if (records.empty()) return;

    const double frameMs = records.front().durationMs;

    DrawRectangle(x, y, kWidth, kHeaderH + static_cast<int>(records.size()) * kRowH + 6,
                  Fade(BLACK, 0.65f));
    DrawText(TextFormat("Frame profiler [%s]", Clock::BackendName()), x + 8, y + 5, 16, LIME);

    int rowY = y + kHeaderH;
    for (const MarkerRecord& r : records) 
    {
        const double percent = frameMs > 0.0 ? r.durationMs / frameMs * 100.0 : 0.0;

        DrawText(r.name.c_str(),     x + 8 + r.depth * kIndent, rowY, kFont, CategoryColor(r.category));
        DrawText(r.category.c_str(), x + 255, rowY, kFont, GRAY);
        DrawText(TextFormat("%7.2f ms", r.durationMs), x + 325, rowY, kFont, WHITE);
        DrawText(TextFormat("%5.1f %%", percent),      x + 405, rowY, kFont, WHITE);
        rowY += kRowH;
    }
}
