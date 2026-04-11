#pragma once

#include "CircularBuffer.h"

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
        void ResetAutoCompletionCursor();
        void ClearAutoCompletion();

        const std::string& GetAutoCompleteString() const;
        int32 GetAutoCompleteStringSize() const;

        static constexpr float ConsoleWindowSizeRatio = 0.7f;
        static constexpr const char* ConsolePrompt = "> ";
        static constexpr int32 ConsolePromptLength = 2;

        DisplayMode m_displayMode = DisplayMode::Hidden;

        char m_buffer[ConsolePromptLength + 1024] = {};

        static constexpr int32 NoAutoCompleteSelection = -1;
        int32 m_autoCompletionCursor = NoAutoCompleteSelection;
        std::vector<std::string> m_autoCompletionList;

        CircularBuffer<std::string, 16> m_history;

        bool m_needClearInputTextSelection = false;
        bool m_needInputTextFocus = false;
    };
}
