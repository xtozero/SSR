#pragma once

#include "common.h"
#include "Math/CXMFloat.h"
#include "Math/Util.h"
#include "Render/Resource.h"
#include "UserInput/UserInput.h"

#include <map>
#include <memory>
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
		Header,
		HeaderMouseOver,
		HeaderActive,
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
		Down,
		Count
	};
}

namespace ImUiComboFlag
{
	using Type = int;
	enum 
	{
		PopupAlignLeft = 1 << 0,
		HeightSmall = 1 << 1,
		HeightRegular = 1 << 2,
		HeightLarge = 1 << 3,
		HeightLargest = 1 << 4,
		NoArrowButton = 1 << 5,
		NoPreview = 1 << 6,
		HeightMask = HeightSmall | HeightRegular | HeightLarge | HeightLargest
	};
}

namespace ImUiSelectableFlags
{
	enum Type
	{

	};
}

namespace ImUiWindowFlags
{
	using Type = int;
	enum
	{
		None = 0,
		NoTitleBar = 1 << 0,
		NoResize = 1 << 1,
		NoCollapse = 1 << 5,
		AlwaysAutoResize = 1 << 6,
		NoSavedSettings = 1 << 8,
		ChildWindow = 1 << 24,
		Tooltip = 1 << 25,
		Popup = 1 << 26,
		Modal = 1 << 27,
	};
}

namespace ImUiPopupPositionPolicy
{
	enum Type
	{
		Default,
		ComboBox
	};
}

struct ImUiStyle
{
	float m_alpha = 1.f;
	float m_windowRounding = 7.f;
	float m_childRounding = 0.f;
	float m_popupRounding = 0.f;
	float m_grabMinSize = 10.f;
	float m_frameRounding = 0.f;
	CXMFLOAT2 m_windowPadding = { 8, 8 };
	CXMFLOAT2 m_windowMinSize = { 32, 32 };
	CXMFLOAT2 m_framePadding = { 4, 3 };
	CXMFLOAT2 m_itemSpacing = { 8, 4 };
	CXMFLOAT2 m_itemInnerSpacing = { 4, 4 };
	CXMFLOAT2 m_displaySafeAreaPadding = { 4, 4 };
	CXMFLOAT4 m_colors[ImUiColor::Count];
};

struct ImDrawCmd
{
	UINT m_indicesCount;
	Rect m_clipRect;
	RE_HANDLE m_textAtlas = RE_HANDLE_TYPE::INVALID_HANDLE;
};

struct ImUiDrawContext
{
	CXMFLOAT2 m_prevCursorPos;
	CXMFLOAT2 m_cursorPos;
	CXMFLOAT2 m_cursorStartPos;
	CXMFLOAT2 m_cursorMaxPos;

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
	void PushClipRect( const Rect& clipRect );
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
	std::stack<Rect> m_clipRect;
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

	int m_id = -1;
	ImUiWindowFlags::Type m_flag = static_cast<ImUiWindowFlags::Type>( 0 );
	CXMFLOAT2 m_pos = { 0.f, 0.f };
	CXMFLOAT2 m_size = { 0.f, 0.f };
	CXMFLOAT2 m_sizeNonCollapsed = { 0.f, 0.f };
	CXMFLOAT2 m_contentsSize = { 0.f, 0.f };
	CXMFLOAT2 m_explicitContentsSize = { 0.f, 0.f };
	Rect m_contentsRegionRect;
	CXMFLOAT2 m_windowPadding = { 0.f, 0.f };
	float m_windowRounding = 0.f;
	bool m_visible = false;
	bool m_wasVisible = false;
	bool m_collapsed = false;
	bool m_collapseToggleReserved = false;
	bool m_skipItem = false;
	ImGUID m_popupID = 0;
	ImDir::Type m_autoPosLastDirection = ImDir::None;
	int m_hiddenFrames = 0;
	ImUiDrawContext m_dc;
	int m_lastActiveFrame = -1;
	float m_itemWidthDefault = 0.f;
	ImDrawList m_drawList;
	ImUiWindow* m_parentWindow = nullptr;
	ImUiWindow* m_rootWindow = nullptr;
	ImUiWindow* m_rootWindowForTitleBarHighlight = nullptr;
	std::vector<ImGUID> m_idStack;

	ImGUID GetID( const char* str );

	float GetTitleBarHeight( ) const;
	Rect GetTitleBarRect( ) const;

	const ImUI& m_imUi;
	ImGUID m_moveID;

	void SetPos( const CXMFLOAT2& pos );
};

