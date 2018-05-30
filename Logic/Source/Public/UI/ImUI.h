#pragma once

#include "common.h"
#include "Math/CXMFloat.h"
#include "Render/Resource.h"
#include "UserInput/UserInput.h"

#include <map>
#include <stack>
#include <unordered_map>
#include <vector>
#include <windef.h>

class ImUI;
class CTextAtlas;

struct ImUiVertex
{
	CXMFLOAT2 m_pos;
	CXMFLOAT2 m_uv = { 1.1f, 1.1f };
	CXMFLOAT4 m_color;
};

namespace ImUiColor
{
	enum Type
	{
		Text,
		WindowBg,
		FrameBg,
		FrameBgMouseOver,
		FrameBgActive,
		TitleBg,
		TitleBgActive,
		TitleBgCollapsed,
		SliderGrab,
		SliderGrabActive,
		Button,
		ButtonMouseOver,
		ButtonPress,
		Count,
	};
}

namespace ImDrawCorner
{
	enum Type
	{
		TopLeft = 1 << 0,
		TopRight = 1 << 1,
		BottomLeft = 1 << 2,
		BottomRight = 1 << 3,
		Top = TopLeft | TopRight,
		Bottom = BottomLeft | BottomRight,
		Left = TopLeft | BottomLeft,
		Right = TopRight | BottomRight,
		All = 0xF
	};
}

namespace ImDir
{
	enum Type
	{
		None = -1,
		Left,
		Right,
		Up,
		Down
	};
}

struct ImUiStyle
{
	float m_alpha = 1.f;
	float m_windowRounding = 7.f;
	float m_grabMinSize = 10.f;
	CXMFLOAT2 m_windowPadding = { 8, 8 };
	CXMFLOAT2 m_framePadding = { 4, 3 };
	CXMFLOAT2 m_itemSpacing = { 8, 4 };
	CXMFLOAT2 m_itemInnerSpacing = { 4, 4 };
	CXMFLOAT4 m_colors[ImUiColor::Count];
};

struct ImDrawCmd
{
	UINT m_indicesCount;
	RECT m_clipRect;
	RE_HANDLE m_textAtlas = RE_HANDLE_TYPE::INVALID_HANDLE;
};

struct ImUiDrawContext
{
	CXMFLOAT2 m_prevCursorPos;
	CXMFLOAT2 m_cursorPos;
	CXMFLOAT2 m_cursorStartPos;

	float m_currentLineHeight;
	float m_currentLineTextBaseOffset;
	float m_prevLineHeight;
	float m_prevLineTextBaseOffset;
	float m_indentX;
};

struct ImDrawList
{
	void Clear( );
	void AddDrawCmd( );
	void PushClipRect( RECT clipRect );
	void PushTextAtlas( RE_HANDLE texHandle );
	void UpdateClipRect( );
	void UpdateTextAtlas( );

	void AddFilledRect( const CXMFLOAT2& pos, const CXMFLOAT2& size, const CXMFLOAT4& color, float rounding = 0.f, int reoundingFlag = ImDrawCorner::All );
	void AddConvexPolyFilled( const CXMFLOAT2* points, const int count, const CXMFLOAT4& color );
	void AddText( CTextAtlas& font, const CXMFLOAT2& pos, const CXMFLOAT4& color, const char* text, int count );
	void AddTriangleFilled( const CXMFLOAT2& v0, const CXMFLOAT2& v1, const CXMFLOAT2& v2, const CXMFLOAT4& color );

	void BufferReserve( int vertexCount, int indexCount );
	void DrawRect( const CXMFLOAT2& pos, const CXMFLOAT2& size, const CXMFLOAT4& color );

	void PathClear( ) { m_path.clear( ); }
	void PathLineTo( const CXMFLOAT2& pos );
	void PathFillConvex( const CXMFLOAT4& color );
	void PathArcToFast( const CXMFLOAT2& centre, float radius, int minOf12, int maxOf12 );
	void PathRect( const CXMFLOAT2& pos, const CXMFLOAT2& size, float rounding = 0.f, int reoundingFlag = ImDrawCorner::All );

