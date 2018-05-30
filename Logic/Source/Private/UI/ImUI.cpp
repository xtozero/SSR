#include "stdafx.h"
#include "UI/ImUI.h"

#include "Core/Timer.h"
#include "Util.h"

#include <algorithm>
#include <fstream>

namespace
{
	bool Contain( const RECT& rect, const CXMFLOAT2& mousePos )
	{
		return rect.left <= mousePos.x && rect.right >= mousePos.x &&
			rect.top <= mousePos.y && rect.bottom >= mousePos.y;
	}

	// CRC 32 hash function
	ImGUID ImHash( const void* data, int size = 0, unsigned int seed = 0 )
	{
		static unsigned int crc32LUT[256] = { 0 };
		if ( crc32LUT[1] == 0 )
		{
			constexpr unsigned int polynomial = 0xEDB88320;
			for ( unsigned int i = 0; i < 256; ++i )
			{
				unsigned int crc = i;
				for ( unsigned int j = 0; j < 8; ++j )
				{
					crc = ( crc >> 1 ) ^ ( unsigned int( -int( crc & 1 ) ) & polynomial );
				}
				crc32LUT[i] = crc;
			}
		}

		seed = ~seed;
		unsigned int crc = seed;
		const unsigned char* current = (const unsigned char*)data;

		if ( size > 0 )
		{
			for ( int i = 0; i < size; ++i )
			{
				crc = ( crc >> 8 ) ^ crc32LUT[( crc & 0xFF ) ^ *current++];
			}
		}
		else
		{
			while ( unsigned char c = *current++ )
			{
				crc = ( crc >> 8 ) ^ crc32LUT[( crc & 0xFF ) ^ c];
			}
		}
		
		return ~crc;
	}

	float clamp( const float value, const float minValue, const float maxValue )
	{
		return max( min( value, maxValue ), minValue );
	}

	float lerp( float t, float lhs, float rhs )
	{
		t = clamp( t, 0.f, 1.0f );
		if ( lhs > rhs )
		{
			std::swap( lhs, rhs );
		}

		return lhs + ( rhs - lhs ) * t;
	}

	char* ImStrchr( char* str, const char* endStr, char chr )
	{
		while ( str != endStr )
		{
			if ( *str == chr )
			{
				return str;
			}
			++str;
		}

		return nullptr;
	}

	void* SettingHandlerWindow_ReadOpen( ImUI* ui, ImUiSettingHandler* handler, const char* name )
	{
		ImUiWindowSetting* settings = ui->FindWindowSettings( name );
		if ( settings == nullptr )
		{
			settings = ui->AddWindowSettings( name );
		}

		return static_cast<void*>( settings );
	}

	void SettingHandlerWindow_ReadLine( ImUI* ui, ImUiSettingHandler* handler, void* entry, const char* line )
	{
		ImUiWindowSetting* settings = static_cast<ImUiWindowSetting*>( entry );
		float x, y;
		int i;
		if ( sscanf_s( line, "Pos=%f,%f", &x, &y ) == 2 )         settings->m_pos = CXMFLOAT2( x, y );
		else if ( sscanf_s( line, "Size=%f,%f", &x, &y ) == 2 )   settings->m_size = CXMFLOAT2( x, y );
		else if ( sscanf_s( line, "Collapsed=%d", &i ) == 1 )     settings->m_collapsed = ( i != 0 );
	}
}

ImGUID ImUiWindow::GetID( const char* str )
{
	ImGUID seed = m_idStack.back( );
	ImGUID id = ImHash( str, 0, seed );
	return id;
}

float ImUiWindow::GetTitleBarHeight( ) const
{
	return m_imUi.GetCurStyle( ).m_framePadding.y * 2 + m_imUi.CalcTextSize( nullptr, 0 ).y;
}

CXMFLOAT2 CTextAtlas::CalcTextSize( const char* text, int count ) const
{
	CXMFLOAT2 size = { 0.f, m_fontHeight };

	for ( int i = 0; i < count; ++i )
	{
		auto found = m_fontInfo.find( text[i] );
		size.x += found->second.m_size + 1;
	}

	return size;
}

FontUV* CTextAtlas::FindGlyph( char ch )
{
	auto found = m_fontInfo.find( ch );

	return ( found != m_fontInfo.end() ) ? &found->second : nullptr;
}