struct ImGuiIO
{
	CXMFLOAT2 m_displaySize = { -1.f, -1.f };
	CXMFLOAT2 m_mousePos = { 0.f, 0.f };
	CXMFLOAT2 m_mouseClickedPos[5];
	float m_mouseClickedTime[5];
	bool m_mouseClicked[5];
	bool m_mouseDown[5];
	bool m_mouseDoubleClick[5];
	float m_mouseDownDuration[5];
	float m_mouseDownDurationPrev[5];
	float m_keyRepeatDelay = 0.25f;
	float m_mouseDoubleDownTime = 0.3f;
	float m_mouseDoubleDownMaxDist = 6.f;

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

struct ImUiSizeCallbackData
{
	void* m_userData;
	CXMFLOAT2 m_pos;
	CXMFLOAT2 m_currentSize;
	CXMFLOAT2 m_desiredSize;
};

namespace ImUiCond
{
	using Type = int;
	enum
	{
		None = 0,
		Always = 1 << 0,
		Once = 1 << 1,
		FirstUseEver = 1 << 2,
		Appering = 1 << 3
	};
}
using ImUiSizeCallBack = void( * )( ImUiSizeCallbackData* data );

struct ImUiNextWindowData
{
	ImUiCond::Type m_posCond = ImUiCond::None;
	ImUiCond::Type m_sizeCond = ImUiCond::None;
	ImUiCond::Type m_contentSizeCond = ImUiCond::None;
	ImUiCond::Type m_constraintSizeCond = ImUiCond::None;
	ImUiCond::Type m_focusCond = ImUiCond::None;
	ImUiCond::Type m_bgAlphaCond = ImUiCond::None;
	ImUiCond::Type m_collapsedCond = ImUiCond::None;
	CXMFLOAT2 m_pos = { 0.f, 0.f };
	CXMFLOAT2 m_posPivot = { 0.f, 0.f };
	CXMFLOAT2 m_size = { 0.f, 0.f };
	CXMFLOAT2 m_contentSize = { 0.f, 0.f };
	Rect m_constraintSizeRect = { 0.f, 0.f, 0.f, 0.f };
	ImUiSizeCallBack m_sizeCallback = nullptr;
	void* m_sizeCallbackUserData = nullptr;
	float m_bgAlpha = FLT_MAX;
	bool m_collapsed = false;

	void Clear( )
	{
		m_posCond = m_sizeCond = m_contentSizeCond = m_constraintSizeCond = m_focusCond = m_bgAlphaCond = m_collapsedCond = ImUiCond::None;
	}
};

class ImUI
{
public:
	bool Initialize( );
	void SetDefaultText( const std::string& fontName, const CTextAtlas& textAtlas );
	void StyleColorDark( );
	
	void BeginFrame( const Rect& clientRect, float elapsedTime, float totalTime );
	void EndFrame( );
	bool Window( const char* name, ImUiWindowFlags::Type flags = ImUiWindowFlags::None );
	void EndWindow( );
	bool Button( const char* label, const CXMFLOAT2& size = { 0.f, 0.f } );
	bool SliderFloat( const char* label, float* v, float min, float max, const char* displayFormat = nullptr );
	bool SliderInt( const char* label, int* v, int min, int max, const char* displayFormat = nullptr );
	
	bool BeginCombo( const char* label, const char* prevValue, ImUiComboFlag::Type flags = static_cast<ImUiComboFlag::Type>( 0 ) );
	void EndCombo( );
	bool Combo( const char* label, int* currentItem, const char* const items[], int itemsCount, int heightInItem = -1 );
	bool Combo( const char* label, int* currentItem, bool( *itemsGettter )( void* data, int idx, const char** outText ), void* data, int itemCount, int heightInItem = -1 );
	
	bool Selectable( const char* label, bool selected = false, ImUiSelectableFlags::Type flags = static_cast<ImUiSelectableFlags::Type>( 0 ), const CXMFLOAT2& sizeArg = { 0.f, 0.f } );

	template <typename... T>
	void Text( const char* format, T... args );

	void TextUnformatted( const char* text, int count );

	void SameLine( float xPos = 0.f, float wSpacing = -1.0f );

	bool HandleUserInput( const UserInput& input );

	ImDrawData Render( );

	const ImUiStyle& GetCurStyle( ) const { return m_curStyle; }
	const Rect& GetClientRect( ) const { return m_clientRect; }

	CXMFLOAT2 CalcTextSize( const char* text, int count ) const;
	CXMFLOAT2 GetWindowContextRegionMax( );
	float GetFontHeight( ) const;
	float GetFrameHeight( ) const;

	ImUiWindowSetting* FindWindowSettings( const char* name );
	ImUiWindowSetting* AddWindowSettings( const char* name );