	std::vector<CXMFLOAT2> m_path;
	std::vector<ImDrawCmd> m_cmdBuffer;
	std::stack<RECT> m_clipRect;
	std::stack<RE_HANDLE> m_textAtlas;

	UINT m_curIndex = 0;
	std::vector<DWORD> m_indices;
	DWORD* m_pIndexWriter = nullptr;
	std::vector<ImUiVertex> m_vertices;
	ImUiVertex* m_pVertexWriter = nullptr;

	const ImUI* m_imUi = nullptr;
};

struct ImDrawData
{
	std::vector<ImDrawList*> m_drawLists;
	int m_totalVertexCount;
	int m_totalIndexCount;
};

using ImGUID = unsigned int;

struct ImUiWindow
{
	ImUiWindow( const ImUI& imUi ) : m_imUi( imUi ) 
	{
		m_drawList.m_imUi = &m_imUi;
	}

	CXMFLOAT2 m_pos = { 0.f, 0.f };
	CXMFLOAT2 m_size = { 0.f, 0.f };
	CXMFLOAT2 m_sizeNonCollapsed = { 0.f, 0.f };
	CXMFLOAT2 m_windowPadding = { 0.f, 0.f };
	bool m_visible = false;
	bool m_collapsed = false;
	bool m_collapseToggleReserved = false;
	bool m_skipItem = false;
	ImUiDrawContext m_dc;
	float m_itemWidthDefault;
	ImDrawList m_drawList;
	std::vector<ImGUID> m_idStack;

	ImGUID GetID( const char* str );

	float GetTitleBarHeight( ) const;

	const ImUI& m_imUi;
	ImGUID m_moveID;
};

struct ImGuiIO
{
	CXMFLOAT2 m_mousePos = { 0.f, 0.f };
	bool m_mouseDown[5];
	float m_mouseDownDuration[5];
	float m_mouseDownDurationPrev[5];
	float m_keyRepeatDelay = 0.25f;

	CXMFLOAT2 m_prevMousePos = { 0.f, 0.f };
};

struct FontUV
{
	CXMFLOAT2 m_u;
	CXMFLOAT2 m_v;
	float m_size;
};

class CTextAtlas
{
public:
	CXMFLOAT2 CalcTextSize( const char* text, int count ) const;
	FontUV* FindGlyph( char ch );

	RE_HANDLE m_texture = RE_HANDLE_TYPE::INVALID_HANDLE;

	std::map<char, FontUV> m_fontInfo;
	float m_fontHeight = 0.f;
	float m_fontSpacing = 0.f;
	CXMFLOAT2 m_displayOffset = { 0.f, 0.f };
};

struct ImUiWindowSetting
{
	std::string m_name;
	ImGUID m_id;
	CXMFLOAT2 m_pos;
	CXMFLOAT2 m_size;
	bool m_collapsed;
};

struct ImUiSettingHandler
{
	void* ( *ReadOpenFn )( ImUI* ui, ImUiSettingHandler* handler, const char* name );
	void ( *ReadLineFn )( ImUI* ui, ImUiSettingHandler* handler, void* entry, const char* line );
};

class ImUI
{
public:
	bool Initialize( );
	void SetDefaultText( const std::string& fontName, const CTextAtlas& textAtlas );
	void StyleColorDark( );
	
	void BeginFrame( RECT clientRect );
	void EndFrame( );
	bool Window( const char* name );
	bool Button( const char* label, const CXMFLOAT2& size = CXMFLOAT2( 0.f, 0.f ) );
	bool SliderFloat( const char* label, float* v, float min, float max, const char* displayFormat = nullptr );
	bool SliderInt( const char* label, int* v, int min, int max, const char* displayFormat = nullptr );
	