bool ImUI::Initialize( )
{
	StyleColorDark( );
	
	ImUiSettingHandler windowCfgHandler;
	windowCfgHandler.ReadOpenFn = SettingHandlerWindow_ReadOpen;
	windowCfgHandler.ReadLineFn = SettingHandlerWindow_ReadLine;
	m_settingHandler.emplace( "Window", windowCfgHandler );

	LoadSettingFromDisk( "../cfg/imUI_setting.cfg" );

	for ( int i = 0, end = _countof( m_circleVertex ); i < end; ++i )
	{
		const float angle = ( static_cast<float>( i ) * DirectX::XM_2PI ) / end;
		m_circleVertex[i] = CXMFLOAT2( cosf( angle ), sinf( angle ) );
	}

	return true;
}

void ImUI::SetDefaultText( const std::string& fontName, const CTextAtlas& textAtlas )
{
	auto found = m_textAtlasLUT.find( fontName );
	if ( found != m_textAtlasLUT.end() )
	{
		m_curTextAltas = found->second;
		m_textAtlas[found->second] = textAtlas;
	}
	else
	{
		m_curTextAltas = static_cast<int>( m_textAtlas.size( ) );
		m_textAtlasLUT.emplace( fontName, m_curTextAltas );
		m_textAtlas.emplace_back( textAtlas );
	}
}

void ImUI::StyleColorDark( )
{
	CXMFLOAT4* colors = m_curStyle.m_colors;

	colors[ImUiColor::Text] = CXMFLOAT4( 1.f, 1.f, 1.f, 1.f );
	colors[ImUiColor::WindowBg] = CXMFLOAT4( 0.06f, 0.06f, 0.06f, 0.94f );
	colors[ImUiColor::FrameBg] = CXMFLOAT4( 0.16f, 0.29f, 0.48f, 0.54f );
	colors[ImUiColor::FrameBgMouseOver] = CXMFLOAT4( 0.26f, 0.59f, 0.98f, 0.4f );
	colors[ImUiColor::FrameBgActive] = CXMFLOAT4( 0.26f, 0.59f, 0.98f, 0.67f );
	colors[ImUiColor::TitleBg] = CXMFLOAT4( 0.04f, 0.04f, 0.04f, 1.f );
	colors[ImUiColor::TitleBgActive] = CXMFLOAT4( 0.16f, 0.29f, 0.48f, 1.f );
	colors[ImUiColor::TitleBgCollapsed] = CXMFLOAT4( 0.00f, 0.00f, 0.00f, 0.51f );
	colors[ImUiColor::SliderGrab] = CXMFLOAT4( 0.24f, 0.52f, 0.88f, 1.f );
	colors[ImUiColor::SliderGrabActive] = CXMFLOAT4( 0.26f, 0.59f, 0.98f, 1.f );
	colors[ImUiColor::Button] = CXMFLOAT4( 0.26f, 0.59f, 0.98f, 0.4f );
	colors[ImUiColor::ButtonMouseOver] = CXMFLOAT4( 0.26f, 0.59f, 0.98f, 1.f );
	colors[ImUiColor::ButtonPress] = CXMFLOAT4( 0.06f, 0.53f, 0.98f, 1.f );
}

void ImUI::BeginFrame( RECT clientRect )
{
	m_clientRect = clientRect;

	for ( int i = 0; i < 5; ++i )
	{
		m_io.m_mouseDownDurationPrev[i] = m_io.m_mouseDownDuration[i];
		m_io.m_mouseDownDuration[i] = m_io.m_mouseDown[i] ? ( m_io.m_mouseDownDuration[i] < 0.f ? 0.f : m_io.m_mouseDownDuration[i] + CTimer::GetInstance( ).GetElapsedTIme( ) ) : -1.f;
	}

	SetMouseOveredID( 0 );

	UpdateMovingWindow( );

	m_mouseOveredWindow = FindMouseOverWindow( );
}

void ImUI::EndFrame( )
{
	if ( m_activeItemID == 0 && m_mouseOveredID == 0 )
	{
		if ( m_io.m_mouseDown[0] )
		{
			if ( m_mouseOveredWindow )
			{
				FocusWindow( m_mouseOveredWindow );
				SetActiveItemID( m_mouseOveredWindow->m_moveID );
				m_activeIDClickOffset = m_io.m_mousePos - m_mouseOveredWindow->m_pos;
				m_moveingWindow = m_mouseOveredWindow;
			}
			else
			{
				FocusWindow( nullptr );
			}
		}
	}
}

