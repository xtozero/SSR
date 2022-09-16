#include "stdafx.h"
#include "UI/ImUI.h"

#include <algorithm>

void ImDrawList::Clear()
{
	m_path.clear();
	m_cmdBuffer.clear();
	m_curIndex = 0;
	m_indices.clear();
	m_pIndexWriter = nullptr;
	m_vertices.clear();
	m_pVertexWriter = nullptr;

	for ( size_t i = 0, end = m_clipRect.size(); i < end; ++i )
	{
		m_clipRect.pop();
	}

	//for ( size_t i = 0, end = m_textAtlas.size( ); i < end; ++i )
	//{
	//	m_textAtlas.pop( );
	//}
}

void ImDrawList::AddDrawCmd()
{
	m_cmdBuffer.emplace_back();
	ImDrawCmd& cmd = m_cmdBuffer.back();

	cmd.m_indicesCount = 0;
	cmd.m_clipRect = m_clipRect.size() ? m_clipRect.top() : m_imUi->GetClientRect();
	//cmd.m_textAtlas = m_textAtlas.size() ? m_textAtlas.top( ) : RE_HANDLE::InValidHandle( );
}

void ImDrawList::PushClipRect( const Rect& clipRect )
{
	m_clipRect.push( clipRect );
	UpdateClipRect();
}

//void ImDrawList::PushTextAtlas( RE_HANDLE texHandle )
//{
//	m_textAtlas.push( texHandle );
//	UpdateTextAtlas( );
//}

void ImDrawList::UpdateClipRect()
{
	ImDrawCmd* curCmd = m_cmdBuffer.empty() ? nullptr : &m_cmdBuffer.back();
	if ( curCmd == nullptr || ( curCmd->m_clipRect != m_clipRect.top() ) )
	{
		AddDrawCmd();
		return;
	}
}

void ImDrawList::UpdateTextAtlas()
{
	//ImDrawCmd* curCmd = m_cmdBuffer.empty( ) ? nullptr : &m_cmdBuffer.back( );
	//if ( curCmd == nullptr || ( curCmd->m_textAtlas != m_textAtlas.top( ) ) )
	//{
	//	AddDrawCmd( );
	//	return;
	//}
}

void ImDrawList::AddFilledRect( const Point2& pos, const Vector2& size, const ColorF& color, float rounding, int32 roundingFlag )
{
	if ( color.w == 0.f )
	{
		return;
	}

	if ( rounding > 0.f )
	{
		PathRect( pos, size, rounding, roundingFlag );
		PathFillConvex( color );
	}
	else
	{
		BufferReserve( 4, 6 );
		DrawRect( pos, size, color );
	}
}

void ImDrawList::AddConvexPolyFilled( const Point2* points, const int32 count, const ColorF& color )
{
	if ( color.w == 0.f )
	{
		return;
	}

	int32 indexCount = ( count - 2 ) * 3;
	int32 vertexCount = count;

	BufferReserve( vertexCount, indexCount );
	for ( int32 i = 0; i < vertexCount; ++i )
	{
		m_pVertexWriter[0].m_pos = points[i];
		m_pVertexWriter[0].m_color = color;
		++m_pVertexWriter;
	}
	for ( int32 i = 2; i < vertexCount; ++i )
	{
		m_pIndexWriter[0] = m_curIndex;
		m_pIndexWriter[1] = m_curIndex + i - 1;
		m_pIndexWriter[2] = m_curIndex + i;

		m_pIndexWriter += 3;
	}

	m_curIndex += static_cast<uint32>( vertexCount );
}

void ImDrawList::AddText( CTextAtlas& font, const Point2& pos, const ColorF& color, const char* text, uint32 count )
{
	if ( color.w == 0.f )
	{
		return;
	}

	if ( count == 0 )
	{
		return;
	}

	float x = pos.x + font.m_displayOffset.x;
	float y = pos.y + font.m_displayOffset.y;

	assert( ( count * 4 ) <= UINT_MAX );
	uint32 vtxCount = count * 4;
	assert( ( count * 6 ) <= UINT_MAX );
	uint32 idxCount = count * 6;
	BufferReserve( vtxCount, idxCount );

	for ( uint32 i = 0; i < count; ++i )
	{
		if ( FontUV* glyph = font.FindGlyph( text[i] ) )
		{
			float x1 = x;
			float x2 = x + glyph->m_size;
			float y1 = y;
			float y2 = y + font.m_fontHeight;

			float u1 = glyph->m_u.x;
			float u2 = glyph->m_u.y;
			float v1 = glyph->m_v.x;
			float v2 = glyph->m_v.y;

			m_pIndexWriter[0] = m_curIndex; m_pIndexWriter[1] = m_curIndex + 1; m_pIndexWriter[2] = m_curIndex + 2;
			m_pIndexWriter[3] = m_curIndex; m_pIndexWriter[4] = m_curIndex + 2; m_pIndexWriter[5] = m_curIndex + 3;
			m_pVertexWriter[0].m_pos.x = x1; m_pVertexWriter[0].m_pos.y = y1; m_pVertexWriter[0].m_uv.x = u1; m_pVertexWriter[0].m_uv.y = v1; m_pVertexWriter[0].m_color = color;
			m_pVertexWriter[1].m_pos.x = x2; m_pVertexWriter[1].m_pos.y = y1; m_pVertexWriter[1].m_uv.x = u2; m_pVertexWriter[1].m_uv.y = v1; m_pVertexWriter[1].m_color = color;
			m_pVertexWriter[2].m_pos.x = x2; m_pVertexWriter[2].m_pos.y = y2; m_pVertexWriter[2].m_uv.x = u2; m_pVertexWriter[2].m_uv.y = v2; m_pVertexWriter[2].m_color = color;
			m_pVertexWriter[3].m_pos.x = x1; m_pVertexWriter[3].m_pos.y = y2; m_pVertexWriter[3].m_uv.x = u1; m_pVertexWriter[3].m_uv.y = v2; m_pVertexWriter[3].m_color = color;

			if ( text[i] == ' ' )
			{
				m_pVertexWriter[0].m_color.w = 0.f;
				m_pVertexWriter[1].m_color.w = 0.f;
				m_pVertexWriter[2].m_color.w = 0.f;
				m_pVertexWriter[3].m_color.w = 0.f;
			}

			m_pIndexWriter += 6;
			m_pVertexWriter += 4;
			m_curIndex += 4;

			x += glyph->m_size + font.m_fontSpacing;
		}
	}
}