	CXMFLOAT2 m_circleVertex[12];
private:
	ImUiWindow* FindWindow( const char* name );
	ImUiWindow* FindMouseOverWindow( );
	ImUiWindow* CreateImUiWindow( const char* name, const CXMFLOAT2& size );
	CXMFLOAT2 CalcItemSize( const CXMFLOAT2& size );
	float CalcItemWidth( );
	float CalcMaxPopupHeightFromItemCount( int itemCount );
	bool ButtonEX( const char* label, const CXMFLOAT2& size = { 0.f, 0.f } );
	bool ButtonBehavior( const Rect& boundingbox, ImGUID id, bool& mouseOvered, bool& mouseHeld );

	bool SliderBehavior( const Rect& boundingbox, ImGUID id, float* v, float min, float max );
	float SliderBehaviorCalcRatioFromValue( float v, float min, float max, float linearZeroPos );

	void RenderFrame( const CXMFLOAT2& pos, const CXMFLOAT2& size, const CXMFLOAT4& color, float rounding = 0.f );
	void RenderText( const CXMFLOAT2& pos, const char* text, int count );
	void RenderClippedText( const CXMFLOAT2& posMin, const CXMFLOAT2& posMax, const char* text, int count, const CXMFLOAT2& align );
	void RenderArrow( const CXMFLOAT2& pos, ImDir::Type dir, float scale = 1.f );
	void PushClipRect( const Rect& clipRect );

	void ItemSize( const CXMFLOAT2& size, float textOffsetY = 0.f );
	bool MouseOveredItem( const Rect& boundingbox, ImGUID id );
	bool IsWindowContentMouseOverable( ImUiWindow* window );

	CXMFLOAT4 GetItemColor( ImUiColor::Type colorId, float alphaMul = 1.f );

	void SetActiveItemID( ImGUID id );
	void SetMouseOveredID( ImGUID id );

	void SetCurrentWindow( ImUiWindow* window );
	void FocusWindow( ImUiWindow* window );
	void UpdateMovingWindow( );

	void LoadSettingFromDisk( const char* fileName );
	void LoadSettingFromMemory( const char* buf, int count );
	ImUiSettingHandler* FindSettingHandler( const char* typeName );

	void OpenPopupEx( ImGUID id );
	void EndPopup( );
	void ClosePopupsOverWindow( ImUiWindow* refWindow );
	void ClosePopupToLevel( int remaining );
	void CloseCurrentPopup( );
	bool IsPopupOpen( ImGUID id );

	CXMFLOAT2 CalcAfterConstraintSize( ImUiWindow* window, const CXMFLOAT2& size );
	CXMFLOAT2 CalcAutoFitSize( ImUiWindow* window, const CXMFLOAT2& contentsSize );
	CXMFLOAT2 FindBestWindowPosForPopup( const CXMFLOAT2& refPos, const CXMFLOAT2& size, ImDir::Type& lastDir, const Rect& avoid, ImUiPopupPositionPolicy::Type policy = ImUiPopupPositionPolicy::Default );

	void SetNextWindowPos( const CXMFLOAT2& pos, ImUiCond::Type cond = ImUiCond::None, const CXMFLOAT2& pivot = { 0.f, 0.f } );
	void SetNextWindowConstraintSize( const Rect& constraintSize, ImUiSizeCallBack callback = nullptr, void* callbackData = nullptr );

	int m_frameCount = 0;

	Rect m_clientRect;

	std::map<std::string, int> m_windowLUT;
	std::vector<std::unique_ptr<ImUiWindow>> m_windows;
	std::vector<ImUiWindow*> m_currentWindowStack;
	ImUiWindow* m_curWindow = nullptr;
	ImUiWindow* m_navWindow = nullptr;
	ImUiWindow* m_mouseOveredWindow = nullptr;
	ImUiWindow* m_moveingWindow = nullptr;
	ImGUID m_activeItemID;
	ImGUID m_mouseOveredID;
	ImUiNextWindowData m_nextWindowData;

	ImUiStyle m_curStyle;

	ImGuiIO m_io;

	CXMFLOAT2 m_activeIDClickOffset = { -1, -1 };

	std::map<std::string, int> m_textAtlasLUT;
	std::vector<CTextAtlas> m_textAtlas;
	int m_curTextAltas = -1;

	std::unordered_map<std::string, ImUiWindowSetting> m_settingWindows;
	std::unordered_map<std::string, ImUiSettingHandler> m_settingHandler;

	struct ImUIPopupRef
	{
		ImGUID m_popupId;
		ImUiWindow* m_window;
		ImUiWindow* m_parentWindow;
		int m_openFrameCount;
		ImGUID m_openParentID;
		CXMFLOAT2 m_openPopupPos;
		CXMFLOAT2 m_openMousePos;
	};

	std::vector<ImUIPopupRef> m_openPopupStack;
	std::vector<ImUIPopupRef> m_currentPopupStack;
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