bool ImUI::Window( const char* name )
{
	ImUiWindow* window = FindWindow( name );
	if ( window == nullptr )
	{
		CXMFLOAT2 defaultSize( 400, 400 );
		window = CreateImUiWindow( name, defaultSize );
	}

	m_curWindow = window;

	// 임시
	window->m_visible = true;
	window->m_windowPadding = m_curStyle.m_windowPadding;
	window->m_itemWidthDefault = window->m_size.x * 0.65f;

	// DrawContext 초기화
	ImUiDrawContext& dc = window->m_dc;

	dc.m_indentX = window->m_windowPadding.x;
	float titleBarHeight = window->GetTitleBarHeight( );
	dc.m_cursorStartPos = window->m_pos + CXMFLOAT2( dc.m_indentX, window->m_windowPadding.y + titleBarHeight );
	dc.m_cursorPos = dc.m_cursorStartPos;
	dc.m_prevCursorPos = dc.m_cursorStartPos;
	dc.m_currentLineHeight = 0.f;
	dc.m_prevLineHeight = 0.f;
	dc.m_currentLineTextBaseOffset = 0.f;
	dc.m_prevLineTextBaseOffset = 0.f;

	if ( window->m_collapseToggleReserved )
	{
		window->m_collapsed = !window->m_collapsed;
		FocusWindow( window );
	}

	window->m_collapseToggleReserved = false;

	window->m_size = window->m_collapsed ? CXMFLOAT2( window->m_sizeNonCollapsed.x, titleBarHeight ) : window->m_sizeNonCollapsed;

	// 그리기
	window->m_drawList.Clear( );
	window->m_drawList.PushTextAtlas( m_textAtlas[m_curTextAltas].m_texture );
	PushClipRect( m_clientRect );
	
	if ( window->m_collapsed )
	{
		CXMFLOAT4 titleColor = GetItemColor( ImUiColor::TitleBgCollapsed );
		RenderFrame( window->m_pos, CXMFLOAT2( window->m_size.x, titleBarHeight ), titleColor, m_curStyle.m_windowRounding );
	}
	else
	{
		CXMFLOAT4 bgColor = GetItemColor( ImUiColor::WindowBg );
		// background
		window->m_drawList.AddFilledRect( window->m_pos + CXMFLOAT2( 0, titleBarHeight ), window->m_size - CXMFLOAT2( 0, titleBarHeight ), bgColor, m_curStyle.m_windowRounding, ImDrawCorner::Bottom );

		// Title bar
		bool isTitleBarHighlight = ( m_navWindow && m_navWindow == window );
		CXMFLOAT4 titleColor = GetItemColor( isTitleBarHighlight ? ImUiColor::TitleBgActive : ImUiColor::TitleBg );
		window->m_drawList.AddFilledRect( window->m_pos, CXMFLOAT2( window->m_size.x, titleBarHeight ), titleColor, m_curStyle.m_windowRounding, ImDrawCorner::Top );
	}

	// Collapse botton
	{
		CXMFLOAT2 arrowMin = m_curWindow->m_pos + m_curStyle.m_framePadding;
		float textSize = CalcTextSize( nullptr, 0 ).y;
		RECT arrowBoundingBox = {
			static_cast<long>( arrowMin.x + 1.f ),
			static_cast<long>( arrowMin.y + 1.f ),
			static_cast<long>( arrowMin.x + textSize - 1.f ),
			static_cast<long>( arrowMin.y + textSize - 1.f )
		};

		ImGUID collapseId = m_curWindow->GetID( "COLLAPSE" );
		bool mouseOvered = false;
		bool mouseHeld = false;
		
		if ( ButtonBehavior( arrowBoundingBox, collapseId, mouseOvered, mouseHeld ) )
		{
			m_curWindow->m_collapseToggleReserved = true;
		}

		RenderArrow( m_curWindow->m_pos + m_curStyle.m_framePadding, m_curWindow->m_collapsed ? ImDir::Right : ImDir::Down, 1.0f );
	}

	// Title text
	{
		int textCount = std::strlen( name );
		CXMFLOAT2 textSize = CalcTextSize( name, textCount );

		float padLeft = m_curStyle.m_framePadding.x + m_curStyle.m_itemInnerSpacing.x + textSize.y;
		float padRight = m_curStyle.m_framePadding.x;

		CXMFLOAT2 textPosMin = window->m_pos;
		textPosMin.x += padLeft;
		CXMFLOAT2 textPosMax = window->m_pos + CXMFLOAT2( window->m_size.x, titleBarHeight );
		textPosMax.x -= padRight;

		RenderClippedText( textPosMin, textPosMax, name, textCount, CXMFLOAT2( 0.f, 0.5f ) );
	}

	m_curWindow->m_skipItem = m_curWindow->m_collapsed || !m_curWindow->m_visible;
	return m_curWindow->m_skipItem;
}

