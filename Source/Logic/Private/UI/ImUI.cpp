#include "stdafx.h"
#include "UI/ImUI.h"

#include "Core/Timer.h"
#include "Crc32Hash.h"

#include <algorithm>
#include <fstream>

namespace
{
	bool Contain( const Rect& rect, const Point2& mousePos )
	{
		return rect.m_leftTop.x <= mousePos.x && rect.m_rightBottom.x >= mousePos.x &&
			rect.m_leftTop.y <= mousePos.y && rect.m_rightBottom.y >= mousePos.y;
	}

	bool Contain( const Rect& lhs, const Rect& rhs )
	{
		return lhs.m_leftTop.x <= rhs.m_leftTop.x && lhs.m_rightBottom.x >= rhs.m_rightBottom.x &&
			lhs.m_leftTop.y <= rhs.m_leftTop.y && lhs.m_rightBottom.y >= rhs.m_rightBottom.y;
	}

	float clamp( const float value, const float minValue, const float maxValue )
	{
		return std::max( std::min( value, maxValue ), minValue );
	}

	Vector2 clamp( const Vector2& value, const Vector2& minVlaue, const Vector2& maxValue )
	{
		return Vector2( clamp( value.x, minVlaue.x, maxValue.x ), clamp( value.y, minVlaue.y, maxValue.y ) );
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

	void* SettingHandlerWindow_ReadOpen( ImUI* ui, ImUiSettingHandler* /*handler*/, const char* name )
	{
		ImUiWindowSetting* settings = ui->FindWindowSettings( name );
		if ( settings == nullptr )
		{
			settings = ui->AddWindowSettings( name );
		}

		return static_cast<void*>( settings );
	}

	void SettingHandlerWindow_ReadLine( ImUI* /*ui*/, ImUiSettingHandler* /*handler*/, void* entry, const char* line )
	{
		ImUiWindowSetting* settings = static_cast<ImUiWindowSetting*>( entry );
		float x, y;
		int32 i;
		if ( sscanf_s( line, "Pos=%f,%f", &x, &y ) == 2 )
		{
			settings->m_pos = Point2( x, y );
		}
		else if ( sscanf_s( line, "Size=%f,%f", &x, &y ) == 2 )
		{
			settings->m_size = Vector2( x, y );
		}
		else if ( sscanf_s( line, "Collapsed=%d", &i ) == 1 )
		{
			settings->m_collapsed = ( i != 0 );
		}
	}

	static bool ItemsArrayGetter( void* data, int32 idx, const char** out_text )
	{
		const char* const* items = (const char* const*)data;
		if ( out_text )
			*out_text = items[idx];
		return true;
	}

	Vector2 CalcContentsSize( ImUiWindow* window )
	{
		Vector2 sz(
			( window->m_explicitContentsSize.x != 0.f ) ? window->m_explicitContentsSize.x : ( window->m_dc.m_cursorMaxPos.x - window->m_pos.x ),
			( window->m_explicitContentsSize.y != 0.f ) ? window->m_explicitContentsSize.y : ( window->m_dc.m_cursorMaxPos.y - window->m_pos.y )
		);

		return sz + window->m_windowPadding;
	}

	Vector2 Max( const Vector2& lhs, const Vector2& rhs )
	{
		return Vector2( std::max( lhs.x, rhs.x ), std::max( lhs.x, rhs.x ) );
	}

	void Expand( Rect& out, const float amount )
	{
		out.m_leftTop.x -= amount;
		out.m_leftTop.y -= amount;
		out.m_rightBottom.x += amount;
		out.m_rightBottom.y += amount;
	}

	void Expand( Rect& out, const Vector2& amount )
	{
		out.m_leftTop.x -= amount.x;
		out.m_leftTop.y -= amount.y;
		out.m_rightBottom.x += amount.x;
		out.m_rightBottom.y += amount.y;
	}
}

ImGUID ImUiWindow::GetID( const char* str )
{
	ImGUID seed = m_idStack.back( );
	ImGUID id = Crc32Hash( str, 0, seed );
	return id;
}

float ImUiWindow::GetTitleBarHeight( ) const
{
	return ( m_flag & ImUiWindowFlags::NoTitleBar ) ? 0.f : m_imUi.GetCurStyle( ).m_framePadding.y * 2 + m_imUi.CalcTextSize( nullptr, 0 ).y;
}

Rect ImUiWindow::GetTitleBarRect( ) const
{
	return Rect( m_pos.x, m_pos.y, m_pos.x + m_sizeNonCollapsed.x, m_pos.y + GetTitleBarHeight( ) );
}

void ImUiWindow::SetPos( const Point2& pos )
{
	Point2 oldPos = m_pos;

	m_pos = pos;
	m_dc.m_cursorPos += ( m_pos - oldPos );
	m_dc.m_cursorMaxPos += ( m_pos - oldPos );
}

Vector2 CTextAtlas::CalcTextSize( const char* text, uint32 count ) const
{
	Vector2 size = { 0.f, m_fontHeight };

	for ( uint32 i = 0; i < count; ++i )
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

	LoadSettingFromDisk( "./Configs/imUI_setting.cfg" );

	for ( int32 i = 0, end = _countof( m_circleVertex ); i < end; ++i )
	{
		const float angle = ( static_cast<float>( i ) * DirectX::XM_2PI ) / end;
		m_circleVertex[i] = Point2( cosf( angle ), sinf( angle ) );
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
		m_curTextAltas = static_cast<int32>( m_textAtlas.size( ) );
		m_textAtlasLUT.emplace( fontName, m_curTextAltas );
		m_textAtlas.emplace_back( textAtlas );
	}
}

void ImUI::StyleColorDark( )
{
	ColorF* colors = m_curStyle.m_colors;

	colors[ImUiColor::Text] = ColorF( 1.f, 1.f, 1.f, 1.f );
	colors[ImUiColor::WindowBg] = ColorF( 0.06f, 0.06f, 0.06f, 0.94f );
	colors[ImUiColor::FrameBg] = ColorF( 0.16f, 0.29f, 0.48f, 0.54f );
	colors[ImUiColor::FrameBgMouseOver] = ColorF( 0.26f, 0.59f, 0.98f, 0.4f );
	colors[ImUiColor::FrameBgActive] = ColorF( 0.26f, 0.59f, 0.98f, 0.67f );
	colors[ImUiColor::TitleBg] = ColorF( 0.04f, 0.04f, 0.04f, 1.f );
	colors[ImUiColor::TitleBgActive] = ColorF( 0.16f, 0.29f, 0.48f, 1.f );
	colors[ImUiColor::TitleBgCollapsed] = ColorF( 0.00f, 0.00f, 0.00f, 0.51f );
	colors[ImUiColor::SliderGrab] = ColorF( 0.24f, 0.52f, 0.88f, 1.f );
	colors[ImUiColor::SliderGrabActive] = ColorF( 0.26f, 0.59f, 0.98f, 1.f );
	colors[ImUiColor::Button] = ColorF( 0.26f, 0.59f, 0.98f, 0.4f );
	colors[ImUiColor::ButtonMouseOver] = ColorF( 0.26f, 0.59f, 0.98f, 1.f );
	colors[ImUiColor::ButtonPress] = ColorF( 0.06f, 0.53f, 0.98f, 1.f );
	colors[ImUiColor::Header] = ColorF( 0.26f, 0.59f, 0.98f, 0.31f );
	colors[ImUiColor::HeaderMouseOver] = ColorF( 0.26f, 0.59f, 0.98f, 0.80f );
	colors[ImUiColor::HeaderActive] = ColorF( 0.26f, 0.59f, 0.98f, 1.00f );
}

void ImUI::BeginFrame( const Rect& clientRect, float elapsedTime, float totalTime )
{
	using namespace DirectX;

	++m_frameCount;

	m_clientRect = clientRect;
	m_io.m_displaySize = m_clientRect.GetWidthHeight();

	m_mouseOveredID = 0;

	for ( int32 i = 0; i < 5; ++i )
	{
		m_io.m_mouseClicked[i] = m_io.m_mouseDown[i] && m_io.m_mouseDownDuration[i] < 0.f;
		m_io.m_mouseDownDurationPrev[i] = m_io.m_mouseDownDuration[i];
		m_io.m_mouseDownDuration[i] = m_io.m_mouseDown[i] ? ( m_io.m_mouseDownDuration[i] < 0.f ? 0.f : m_io.m_mouseDownDuration[i] + elapsedTime ) : -1.f;
		m_io.m_mouseDoubleClick[i] = false;
		if ( m_io.m_mouseClicked[i] )
		{
			if ( totalTime - m_io.m_mouseClickedTime[i] < m_io.m_mouseDoubleDownTime )
			{
				if ( XMVectorGetX( XMVector2LengthSq( m_io.m_mousePos - m_io.m_mouseClickedPos[i] ) ) < m_io.m_mouseDoubleDownMaxDist * m_io.m_mouseDoubleDownMaxDist )
				{
					m_io.m_mouseDoubleClick[i] = true;
				}
				m_io.m_mouseClickedTime[i] = -FLT_MAX;
			}
			else
			{
				m_io.m_mouseClickedTime[i] = totalTime;
			}
			m_io.m_mouseClickedPos[i] = m_io.m_mousePos;
		}
	}

	SetMouseOveredID( 0 );

	UpdateMovingWindow( );

	m_mouseOveredWindow = FindMouseOverWindow( );

	for ( auto& window : m_windows )
	{
		window->m_wasVisible = window->m_visible;
		window->m_visible = false;
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
		Vector2 defaultSize( 400, 400 );
		window = CreateImUiWindow( name, defaultSize );
	}

	ImUiWindow* parentWindowInStack = m_currentWindowStack.empty( ) ? nullptr : m_currentWindowStack.back( );
	ImUiWindow* parentWindow = parentWindowInStack;

	m_currentWindowStack.push_back( window );
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
		Rect titleBarRect = window->GetTitleBarRect( );

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

	if ( ( flags & ( ImUiWindowFlags::Popup | ImUiWindowFlags::Tooltip ) ) != 0 && windowJustActivatedByUser )
	{
		window->m_hiddenFrames = 1;
		if ( flags & ImUiWindowFlags::AlwaysAutoResize )
		{
			window->m_size.x = window->m_sizeNonCollapsed.x = 0.f;
			window->m_size.y = window->m_sizeNonCollapsed.y = 0.f;
			window->m_contentsSize = Vector2::ZeroVector;
		}
	}

	Vector2 autoFitSize = CalcAutoFitSize( window, window->m_contentsSize );
	Vector2 modifiedSize( FLT_MAX, FLT_MAX );
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
	window->m_size = window->m_collapsed && !(flags & ImUiWindowFlags::ChildWindow) ? Vector2( window->m_sizeNonCollapsed.x, titleBarHeight ) : window->m_sizeNonCollapsed;

	if ( window->m_size.x > 0.f && !( flags & ImUiWindowFlags::Tooltip ) && !( flags & ImUiWindowFlags::AlwaysAutoResize ) )
	{
		window->m_itemWidthDefault = window->m_size.x * 0.65f;
	}
	else
	{
		window->m_itemWidthDefault = GetFontHeight( ) * 16.f;
	}

	window->m_contentsRegionRect.m_leftTop.x = window->m_windowPadding.x;
	window->m_contentsRegionRect.m_leftTop.y = window->m_windowPadding.y + window->GetTitleBarHeight( );
	window->m_contentsRegionRect.m_rightBottom.x = -window->m_windowPadding.x + ( window->m_explicitContentsSize.x != 0.f ? window->m_explicitContentsSize.x : window->m_size.x );
	window->m_contentsRegionRect.m_rightBottom.y = -window->m_windowPadding.y + ( window->m_explicitContentsSize.y != 0.f ? window->m_explicitContentsSize.y : window->m_size.y );

	// DrawContext 초기화
	ImUiDrawContext& dc = window->m_dc;

	dc.m_indentX = window->m_windowPadding.x;
	dc.m_cursorStartPos = window->m_pos + Vector2( dc.m_indentX, window->m_windowPadding.y + titleBarHeight );
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
//	window->m_drawList.PushTextAtlas( m_textAtlas[m_curTextAltas].m_texture );
	PushClipRect( m_clientRect );
	
	float windowRounding = window->m_windowRounding;
	if ( window->m_collapsed )
	{
		ColorF titleColor = GetItemColor( ImUiColor::TitleBgCollapsed );
		RenderFrame( window->m_pos, Vector2( window->m_size.x, titleBarHeight ), titleColor, windowRounding );
	}
	else
	{
		ColorF bgColor = GetItemColor( ImUiColor::WindowBg );
		// background
		window->m_drawList.AddFilledRect( window->m_pos + Vector2( 0, titleBarHeight ), window->m_size - Vector2( 0, titleBarHeight ), bgColor, windowRounding, ( flags & ImUiWindowFlags::NoTitleBar ) ? ImDrawCorner::All : ImDrawCorner::Bottom );

		// Title bar
		if ( !( flags & ImUiWindowFlags::NoTitleBar ) )
		{
			bool isTitleBarHighlight = wantFocus || ( m_navWindow && window->m_rootWindowForTitleBarHighlight == m_navWindow->m_rootWindowForTitleBarHighlight );
			ColorF titleColor = GetItemColor( isTitleBarHighlight ? ImUiColor::TitleBgActive : ImUiColor::TitleBg );
			window->m_drawList.AddFilledRect( window->m_pos, Vector2( window->m_size.x, titleBarHeight ), titleColor, windowRounding, ImDrawCorner::Top );
		}
	}

	if ( !( flags & ImUiWindowFlags::NoTitleBar ) )
	{
		if ( !( flags & ImUiWindowFlags::NoCollapse ) )
		{
			// Collapse botton
			Point2 arrowMin = m_curWindow->m_pos + m_curStyle.m_framePadding;
			float fontSize = CalcTextSize( nullptr, 0 ).y;
			Rect arrowBoundingBox( arrowMin.x + 1.f,
								arrowMin.y + 1.f,
								arrowMin.x + fontSize - 1.f,
								arrowMin.y + fontSize - 1.f );

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
		size_t textCount = std::strlen( name );
		assert( textCount <= UINT_MAX );
		Vector2 textSize = CalcTextSize( name, static_cast<uint32>( textCount ) );

		float padLeft = m_curStyle.m_framePadding.x + m_curStyle.m_itemInnerSpacing.x + textSize.y;
		float padRight = m_curStyle.m_framePadding.x;

		Point2 textPosMin = window->m_pos;
		textPosMin.x += padLeft;
		Point2 textPosMax = window->m_pos + Vector2( window->m_size.x, titleBarHeight );
		textPosMax.x -= padRight;

		RenderClippedText( textPosMin, textPosMax, name, static_cast<uint32>( textCount ), Vector2( 0.f, 0.5f ) );
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

	SetCurrentWindow( m_currentWindowStack.empty() ? nullptr : m_currentWindowStack.back( ) );
}

bool ImUI::Button( const char* label, const Vector2& size )
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

	const Point2 pos = dc.m_cursorPos;
	size_t labelLength = strlen( label );
	assert( labelLength <= UINT_MAX );
	const Vector2 labelSize = CalcTextSize( label, static_cast<uint32>( labelLength ) );

	Rect boundingBox( pos.x,
					pos.y,
					pos.x + w,
					pos.y + labelSize.y + m_curStyle.m_framePadding.y * 2.f );

	ItemSize( Vector2( w, labelSize.y + m_curStyle.m_framePadding.y * 2.f ) );
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
	std::snprintf( valueBuf, std::extent_v<decltype(valueBuf)>, displayFormat, *v );
	size_t count = std::strlen( valueBuf );

	assert( count <= UINT_MAX );
	RenderClippedText( boundingBox.m_leftTop, boundingBox.m_rightBottom, valueBuf, static_cast<uint32>( count ), Vector2( 0.5f, 0.5f ) );

	return valueChanged;
}

bool ImUI::SliderInt( const char* label, int32* v, int32 min, int32 max, const char* displayFormat )
{
	if ( displayFormat == nullptr )
	{
		displayFormat = "%.0f";
	}

	float floatValue = static_cast<float>( *v );
	bool valueChanged = SliderFloat( label, &floatValue, static_cast<float>( min ), static_cast<float>( max ), displayFormat );
	*v = static_cast<int32>( floatValue );

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

	Vector2 arrowSize( ( flags & ImUiComboFlag::NoArrowButton ) ? 0.f : GetFrameHeight( ), 0.f );
	const size_t labelLen = strlen( label );
	assert( labelLen <= UINT_MAX );
	Vector2 labelSize = CalcTextSize( label, static_cast<uint32>( labelLen ) );
	const float width = CalcItemWidth( );

	Vector2 itemSize( width, labelSize.y + m_curStyle.m_framePadding.y * 2.0f );
		
	Rect frameBB( m_curWindow->m_dc.m_cursorPos.x,
				m_curWindow->m_dc.m_cursorPos.y,
				m_curWindow->m_dc.m_cursorPos.x + width,
				m_curWindow->m_dc.m_cursorPos.y + itemSize.y );

	Rect totalBB( m_curWindow->m_dc.m_cursorPos.x,
				m_curWindow->m_dc.m_cursorPos.y,
				frameBB.m_rightBottom.x + ( ( labelSize.x > 0.f ) ? m_curStyle.m_itemInnerSpacing.x + labelSize.x : 0 ),
				frameBB.m_rightBottom.y );

	ItemSize( itemSize );

	bool overed, held;
	bool pressed = ButtonBehavior( frameBB, id, overed, held );

	const ColorF frameColor = GetItemColor( overed ? ImUiColor::FrameBgMouseOver : ImUiColor::FrameBg );

	Vector2 frameWH = frameBB.GetWidthHeight( );

	if ( ( flags & ImUiComboFlag::NoPreview ) == false )
	{
		m_curWindow->m_drawList.AddFilledRect( frameBB.m_leftTop, frameWH - arrowSize, frameColor, m_curStyle.m_frameRounding, ImDrawCorner::Left );
	}

	if ( ( flags & ImUiComboFlag::NoArrowButton ) == false )
	{
		Point2 lt( frameBB.m_rightBottom.x - arrowSize.x, frameBB.m_leftTop.y );
		m_curWindow->m_drawList.AddFilledRect( lt, Vector2( arrowSize.x, frameWH.y ), GetItemColor( overed ? ImUiColor::ButtonMouseOver : ImUiColor::Button ), m_curStyle.m_frameRounding, ( width <= arrowSize.x ) ? ImDrawCorner::All : ImDrawCorner::Right );
		RenderArrow( lt + m_curStyle.m_framePadding, ImDir::Down );
	}

	if ( ( prevValue != nullptr ) && ( ( flags & ImUiComboFlag::NoPreview ) == false ) )
	{
		Rect valueBB( frameBB.m_leftTop, frameBB.m_rightBottom - arrowSize );

		size_t prevValueLength = strlen( prevValue );
		assert( prevValueLength <= UINT_MAX );
		RenderClippedText(
			frameBB.m_leftTop + m_curStyle.m_framePadding,
			valueBB.m_rightBottom,
			prevValue,
			static_cast<uint32>( prevValueLength ),
			Vector2( 0.f, 0.f ) );
	}

	if ( labelSize.x > 0.f )
	{
		RenderText(
			Point2( frameBB.m_rightBottom.x, frameBB.m_leftTop.y ) + m_curStyle.m_itemInnerSpacing,
			label,
			static_cast<uint32>( labelLen )
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

	bool backupNextWindowSizeConstraint = ( m_nextWindowData.m_constraintSizeCond > 0 );
	m_nextWindowData.m_constraintSizeCond = 0;

	if ( backupNextWindowSizeConstraint )
	{
		m_nextWindowData.m_contentSizeCond = backupNextWindowSizeConstraint;
		m_nextWindowData.m_constraintSizeRect.m_leftTop.x = std::max( m_nextWindowData.m_constraintSizeRect.m_leftTop.x, width );
	}
	else
	{
		if ( ( flags & ImUiComboFlag::HeightMask ) == false )
		{
			flags |= ImUiComboFlag::HeightRegular;
		}

		int32 popupMaxHeihtInItems = -1;

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
		Rect constraintSize( width, 0.f, FLT_MAX, CalcMaxPopupHeightFromItemCount( popupMaxHeihtInItems ) );
		SetNextWindowConstraintSize( constraintSize );
	}

	char name[16];
	std::snprintf( name, std::extent_v<decltype(name)>, "##Combo_%02zd", m_currentPopupStack.size( ) );

	if ( ImUiWindow* popupWindow = FindWindow( name ) )
	{
		if ( popupWindow->m_wasVisible )
		{
			Vector2 contentsSize = CalcContentsSize( popupWindow );
			Vector2 expectedSize = CalcAfterConstraintSize( popupWindow, CalcAutoFitSize( popupWindow, contentsSize ) );
			Point2 pos = FindBestWindowPosForPopup( Point2( frameBB.m_leftTop.x, frameBB.m_rightBottom.y ), expectedSize, popupWindow->m_autoPosLastDirection, frameBB, ImUiPopupPositionPolicy::ComboBox );
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

bool ImUI::Combo( const char* label, int32* currentItem, const char* const items[], int32 itemsCount, int32 heightInItem )
{
	bool valueChanged = Combo( label, currentItem, ItemsArrayGetter, (void*)( items ), itemsCount, heightInItem );
	return valueChanged;
}

bool ImUI::Combo( const char* label, int32* currentItem, bool( *itemsGettter )( void* data, int32 idx, const char **outText ), void* data, int32 itemCount, int32 heightInItem )
{
	const char* prevText = nullptr;
	if ( *currentItem >= 0 && *currentItem < itemCount )
	{
		itemsGettter( data, *currentItem, &prevText );
	}

	if ( heightInItem != -1 && ( m_nextWindowData.m_constraintSizeCond == false ) )
	{
		Rect constraintSize( 0.f, 0.f, FLT_MAX, CalcMaxPopupHeightFromItemCount( heightInItem ) );
		SetNextWindowConstraintSize( constraintSize );
	}

	if ( !BeginCombo( label, prevText ) )
	{
		return false;
	}

	bool valueChanged = false;
	for ( int32 i = 0; i < itemCount; ++i )
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

bool ImUI::Selectable( const char* label, bool selected, ImUiSelectableFlags::Type /*flags*/, const Vector2& sizeArg )
{
	assert( m_curWindow != nullptr );
	if ( m_curWindow->m_skipItem )
	{
		return false;
	}

	ImGUID id = m_curWindow->GetID( label );
	size_t labelLength = strlen( label );
	assert( labelLength <= UINT_MAX );
	Vector2 labelSize = CalcTextSize( label, static_cast<uint32>( labelLength ) );
	Vector2 size( sizeArg.x != 0.f ? sizeArg.x : labelSize.x, sizeArg.y != 0.f ? sizeArg.y : labelSize.y );
	Point2 pos = m_curWindow->m_dc.m_cursorPos;
	pos.y += m_curWindow->m_dc.m_currentLineTextBaseOffset;
	ItemSize( size );

	const Vector2& windowPadding = m_curWindow->m_windowPadding;
	float maxX = GetWindowContextRegionMax().x;
	float drawWidth = std::max( labelSize.x, m_curWindow->m_pos.x + maxX - windowPadding.x - m_curWindow->m_dc.m_cursorPos.x );
	Vector2 drawSize( sizeArg.x != 0 ? sizeArg.x : drawWidth, sizeArg.y != 0 ? sizeArg.y : size.y );
	Rect bbWithSpacing( pos.x, pos.y, pos.x + drawSize.x, pos.y + drawSize.y );

	float spacing_L = static_cast<float>(static_cast<int32>(m_curStyle.m_itemSpacing.x * 0.5f));
	float spacing_T = static_cast<float>(static_cast<int32>(m_curStyle.m_itemSpacing.y * 0.5f));
	float spacing_R = m_curStyle.m_itemSpacing.x - spacing_L;
	float spacing_B = m_curStyle.m_itemSpacing.y - spacing_T;

	bbWithSpacing.m_leftTop.x -= spacing_L;
	bbWithSpacing.m_leftTop.y -= spacing_T;
	bbWithSpacing.m_rightBottom.x += spacing_R;
	bbWithSpacing.m_rightBottom.y += spacing_B;

	bool mouseOvered = false;
	bool mouseHeld = false;
	bool mousePressed = ButtonBehavior( bbWithSpacing, id, mouseOvered, mouseHeld );

	if ( mouseOvered || selected )
	{
		ColorF color = GetItemColor( ( mouseHeld && mouseOvered ) ? ImUiColor::HeaderActive : mouseOvered ? ImUiColor::HeaderMouseOver : ImUiColor::Header );
		RenderFrame( bbWithSpacing.m_leftTop, bbWithSpacing.GetWidthHeight(), color );
	}

	RenderClippedText( pos, bbWithSpacing.m_rightBottom, label, static_cast<uint32>( labelLength ), Vector2( 0.f, 0.f ) );

	if ( mousePressed && ( m_curWindow->m_flag & ImUiWindowFlags::Popup ) )
	{
		CloseCurrentPopup( );
	}

	return mousePressed;
}

void ImUI::TextUnformatted( const char* text, uint32 count )
{
	assert( m_curWindow != nullptr );
	if ( m_curWindow->m_skipItem )
	{
		return;
	}

	ImUiDrawContext& dc = m_curWindow->m_dc;

	const Point2 textPos( dc.m_cursorPos.x, dc.m_cursorPos.y + dc.m_currentLineTextBaseOffset );

	const Vector2 textSize = CalcTextSize( text, count );
	ItemSize( textSize );

	RenderClippedText( textPos, textPos + textSize, text, count, Vector2( 0.f, 0.f ) );
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
}

ImDrawData ImUI::Render( )
{
	ImDrawData drawData;
	drawData.m_totalIndexCount = 0;
	drawData.m_totalVertexCount = 0;

	for ( auto& window : m_windows )
	{
		if ( window->m_visible && ( window->m_hiddenFrames == 0 ) )
		{
			if ( window->m_drawList.m_cmdBuffer.empty( ) )
			{
				continue;
			}

			drawData.m_drawLists.push_back( &window->m_drawList );
			assert( ( drawData.m_totalVertexCount + window->m_drawList.m_vertices.size( ) ) <= UINT_MAX );
			drawData.m_totalVertexCount += static_cast<uint32>( window->m_drawList.m_vertices.size( ) );
			assert( ( drawData.m_totalIndexCount + window->m_drawList.m_indices.size( ) ) <= UINT_MAX );
			drawData.m_totalIndexCount += static_cast<uint32>( window->m_drawList.m_indices.size( ) );
		}
	}

	return drawData;
}

Vector2 ImUI::CalcTextSize( const char* text, uint32 count ) const
{
	assert( m_curTextAltas != -1 );

	const CTextAtlas& font = m_textAtlas[m_curTextAltas];

	if ( count == 0 )
	{
		return Vector2( 0, font.m_fontHeight );
	}

	return font.CalcTextSize( text, count );
}

Vector2 ImUI::GetWindowContextRegionMax( )
{
	assert( m_curWindow != nullptr );

	return  m_curWindow->m_contentsRegionRect.m_rightBottom;
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
		return m_windows[found->second].get();
	}

	return nullptr;
}

ImUiWindow* ImUI::FindMouseOverWindow( )
{
	for ( int32 i = static_cast<int32>( m_windows.size() ) - 1; i >= 0; --i )
	{
		ImUiWindow* window = m_windows[i].get();

		if ( window->m_visible == false )
		{
			continue;
		}

		Rect boundingBox( window->m_pos.x,
						window->m_pos.y, 
						window->m_pos.x + window->m_size.x,
						window->m_pos.y + window->m_size.y );

		if ( Contain( boundingBox, m_io.m_mousePos ) )
		{
			return window;
		}
	}

	return nullptr;
}

ImUiWindow* ImUI::CreateImUiWindow( const char* name, const Vector2& size )
{
	assert( m_windows.size( ) <= INT_MAX );
	int32 id = static_cast<int32>( m_windows.size( ) );
	m_windowLUT.emplace( name, id );
	m_windows.emplace_back( std::make_unique<ImUiWindow>( *this ) );
	
	ImUiWindow* window = m_windows.back( ).get();
	window->m_id = id;

	if ( ImUiWindowSetting* settings = FindWindowSettings( name ) )
	{
		window->m_pos = settings->m_pos;
		window->m_size = settings->m_size;
		window->m_collapsed = settings->m_collapsed;
	}
	else
	{
		window->m_pos = Point2( 60, 60 );
		window->m_size = size;
	}
	
	window->m_sizeNonCollapsed = window->m_size;
	window->m_idStack.push_back( Crc32Hash( name ) );

	window->m_moveID = window->GetID( "MOVE" );

	return window;
}

Vector2 ImUI::CalcItemSize( const Vector2& size )
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

	w = static_cast<float>( static_cast<int32>( w ) );
	return w;
}

float ImUI::CalcMaxPopupHeightFromItemCount( int32 itemCount )
{
	if ( itemCount <= 0 )
	{
		return FLT_MAX;
	}

	return ( GetFontHeight( ) + m_curStyle.m_itemSpacing.y ) * itemCount - m_curStyle.m_itemSpacing.y + ( m_curStyle.m_windowPadding.y * 2.f );
}

bool ImUI::ButtonEX( const char* label, const Vector2& /*size*/ )
{
	assert( m_curWindow != nullptr );
	if ( m_curWindow->m_skipItem )
	{
		return false;
	}

	ImUiDrawContext& dc = m_curWindow->m_dc;

	const Point2 pos = dc.m_cursorPos;
	const size_t strCount = strlen( label );
	assert( strCount <= UINT_MAX );
	const Vector2 labelSize = CalcTextSize( label, static_cast<uint32>( strCount ) );

	const Vector2& buttonSize = CalcItemSize( Vector2( labelSize.x + m_curStyle.m_framePadding.x * 2.f, labelSize.y + m_curStyle.m_framePadding.y * 2.f ) );

	const Rect boundingBox( pos.x,
							pos.y,
							pos.x + buttonSize.x,
							pos.y + buttonSize.y );

	ImGUID id = m_curWindow->GetID( label );

	bool mouseOvered = false;
	bool mouseHeld = false;
	bool mousePreesed = ButtonBehavior( boundingBox, id, mouseOvered, mouseHeld );
	ItemSize( buttonSize );

	// 그리기

	// 임시
	const ColorF& color = GetItemColor( ( mouseOvered && mouseHeld ) ? ImUiColor::ButtonPress : ( mouseOvered ? ImUiColor::ButtonMouseOver : ImUiColor::Button ) );
	m_curWindow->m_drawList.AddFilledRect( pos, buttonSize, color );
	RenderClippedText( pos + m_curStyle.m_framePadding, boundingBox.m_rightBottom - m_curStyle.m_framePadding, label, static_cast<uint32>( strCount ), Vector2( 0.f, 0.f ) );

	return mousePreesed;
}

bool ImUI::ButtonBehavior( const Rect& boundingBox, ImGUID id, bool& mouseOvered, bool& mouseHeld )
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

bool ImUI::SliderBehavior( const Rect& boundingbox, ImGUID id, float* v, float min, float max )
{
	assert( m_curWindow != nullptr );

	const ColorF frameColor = GetItemColor( m_activeItemID == id ? ImUiColor::FrameBgActive : m_mouseOveredID == id ? ImUiColor::FrameBgMouseOver : ImUiColor::FrameBg );

	// 그리기

	// 임시
	const Point2& framePos = boundingbox.m_leftTop;
	const Vector2& frameSize = boundingbox.GetWidthHeight();

	m_curWindow->m_drawList.AddFilledRect( framePos, frameSize, frameColor );

	constexpr float grabPadding = 2.0f;
	const float sliderSize = frameSize.x - grabPadding * 2.0f;
	float grabSize = std::min( m_curStyle.m_grabMinSize, sliderSize );

	const float sliderUsablePosMin = boundingbox.m_leftTop.x + grabPadding + grabSize * 0.5f;
	const float sliderUsablePosMax = boundingbox.m_rightBottom.x - grabPadding - grabSize * 0.5f;

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
			clicked = ( sliderSize > 0.f ) ? clamp( ( mousePos - boundingbox.m_leftTop.x ) / sliderSize, 0.f, 1.0f ) : 0.f;
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

	Point2 pos( grabPos - grabSize * 0.5f, framePos.y + grabPadding );
	Vector2 size( grabSize, frameSize.y - grabPadding * 2.f );

	m_curWindow->m_drawList.AddFilledRect( pos, size, GetItemColor( m_activeItemID == id ? ImUiColor::SliderGrab : ImUiColor::SliderGrabActive ) );

	return valueChanged;
}

float ImUI::SliderBehaviorCalcRatioFromValue( float v, float min, float max, float /*linearZeroPos*/ )
{
	if ( min == max )
	{
		return 0.f;
	}

	float clamped = clamp( v, min, max );

	return ( clamped - min ) / ( max - min );
}

void ImUI::RenderFrame( const Point2& pos, const Vector2& size, const ColorF& color, float rounding )
{
	assert( m_curWindow != nullptr );
	m_curWindow->m_drawList.AddFilledRect( pos, size, color, rounding, ImDrawCorner::All );
}

void ImUI::RenderText( const Point2& pos, const char* text, int32 count )
{
	if ( count == 0 )
	{
		return;
	}

	assert( m_curWindow != nullptr );
	assert( m_curTextAltas != -1 );

	const Vector2 textSize = CalcTextSize( text, count );

	m_curWindow->m_drawList.AddText( m_textAtlas[m_curTextAltas], pos, GetItemColor( ImUiColor::Text ), text, count );
}

void ImUI::RenderClippedText( const Point2& posMin, const Point2& posMax, const char* text, uint32 count, const Vector2& align )
{
	if ( count == 0 )
	{
		return;
	}

	assert( m_curWindow != nullptr );
	assert( m_curTextAltas != -1 );
	
	const Vector2 textSize = CalcTextSize( text, count );

	Point2 pos = posMin;
	if ( align.x > 0.f )
	{
		pos.x = floor( std::max( pos.x, pos.x + ( posMax.x - posMin.x - textSize.x ) * align.x ) );
	}
	if ( align.y > 0.f )
	{
		pos.y = floor( std::max( pos.y, pos.y + ( posMax.y - posMin.y - textSize.y ) * align.y ) );
	}

	m_curWindow->m_drawList.AddText( m_textAtlas[m_curTextAltas], pos, GetItemColor( ImUiColor::Text ), text, count );
}

void ImUI::RenderArrow( const Point2&pos, ImDir::Type dir, float scale)
{
	const float h = CalcTextSize( nullptr, 0 ).y;
	float r = h * 0.4f * scale;
	Point2 center = pos + Vector2( h * 0.5f, h * 0.5f * scale );

	Vector2 v[3];

	switch ( dir )
	{
	case ImDir::Up:
	case ImDir::Down:
		if ( dir == ImDir::Up )
		{
			r = -r;
		}

		center.y -= r * 0.25f;
		v[0] = Vector2( 0 , 1 ) * r;
		v[1] = Vector2( 0.866f, -0.5f ) * r;
		v[2] = Vector2( -0.866f, -0.5f ) * r;
		break;
	case ImDir::Left:
	case ImDir::Right:
		if ( dir == ImDir::Left )
		{
			r = -r;
		}

		center.x -= r * 0.25f;
		v[0] = Vector2( 1, 0 ) * r;
		v[1] = Vector2( -0.5f, -0.866f ) * r;
		v[2] = Vector2( -0.5f, 0.866f ) * r;
		break;
	default:
		assert( 0 );
		break;
	}

	m_curWindow->m_drawList.AddTriangleFilled( center + v[0], center + v[1], center + v[2], GetItemColor( ImUiColor::Text ) );
}

void ImUI::PushClipRect( const Rect& clipRect )
{
	assert( m_curWindow != nullptr );
	m_curWindow->m_drawList.PushClipRect( clipRect );
}

void ImUI::ItemSize( const Vector2& size, float textOffsetY )
{
	assert( m_curWindow != nullptr );

	ImUiDrawContext& dc = m_curWindow->m_dc;
	const float lineHeight = std::max( dc.m_currentLineHeight, size.y );
	const float lineTextBaseOffset = std::max( dc.m_currentLineTextBaseOffset, textOffsetY );

	dc.m_prevCursorPos = Point2( dc.m_cursorPos.x + size.x, dc.m_cursorPos.y );
	dc.m_cursorPos = Point2( m_curWindow->m_pos.x + dc.m_indentX, dc.m_cursorPos.y + lineHeight + m_curStyle.m_itemSpacing.y );
	dc.m_cursorMaxPos.x = std::max( dc.m_cursorMaxPos.x, dc.m_prevCursorPos.x );
	dc.m_cursorMaxPos.y = std::max( dc.m_cursorMaxPos.y, dc.m_cursorPos.y - m_curStyle.m_itemSpacing.y );

	dc.m_prevLineHeight = lineHeight;
	dc.m_prevLineTextBaseOffset = lineTextBaseOffset;
	dc.m_currentLineHeight = 0.f;
	dc.m_currentLineTextBaseOffset = 0.f;
}

bool ImUI::MouseOveredItem( const Rect& boundingbox, ImGUID id )
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

ColorF ImUI::GetItemColor( ImUiColor::Type colorId, float alphaMul )
{
	ColorF color = m_curStyle.m_colors[colorId];
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

void ImUI::SetCurrentWindow( ImUiWindow* window )
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
		Point2 pos = m_io.m_mousePos - m_activeIDClickOffset;
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
		size_t iFileSize = fileSize;

		char* buf = new char[iFileSize];

		cfgfile.read( buf, iFileSize );

		LoadSettingFromMemory( buf, iFileSize );

		delete[] buf;
	}
}

void ImUI::LoadSettingFromMemory( const char* buf, size_t count )
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

ImUiSettingHandler* ImUI::FindSettingHandler( const char* typeName )
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
	size_t curStackSize = m_currentPopupStack.size( );

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

	size_t n = 0;
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
			for ( size_t m = n; m < m_openPopupStack.size( ) && ( hasFocus == false ); ++m )
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

void ImUI::ClosePopupToLevel( size_t remaining )
{
	ImUiWindow* focusWindow = ( remaining > 0 ) ? m_openPopupStack[remaining - 1].m_window : m_openPopupStack[0].m_parentWindow;
	FocusWindow( focusWindow );
	m_openPopupStack.resize( remaining );
}

void ImUI::CloseCurrentPopup( )
{
	size_t popupIdx = m_currentPopupStack.size( ) - 1;
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

Vector2 ImUI::CalcAfterConstraintSize( ImUiWindow* window, const Vector2& size )
{
	Vector2 newSize = size;

	if ( m_nextWindowData.m_constraintSizeCond != 0 )
	{
		const Rect& cr = m_nextWindowData.m_constraintSizeRect;

		newSize.x = (cr.m_leftTop.x >= 0 && cr.m_rightBottom.x >= 0) ? clamp( newSize.x, cr.m_leftTop.x, cr.m_rightBottom.x ) : window->m_sizeNonCollapsed.x;
		newSize.y = (cr.m_leftTop.y >= 0 && cr.m_rightBottom.y >= 0) ? clamp( newSize.y, cr.m_leftTop.y, cr.m_rightBottom.y ) : window->m_sizeNonCollapsed.y;
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
		newSize.y = std::max( newSize.y, window->GetTitleBarHeight( ) + std::max( 0.f, m_curStyle.m_windowRounding - 1.f ) );
	}

	return newSize;
}

Vector2 ImUI::CalcAutoFitSize( ImUiWindow* window, const Vector2& contentsSize )
{
	ImUiWindowFlags::Type flags = window->m_flag;
	Vector2 autoFitSize;

	if ( ( flags & ImUiWindowFlags::Tooltip ) != 0 )
	{
		autoFitSize = contentsSize;
	}
	else
	{
		autoFitSize = clamp( contentsSize, m_curStyle.m_windowMinSize, Max( m_curStyle.m_windowMinSize, m_io.m_displaySize - m_curStyle.m_displaySafeAreaPadding ) );
		Vector2 autoFitSizeAfterConstraint = CalcAfterConstraintSize( window, autoFitSize );
	}

	return autoFitSize;
}

Vector2 ImUI::FindBestWindowPosForPopup( const Point2& refPos, const Vector2& size, ImDir::Type& lastDir, const Rect& avoid, ImUiPopupPositionPolicy::Type policy )
{
	const Vector2& safePadding = m_curStyle.m_displaySafeAreaPadding;
	Rect outer = m_clientRect;
	Vector2 widthHeight = m_clientRect.GetWidthHeight( );
	Expand( outer, 
		Vector2( ( size.x - ( widthHeight.x ) > safePadding.x * 2 ) ? -safePadding.x : 0.f,
		( size.y - ( widthHeight.y ) > safePadding.y * 2 ) ? -safePadding.y : 0.f)
	);
	
	if ( policy == ImUiPopupPositionPolicy::ComboBox )
	{
		ImDir::Type dirOrder[ImDir::Count] = { ImDir::Down, ImDir::Right, ImDir::Left, ImDir::Up };
		for ( int32 i = ( lastDir != ImDir::None ) ? -1 : 0; i < ImDir::Count; ++i )
		{
			ImDir::Type dir = ( i == -1 ) ? lastDir : dirOrder[i];
			if ( i != -1 && dir == lastDir )
			{
				continue;
			}

			Point2 pos;
			switch ( dir )
			{
			case ImDir::Down:
				pos = Point2( avoid.m_leftTop.x, avoid.m_rightBottom.y );
				break;
			case ImDir::Right:
				pos = Point2( avoid.m_leftTop.x, avoid.m_leftTop.y - size.y );
				break;
			case ImDir::Left:
				pos = Point2( avoid.m_rightBottom.x - size.x, avoid.m_rightBottom.y );
				break;
			case ImDir::Up:
				pos = Point2( avoid.m_rightBottom.x - size.x, avoid.m_leftTop.y - size.y );
				break;
			}

			Rect r( pos.x, pos.y, pos.x + size.x, pos.y + size.y );
			if ( Contain( outer, r ) == false )
			{
				continue;
			}

			lastDir = dir;
			return pos;
		}
	}

	Point2 clampedBasePos = clamp( refPos, outer.m_leftTop, outer.m_rightBottom );

	ImDir::Type dirOrder[ImDir::Count] = { ImDir::Right, ImDir::Down, ImDir::Up, ImDir::Left };
	for ( int32 i = ( lastDir != ImDir::None ) ? -1 : 0; i < ImDir::Count; ++i )
	{
		ImDir::Type dir = ( i == -1 ) ? lastDir : dirOrder[i];
		if ( i != -1 && dir == lastDir )
		{
			continue;
		}

		float availW = ( dir == ImDir::Left ? avoid.m_leftTop.x : avoid.m_rightBottom.x ) - ( dir == ImDir::Right ? avoid.m_rightBottom.x : outer.m_leftTop.x );
		float availH = ( dir == ImDir::Up ? avoid.m_leftTop.y : avoid.m_rightBottom.y ) - ( dir == ImDir::Down ? avoid.m_rightBottom.y : outer.m_leftTop.y );
		if ( availW < size.x || availH < size.y )
		{
			continue;
		}

		Point2 pos;
		pos.x = ( dir == ImDir::Left ) ? avoid.m_leftTop.x - size.x : ( dir == ImDir::Right ) ? avoid.m_rightBottom.x : clampedBasePos.x;
		pos.y = ( dir == ImDir::Up ) ? avoid.m_leftTop.y - size.y : ( dir == ImDir::Down ) ? avoid.m_rightBottom.y : clampedBasePos.y;
		lastDir = dir;
		return pos;
	}

	lastDir = ImDir::None;
	Point2 pos = refPos;
	pos.x = std::max( std::min( pos.x + size.x, outer.m_rightBottom.x ) - size.x, outer.m_leftTop.x );
	pos.y = std::max( std::min( pos.y + size.y, outer.m_rightBottom.y ) - size.y, outer.m_leftTop.y );
	return pos;
}

void ImUI::SetNextWindowPos( const Point2& pos, ImUiCond::Type cond, const Point2& pivot )
{
	if ( Vector2() == Vector2() )
	{

	}

	m_nextWindowData.m_pos = pos;
	m_nextWindowData.m_posPivot = pivot;
	m_nextWindowData.m_posCond = cond ? cond : ImUiCond::Always;
}

void ImUI::SetNextWindowConstraintSize( const Rect& constraintSize, ImUiSizeCallBack callback, void* callbackData )
{
	m_nextWindowData.m_constraintSizeCond = ImUiCond::Always;
	m_nextWindowData.m_constraintSizeRect = constraintSize;
	m_nextWindowData.m_sizeCallback = callback;
	m_nextWindowData.m_sizeCallbackUserData = callbackData;
}
