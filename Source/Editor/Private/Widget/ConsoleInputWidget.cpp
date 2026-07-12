#include "ConsoleInputWidget.h"

#include "ConsoleMessage/ConsoleMessageExecutor.h"
#include "Core/LogMessage.h"

#include <algorithm>
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>
#include <ranges>

using ::engine::UserInput;
using ::engine::UserInputCode;

namespace editor
{
    void ConsoleInputWidget::DrawFull()
    {
        float baseY = ImGui::GetCursorPos().y + ImGui::GetWindowHeight() * ConsoleWindowSizeRatio;

        DrawConsole();
        DrawAutoCompletion( baseY );
        DrawConsoleInput( baseY );
    }

    void ConsoleInputWidget::DrawMinimal()
    {
        float baseY = ImGui::GetContentRegionAvail().y + ImGui::GetCursorPos().y - ImGui::GetFrameHeight();

        DrawAutoCompletion( baseY );
        DrawConsoleInput( baseY );
    }

    void ConsoleInputWidget::Draw()
    {
        switch ( m_displayMode )
        {
        case DisplayMode::Hidden:
            break;
        case DisplayMode::Minimal:
            DrawMinimal();
            break;
        case DisplayMode::Full:
            DrawFull();
            break;
        }
    }

    void ConsoleInputWidget::HandleUserInput( const engine::UserInput& input )
    {
        if ( input.IsKeyJustReleased( UserInputCode::UIC_TILDE ) )
        {
            ToggleDisplayMode();
        }
        else if ( input.IsKeyJustReleased( UserInputCode::UIC_ESCAPE ) )
        {
            SetDisplayMode( DisplayMode::Hidden );
        }
    }

    int32 ConsoleInputWidget::ConsoleInputTextCallback( ImGuiInputTextCallbackData* data )
    {
        auto& thisWidget = *static_cast<ConsoleInputWidget*>( data->UserData );

        switch ( data->EventFlag )
        {
        case ImGuiInputTextFlags_CallbackCompletion:
            if ( ( thisWidget.GetAutoCompleteStringSize() > 0 ) && ( thisWidget.m_autoCompletionCursor >= 0 ) )
            {
                int32 actualBufferLen = std::max( 0, data->BufTextLen - ConsolePromptLength );
                data->DeleteChars( ConsolePromptLength, actualBufferLen );

                data->InsertChars( ConsolePromptLength, thisWidget.GetAutoCompleteString().c_str() );

                thisWidget.ClearAutoCompletion();
            }
            break;
        case ImGuiInputTextFlags_CallbackHistory:
            {
                auto numAutoCompletion = thisWidget.GetAutoCompleteStringSize();
                if ( numAutoCompletion > 0 )
                {
                    if ( data->EventKey == ImGuiKey_UpArrow )
                    {
                        --thisWidget.m_autoCompletionCursor;
                    }
                    else
                    {
                        ++thisWidget.m_autoCompletionCursor;
                    }

                    thisWidget.m_autoCompletionCursor = std::max( thisWidget.m_autoCompletionCursor, NoAutoCompleteSelection );
                    thisWidget.m_autoCompletionCursor += numAutoCompletion;
                    thisWidget.m_autoCompletionCursor %= numAutoCompletion;
                }
            }
            break;
        case ImGuiInputTextFlags_CallbackAlways:
            if ( thisWidget.m_needClearInputTextSelection )
            {
                data->ClearSelection();
                thisWidget.m_needClearInputTextSelection = false;
            }

            if ( data->BufTextLen < ConsolePromptLength )
            {
                data->DeleteChars( 0, data->BufTextLen );
                data->InsertChars( 0, ConsolePrompt, ConsolePrompt + ConsolePromptLength );
            }

            data->CursorPos = std::max<int32>( data->CursorPos, ConsolePromptLength );
            break;
        case ImGuiInputTextFlags_CallbackCharFilter:
            return ( data->EventChar == '`' ) ? 1 : 0;
        case ImGuiInputTextFlags_CallbackEdit:
            if ( data->BufTextLen > ConsolePromptLength )
            {
                size_t oldAutoCompletionCount = thisWidget.m_autoCompletionList.size();
                thisWidget.UpdateAutoCompletion( std::string_view{ data->Buf + ConsolePromptLength, static_cast<size_t>( data->BufTextLen - ConsolePromptLength ) } );
                size_t newAutoCompletionCount = thisWidget.m_autoCompletionList.size();

                bool needResetCursor = oldAutoCompletionCount != newAutoCompletionCount;
                if ( needResetCursor )
                {
                    thisWidget.ResetAutoCompletionCursor();
                }
            }
            else
            {
                thisWidget.ClearAutoCompletion();
            }
            break;
        }

        return 0;
    }