bool ImUI::Button( const char* label, const CXMFLOAT2& size )
{
	return ButtonEX( label, size );
}

bool ImUI::SliderFloat( const char* label, float* v, float min, float max, const char* displayFormat )
{
	assert( m_curWindow != nullptr );
	if ( m_curWindow->m_skipItem )
	{
		return false;
	}

	ImGUID id = m_curWindow->GetID( label );

	const float w = m_curWindow->m_itemWidthDefault;

	ImUiDrawContext& dc = m_curWindow->m_dc;

	const CXMFLOAT2 pos = dc.m_cursorPos;
	const CXMFLOAT2 labelSize = CalcTextSize( label, strlen( label ) );

	const RECT boundingBox = {
		static_cast<long>( pos.x ),
		static_cast<long>( pos.y ),
		static_cast<long>( pos.x + w ),
		static_cast<long>( pos.y + labelSize.y + m_curStyle.m_framePadding.y * 2.f ),
	};

	ItemSize( CXMFLOAT2( w, labelSize.y + m_curStyle.m_framePadding.y * 2.f ) );
	bool mouseOvered = MouseOveredItem( boundingBox, id );

	if ( mouseOvered && m_io.m_mouseDown[0] )
	{
		SetActiveItemID( id );
		FocusWindow( m_curWindow );
	}

	bool valueChanged = SliderBehavior( boundingBox, id, v, min, max );
	
	if ( displayFormat == nullptr )
	{
		displayFormat = "%.3f";
	}

	char valueBuf[64] = {};
	sprintf_s( valueBuf, displayFormat, *v );
	int count = std::strlen( valueBuf );

	CXMFLOAT2 posMin( static_cast<float>( boundingBox.left ), static_cast<float>( boundingBox.top ) );
	CXMFLOAT2 posMax( static_cast<float>( boundingBox.right ), static_cast<float>( boundingBox.bottom ) );

	RenderClippedText( posMin, posMax, valueBuf, count, CXMFLOAT2( 0.5f, 0.5f ) );

	return valueChanged;
}

bool ImUI::SliderInt( const char* label, int* v, int min, int max, const char* displayFormat )
{
	if ( displayFormat == nullptr )
	{
		displayFormat = "%.0f";
	}

	float floatValue = static_cast<float>( *v );
	bool valueChanged = SliderFloat( label, &floatValue, static_cast<float>( min ), static_cast<float>( max ), displayFormat );
	*v = static_cast<int>( floatValue );

	return valueChanged;
}

void ImUI::TextUnformatted( const char* text, int count )
{
	assert( m_curWindow != nullptr );
	if ( m_curWindow->m_skipItem )
	{
		return;
	}

	ImUiDrawContext& dc = m_curWindow->m_dc;

	const CXMFLOAT2 textPos( dc.m_cursorPos.x, dc.m_cursorPos.y + dc.m_currentLineTextBaseOffset );

	const CXMFLOAT2 textSize = CalcTextSize( text, count );
	ItemSize( textSize );

	RenderClippedText( textPos, textPos + textSize, text, count, CXMFLOAT2( 0.f, 0.f ) );
}

void ImUI::SameLine( float xPos, float wSpacing )
{
	assert( m_curWindow != nullptr );

	ImUiDrawContext& dc = m_curWindow->m_dc;

	if ( xPos != 0.f )
	{
		if ( wSpacing < 0.f )
		{
			wSpacing = 0.f;
		}

		dc.m_cursorPos.x = m_curWindow->m_pos.x + xPos + wSpacing;
		dc.m_cursorPos.y = dc.m_prevCursorPos.y;
	}
	else
	{
		if ( wSpacing < 0.f )
		{
			wSpacing = m_curStyle.m_itemSpacing.x;
		}

		dc.m_cursorPos.x = dc.m_prevCursorPos.x + wSpacing;
		dc.m_cursorPos.y = dc.m_prevCursorPos.y;
	}

	dc.m_currentLineHeight = dc.m_prevLineHeight;
	dc.m_currentLineTextBaseOffset = dc.m_prevLineTextBaseOffset;
}

