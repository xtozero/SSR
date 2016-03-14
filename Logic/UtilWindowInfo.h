#pragma once

class CUtilWindowInfo
{
public:
	static CUtilWindowInfo& GetInstance( )
	{
		static CUtilWindowInfo windowInfo;
		return windowInfo;
	}

	unsigned int GetWidth( ) { return m_width; }
	unsigned int GetHeight( ) { return m_height; }

	void SetRect( unsigned int width, unsigned int height )
	{
		m_width = width;
		m_height = height;
	}

	CUtilWindowInfo() : m_width( 0 ),
		m_height( 0 )
	{}

	~CUtilWindowInfo( ) = default;

private:
	unsigned int m_width;
	unsigned int m_height;
};