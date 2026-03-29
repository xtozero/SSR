#include "ConsoleInputWidget.h"

#include "ConsoleMessage/ConsoleMessageExecutor.h"
#include "Core/LogMessage.h"

#include <algorithm>
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>

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
            if ( thisWidget.m_autoCompletionList.empty() == false )
            {
                int32 actualBufferLen = std::max( 0, data->BufTextLen - static_cast<int32>( BufferStartOffset ) );
                data->DeleteChars( BufferStartOffset, actualBufferLen );

                int32 index = std::clamp<int32>( thisWidget.m_autoCompletionCursor, 0, thisWidget.m_autoCompletionList.size() );
                data->InsertChars( BufferStartOffset, thisWidget.m_autoCompletionList[index].c_str() );

                thisWidget.ClearAutoCompletion();
            }
            break;
        case ImGuiInputTextFlags_CallbackHistory:
            if ( data->EventKey == ImGuiKey_UpArrow )
            {
                --thisWidget.m_autoCompletionCursor;
                thisWidget.m_autoCompletionCursor = std::max( thisWidget.m_autoCompletionCursor, 0 );
            }
            else
            {
                ++thisWidget.m_autoCompletionCursor;
                auto maxIndex = static_cast<int32>( thisWidget.m_autoCompletionList.size() ) - 1;
                thisWidget.m_autoCompletionCursor = std::min( thisWidget.m_autoCompletionCursor, maxIndex );
            }
            break;
        case ImGuiInputTextFlags_CallbackAlways:
            if ( thisWidget.m_needClearInputTextSelection )
            {
                data->ClearSelection();
                thisWidget.m_needClearInputTextSelection = false;
            }

            if ( data->BufTextLen < BufferStartOffset )
            {
                data->DeleteChars( 0, data->BufTextLen );
                data->InsertChars( 0, ConsoleInputDefaultString.data() );
            }
            break;
        case ImGuiInputTextFlags_CallbackCharFilter:
            return ( data->EventChar == '`' ) ? 1 : 0;
        case ImGuiInputTextFlags_CallbackEdit:
            if ( data->BufTextLen > static_cast<int32>( BufferStartOffset ) )
            {
                thisWidget.UpdateAutoCompletion( std::string_view{ data->Buf + BufferStartOffset, static_cast<size_t>( data->BufTextLen - BufferStartOffset ) } );
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
        if ( m_autoCompletionList.empty() )
        {
            return;
        }

        float contentHeight = ImGui::GetTextLineHeightWithSpacing() * m_autoCompletionList.size();
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
            for ( size_t i = 0; i < m_autoCompletionList.size(); ++i )
            {
                const char* name = m_autoCompletionList[i].c_str();

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
                if ( m_autoCompletionList.empty() || ( m_autoCompletionCursor < 0 ) )
                {
                    std::string consoleMessage( m_buffer + BufferStartOffset );

                    GetInterface<engine::ILogMessage>()->Log( consoleMessage );
                    GetInterface<engine::IConsoleMessageExecutor>()->AppendCommand( std::move( consoleMessage ) );

                    m_buffer[BufferStartOffset] = '\0';
                    ClearAutoCompletion();
                }
                else
                {
                    char* consoleMessegeBegin = m_buffer + BufferStartOffset;
                    constexpr size_t actualBufferLen = sizeof( m_buffer ) - BufferStartOffset;
                    strcpy_s( consoleMessegeBegin, actualBufferLen, m_autoCompletionList[m_autoCompletionCursor].c_str() );

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

        for ( auto& [name, consoleMessage] : GetInterface<engine::IConsoleMessageExecutor>()->GetConsoleMessages() )
        {
            if ( name.starts_with( input ) )
            {
                m_autoCompletionList.emplace_back( name );
            }
        }
    }

    void ConsoleInputWidget::ClearAutoCompletion()
    {
        m_autoCompletionCursor = -1;
        m_autoCompletionList.clear();
    }
}