	template <typename... T>
	void Text( const char* format, T... args );

	void TextUnformatted( const char* text, int count );

	void SameLine( float xPos = 0.f, float wSpacing = -1.0f );

	bool HandleUserInput( const UserInput& input );

	ImDrawData Render( );

	const ImUiStyle& GetCurStyle( ) const { return m_curStyle; }
	const RECT& GetClientRect( ) const { return m_clientRect; }

	CXMFLOAT2 CalcTextSize( const char* text, int count ) const;

	ImUiWindowSetting* FindWindowSettings( const char* name );
	ImUiWindowSetting* AddWindowSettings( const char* name );

	CXMFLOAT2 m_circleVertex[12];
private:
	ImUiWindow* FindWindow( const char* name );
	ImUiWindow* FindMouseOverWindow( );
	ImUiWindow* CreateImUiWindow( const char* name, const CXMFLOAT2& size );
	CXMFLOAT2 CalcItemSize( const CXMFLOAT2& size );
	bool ButtonEX( const char* label, const CXMFLOAT2& size = CXMFLOAT2( 0.f, 0.f ) );
	bool ButtonBehavior( const RECT& boundingbox, ImGUID id, bool& mouseOvered, bool& mouseHeld );

	bool SliderBehavior( const RECT& boundingbox, ImGUID id, float* v, float min, float max );
	float SliderBehaviorCalcRatioFromValue( float v, float min, float max, float linearZeroPos );

	void RenderFrame( const CXMFLOAT2& pos, const CXMFLOAT2& size, const CXMFLOAT4& color, float rounding = 0.f );
	void RenderClippedText( const CXMFLOAT2& posMin, const CXMFLOAT2& posMax, const char* text, int count, const CXMFLOAT2& align );
	void RenderArrow( const CXMFLOAT2& pos, ImDir::Type dir, float scale );
	void PushClipRect( const RECT& clipRect );

	void ItemSize( const CXMFLOAT2& size, float textOffsetY = 0.f );
	bool MouseOveredItem( const RECT& boundingbox, ImGUID id );

	CXMFLOAT4 GetItemColor( ImUiColor::Type colorId, float alphaMul = 1.f );

	void SetActiveItemID( ImGUID id );
	void SetMouseOveredID( ImGUID id );

	void FocusWindow( ImUiWindow* window );
	void UpdateMovingWindow( );

	void LoadSettingFromDisk( const char* fileName );
	void LoadSettingFromMemory( const char* buf, int count );
	ImUiSettingHandler* FindSettingHandler( const char* typeName );

	RECT m_clientRect = { 0, 0, 0, 0 };

	std::map<std::string, int> m_windowLUT;
	std::vector<ImUiWindow> m_windows;
	ImUiWindow* m_curWindow = nullptr;
	ImUiWindow* m_navWindow = nullptr;
	ImUiWindow* m_mouseOveredWindow = nullptr;
	ImUiWindow* m_moveingWindow = nullptr;
	ImGUID m_activeItemID;
	ImGUID m_mouseOveredID;

	ImUiStyle m_curStyle;

	ImGuiIO m_io;

	CXMFLOAT2 m_activeIDClickOffset = { -1, -1 };

	std::map<std::string, int> m_textAtlasLUT;
	std::vector<CTextAtlas> m_textAtlas;
	int m_curTextAltas = -1;

	std::unordered_map<std::string, ImUiWindowSetting> m_settingWindows;
	std::unordered_map<std::string, ImUiSettingHandler> m_settingHandler;
};

template <typename... T>
inline void ImUI::Text( const char* format, T... args )
{
	assert( m_curWindow != nullptr );
	if ( m_curWindow->m_skipItem )
	{
		return;
	}

	char temp[1024] = {};
	sprintf_s( temp, format, args... );
	int count = std::strlen( temp );
	TextUnformatted( temp, count );
}
