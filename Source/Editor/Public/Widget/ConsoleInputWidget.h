#pragma once

#include <string>
#include <string_view>
#include <vector>

struct ImGuiInputTextCallbackData;

namespace editor
{
    class ConsoleInputWidget
    {
    public:
        void Draw();
        void HandleUserInput( const engine::UserInput& input );

    private:
        enum class DisplayMode
        {
            Hidden = 0,
            Minimal,
            Full,
        };

        static int32 ConsoleInputTextCallback( ImGuiInputTextCallbackData* data );

        void ToggleDisplayMode();
        void SetDisplayMode( DisplayMode displayMode );

        void DrawConsole() const;
        void DrawAutoCompletion( float baseY );
        void DrawConsoleInput( float baseY );

        void DrawMinimal();
        void DrawFull();

        void UpdateAutoCompletion( const std::string_view& input );
        void ClearAutoCompletion();

        static constexpr float ConsoleWindowSizeRatio = 0.7f;
        static constexpr std::string_view ConsoleInputDefaultString = "> ";
        static constexpr size_t BufferStartOffset = ConsoleInputDefaultString.size();

        DisplayMode m_displayMode = DisplayMode::Hidden;

        char m_buffer[BufferStartOffset + 1024] = {};

        int32 m_autoCompletionCursor = -1;
        std::vector<std::string> m_autoCompletionList;

        bool m_needClearInputTextSelection = false;
        bool m_needInputTextFocus = false;
    };
}