    void ConsoleInputWidget::ToggleDisplayMode()
    {
        switch ( m_displayMode )
        {
        case DisplayMode::Hidden:
            SetDisplayMode( DisplayMode::Minimal );
            break;
        case DisplayMode::Minimal:
            SetDisplayMode( DisplayMode::Full );
            break;
        case DisplayMode::Full:
            SetDisplayMode( DisplayMode::Hidden );
            break;
        }
    }

    void ConsoleInputWidget::SetDisplayMode( DisplayMode displayMode )
    {
        m_displayMode = displayMode;
        m_needInputTextFocus = m_displayMode != DisplayMode::Hidden;
    }

    void ConsoleInputWidget::DrawConsole() const
    {
        ImGui::SetNextWindowPos( ImGui::GetCursorScreenPos() );

        ImGui::PushStyleColor( ImGuiCol_ChildBg, static_cast<ImVec4>( ImColor( 0.f, 0.f, 0.f, 0.5f ) ) );

        ImVec2 ConsoleWindowSize( ImGui::GetContentRegionAvail().x, ImGui::GetWindowHeight() * ConsoleWindowSizeRatio );
        ImGui::BeginChild( "ConsoleWindow", ConsoleWindowSize );
        {
            ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );

            engine::ILogMessage* logMessage = GetInterface<engine::ILogMessage>();

            ImGuiListClipper clipper;
			clipper.Begin( logMessage->Num() );
            while ( clipper.Step() )
            {
                logMessage->ForLogEntry( clipper.DisplayStart, clipper.DisplayEnd,
                                        []( const engine::LogEntry& entry )
                                        {
                                            ImGui::Text( entry.m_message.c_str() );
                                        } );
            }

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();

        ImGui::PopStyleColor();
    }

    void ConsoleInputWidget::DrawAutoCompletion( float baseY )
    {
        if ( ( m_autoCompletionCursor == NoAutoCompleteSelection ) && m_autoCompletionList.empty() )
        {
            return;
        }

        size_t itemSize = GetAutoCompleteStringSize();
        if ( itemSize == 0 )
        {
            return;
        }

        float contentHeight = ImGui::GetTextLineHeightWithSpacing() * itemSize;
        const auto& imGuiStyle = ImGui::GetStyle();
        float paddingSize = imGuiStyle.ChildBorderSize + imGuiStyle.WindowPadding.y + imGuiStyle.FramePadding.y;
        float cursorPosY = baseY - contentHeight - paddingSize;

        ImGui::SetCursorPosY( cursorPosY );

        ImGuiChildFlags childFlags = ImGuiChildFlags_Borders
            | ImGuiChildFlags_AutoResizeX
            | ImGuiChildFlags_AutoResizeY
            | ImGuiChildFlags_AlwaysAutoResize;

        ImGui::PushStyleColor( ImGuiCol_ChildBg, ImGui::GetColorU32( ImGuiCol_WindowBg ) );

        ImGui::BeginChild( "AutoCompletionWindow", ImVec2(0, 0), childFlags, 0 );
        {
            for ( int32 i = 0; i < itemSize; ++i )
            {
                const char* name = m_autoCompletionList.empty() ? m_history.At( i ).c_str() : m_autoCompletionList[i].c_str();

                bool selected = m_autoCompletionCursor == i;
                if ( selected )
                {
                    ImVec2 min = ImGui::GetCursorScreenPos();
                    ImVec2 max = min + ImVec2( ImGui::CalcTextSize( name ).x, ImGui::GetTextLineHeight() );

                    min -= imGuiStyle.FramePadding;
                    max += imGuiStyle.FramePadding;

                    ImGui::GetWindowDrawList()->AddRectFilled( min, max, ImGui::GetColorU32( ImGuiCol_HeaderHovered ) );
                }
                ImGui::Text( name );
            }
        }
        ImGui::EndChild();

        ImGui::PopStyleColor();
    }

