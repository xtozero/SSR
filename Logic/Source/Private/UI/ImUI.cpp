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

	bool Contain( const RECT& lhs, const RECT& rhs )
	{
		return lhs.left <= rhs.left && lhs.right >= rhs.right &&
			lhs.top <= rhs.top && lhs.bottom >= rhs.bottom;
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

	CXMFLOAT2 clamp( const CXMFLOAT2& value, const CXMFLOAT2& minVlaue, const CXMFLOAT2& maxValue )
	{
		return CXMFLOAT2( clamp( value.x, minVlaue.x, maxValue.x ), clamp( value.y, minVlaue.y, maxValue.y ) );
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

	static bool ItemsArrayGetter( void* data, int idx, const char** out_text )
	{
		const char* const* items = (const char* const*)data;
		if ( out_text )
			*out_text = items[idx];
		return true;
	}

	CXMFLOAT2 CalcContentsSize( ImUiWindow* window )
	{
		CXMFLOAT2 sz(
			( window->m_explicitContentsSize.x != 0.f ) ? window->m_explicitContentsSize.x : ( window->m_dc.m_cursorMaxPos.x - window->m_pos.x ),
			( window->m_explicitContentsSize.y != 0.f ) ? window->m_explicitContentsSize.y : ( window->m_dc.m_cursorMaxPos.y - window->m_pos.y )
		);

		return sz + window->m_windowPadding;
	}

	CXMFLOAT2 Max( const CXMFLOAT2& lhs, const CXMFLOAT2& rhs )
	{
		return CXMFLOAT2( max( lhs.x, rhs.x ), max( lhs.x, rhs.x ) );
	}

	void Expand( RECT& out, const float amount )
	{
		out.left -= amount;
		out.top -= amount;
		out.right += amount;
		out.bottom += amount;
	}

	void Expand( RECT& out, const CXMFLOAT2& amount )
	{
		out.left -= amount.x;
		out.top -= amount.y;
		out.right += amount.x;
		out.bottom += amount.y;
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
	return ( m_flag & ImUiWindowFlags::NoTitleBar ) ? 0.f : m_imUi.GetCurStyle( ).m_framePadding.y * 2 + m_imUi.CalcTextSize( nullptr, 0 ).y;
}

RECT ImUiWindow::GetTitleBarRect( ) const
{
	return { static_cast<LONG>( m_pos.x ), 
			static_cast<LONG>( m_pos.y ), 
			static_cast<LONG>( m_pos.x + m_sizeNonCollapsed.x ), 
			static_cast<LONG>( m_pos.y + GetTitleBarHeight( ) ) };
}

void ImUiWindow::SetPos( const CXMFLOAT2& pos )
{
	CXMFLOAT2 oldPos = m_pos;

	m_pos = pos;
	m_dc.m_cursorPos += ( m_pos - oldPos );
	m_dc.m_cursorMaxPos += ( m_pos - oldPos );
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
	colors[ImUiColor::Header] = CXMFLOAT4( 0.26f, 0.59f, 0.98f, 0.31f );
	colors[ImUiColor::HeaderMouseOver] = CXMFLOAT4( 0.26f, 0.59f, 0.98f, 0.80f );
	colors[ImUiColor::HeaderActive] = CXMFLOAT4( 0.26f, 0.59f, 0.98f, 1.00f );
}

void ImUI::BeginFrame( RECT clientRect )
{
	using namespace DirectX;

	++m_frameCount;

	m_clientRect = clientRect;
	m_io.m_displaySize = CXMFLOAT2( clientRect.right - clientRect.left, clientRect.bottom - clientRect.top );

	m_mouseOveredID = 0;

	for ( int i = 0; i < 5; ++i )
	{
		m_io.m_mouseClicked[i] = m_io.m_mouseDown[i] && m_io.m_mouseDownDuration[i] < 0.f;
		m_io.m_mouseDownDurationPrev[i] = m_io.m_mouseDownDuration[i];
		m_io.m_mouseDownDuration[i] = m_io.m_mouseDown[i] ? ( m_io.m_mouseDownDuration[i] < 0.f ? 0.f : m_io.m_mouseDownDuration[i] + CTimer::GetInstance( ).GetElapsedTIme( ) ) : -1.f;
		m_io.m_mouseDoubleClick[i] = false;
		if ( m_io.m_mouseClicked[i] )
		{
			if ( CTimer::GetInstance().GetTotalTime() - m_io.m_mouseClickedTime[i] < m_io.m_mouseDoubleDownTime )
			{
				if ( XMVectorGetX( XMVector2LengthSq( m_io.m_mousePos - m_io.m_mouseClickedPos[i] ) ) < m_io.m_mouseDoubleDownMaxDist * m_io.m_mouseDoubleDownMaxDist )
				{
					m_io.m_mouseDoubleClick[i] = true;
				}
				m_io.m_mouseClickedTime[i] = -FLT_MAX;
			}
			else
			{
				m_io.m_mouseClickedTime[i] = CTimer::GetInstance( ).GetTotalTime( );
			}
			m_io.m_mouseClickedPos[i] = m_io.m_mousePos;
		}
	}

	SetMouseOveredID( 0 );

	UpdateMovingWindow( );

	m_mouseOveredWindow = FindMouseOverWindow( );

	for ( auto& window : m_windows )
	{
		window.m_wasVisible = window.m_visible;
		window.m_visible = false;
	}

	m_currentWindowStack.resize( 0 );
	m_currentPopupStack.resize( 0 );
	ClosePopupsOverWindow( m_navWindow );
}

void ImUI::EndFrame( )
{
	if ( m_activeItemID == 0 && m_mouseOveredID == 0 )
	{
		if ( m_io.m_mouseClicked[0] )
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

bool ImUI::Window( const char* name, ImUiWindowFlags::Type flags )
{
	ImUiWindow* window = FindWindow( name );
	if ( window == nullptr )
	{
		CXMFLOAT2 defaultSize( 400, 400 );
		window = CreateImUiWindow( name, defaultSize );
	}

	ImUiWindow* parentWindowInStack = m_currentWindowStack.empty( ) ? nullptr : &m_windows[ m_currentWindowStack.back( ) ];
	ImUiWindow* parentWindow = parentWindowInStack;

	m_currentWindowStack.push_back( window->m_id );
	SetCurrentWindow( window );

	window->m_flag = flags;

	bool windowJustActivatedByUser = ( window->m_lastActiveFrame < m_frameCount - 1 );
	if ( flags & ImUiWindowFlags::Popup )
	{
		ImUIPopupRef& popupRef = m_openPopupStack[m_currentPopupStack.size( )];
		windowJustActivatedByUser |= ( window->m_popupID != popupRef.m_popupId );
		windowJustActivatedByUser |= ( window != popupRef.m_window );
	}

	// 임시
	window->m_visible = true;
	window->m_lastActiveFrame = m_frameCount;

	if ( flags & ImUiWindowFlags::Popup )
	{
		ImUIPopupRef& popupRef = m_openPopupStack[m_currentPopupStack.size( )];
		popupRef.m_window = window;
		m_currentPopupStack.emplace_back( popupRef );
		window->m_popupID = popupRef.m_popupId;
	}

	if ( m_nextWindowData.m_posCond )
	{
		window->SetPos( m_nextWindowData.m_pos );

		m_nextWindowData.m_posCond = 0;
	}

	bool isChildTooltip = ( flags & ImUiWindowFlags::ChildWindow ) && ( flags & ImUiWindowFlags::Tooltip );

	window->m_parentWindow = parentWindow;
	window->m_rootWindow = window->m_rootWindowForTitleBarHighlight = window;
	if ( parentWindow && ( flags & ImUiWindowFlags::ChildWindow ) && ( isChildTooltip == false ) )
	{
		window->m_rootWindow = parentWindow->m_rootWindow;
	}
	if ( parentWindow )
	{
		window->m_rootWindowForTitleBarHighlight = parentWindow->m_rootWindowForTitleBarHighlight;
	}
	window->m_windowRounding = ( flags & ImUiWindowFlags::ChildWindow ) ? m_curStyle.m_childRounding : ( ( flags & ImUiWindowFlags::Popup ) && !( flags & ImUiWindowFlags::Modal ) ) ? m_curStyle.m_popupRounding : m_curStyle.m_windowRounding;
	window->m_windowPadding = m_curStyle.m_windowPadding;

	if ( ( flags & ImUiWindowFlags::NoTitleBar ) == 0 && ( flags & ImUiWindowFlags::NoCollapse ) == 0 )
	{
		RECT titleBarRect = window->GetTitleBarRect( );

		if ( window->m_collapseToggleReserved ||
			( m_mouseOveredWindow == window && Contain( titleBarRect, m_io.m_mousePos ) && m_io.m_mouseDoubleClick[0] ) )
		{
			window->m_collapsed = !window->m_collapsed;
			FocusWindow( window );
		}
	}

	window->m_collapseToggleReserved = false;

	window->m_contentsSize = CalcContentsSize( window );

	if ( window->m_hiddenFrames > 0 )
	{
		--window->m_hiddenFrames;
	}

	if ( flags & ( ImUiWindowFlags::Popup | ImUiWindowFlags::Tooltip ) != 0 && windowJustActivatedByUser )
	{
		window->m_hiddenFrames = 1;
		if ( flags & ImUiWindowFlags::AlwaysAutoResize )
		{
			window->m_size.x = window->m_sizeNonCollapsed.x = 0.f;
			window->m_size.y = window->m_sizeNonCollapsed.y = 0.f;
			window->m_contentsSize = CXMFLOAT2( 0.f, 0.f );
		}
	}

	CXMFLOAT2 autoFitSize = CalcAutoFitSize( window, window->m_contentsSize );
	CXMFLOAT2 modifiedSize( FLT_MAX, FLT_MAX );
	if ( flags & ImUiWindowFlags::AlwaysAutoResize && ( window->m_collapsed == false ) )
	{
		window->m_sizeNonCollapsed.x = modifiedSize.x = autoFitSize.x;
		window->m_sizeNonCollapsed.y = modifiedSize.y = autoFitSize.y;
	}

	window->m_sizeNonCollapsed = CalcAfterConstraintSize( window, window->m_sizeNonCollapsed );

	if ( windowJustActivatedByUser )
	{
		window->m_autoPosLastDirection = ImDir::None;
		if ( ( flags & ImUiWindowFlags::Popup ) )
		{
			window->m_pos = m_currentPopupStack.back( ).m_openPopupPos;
		}
	}

	float titleBarHeight = window->GetTitleBarHeight( );
	window->m_size = window->m_collapsed && !(flags & ImUiWindowFlags::ChildWindow) ? CXMFLOAT2( window->m_sizeNonCollapsed.x, titleBarHeight ) : window->m_sizeNonCollapsed;

	if ( window->m_size.x > 0.f && !( flags & ImUiWindowFlags::Tooltip ) && !( flags & ImUiWindowFlags::AlwaysAutoResize ) )
	{
		window->m_itemWidthDefault = window->m_size.x * 0.65f;
	}
	else
	{
		window->m_itemWidthDefault = GetFontHeight( ) * 16.f;
	}

	window->m_contentsRegionRect.left = window->m_windowPadding.x;
	window->m_contentsRegionRect.top = window->m_windowPadding.y + window->GetTitleBarHeight( );
	window->m_contentsRegionRect.right = -window->m_windowPadding.x + ( window->m_explicitContentsSize.x != 0.f ? window->m_explicitContentsSize.x : window->m_size.x );
	window->m_contentsRegionRect.bottom = -window->m_windowPadding.y + ( window->m_explicitContentsSize.y != 0.f ? window->m_explicitContentsSize.y : window->m_size.y );

	// DrawContext 초기화
	ImUiDrawContext& dc = window->m_dc;

	dc.m_indentX = window->m_windowPadding.x;
	dc.m_cursorStartPos = window->m_pos + CXMFLOAT2( dc.m_indentX, window->m_windowPadding.y + titleBarHeight );
	dc.m_cursorPos = dc.m_cursorStartPos;
	dc.m_prevCursorPos = dc.m_cursorStartPos;
	dc.m_cursorMaxPos = dc.m_cursorStartPos;
	dc.m_currentLineHeight = 0.f;
	dc.m_prevLineHeight = 0.f;
	dc.m_currentLineTextBaseOffset = 0.f;
	dc.m_prevLineTextBaseOffset = 0.f;

	bool wantFocus = false;
	if ( windowJustActivatedByUser )
	{
		if ( ( flags & ( ImUiWindowFlags::ChildWindow | ImUiWindowFlags::Tooltip ) ) == false || ( flags & ImUiWindowFlags::Popup ) )
		{
			wantFocus = true;
		}
	}

	if ( wantFocus )
	{
		FocusWindow( window );
	}

	// 그리기
	window->m_drawList.Clear( );
	window->m_drawList.PushTextAtlas( m_textAtlas[m_curTextAltas].m_texture );
	PushClipRect( m_clientRect );
	
	float windowRounding = window->m_windowRounding;
	if ( window->m_collapsed )
	{
		CXMFLOAT4 titleColor = GetItemColor( ImUiColor::TitleBgCollapsed );
		RenderFrame( window->m_pos, CXMFLOAT2( window->m_size.x, titleBarHeight ), titleColor, windowRounding );
	}
	else
	{
		CXMFLOAT4 bgColor = GetItemColor( ImUiColor::WindowBg );
		// background
		window->m_drawList.AddFilledRect( window->m_pos + CXMFLOAT2( 0, titleBarHeight ), window->m_size - CXMFLOAT2( 0, titleBarHeight ), bgColor, windowRounding, ( flags & ImUiWindowFlags::NoTitleBar ) ? ImDrawCorner::All : ImDrawCorner::Bottom );

		// Title bar
		if ( !( flags & ImUiWindowFlags::NoTitleBar ) )
		{
			bool isTitleBarHighlight = wantFocus || ( m_navWindow && window->m_rootWindowForTitleBarHighlight == m_navWindow->m_rootWindowForTitleBarHighlight );
			CXMFLOAT4 titleColor = GetItemColor( isTitleBarHighlight ? ImUiColor::TitleBgActive : ImUiColor::TitleBg );
			window->m_drawList.AddFilledRect( window->m_pos, CXMFLOAT2( window->m_size.x, titleBarHeight ), titleColor, windowRounding, ImDrawCorner::Top );
		}
	}

	if ( !( flags & ImUiWindowFlags::NoTitleBar ) )
	{
		if ( !( flags & ImUiWindowFlags::NoCollapse ) )
		{
			// Collapse botton
			CXMFLOAT2 arrowMin = m_curWindow->m_pos + m_curStyle.m_framePadding;
			float fontSize = CalcTextSize( nullptr, 0 ).y;
			RECT arrowBoundingBox = {
				static_cast<long>( arrowMin.x + 1.f ),
				static_cast<long>( arrowMin.y + 1.f ),
				static_cast<long>( arrowMin.x + fontSize - 1.f ),
				static_cast<long>( arrowMin.y + fontSize - 1.f )
			};

			ImGUID collapseId = m_curWindow->GetID( "COLLAPSE" );
			bool mouseOvered = false;
			bool mouseHeld = false;

			if ( ButtonBehavior( arrowBoundingBox, collapseId, mouseOvered, mouseHeld ) )
			{
				m_curWindow->m_collapseToggleReserved = true;
			}

			RenderArrow( m_curWindow->m_pos + m_curStyle.m_framePadding, m_curWindow->m_collapsed ? ImDir::Right : ImDir::Down );
		}

		// Title text
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

	m_nextWindowData.m_constraintSizeCond = 0;

	m_curWindow->m_skipItem = m_curWindow->m_collapsed || !m_curWindow->m_visible;
	return !m_curWindow->m_skipItem;
}

void ImUI::EndWindow( )
{
	m_currentWindowStack.pop_back( );
	if ( m_curWindow->m_flag & ImUiWindowFlags::Popup )
	{
		m_currentPopupStack.pop_back( );
	}

	SetCurrentWindow( m_currentWindowStack.empty() ? nullptr : &m_windows[ m_currentWindowStack.back( ) ] );
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

	if ( mouseOvered && m_io.m_mouseClicked[0] )
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

bool ImUI::BeginCombo( const char* label, const char* prevValue, ImUiComboFlag::Type flags )
{
	assert( m_curWindow != nullptr );
	if ( m_curWindow->m_skipItem )
	{
		return false;
	}

	assert( ( flags & ( ImUiComboFlag::NoArrowButton | ImUiComboFlag::NoPreview ) ) != ( ImUiComboFlag::NoArrowButton | ImUiComboFlag::NoPreview ) );

	ImGUID id = m_curWindow->GetID( label );

	const float arrowSize = ( flags & ImUiComboFlag::NoArrowButton ) ? 0 : GetFrameHeight( );
	const int labelLen = strlen( label );
	CXMFLOAT2 labelSize = CalcTextSize( label, labelLen );
	const float width = CalcItemWidth( );

	CXMFLOAT2 itemSize( width, labelSize.y + m_curStyle.m_framePadding.y * 2.0f );
		
	const RECT frameBB = {
		static_cast<LONG>( m_curWindow->m_dc.m_cursorPos.x ),
		static_cast<LONG>( m_curWindow->m_dc.m_cursorPos.y ),
		static_cast<LONG>( m_curWindow->m_dc.m_cursorPos.x + width ),
		static_cast<LONG>( m_curWindow->m_dc.m_cursorPos.y + itemSize.y )
	};

	const RECT totalBB = {
		static_cast<LONG>( m_curWindow->m_dc.m_cursorPos.x ),
		static_cast<LONG>( m_curWindow->m_dc.m_cursorPos.y ),
		static_cast<LONG>( frameBB.right + ( ( labelSize.x > 0.f ) ? m_curStyle.m_itemInnerSpacing.x + labelSize.x : 0 ) ),
		static_cast<LONG>( frameBB.bottom )
	};

	ItemSize( itemSize );

	bool overed, held;
	bool pressed = ButtonBehavior( frameBB, id, overed, held );

	const RECT valueBB = {
		frameBB.left,
		frameBB.top,
		frameBB.right - arrowSize,
		frameBB.bottom
	};

	const CXMFLOAT4 frameColor = GetItemColor( overed ? ImUiColor::FrameBgMouseOver : ImUiColor::FrameBg );

	if ( ( flags & ImUiComboFlag::NoPreview ) == false )
	{
		m_curWindow->m_drawList.AddFilledRect( CXMFLOAT2( frameBB.left, frameBB.top ), CXMFLOAT2( frameBB.right - frameBB.left - arrowSize, frameBB.bottom - frameBB.top ), frameColor, m_curStyle.m_frameRounding, ImDrawCorner::Left );
	}

	if ( ( flags & ImUiComboFlag::NoArrowButton ) == false )
	{
		m_curWindow->m_drawList.AddFilledRect( CXMFLOAT2( frameBB.right - arrowSize, frameBB.top ), CXMFLOAT2( arrowSize, frameBB.bottom - frameBB.top ), GetItemColor( overed ? ImUiColor::ButtonMouseOver : ImUiColor::Button ), m_curStyle.m_frameRounding, ( width <= arrowSize ) ? ImDrawCorner::All : ImDrawCorner::Right );
		RenderArrow( CXMFLOAT2( frameBB.right - arrowSize + m_curStyle.m_framePadding.x, frameBB.top + m_curStyle.m_framePadding.y ), ImDir::Down );
	}

	if ( ( prevValue != nullptr ) && ( ( flags & ImUiComboFlag::NoPreview ) == false ) )
	{
		RenderClippedText( 
			CXMFLOAT2( frameBB.left + m_curStyle.m_framePadding.x, frameBB.top + m_curStyle.m_framePadding.y ),
			CXMFLOAT2( valueBB.right, valueBB.bottom ),
			prevValue,
			strlen( prevValue ),
			CXMFLOAT2( 0.f, 0.f ) );
	}

	if ( labelSize.x > 0.f )
	{
		RenderText(
			CXMFLOAT2( frameBB.right + m_curStyle.m_itemInnerSpacing.x, frameBB.top + m_curStyle.m_framePadding.y ),
			label,
			labelLen
		);
	}

	bool popupOpen = IsPopupOpen( id );
	if ( pressed && !popupOpen )
	{
		OpenPopupEx( id );
		popupOpen = true;
	}

	if ( popupOpen == false )
	{
		return false;
	}

	bool backupNextWindowSizeConstraint = m_nextWindowData.m_constraintSizeCond;
	m_nextWindowData.m_constraintSizeCond = 0;

	if ( backupNextWindowSizeConstraint )
	{
		m_nextWindowData.m_contentSizeCond = backupNextWindowSizeConstraint;
		m_nextWindowData.m_constraintSizeRect.left = max( m_nextWindowData.m_constraintSizeRect.left, width );
	}
	else
	{
		if ( ( flags & ImUiComboFlag::HeightMask ) == false )
		{
			flags |= ImUiComboFlag::HeightRegular;
		}

		int popupMaxHeihtInItems = -1;

		switch ( flags & ImUiComboFlag::HeightMask )
		{
		case ImUiComboFlag::HeightRegular:
			popupMaxHeihtInItems = 8;
			break;
		case ImUiComboFlag::HeightSmall:
			popupMaxHeihtInItems = 4;
			break;
		case ImUiComboFlag::HeightLarge:
			popupMaxHeihtInItems = 20;
			break;
		}
		RECT constraintSize = { static_cast<LONG>( width ), 0L, LONG_MAX, static_cast<LONG>( CalcMaxPopupHeightFromItemCount( popupMaxHeihtInItems ) ) };
		SetNextWindowConstraintSize( constraintSize );
	}

	char name[16];
	sprintf_s( name, "##Combo_%02d", m_currentPopupStack.size( ) );

	if ( ImUiWindow* popupWindow = FindWindow( name ) )
	{
		if ( popupWindow->m_wasVisible )
		{
			CXMFLOAT2 contentsSize = CalcContentsSize( popupWindow );
			CXMFLOAT2 expectedSize = CalcAfterConstraintSize( popupWindow, CalcAutoFitSize( popupWindow, contentsSize ) );
			CXMFLOAT2 pos = FindBestWindowPosForPopup( CXMFLOAT2( frameBB.left, frameBB.bottom ), expectedSize, popupWindow->m_autoPosLastDirection, frameBB, ImUiPopupPositionPolicy::ComboBox );
			SetNextWindowPos( pos );
		}
	}

	ImUiWindowFlags::Type windowFlags = ImUiWindowFlags::AlwaysAutoResize |
										ImUiWindowFlags::Popup |
										ImUiWindowFlags::NoTitleBar |
										ImUiWindowFlags::NoResize | 
										ImUiWindowFlags::NoSavedSettings;
	if ( Window( name, windowFlags ) == false )
	{
		assert( false );
		return false;
	}

	if ( m_curStyle.m_framePadding.x != m_curStyle.m_windowPadding.x )
	{

	}

	return true;
}

void ImUI::EndCombo( )
{
	EndPopup( );
}

bool ImUI::Combo( const char* label, int* currentItem, const char* const items[], int itemsCount, int heightInItem )
{
	bool valueChanged = Combo( label, currentItem, ItemsArrayGetter, (void*)( items ), itemsCount, heightInItem );
	return valueChanged;
}

bool ImUI::Combo( const char* label, int* currentItem, bool( *itemsGettter )( void *data, int idx, const char **outText ), void * data, int itemCount, int heightInItem )
{
	const char* prevText = nullptr;
	if ( *currentItem >= 0 && *currentItem < itemCount )
	{
		itemsGettter( data, *currentItem, &prevText );
	}

	if ( heightInItem != -1 && ( m_nextWindowData.m_constraintSizeCond == false ) )
	{
		LONG popupMaxHeight = static_cast<LONG>( CalcMaxPopupHeightFromItemCount( heightInItem ) );
		RECT constraintSize = { 0L, 0L, LONG_MAX, popupMaxHeight };
		SetNextWindowConstraintSize( constraintSize );
	}

	if ( !BeginCombo( label, prevText ) )
	{
		return false;
	}

	bool valueChanged = false;
	for ( int i = 0; i < itemCount; ++i )
	{
		const bool itemSelected = ( i == *currentItem );
		const char* itemText;

		if ( !itemsGettter( data, i, &itemText ) )
		{
			itemText = "*Unknown item*";
		}

		if ( Selectable( itemText, itemSelected ) )
		{
			valueChanged = true;
			*currentItem = i;
		}
		if ( itemSelected )
		{
			
		}
	}

	EndCombo( );
	return valueChanged;
}

bool ImUI::Selectable( const char* label, bool selected, ImUiSelectableFlags::Type flags, const CXMFLOAT2& sizeArg )
{
	assert( m_curWindow != nullptr );
	if ( m_curWindow->m_skipItem )
	{
		return false;
	}

	ImGUID id = m_curWindow->GetID( label );
	CXMFLOAT2 labelSize = CalcTextSize( label, strlen( label ) );
	CXMFLOAT2 size( sizeArg.x != 0.f ? sizeArg.x : labelSize.x, sizeArg.y != 0.f ? sizeArg.y : labelSize.y );
	CXMFLOAT2 pos = m_curWindow->m_dc.m_cursorPos;
	pos.y += m_curWindow->m_dc.m_currentLineTextBaseOffset;
	ItemSize( size );

	RECT bb = { pos.x, pos.y, pos.x + size.x, pos.y + size.y };

	const CXMFLOAT2& windowPadding = m_curWindow->m_windowPadding;
	float maxX = GetWindowContextRegionMax().x;
	float drawWidth = max( labelSize.x, m_curWindow->m_pos.x + maxX - windowPadding.x - m_curWindow->m_dc.m_cursorPos.x );
	CXMFLOAT2 drawSize( sizeArg.x != 0 ? sizeArg.x : drawWidth, sizeArg.y != 0 ? sizeArg.y : size.y );
	RECT bbWithSpacing = { pos.x, pos.y, pos.x + drawSize.x, pos.y + drawSize.y };

	float spacing_L = static_cast<float>(static_cast<int>(m_curStyle.m_itemSpacing.x * 0.5f));
	float spacing_T = static_cast<float>(static_cast<int>(m_curStyle.m_itemSpacing.y * 0.5f));
	float spacing_R = m_curStyle.m_itemSpacing.x - spacing_L;
	float spacing_B = m_curStyle.m_itemSpacing.y - spacing_T;

	bbWithSpacing.left -= spacing_L;
	bbWithSpacing.top -= spacing_T;
	bbWithSpacing.right += spacing_R;
	bbWithSpacing.bottom += spacing_B;

	bool mouseOvered = false;
	bool mouseHeld = false;
	bool mousePressed = ButtonBehavior( bbWithSpacing, id, mouseOvered, mouseHeld );

	if ( mouseOvered || selected )
	{
		CXMFLOAT4 color = GetItemColor( ( mouseHeld && mouseOvered ) ? ImUiColor::HeaderActive : mouseOvered ? ImUiColor::HeaderMouseOver : ImUiColor::Header );
		RenderFrame( CXMFLOAT2( bbWithSpacing.left, bbWithSpacing.top ), CXMFLOAT2( bbWithSpacing.right - bbWithSpacing.left, bbWithSpacing.bottom - bbWithSpacing.top ), color );
	}

	RenderClippedText( CXMFLOAT2( bb.left, bb.top ), CXMFLOAT2( bbWithSpacing.right, bbWithSpacing.bottom ), label, strlen( label ), CXMFLOAT2( 0.f, 0.f ) );

	if ( mousePressed && ( m_curWindow->m_flag & ImUiWindowFlags::Popup ) )
	{
		CloseCurrentPopup( );
	}

	return mousePressed;
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
		if ( window.m_visible && ( window.m_hiddenFrames == 0 ) )
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

CXMFLOAT2 ImUI::GetWindowContextRegionMax( )
{
	assert( m_curWindow != nullptr );

	return CXMFLOAT2( m_curWindow->m_contentsRegionRect.right, m_curWindow->m_contentsRegionRect.bottom );
}

float ImUI::GetFontHeight( ) const
{
	assert( m_curTextAltas != -1 );
	return m_textAtlas[m_curTextAltas].m_fontHeight;
}

float ImUI::GetFrameHeight( ) const
{
	assert( m_curTextAltas != -1 );
	return m_textAtlas[m_curTextAltas].m_fontHeight + m_curStyle.m_framePadding.y * 2.f;
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
	for ( int i = static_cast<int>( m_windows.size() ) - 1; i >= 0; --i )
	{
		if ( m_windows[i].m_visible == false )
		{
			continue;
		}

		RECT boundingBox = { 
			static_cast<long>( m_windows[i].m_pos.x ),
			static_cast<long>( m_windows[i].m_pos.y ), 
			static_cast<long>( m_windows[i].m_pos.x + m_windows[i].m_size.x ),
			static_cast<long>( m_windows[i].m_pos.y + m_windows[i].m_size.y ) };

		if ( Contain( boundingBox, m_io.m_mousePos ) )
		{
			return (&m_windows[i]);
		}
	}

	return nullptr;
}

ImUiWindow* ImUI::CreateImUiWindow( const char* name, const CXMFLOAT2& size )
{
	int id = static_cast<int>( m_windows.size( ) );
	m_windowLUT.emplace( name, m_windows.size( ) );
	m_windows.emplace_back( *this );
	
	ImUiWindow& window = m_windows.back( );
	window.m_id = id;

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

float ImUI::CalcItemWidth( )
{
	assert( m_curWindow != nullptr );
	float w = m_curWindow->m_itemWidthDefault;
	if ( w < 0.f )
	{
		assert( false && "Not Implemented" );
	}

	w = static_cast<float>( static_cast<int>( w ) );
	return w;
}

float ImUI::CalcMaxPopupHeightFromItemCount( int itemCount )
{
	if ( itemCount <= 0 )
	{
		return FLT_MAX;
	}

	return ( GetFontHeight( ) + m_curStyle.m_itemSpacing.y ) * itemCount - m_curStyle.m_itemSpacing.y + ( m_curStyle.m_windowPadding.y * 2.f );
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
		if ( m_io.m_mouseClicked[0] )
		{
			SetActiveItemID( id );
			FocusWindow( m_curWindow );
		}
	}

	if ( m_activeItemID == id )
	{
		if ( m_io.m_mouseClicked[0] )
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

void ImUI::RenderText( const CXMFLOAT2& pos, const char* text, int count )
{
	if ( count == 0 )
	{
		return;
	}

	assert( m_curWindow != nullptr );
	assert( m_curTextAltas != -1 );

	const CXMFLOAT2 textSize = CalcTextSize( text, count );

	m_curWindow->m_drawList.AddText( m_textAtlas[m_curTextAltas], pos, GetItemColor( ImUiColor::Text ), text, count );
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
	dc.m_cursorMaxPos.x = max( dc.m_cursorMaxPos.x, dc.m_prevCursorPos.x );
	dc.m_cursorMaxPos.y = max( dc.m_cursorMaxPos.y, dc.m_cursorPos.y - m_curStyle.m_itemSpacing.y );

	dc.m_prevLineHeight = lineHeight;
	dc.m_prevLineTextBaseOffset = lineTextBaseOffset;
	dc.m_currentLineHeight = 0.f;
	dc.m_currentLineTextBaseOffset = 0.f;
}

bool ImUI::MouseOveredItem( const RECT& boundingbox, ImGUID id )
{
	assert( m_curWindow != nullptr );

	if ( m_mouseOveredWindow != m_curWindow )
	{
		return false;
	}
	if ( Contain( boundingbox , m_io.m_mousePos ) == false )
	{
		return false;
	}
	if ( IsWindowContentMouseOverable( m_curWindow ) == false )
	{
		return false;
	}

	SetMouseOveredID( id );

	return true;
}

bool ImUI::IsWindowContentMouseOverable( ImUiWindow* window )
{
	if ( m_navWindow )
	{
		if ( ImUiWindow* focusedRootWindow = m_navWindow->m_rootWindow )
		{
			if ( focusedRootWindow->m_wasVisible && focusedRootWindow != window->m_rootWindow )
			{
				if ( focusedRootWindow->m_flag & ImUiWindowFlags::Popup )
				{
					return false;
				}
			}
		}
	}

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

void ImUI::SetCurrentWindow( ImUiWindow * window )
{
	m_curWindow = window;
}

void ImUI::FocusWindow( ImUiWindow* window )
{
	if ( m_navWindow != window )
	{
		m_navWindow = window;
	}

	if ( window == nullptr )
	{
		return;
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

void ImUI::OpenPopupEx( ImGUID id )
{
	int curStackSize = m_currentPopupStack.size( );

	ImUIPopupRef popupRef = {
		id,
		nullptr,
		m_curWindow,
		m_frameCount,
		m_curWindow->m_idStack.back( ),
		m_io.m_mousePos,
		m_io.m_mousePos
	};

	if ( m_openPopupStack.size( ) < curStackSize + 1 )
	{
		m_openPopupStack.push_back( popupRef );
	}
	else
	{
		m_openPopupStack.resize( curStackSize + 1 );

		if ( m_openPopupStack[curStackSize].m_popupId == id && m_openPopupStack[curStackSize].m_openFrameCount == m_frameCount - 1 )
		{
			m_openPopupStack[curStackSize].m_openFrameCount = popupRef.m_openFrameCount;
		}
		else
		{
			m_openPopupStack[curStackSize] = popupRef;
		}
	}
}

void ImUI::EndPopup( )
{
	assert( m_curWindow->m_flag & ImUiWindowFlags::Popup );
	assert( m_currentPopupStack.size( ) > 0 );

	EndWindow( );
}

void ImUI::ClosePopupsOverWindow( ImUiWindow* refWindow )
{
	if ( m_openPopupStack.empty( ) )
	{
		return;
	}

	int n = 0;
	if ( refWindow )
	{
		for ( n = 0; n < m_openPopupStack.size( ); ++n )
		{
			ImUIPopupRef& popup = m_openPopupStack[n];
			if ( popup.m_window == nullptr )
			{
				continue;
			}

			assert( ( popup.m_window->m_flag & ImUiWindowFlags::Popup ) > 0 );
			if ( popup.m_window->m_flag & ImUiWindowFlags::ChildWindow )
			{
				continue;
			}

			bool hasFocus = false;
			for ( int m = n; m < m_openPopupStack.size( ) && ( hasFocus == false ); ++m )
			{
				hasFocus = ( m_openPopupStack[m].m_window && m_openPopupStack[m].m_window->m_rootWindow == refWindow->m_rootWindow );
			}
			if ( hasFocus == false )
			{
				break;
			}
		}
	}
	if ( n < m_openPopupStack.size( ) )
	{
		ClosePopupToLevel( n );
	}
}

void ImUI::ClosePopupToLevel( int remaining )
{
	ImUiWindow* focusWindow = ( remaining > 0 ) ? m_openPopupStack[remaining - 1].m_window : m_openPopupStack[0].m_parentWindow;
	FocusWindow( focusWindow );
	m_openPopupStack.resize( remaining );
}

void ImUI::CloseCurrentPopup( )
{
	int popupIdx = m_currentPopupStack.size( ) - 1;
	if ( popupIdx < 0 || popupIdx >= m_openPopupStack.size() || m_currentPopupStack[popupIdx].m_popupId != m_openPopupStack[popupIdx].m_popupId )
	{
		return;
	}
	while ( popupIdx > 0 && m_openPopupStack[popupIdx].m_window && ( m_openPopupStack[popupIdx].m_window->m_flag & ImUiWindowFlags::ChildWindow ) )
	{
		--popupIdx;
	}
	ClosePopupToLevel( popupIdx );
}

bool ImUI::IsPopupOpen( ImGUID id )
{
	return ( m_openPopupStack.size() > m_currentPopupStack.size() ) && m_openPopupStack[m_currentPopupStack.size()].m_popupId == id;
}

CXMFLOAT2 ImUI::CalcAfterConstraintSize( ImUiWindow* window, const CXMFLOAT2& size )
{
	CXMFLOAT2 newSize = size;

	if ( m_nextWindowData.m_constraintSizeCond != 0 )
	{
		const RECT& cr = m_nextWindowData.m_constraintSizeRect;

		newSize.x = (cr.left >= 0 && cr.right >= 0) ? clamp( newSize.x, cr.left, cr.right ) : window->m_sizeNonCollapsed.x;
		newSize.y = (cr.top >= 0 && cr.bottom >= 0) ? clamp( newSize.y, cr.top, cr.bottom ) : window->m_sizeNonCollapsed.y;
		if ( m_nextWindowData.m_sizeCallback )
		{
			ImUiSizeCallbackData data;
			data.m_userData = m_nextWindowData.m_sizeCallbackUserData;
			data.m_pos = window->m_pos;
			data.m_currentSize = window->m_sizeNonCollapsed;
			data.m_desiredSize = newSize;
			m_nextWindowData.m_sizeCallback( &data );
			newSize = data.m_desiredSize;
		}
	}

	if ( !( window->m_flag & ( ImUiWindowFlags::ChildWindow | ImUiWindowFlags::AlwaysAutoResize ) ) )
	{
		newSize = Max( newSize, m_curStyle.m_windowMinSize );
		newSize.y = max( newSize.y, window->GetTitleBarHeight( ) + max( 0.f, m_curStyle.m_windowRounding - 1.f ) );
	}

	return newSize;
}

CXMFLOAT2 ImUI::CalcAutoFitSize( ImUiWindow* window, const CXMFLOAT2& contentsSize )
{
	ImUiWindowFlags::Type flags = window->m_flag;
	CXMFLOAT2 autoFitSize;

	if ( ( flags & ImUiWindowFlags::Tooltip ) != 0 )
	{
		autoFitSize = contentsSize;
	}
	else
	{
		autoFitSize = clamp( contentsSize, m_curStyle.m_windowMinSize, Max( m_curStyle.m_windowMinSize, m_io.m_displaySize - m_curStyle.m_displaySafeAreaPadding ) );
		CXMFLOAT2 autoFitSizeAfterConstraint = CalcAfterConstraintSize( window, autoFitSize );
	}

	return autoFitSize;
}

CXMFLOAT2 ImUI::FindBestWindowPosForPopup( const CXMFLOAT2& refPos, const CXMFLOAT2& size, ImDir::Type& lastDir, const RECT& avoid, ImUiPopupPositionPolicy::Type policy )
{
	const CXMFLOAT2& safePadding = m_curStyle.m_displaySafeAreaPadding;
	RECT outer = m_clientRect;
	Expand( outer, 
		CXMFLOAT2( ( size.x - ( outer.right - outer.left ) > safePadding.x * 2 ) ? -safePadding.x : 0.f,
		( size.y - ( outer.bottom - outer.top ) > safePadding.y * 2 ) ? -safePadding.y : 0.f)
	);
	
	if ( policy == ImUiPopupPositionPolicy::ComboBox )
	{
		ImDir::Type dirOrder[ImDir::Count] = { ImDir::Down, ImDir::Right, ImDir::Left, ImDir::Up };
		for ( int i = ( lastDir != ImDir::None ) ? -1 : 0; i < ImDir::Count; ++i )
		{
			ImDir::Type dir = ( i == -1 ) ? lastDir : dirOrder[i];
			if ( i != -1 && dir == lastDir )
			{
				continue;
			}

			CXMFLOAT2 pos;
			switch ( dir )
			{
			case ImDir::Down:
				pos = CXMFLOAT2( avoid.left, avoid.bottom );
				break;
			case ImDir::Right:
				pos = CXMFLOAT2( avoid.left, avoid.top - size.y );
				break;
			case ImDir::Left:
				pos = CXMFLOAT2( avoid.right - size.x, avoid.bottom );
				break;
			case ImDir::Up:
				pos = CXMFLOAT2( avoid.right - size.x, avoid.top - size.y );
				break;
			}

			RECT r = { pos.x, pos.y, pos.x + size.x, pos.y + size.y };
			if ( Contain( outer, r ) == false )
			{
				continue;
			}

			lastDir = dir;
			return pos;
		}
	}

	CXMFLOAT2 clampedBasePos = clamp( refPos, CXMFLOAT2( outer.left, outer.top ), CXMFLOAT2( outer.right, outer.bottom ) );

	ImDir::Type dirOrder[ImDir::Count] = { ImDir::Right, ImDir::Down, ImDir::Up, ImDir::Left };
	for ( int i = ( lastDir != ImDir::None ) ? -1 : 0; i < ImDir::Count; ++i )
	{
		ImDir::Type dir = ( i == -1 ) ? lastDir : dirOrder[i];
		if ( i != -1 && dir == lastDir )
		{
			continue;
		}

		float availW = ( dir == ImDir::Left ? avoid.left : avoid.right ) - ( dir == ImDir::Right ? avoid.right : outer.left );
		float availH = ( dir == ImDir::Up ? avoid.top : avoid.bottom ) - ( dir == ImDir::Down ? avoid.bottom : outer.top );
		if ( availW < size.x || availH < size.y )
		{
			continue;
		}

		CXMFLOAT2 pos;
		pos.x = ( dir == ImDir::Left ) ? avoid.left - size.x : ( dir == ImDir::Right ) ? avoid.right : clampedBasePos.x;
		pos.y = ( dir == ImDir::Up ) ? avoid.top - size.y : ( dir == ImDir::Down ) ? avoid.bottom : clampedBasePos.y;
		lastDir = dir;
		return pos;
	}

	lastDir = ImDir::None;
	CXMFLOAT2 pos = refPos;
	pos.x = max( min( pos.x + size.x, outer.right ) - size.x, outer.left );
	pos.y = max( min( pos.y + size.y, outer.bottom ) - size.y, outer.top );
	return pos;
}

void ImUI::SetNextWindowPos( const CXMFLOAT2& pos, ImUiCond::Type cond, const CXMFLOAT2& pivot )
{
	m_nextWindowData.m_pos = pos;
	m_nextWindowData.m_posPivot = pivot;
	m_nextWindowData.m_posCond = cond ? cond : ImUiCond::Always;
}

void ImUI::SetNextWindowConstraintSize( const RECT& constraintSize, ImUiSizeCallBack callback, void * callbackData )
{
	m_nextWindowData.m_constraintSizeCond = ImUiCond::Always;
	m_nextWindowData.m_constraintSizeRect = constraintSize;
	m_nextWindowData.m_sizeCallback = callback;
	m_nextWindowData.m_sizeCallbackUserData = callbackData;
}