void ImDrawList::AddTriangleFilled( const Point2& v0, const Point2& v1, const Point2& v2, const ColorF& color )
{
	if ( color.w == 0.f )
	{
		return;
	}

	PathLineTo( v0 );
	PathLineTo( v1 );
	PathLineTo( v2 );
	PathFillConvex( color );
}

void ImDrawList::BufferReserve( int32 vertexCount, int32 indexCount )
{
	assert( m_cmdBuffer.size() > 0 );
	ImDrawCmd& cmd = m_cmdBuffer.back();

	cmd.m_indicesCount += indexCount;

	size_t oldVerticesSize = m_vertices.size();
	m_vertices.resize( oldVerticesSize + vertexCount );
	m_pVertexWriter = m_vertices.data() + oldVerticesSize;

	size_t oldIndicesSize = m_indices.size();
	m_indices.resize( oldIndicesSize + indexCount );
	m_pIndexWriter = m_indices.data() + oldIndicesSize;
}

void ImDrawList::DrawRect( const Point2& pos, const Vector2& size, const ColorF& color )
{
	Point2 lt = pos;
	Point2 rt( pos.x + size.x, pos.y );
	Point2 lb( pos.x, pos.y + size.y );
	Point2 rb( pos + size );

	m_pIndexWriter[0] = m_curIndex; m_pIndexWriter[1] = m_curIndex + 1; m_pIndexWriter[2] = m_curIndex + 2;
	m_pIndexWriter[3] = m_curIndex; m_pIndexWriter[4] = m_curIndex + 2; m_pIndexWriter[5] = m_curIndex + 3;

	m_pVertexWriter[0].m_pos = lt; m_pVertexWriter[0].m_color = color;
	m_pVertexWriter[1].m_pos = rt; m_pVertexWriter[1].m_color = color;
	m_pVertexWriter[2].m_pos = rb; m_pVertexWriter[2].m_color = color;
	m_pVertexWriter[3].m_pos = lb; m_pVertexWriter[3].m_color = color;

	m_curIndex += 4;
	m_pIndexWriter += 6;
	m_pVertexWriter += 4;
}

void ImDrawList::PathLineTo( const Point2& pos )
{
	m_path.emplace_back( pos );
}

void ImDrawList::PathFillConvex( const ColorF& color )
{
	AddConvexPolyFilled( m_path.data(), static_cast<int32>( m_path.size() ), color );
	PathClear();
}

void ImDrawList::PathArcToFast( const Point2& centre, float radius, int32 minOf12, int32 maxOf12 )
{
	if ( radius == 0.f || minOf12 > maxOf12 )
	{
		m_path.emplace_back( centre );
		return;
	}
	m_path.reserve( m_path.size() + ( maxOf12 - minOf12 + 1 ) );
	for ( int32 i = minOf12; i <= maxOf12; ++i )
	{
		const Point2& c = m_imUi->m_circleVertex[i % _countof( m_imUi->m_circleVertex )];
		m_path.emplace_back( centre.x + c.x * radius, centre.y + c.y * radius );
	}
}

void ImDrawList::PathRect( const Point2& pos, const Vector2& size, float rounding, int32 roundingFlag )
{
	rounding = std::min( rounding, size.x * ( ( ( roundingFlag & ImDrawCorner::Top ) == ImDrawCorner::Top ) || ( ( roundingFlag & ImDrawCorner::Bottom ) == ImDrawCorner::Bottom ) ? 0.5f : 1.0f ) - 1.0f );
	rounding = std::min( rounding, size.y * ( ( ( roundingFlag & ImDrawCorner::Left ) == ImDrawCorner::Left ) || ( ( roundingFlag & ImDrawCorner::Right ) == ImDrawCorner::Right ) ? 0.5f : 1.0f ) - 1.0f );

	const Point2& a = pos;
	const Point2& b = pos + size;

	const float roundingTopLeft = ( roundingFlag & ImDrawCorner::TopLeft ) ? rounding : 0.f;
	const float roundingTopRight = ( roundingFlag & ImDrawCorner::TopRight ) ? rounding : 0.f;
	const float roundingBottomLeft = ( roundingFlag & ImDrawCorner::BottomLeft ) ? rounding : 0.f;
	const float roundingBottomRight = ( roundingFlag & ImDrawCorner::BottomRight ) ? rounding : 0.f;
	PathArcToFast( Point2( a.x + roundingTopLeft, a.y + roundingTopLeft ), roundingTopLeft, 6, 9 );
	PathArcToFast( Point2( b.x - roundingTopRight, a.y + roundingTopRight ), roundingTopRight, 9, 12 );
	PathArcToFast( Point2( b.x - roundingBottomRight, b.y - roundingBottomRight ), roundingBottomRight, 0, 3 );
	PathArcToFast( Point2( a.x + roundingBottomLeft, b.y - roundingBottomLeft ), roundingBottomLeft, 3, 6 );
}
