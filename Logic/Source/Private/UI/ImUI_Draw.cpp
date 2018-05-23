#include "stdafx.h"
#include "UI/ImUI.h"

using namespace std::rel_ops;

bool operator==( const RECT& lhs, const RECT& rhs )
{
	return memcmp( &lhs, &rhs, sizeof( RECT ) ) == 0;
}

void ImDrawList::Clear( )
{
	m_path.clear( );
	m_cmdBuffer.clear( );
	m_curIndex = 0;
	m_indices.clear( );
	m_pIndexWriter = nullptr;
	m_vertices.clear( );
	m_pVertexWriter = nullptr;
}

void ImDrawList::AddDrawCmd( )
{
	m_cmdBuffer.emplace_back( );
	ImDrawCmd& cmd = m_cmdBuffer.back( );
	
	cmd.m_indicesCount = 0;
	cmd.m_clipRect = m_clipRect.size() ? m_clipRect.top( ) : m_imUi->GetClientRect();
	cmd.m_textAtlas = m_textAtlas.size() ? m_textAtlas.top( ) : RE_HANDLE_TYPE::INVALID_HANDLE;
}

void ImDrawList::PushClipRect( RECT clipRect )
{
	m_clipRect.push( clipRect );
	UpdateClipRect( );
}

void ImDrawList::PushTextAtlas( RE_HANDLE texHandle )
{
	m_textAtlas.push( texHandle );
	UpdateTextAtlas( );
}

void ImDrawList::UpdateClipRect( )
{
	ImDrawCmd* curCmd = m_cmdBuffer.empty( ) ? nullptr : &m_cmdBuffer.back( );
	if ( curCmd == nullptr || ( curCmd->m_clipRect != m_clipRect.top() ) )
	{
		AddDrawCmd( );
		return;
	}
}

void ImDrawList::UpdateTextAtlas( )
{
	ImDrawCmd* curCmd = m_cmdBuffer.empty( ) ? nullptr : &m_cmdBuffer.back( );
	if ( curCmd == nullptr || ( curCmd->m_textAtlas != m_textAtlas.top( ) ) )
	{
		AddDrawCmd( );
		return;
	}
}

void ImDrawList::AddFilledRect( const CXMFLOAT2& pos, const CXMFLOAT2& size, const CXMFLOAT4& color, float rounding, int roundingFlag )
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

void ImDrawList::AddConvexPolyFilled( const CXMFLOAT2* points, const int count, const CXMFLOAT4& color )
{
	if ( color.w == 0.f )
	{
		return;
	}

	int indexCount = ( count - 2 ) * 3;
	int vertexCount = count;
	
	BufferReserve( vertexCount, indexCount );
	for ( int i = 0; i < vertexCount; ++i )
	{
		m_pVertexWriter[0].m_pos = points[i];
		m_pVertexWriter[0].m_color = color;
		++m_pVertexWriter;
	}
	for ( int i = 2; i < vertexCount; ++i )
	{
		m_pIndexWriter[0] = m_curIndex;
		m_pIndexWriter[1] = m_curIndex + i - 1;
		m_pIndexWriter[2] = m_curIndex + i;

		m_pIndexWriter += 3;
	}

	m_curIndex += static_cast<UINT>( vertexCount );
}

void ImDrawList::AddText( CTextAtlas& font, const CXMFLOAT2& pos, const CXMFLOAT4& color, const char* text, int count )
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

	const int vtxCount = count * 4;
	const int idxCount = count * 6;
	BufferReserve( vtxCount, idxCount );

	for ( int i = 0; i < count; ++i )
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

void ImDrawList::AddTriangleFilled( const CXMFLOAT2& v0, const CXMFLOAT2& v1, const CXMFLOAT2& v2, const CXMFLOAT4& color )
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

