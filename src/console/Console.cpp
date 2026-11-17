#include "console/Console.h"

#include "console/CommandProcessor.h"
#include "core/StringUtils.h"

#include <raylib.h>

#include <algorithm>

namespace 
{
    constexpr int kFont = 18;
    constexpr int kLineH = 21;
    constexpr int kMaxInputChars = 120;
    constexpr int kMaxLines = 300;
    constexpr float kSlideSpeed = 5.0f; // 1/s -> ~0.2 s to open
    constexpr float kHeightRatio = 0.5f; // fraction of the screen

    int ConsoleHeight() { return static_cast<int>(GetScreenHeight() * kHeightRatio); }

    int VisibleLines() { return std::max(1, (ConsoleHeight() - kLineH - 18) / kLineH); }
}

Console::Console(CommandProcessor& processor) : Processor(processor) 
{
    Print("Console ready. Type 'help' for commands and parameters.");
}

void Console::Update(float dt) 
{
    const bool toggled = IsKeyPressed(KEY_GRAVE); // "~" / "`"
    if (toggled) 
    {
        Opened = !Opened;
    }
    else if (Opened && IsKeyPressed(KEY_ESCAPE)) 
    {
        Opened = false;
    }


    if (toggled || !Opened) DiscardTypedChars();
    else HandleTyping();

    const float target = Opened ? 1.0f : 0.0f;
    const float step = kSlideSpeed * dt;
    Slide = (Slide < target) ? std::min(Slide + step, target) : std::max(Slide - step, target);
}

void Console::DiscardTypedChars() 
{
    while (GetCharPressed() != 0) {}
}

void Console::HandleTyping() 
{
    for (int c = GetCharPressed(); c != 0; c = GetCharPressed()) 
    {
        const bool printable = c >= 32 && c <= 126;
        if (printable && c != '`' && c != '~' && static_cast<int>(InputText.size()) < kMaxInputChars) 
        {
            InputText.push_back(static_cast<char>(c));
        }
    }

    if ((IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) && !InputText.empty()) 
    {
        InputText.pop_back();
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) 
    {
        Submit();
    }

    const int historySize = static_cast<int>(History.size());
    if (IsKeyPressed(KEY_UP) && historySize > 0) 
    {
        HistoryPos = std::max(0, HistoryPos - 1);
        InputText = History[HistoryPos];
    }
    if (IsKeyPressed(KEY_DOWN) && historySize > 0) 
    {
        HistoryPos = std::min(historySize, HistoryPos + 1);
        InputText = (HistoryPos < historySize) ? History[HistoryPos] : std::string();
    }

    if (IsKeyPressed(KEY_PAGE_UP)) ScrollOffset += VisibleLines() / 2;
    if (IsKeyPressed(KEY_PAGE_DOWN)) ScrollOffset -= VisibleLines() / 2;
    ScrollOffset = std::clamp(ScrollOffset, 0, MaxScroll());
}

void Console::Submit() 
{
    const std::string line = StringUtils::Trim(InputText);
    InputText.clear();
    if (line.empty()) return;

    AddLines("> " + line, LineKind::Echo);
    if (History.empty() || History.back() != line) History.push_back(line);
    HistoryPos = static_cast<int>(History.size());
    ScrollOffset = 0;

    const CommandResult result = Processor.Execute(line);
    if (!result.text.empty()) Print(result.text, !result.ok);
}

void Console::Print(const std::string& text, bool isError) 
{
    AddLines(text, isError ? LineKind::Error : LineKind::Normal);
}

void Console::AddLines(const std::string& text, LineKind kind) 
{
    for (const std::string& line : StringUtils::SplitLines(text)) 
    {
        Lines.push_back({ line, kind });
    }
    while (static_cast<int>(Lines.size()) > kMaxLines) Lines.pop_front();
}

void Console::Clear() 
{
    Lines.clear();
    ScrollOffset = 0;
}

int Console::MaxScroll() const 
{
    return std::max(0, static_cast<int>(Lines.size()) - VisibleLines());
}

void Console::Draw() const 
{
    if (Slide <= 0.0f) return;

    const int width = GetScreenWidth();
    const int height = ConsoleHeight();
    const float eased = 1.0f - (1.0f - Slide) * (1.0f - Slide);
    const int top = static_cast<int>(height * eased) - height;

    DrawRectangle(0, top, width, height, Fade(Color{ 8, 10, 16, 255 }, 0.90f));
    DrawRectangle(0, top + height - 2, width, 2, Color{ 90, 200, 255, 255 });

    const int inputY = top + height - kLineH - 8;

    const int total = static_cast<int>(Lines.size());
    const int visible = VisibleLines();
    for (int i = 0; i < visible; ++i) 
    {
        const int index = total - 1 - ScrollOffset - i;
        if (index < 0) break;

        const Line& line = Lines[index];
        Color color = LIGHTGRAY;
        if (line.kind == LineKind::Echo) color = Color{ 255, 220, 120, 255 };
        if (line.kind == LineKind::Error) color = Color{ 255, 110, 110, 255 };
        DrawText(line.text.c_str(), 10, inputY - 4 - (i + 1) * kLineH, kFont, color);
    }

    const std::string prompt = "> " + InputText;
    DrawText(prompt.c_str(), 10, inputY, kFont, WHITE);
    if (static_cast<int>(GetTime() * 2.0) % 2 == 0) 
    {
        DrawText("_", 10 + MeasureText(prompt.c_str(), kFont), inputY, kFont, WHITE);
    }
    if (ScrollOffset > 0) 
    {
        DrawText("(scrolled - PageDown)", width - 220, inputY, 14, GRAY);
    }
}
