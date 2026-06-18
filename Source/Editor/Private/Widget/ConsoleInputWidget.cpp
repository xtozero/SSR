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
        DrawConsole();
        float consoleBaseY = ImGui::GetWindowContentRegionMin().y + ImGui::GetWindowContentRegionMax().y * ConsoleWindowSizeRatio;

        float autoCompletionBaseY = consoleBaseY - ImGui::GetTextLineHeight();
        DrawAutoCompletion( autoCompletionBaseY );

        float consoleInputBaseY = consoleBaseY - ImGui::GetFrameHeight();
        DrawConsoleInput( consoleInputBaseY );
    }

    void ConsoleInputWidget::DrawMinimal()
    {
        float autoCompletionBaseY = ImGui::GetWindowContentRegionMax().y - ImGui::GetTextLineHeight();
        DrawAutoCompletion( autoCompletionBaseY );

        float consoleInputBaseY = ImGui::GetWindowContentRegionMax().y - ImGui::GetFrameHeight();
        DrawConsoleInput( consoleInputBaseY );
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
        ImGui::SetNextWindowPos( ImGui::GetWindowContentRegionMin() + ImGui::GetWindowPos() );

        ImGui::PushStyleColor( ImGuiCol_ChildBg, static_cast<ImVec4>( ImColor( 0.f, 0.f, 0.f, 0.5f ) ) );

        ImVec2 ConsoleWindowSize( ImGui::GetContentRegionAvail().x, ImGui::GetWindowContentRegionMax().y * ConsoleWindowSizeRatio );
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
        float cursorPosY = baseY - contentHeight;
        ImGui::SetNextWindowPos( ImVec2( ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x, cursorPosY ) );

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoScrollWithMouse
            | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_AlwaysAutoResize
            | ImGuiWindowFlags_NoSavedSettings
            | ImGuiWindowFlags_NoFocusOnAppearing
            | ImGuiWindowFlags_NoNav;

        ImGui::Begin( "AutoCompletionWindow", nullptr, windowFlags );
        {
            for ( int32 i = 0; i < itemSize; ++i )
            {
                const char* name = m_autoCompletionList.empty() ? m_history.At( i ).c_str() : m_autoCompletionList[i].c_str();

                bool selected = m_autoCompletionCursor == i;
                if ( selected )
                {
                    ImVec2 min = ImGui::GetCursorScreenPos();
                    ImVec2 max = min + ImVec2( ImGui::CalcTextSize( name ).x, ImGui::GetTextLineHeight() );

                    min -= ImGui::GetStyle().FramePadding;
                    max += ImGui::GetStyle().FramePadding;

                    ImGui::GetWindowDrawList()->AddRectFilled( min, max, ImGui::GetColorU32(ImGuiCol_HeaderHovered ) );
                }
                ImGui::Text( name );
            }
        }
        ImGui::End();
    }

    void ConsoleInputWidget::DrawConsoleInput( float baseY )
    {
        ImGui::SetCursorPosY( baseY );

        ImVec2 min = ImGui::GetCursorScreenPos();
        ImVec2 max = min + ImVec2( ImGui::GetWindowContentRegionMax().x, 0.f );

        min.y -= 2.f;

        ImVec4 separatorColor = ImGui::GetStyleColorVec4( ImGuiCol_Separator );
        separatorColor.w = 1.f;
        ImGui::GetWindowDrawList()->AddRectFilled( min, max, ImGui::GetColorU32( separatorColor ) );

        ImVec2 ConsoleInputWindowSize( ImGui::GetWindowSize().x, ImGui::GetFrameHeight() );
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