bool ImUI::HandleUserInput( const UserInput& input )
{
	m_io.m_prevMousePos = m_io.m_mousePos;

	switch ( input.m_code )
	{
	case USER_INPUT_CODE::UIC_MOUSE_MOVE:
		m_io.m_mousePos.x += input.m_axis[UserInput::X_AXIS];
		m_io.m_mousePos.y += input.m_axis[UserInput::Y_AXIS];
		break;
	case USER_INPUT_CODE::UIC_MOUSE_LEFT:
	case USER_INPUT_CODE::UIC_MOUSE_RIGHT:
	case USER_INPUT_CODE::UIC_MOUSE_MIDDLE:
		m_io.m_mouseDown[input.m_code - USER_INPUT_CODE::UIC_MOUSE_LEFT] = input.m_axis[UserInput::Z_AXIS] < 0;
	}

	return FindMouseOverWindow( ) != nullptr;
	return true;
}

ImDrawData ImUI::Render( )
{
	ImDrawData drawData;
	drawData.m_totalIndexCount = 0;
	drawData.m_totalVertexCount = 0;

	for ( auto& window : m_windows )
	{
		if ( window.m_visible )
		{
			if ( window.m_drawList.m_cmdBuffer.empty( ) )
			{
				continue;
			}

			drawData.m_drawLists.push_back( &window.m_drawList );
			drawData.m_totalVertexCount += window.m_drawList.m_vertices.size( );
			drawData.m_totalIndexCount += window.m_drawList.m_indices.size( );
		}
	}

	return drawData;
}

CXMFLOAT2 ImUI::CalcTextSize( const char* text, int count ) const
{
	assert( m_curTextAltas != -1 );

	const CTextAtlas& font = m_textAtlas[m_curTextAltas];

	if ( count == 0 )
	{
		return CXMFLOAT2( 0, font.m_fontHeight );
	}

	return font.CalcTextSize( text, count );
}

ImUiWindowSetting* ImUI::FindWindowSettings( const char* name )
{
	auto found = m_settingWindows.find( name );
	
	if ( found != m_settingWindows.end( ) )
	{
		return &found->second;
	}

	return nullptr;
}

ImUiWindowSetting* ImUI::AddWindowSettings( const char* name )
{
	auto setting = m_settingWindows.emplace( name, ImUiWindowSetting( ) );

	return &setting.first->second;
}

ImUiWindow* ImUI::FindWindow( const char* name )
{
	auto found = m_windowLUT.find( name );
	if ( found != m_windowLUT.end( ) )
	{
		return &m_windows[found->second];
	}

	return nullptr;
}

ImUiWindow* ImUI::FindMouseOverWindow( )
{
	for ( ImUiWindow& window : m_windows )
	{
		if ( window.m_visible == false )
		{
			continue;
		}

		RECT boundingBox = { 
			static_cast<long>( window.m_pos.x ),
			static_cast<long>( window.m_pos.y ), 
			static_cast<long>( window.m_pos.x + window.m_size.x ),
			static_cast<long>( window.m_pos.y + window.m_size.y ) };

		if ( Contain( boundingBox, m_io.m_mousePos ) )
		{
			return (&window);
		}
	}

	return nullptr;
}

ImUiWindow* ImUI::CreateImUiWindow( const char* name, const CXMFLOAT2& size )
{
	m_windowLUT.emplace( name, m_windows.size( ) );
	m_windows.emplace_back( *this );
	
	ImUiWindow& window = m_windows.back( );

	if ( ImUiWindowSetting* settings = FindWindowSettings( name ) )
	{
		window.m_pos = settings->m_pos;
		window.m_size = settings->m_size;
		window.m_collapsed = settings->m_collapsed;
	}
	else
	{
		window.m_pos = CXMFLOAT2( 60, 60 );
		window.m_size = size;
	}
	
	window.m_sizeNonCollapsed = window.m_size;
	window.m_idStack.push_back( ImHash( name ) );

	window.m_moveID = window.GetID( "MOVE" );

	return &window;
}

CXMFLOAT2 ImUI::CalcItemSize( const CXMFLOAT2& size )
{
	return size;
}

