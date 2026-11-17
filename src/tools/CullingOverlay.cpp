#include "tools/CullingOverlay.h"

#include "render/Renderer.h"

#include <raylib.h>

namespace 
{
    constexpr int kWidth = 340;
    constexpr int kRowH = 17;
    constexpr int kHeaderH = 26;
    constexpr int kFont = 15;
}

int CullingOverlay::Width() { return kWidth; }

void CullingOverlay::Draw(const Renderer& renderer, int x, int y) const 
{
    const CullingStats& s = renderer.Stats();

    // Rows with at least one object only
    int rows = 0;
    for (std::size_t k = 0; k < CullingStats::kKinds; ++k) 
    {
        if (s.TotalByKind[k] > 0) ++rows;
    }

    const int height = kHeaderH + 3 * kRowH + 8 + (rows + 1) * kRowH + 2 * kRowH + 8;
    DrawRectangle(x, y, kWidth, height, Fade(BLACK, 0.65f));

    const bool on = renderer.IsCullingEnabled();
    DrawText(TextFormat("Frustum culling [%s]%s", on ? "ON" : "OFF", renderer.IsFrustumFrozen() ? " [FROZEN]" : ""),
             x + 8, y + 5, 16, on ? LIME : ORANGE);

    const float total = s.Total > 0 ? static_cast<float>(s.Total) : 1.0f;
    int rowY = y + kHeaderH;
    DrawText(TextFormat("Total objects : %zu", s.Total), x + 8, rowY, kFont, RAYWHITE);
    rowY += kRowH;
    DrawText(TextFormat("Visible (drawn): %zu  (%.1f %%)", s.Visible, s.Visible / total * 100.0f), x + 8, rowY, kFont, LIME);
    rowY += kRowH;
    DrawText(TextFormat("Culled         : %zu  (%.1f %%)", s.Culled, s.Culled / total * 100.0f), x + 8, rowY, kFont, Color{ 255, 120, 120, 255 });
    rowY += kRowH + 8;

    DrawText("type", x + 8, rowY, 13, GRAY);
    DrawText("visible / total", x + 190, rowY, 13, GRAY);
    rowY += kRowH;
    for (std::size_t k = 0; k < CullingStats::kKinds; ++k) 
    {
        if (s.TotalByKind[k] == 0) continue;
        DrawText(ObjectKindName(static_cast<ObjectKind>(k)), x + 8, rowY, 14, LIGHTGRAY);
        DrawText(TextFormat("%zu / %zu", s.VisibleByKind[k], s.TotalByKind[k]), x + 190, rowY, 14, WHITE);
        rowY += kRowH;
    }

    rowY += 8;
    DrawText("F4 culling on/off   F6 freeze frustum", x + 8, rowY, 13, GRAY);
    DrawText("F1 editor  F2 save  F3 load  R camera reset", x + 8, rowY + kRowH, 13, GRAY);
}
