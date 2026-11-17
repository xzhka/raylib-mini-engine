#pragma once

#include <deque>
#include <string>
#include <vector>

class CommandProcessor;

// Quake-style drop-down console: "~" slides it down/up,
// ESC closes it, Up/Down browse command history, PageUp/PageDown scroll the output.
// It only handles input + drawing; what a line MEANS is decided by CommandProcessor.
class Console 
{
    public:
    explicit Console(CommandProcessor& processor);

    void Update(float dt);
    void Draw() const; // call last, on top of everything

    void Print(const std::string& text, bool isError = false);
    void Clear();
    bool IsOpen() const { return Opened; }

    private:
    enum class LineKind { Normal, Echo, Error };
    struct Line 
    {
        std::string text;
        LineKind kind;
    };

    void AddLines(const std::string& text, LineKind kind);
    void HandleTyping();
    void Submit();
    static void DiscardTypedChars();
    int MaxScroll() const;

    CommandProcessor& Processor;
    bool Opened = false;
    float Slide = 0.0f; // 0 = hidden, 1 = fully down
    std::string InputText;
    std::deque<Line> Lines;
    std::vector<std::string> History; // submitted commands
    int HistoryPos = 0;
    int ScrollOffset = 0;
};