bool ImUI::ButtonEX( const char* label, const CXMFLOAT2& size )
{
	assert( m_curWindow != nullptr );
	if ( m_curWindow->m_skipItem )
	{
		return false;
	}

	ImUiDrawContext& dc = m_curWindow->m_dc;

	const CXMFLOAT2 pos = dc.m_cursorPos;
	const int strCount = strlen( label );
	const CXMFLOAT2 labelSize = CalcTextSize( label, strCount );

	CXMFLOAT2& buttonSize = CalcItemSize( CXMFLOAT2( labelSize.x + m_curStyle.m_framePadding.x * 2.f, labelSize.y + m_curStyle.m_framePadding.y * 2.f ) );

	const RECT boundingBox = {
		static_cast<long>( pos.x ),
		static_cast<long>( pos.y ),
		static_cast<long>( pos.x + buttonSize.x ),
		static_cast<long>( pos.y + buttonSize.y ),
	};

	ImGUID id = m_curWindow->GetID( label );

	bool mouseOvered = false;
	bool mouseHeld = false;
	bool mousePreesed = ButtonBehavior( boundingBox, id, mouseOvered, mouseHeld );
	ItemSize( buttonSize );

	// 그리기

	// 임시
	const CXMFLOAT4& color = GetItemColor( ( mouseOvered && mouseHeld ) ? ImUiColor::ButtonPress : ( mouseOvered ? ImUiColor::ButtonMouseOver : ImUiColor::Button ) );
	m_curWindow->m_drawList.AddFilledRect( pos, buttonSize, color );
	RenderClippedText( pos + m_curStyle.m_framePadding, CXMFLOAT2( boundingBox.right - m_curStyle.m_framePadding.x, boundingBox.bottom - m_curStyle.m_framePadding.y ), label, strCount, CXMFLOAT2( 0.f, 0.f ) );

	return mousePreesed;
}

bool ImUI::ButtonBehavior( const RECT& boundingBox, ImGUID id, bool& mouseOvered, bool& mouseHeld )
{
	assert( m_curWindow != nullptr );

	mouseOvered = MouseOveredItem( boundingBox, id );

	bool pressed = false;

	if ( mouseOvered )
	{
		if ( m_io.m_mouseDown[0] )
		{
			SetActiveItemID( id );
			FocusWindow( m_curWindow );
		}
	}

	if ( m_activeItemID == id )
	{
		if ( m_io.m_mouseDown[0] )
		{
			mouseHeld = true;
		}
		else
		{
			if ( mouseOvered )
			{
				pressed = true;
			}
			SetActiveItemID( 0 );
		}
	}

	return pressed;
}

bool ImUI::SliderBehavior( const RECT& boundingbox, ImGUID id, float* v, float min, float max )
{
	assert( m_curWindow != nullptr );

	const CXMFLOAT4 frameColor = GetItemColor( m_activeItemID == id ? ImUiColor::FrameBgActive : m_mouseOveredID == id ? ImUiColor::FrameBgMouseOver : ImUiColor::FrameBg );

	// 그리기

	// 임시
	CXMFLOAT2 framePos = CXMFLOAT2( static_cast<float>( boundingbox.left ), static_cast<float>( boundingbox.top ) );
	CXMFLOAT2 frameSize = CXMFLOAT2( static_cast<float>( boundingbox.right - boundingbox.left ), static_cast<float>( boundingbox.bottom - boundingbox.top ) );

	m_curWindow->m_drawList.AddFilledRect( framePos, frameSize, frameColor );

	constexpr float grabPadding = 2.0f;
	const float sliderSize = frameSize.x - grabPadding * 2.0f;
	float grabSize = min( m_curStyle.m_grabMinSize, sliderSize );

	const float sliderUsablePosMin = boundingbox.left + grabPadding + grabSize * 0.5f;
	const float sliderUsablePosMax = boundingbox.right - grabPadding - grabSize * 0.5f;

	float linearZeroPos = 0.f;
	if ( min * max < 0.f )
	{
		const float distMinToZero = std::fabsf( 0.f - min );
		const float distMaxToZero = std::fabsf( max - 0.f );
		linearZeroPos = distMinToZero / ( distMinToZero + distMaxToZero );
	}
	else
	{
		linearZeroPos = min < 0.f ? 1.f : 0.f;
	}

	bool valueChanged = false;
	if ( m_activeItemID == id )
	{
		bool setNewValue = false;
		float clicked = 0.f;
		if ( m_io.m_mouseDown[0] == false )
		{
			SetActiveItemID( 0 );
		}
		else
		{
			const float mousePos = m_io.m_mousePos.x;
			clicked = ( sliderSize > 0.f ) ? clamp( ( mousePos - boundingbox.left ) / sliderSize, 0.f, 1.0f ) : 0.f;
			setNewValue = true;
		}

		if ( setNewValue )
		{
			float newValue = lerp( clicked, min, max );

			if ( *v != newValue )
			{
				*v = newValue;
				valueChanged = true;
			}
		}
	}

	// 그리기
	float grabRatio = SliderBehaviorCalcRatioFromValue( *v, min, max, linearZeroPos );
	const float grabPos = lerp( grabRatio, sliderUsablePosMin, sliderUsablePosMax );

	CXMFLOAT2 pos( grabPos - grabSize * 0.5f, framePos.y + grabPadding );
	CXMFLOAT2 size( grabSize, frameSize.y - grabPadding * 2.f );

	m_curWindow->m_drawList.AddFilledRect( pos, size, GetItemColor( m_activeItemID == id ? ImUiColor::SliderGrab : ImUiColor::SliderGrabActive ) );

	return valueChanged;
}