void ImDrawList::BufferReserve( int vertexCount, int indexCount )
{
	assert( m_cmdBuffer.size( ) > 0 );
	ImDrawCmd& cmd = m_cmdBuffer.back( );

	cmd.m_indicesCount += indexCount;

	size_t oldVerticesSize = m_vertices.size( );
	m_vertices.resize( oldVerticesSize + vertexCount );
	m_pVertexWriter = m_vertices.data( ) + oldVerticesSize;

	size_t oldIndicesSize = m_indices.size( );
	m_indices.resize( oldIndicesSize + indexCount );
	m_pIndexWriter = m_indices.data( ) + oldIndicesSize;
}

void ImDrawList::DrawRect( const CXMFLOAT2& pos, const CXMFLOAT2& size, const CXMFLOAT4& color )
{
	CXMFLOAT2 lt = pos;
	CXMFLOAT2 rt( pos.x + size.x, pos.y );
	CXMFLOAT2 lb( pos.x, pos.y + size.y );
	CXMFLOAT2 rb( pos + size );

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

void ImDrawList::PathLineTo( const CXMFLOAT2& pos )
{
	m_path.emplace_back( pos );
}

void ImDrawList::PathFillConvex( const CXMFLOAT4 & color )
{
	AddConvexPolyFilled( m_path.data( ), static_cast<int>( m_path.size( ) ), color );
	PathClear( );
}

void ImDrawList::PathArcToFast( const CXMFLOAT2& centre, float radius, int minOf12, int maxOf12 )
{
	if ( radius == 0.f || minOf12 > maxOf12 )
	{
		m_path.emplace_back( centre );
		return;
	}
	m_path.reserve( m_path.size( ) + ( maxOf12 - minOf12 + 1 ) );
	for ( int i = minOf12; i <= maxOf12; ++i )
	{
		const CXMFLOAT2& c = m_dc->m_circleVertex[i % _countof( m_dc->m_circleVertex )];
		m_path.emplace_back( centre.x + c.x * radius, centre.y + c.y * radius );
	}
}

void ImDrawList::PathRect( const CXMFLOAT2& pos, const CXMFLOAT2& size, float rounding, int roundingFlag )
{
	rounding = min( rounding, size.x * ( ( ( roundingFlag & ImDrawCorner::Top ) == ImDrawCorner::Top ) || ( ( roundingFlag & ImDrawCorner::Bottom ) == ImDrawCorner::Bottom ) ? 0.5f : 1.0f ) - 1.0f );
	rounding = min( rounding, size.y * ( ( ( roundingFlag & ImDrawCorner::Left ) == ImDrawCorner::Left ) || ( ( roundingFlag & ImDrawCorner::Right ) == ImDrawCorner::Right ) ? 0.5f : 1.0f ) - 1.0f );

	const CXMFLOAT2& a = pos;
	const CXMFLOAT2& b = pos + size;

	const float roundingTopLeft = ( roundingFlag & ImDrawCorner::TopLeft ) ? rounding : 0.f;
	const float roundingTopRight = ( roundingFlag & ImDrawCorner::TopRight ) ? rounding : 0.f;
	const float roundingBottomLeft = ( roundingFlag & ImDrawCorner::BottomLeft ) ? rounding : 0.f;
	const float roundingBottomRight = ( roundingFlag & ImDrawCorner::BottomRight ) ? rounding : 0.f;
	PathArcToFast( CXMFLOAT2( a.x + roundingTopLeft, a.y + roundingTopLeft ), roundingTopLeft, 6, 9 );
	PathArcToFast( CXMFLOAT2( b.x - roundingTopRight, a.y + roundingTopRight ), roundingTopRight, 9, 12 );
	PathArcToFast( CXMFLOAT2( b.x - roundingBottomRight, b.y - roundingBottomRight ), roundingBottomRight, 0, 3 );
	PathArcToFast( CXMFLOAT2( a.x + roundingBottomLeft, b.y - roundingBottomLeft ), roundingBottomLeft, 3, 6 );
}