    void ConsoleInputWidget::DrawConsoleInput( float baseY )
    {
        ImGui::SetCursorPosY( baseY );

        ImVec2 min = ImGui::GetCursorScreenPos();
        ImVec2 max = min + ImVec2( ImGui::GetContentRegionAvail().x, 0.f );

        min.y -= 2.f;

        ImVec4 separatorColor = ImGui::GetStyleColorVec4( ImGuiCol_Separator );
        separatorColor.w = 1.f;
        ImGui::GetWindowDrawList()->AddRectFilled( min, max, ImGui::GetColorU32( separatorColor ) );

        ImVec2 ConsoleInputWindowSize( ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight() );
        ImGui::BeginChild( "ConsoleInputWindow", ConsoleInputWindowSize );
        {
            if ( m_needInputTextFocus )
            {
                ImGui::SetKeyboardFocusHere();
                m_needInputTextFocus = false;
            }

            ImGui::PushStyleColor( ImGuiCol_FrameBg, static_cast<ImVec4>(ImColor( 0.f, 0.f, 0.f, 0.5f ) ) );
            ImGui::PushItemWidth( ImGui::GetWindowSize().x );

            ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue
                | ImGuiInputTextFlags_CallbackCompletion
                | ImGuiInputTextFlags_CallbackHistory
                | ImGuiInputTextFlags_CallbackAlways
                | ImGuiInputTextFlags_CallbackCharFilter
                | ImGuiInputTextFlags_CallbackEdit;
            if ( ImGui::InputText( "##ConsoleInput", m_buffer, sizeof( m_buffer ), flags, &ConsoleInputTextCallback, this ) )
            {
                if ( ( GetAutoCompleteStringSize() < 0 ) || ( m_autoCompletionCursor < 0 ) )
                {
                    std::string consoleMessage( m_buffer + ConsolePromptLength );

                    GetInterface<engine::ILogMessage>()->Log( consoleMessage );
                    GetInterface<engine::IConsoleMessageExecutor>()->AppendCommand( std::move( consoleMessage ) );

                    if ( consoleMessage.empty() == false )
                    {
                        if ( std::ranges::find( m_history, consoleMessage ) == std::end( m_history ) )
                        {
                            m_history.EnqueueOverwrite( consoleMessage );
                        }
                    }

                    m_buffer[ConsolePromptLength] = '\0';
                    ClearAutoCompletion();

                    if ( m_displayMode == DisplayMode::Minimal )
                    {
                        SetDisplayMode( DisplayMode::Hidden );
                    }
                }
                else
                {
                    char* consoleMessegeBegin = m_buffer + ConsolePromptLength;
                    constexpr size_t actualBufferLen = sizeof( m_buffer ) - ConsolePromptLength;
                    strcpy_s( consoleMessegeBegin, actualBufferLen, GetAutoCompleteString().c_str() );

                    ClearAutoCompletion();
                }

                m_needClearInputTextSelection = true;
                m_needInputTextFocus = true;
            }

            ImGui::PopItemWidth();
            ImGui::PopStyleColor();
        }
        ImGui::EndChild();
    }

    void ConsoleInputWidget::UpdateAutoCompletion( const std::string_view& input )
    {
        m_autoCompletionList.clear();
        if ( input.empty() )
        {
            return;
        }

        for ( const auto& name : GetInterface<engine::IConsoleMessageExecutor>()->GetConsoleMessages() | std::views::keys )
        {
            if ( name.starts_with( input ) )
            {
                m_autoCompletionList.emplace_back( name );
            }
        }

        if ( m_autoCompletionList.empty() )
        {
            ResetAutoCompletionCursor();
        }
    }

    void ConsoleInputWidget::ResetAutoCompletionCursor()
    {
        m_autoCompletionCursor = NoAutoCompleteSelection;
    }

    void ConsoleInputWidget::ClearAutoCompletion()
    {
        ResetAutoCompletionCursor();
        m_autoCompletionList.clear();
    }

    const std::string& ConsoleInputWidget::GetAutoCompleteString() const
    {
        return m_autoCompletionList.empty() ? m_history.At( m_autoCompletionCursor ) : m_autoCompletionList[m_autoCompletionCursor];
    }

    int32 ConsoleInputWidget::GetAutoCompleteStringSize() const
    {
        return m_autoCompletionList.empty() ? m_history.Size() : static_cast<int32>( m_autoCompletionList.size() );
    }
}