float ImUI::SliderBehaviorCalcRatioFromValue( float v, float min, float max, float linearZeroPos )
{
	if ( min == max )
	{
		return 0.f;
	}

	float clamped = clamp( v, min, max );

	return ( clamped - min ) / ( max - min );
}

void ImUI::RenderFrame( const CXMFLOAT2& pos, const CXMFLOAT2& size, const CXMFLOAT4& color, float rounding )
{
	assert( m_curWindow != nullptr );
	m_curWindow->m_drawList.AddFilledRect( pos, size, color, rounding, ImDrawCorner::All );
}

void ImUI::RenderClippedText( const CXMFLOAT2& posMin, const CXMFLOAT2& posMax, const char* text, int count, const CXMFLOAT2& align )
{
	if ( count == 0 )
	{
		return;
	}

	assert( m_curWindow != nullptr );
	assert( m_curTextAltas != -1 );
	
	const CXMFLOAT2 textSize = CalcTextSize( text, count );

	CXMFLOAT2 pos = posMin;
	if ( align.x > 0.f )
	{
		pos.x = floor( max( pos.x, pos.x + ( posMax.x - posMin.x - textSize.x ) * align.x ) );
	}
	if ( align.y > 0.f )
	{
		pos.y = floor( max( pos.y, pos.y + ( posMax.y - posMin.y - textSize.y ) * align.y ) );
	}

	m_curWindow->m_drawList.AddText( m_textAtlas[m_curTextAltas], pos, GetItemColor( ImUiColor::Text ), text, count );
}

void ImUI::RenderArrow( const CXMFLOAT2& pos, ImDir::Type dir, float scale )
{
	const float h = CalcTextSize( nullptr, 0 ).y;
	float r = h * 0.4f * scale;
	CXMFLOAT2 center = pos + CXMFLOAT2( h * 0.5f, h * 0.5f * scale );

	CXMFLOAT2 v[3];

	switch ( dir )
	{
	case ImDir::Up:
	case ImDir::Down:
		if ( dir == ImDir::Up )
		{
			r = -r;
		}

		center.y -= r * 0.25f;
		v[0] = CXMFLOAT2( 0 , 1 ) * r;
		v[1] = CXMFLOAT2( 0.866f, -0.5f ) * r;
		v[2] = CXMFLOAT2( -0.866f, -0.5f ) * r;
		break;
	case ImDir::Left:
	case ImDir::Right:
		if ( dir == ImDir::Left )
		{
			r = -r;
		}

		center.x -= r * 0.25f;
		v[0] = CXMFLOAT2( 1, 0 ) * r;
		v[1] = CXMFLOAT2( -0.5f, -0.866f ) * r;
		v[2] = CXMFLOAT2( -0.5f, 0.866f ) * r;
		break;
	default:
		assert( 0 );
		break;
	}

	m_curWindow->m_drawList.AddTriangleFilled( center + v[0], center + v[1], center + v[2], GetItemColor( ImUiColor::Text ) );
}

void ImUI::PushClipRect( const RECT& clipRect )
{
	assert( m_curWindow != nullptr );
	m_curWindow->m_drawList.PushClipRect( clipRect );
}

void ImUI::ItemSize( const CXMFLOAT2& size, float textOffsetY )
{
	assert( m_curWindow != nullptr );

	ImUiDrawContext& dc = m_curWindow->m_dc;
	const float lineHeight = max( dc.m_currentLineHeight, size.y );
	const float lineTextBaseOffset = max( dc.m_currentLineTextBaseOffset, textOffsetY );

	dc.m_prevCursorPos = CXMFLOAT2( dc.m_cursorPos.x + size.x, dc.m_cursorPos.y );
	dc.m_cursorPos = CXMFLOAT2( m_curWindow->m_pos.x + dc.m_indentX, dc.m_cursorPos.y + lineHeight + m_curStyle.m_itemSpacing.y );

	dc.m_prevLineHeight = lineHeight;
	dc.m_prevLineTextBaseOffset = lineTextBaseOffset;
	dc.m_currentLineHeight = 0.f;
	dc.m_currentLineTextBaseOffset = 0.f;
}

bool ImUI::MouseOveredItem( const RECT& boundingbox, ImGUID id )
{
	assert( m_curWindow != nullptr );

	if ( Contain( boundingbox , m_io.m_mousePos ) == false )
	{
		return false;
	}

	SetMouseOveredID( id );

	return true;
}

CXMFLOAT4 ImUI::GetItemColor( ImUiColor::Type colorId, float alphaMul )
{
	CXMFLOAT4 color = m_curStyle.m_colors[colorId];
	color.w *= m_curStyle.m_alpha * alphaMul;
	return color;
}

void ImUI::SetActiveItemID( ImGUID id )
{
	m_activeItemID = id;
}

void ImUI::SetMouseOveredID( ImGUID id )
{
	m_mouseOveredID = id;
}

void ImUI::FocusWindow( ImUiWindow* window )
{
	if ( m_navWindow != window )
	{
		m_navWindow = window;
	}
}

void ImUI::UpdateMovingWindow( )
{
	if ( m_moveingWindow && m_moveingWindow->m_moveID == m_activeItemID )
	{
		CXMFLOAT2 pos = m_io.m_mousePos - m_activeIDClickOffset;
		if ( m_io.m_mouseDown[0] )
		{
			m_moveingWindow->m_pos = pos;
			FocusWindow( m_moveingWindow );
		}
		else
		{
			SetActiveItemID( 0 );
			m_moveingWindow = nullptr;
		}
	}
}

void ImUI::LoadSettingFromDisk( const char* fileName )
{
	if ( fileName == nullptr )
	{
		return;
	}

	std::ifstream cfgfile( fileName, std::ios::binary | std::ios::ate );
 	if ( cfgfile )
	{
		std::streamoff fileSize = cfgfile.tellg();
		cfgfile.seekg( 0, std::ios::beg );

		assert( INT_MAX >= fileSize );
		int iFileSize = static_cast<int>( fileSize );

		char* buf = new char[iFileSize];

		cfgfile.read( buf, iFileSize );

		LoadSettingFromMemory( buf, iFileSize );

		delete[] buf;
	}
}

void ImUI::LoadSettingFromMemory( const char* buf, int count )
{
	char* duplicatedBuf = new char[count + 1];
	memcpy_s( duplicatedBuf, count, buf, count );

	char* endBuf = duplicatedBuf + count;
	endBuf[0] = '\0';

	char* endLine = nullptr;

	void* entryData = nullptr;
	ImUiSettingHandler* entryHandler = nullptr;

	for ( char* line = duplicatedBuf; line < endBuf; line = endLine + 1 )
	{
		while ( *line == '\n' || *line == '\r' )
		{
			line++;
		}
		endLine = line;
		while ( endLine < endBuf && *endLine != '\n' && *endLine != '\r' )
		{
			endLine++;
		}
		endLine[0] = '\0';

		if ( line[0] == '[' && endLine > line && endLine[-1] == ']' )
		{
			endLine[-1] = '\0';
			const char* nameEnd = endLine - 1;
			const char* typeBegin = line + 1;
			char* typeEnd = ImStrchr( line + 1, nameEnd, ']' );
			const char* nameBegin = typeEnd ? ImStrchr( typeEnd + 1, nameEnd, '[' ) : nullptr;
			if ( typeEnd == nullptr || nameBegin == nullptr )
			{
				assert( false );
			}
			else
			{
				*typeEnd = '\0';
				++nameBegin;
			}
			entryHandler = FindSettingHandler( typeBegin );
			entryData = entryHandler ? entryHandler->ReadOpenFn( this, entryHandler, nameBegin ) : nullptr;
		}
		else if ( entryHandler != nullptr && entryData != nullptr )
		{
			entryHandler->ReadLineFn( this, entryHandler, entryData, line );
		}
	}

	delete[] duplicatedBuf;
}

ImUiSettingHandler * ImUI::FindSettingHandler( const char* typeName )
{
	auto found = m_settingHandler.find( typeName );
	if ( found != m_settingHandler.end( ) )
	{
		return &found->second;
	}

	return nullptr;
}
