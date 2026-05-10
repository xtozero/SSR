#pragma once

#include "SizedTypes.h"

#include <algorithm>
#include <charconv>
#include <string_view>

class TextTokenaizer
{
public:
	void Parse( const char* contents, size_t length )
	{
		m_begin = contents;
		m_end = contents + length;
		m_current = contents;
	}

	bool CanRead() const
	{
		return m_current < m_end;
	}

	void SkipBlank()
	{
		while ( CanRead() )
		{
			char c = PeekNextChar();
			if ( c == ' ' || c == '\t' )
			{
				GetNextChar();
			}
			else
			{
				break;
			}
		}
	}

	void SkipWhiteSpace()
	{
		while ( CanRead() )
		{
			char c = PeekNextChar();
			if ( c == ' ' || c == '\t' || c == '\n' || c == '\r' )
			{
				GetNextChar();
			}
			else
			{
				break;
			}
		}
	}

	void SkipUntilNewline()
	{
		while ( CanRead() )
		{
			char c = PeekNextChar();
			if ( c == '\n' )
			{
				break;
			}
			else
			{
				GetNextChar();
			}
		}
	}

	void SkipLine()
	{
		while ( CanRead() )
		{
			char c = GetNextChar();
			if ( c == '\n' )
			{
				break;
			}
		}
	}

	bool IsLineEnd() const
	{
		char c = PeekNextChar();
		return c == '\n' || c == '\r';
	}

	bool IsWhiteSpace() const
	{
		char c = PeekNextChar();
		return c == ' ' || c == '\t' || c == '\n' || c == '\r';
	}

	char GetNextChar()
	{
		if ( CanRead() )
		{
			char c = *m_current++;
			if ( c == '\n' )
			{
				++m_line;
			}

			return c;
		}

		return 0;
	}

	char PeekNextChar() const
	{
		return CanRead() ? *m_current : 0;
	}

	bool MatchNextString( const char* str, size_t length )
	{
		if ( ( m_current > m_end ) || ( static_cast<size_t>( m_end - m_current ) < length ) )
		{
			return false;
		}

		if ( std::strncmp( m_current, str, length ) )
		{
			return false;
		}

		m_current += length;
		return true;
	}

	std::string_view ReadWord()
	{
		const char* begin = m_current;

		while ( CanRead() )
		{
			char c = PeekNextChar();
			if ( c != ' ' && c != '\t' && c != '\n' && c != '\r' )
			{
				GetNextChar();
			}
			else
			{
				break;
			}
		}

		return std::string_view( begin, m_current );
	}

	std::string_view ReadLine( bool removeNewline = true )
	{
		const char* begin = m_current;

		while ( CanRead() )
		{
			char c = PeekNextChar();
			if ( c != '\n' && c != '\r' )
			{
				GetNextChar();
			}
			else
			{
				break;
			}
		}

		if ( removeNewline )
		{
			std::string_view line( begin, m_current );
			SkipWhiteSpace();
			return line;
		}
		else
		{
			while ( CanRead() )
			{
				char c = PeekNextChar();
				if ( c == '\n' || c == '\r' )
				{
					GetNextChar();
				}
				else
				{
					break;
				}
			}

			return std::string_view( begin, m_current );
		}
	}

	template <typename Ret>
	Ret ReadNumber()
	{
		SkipWhiteSpace();

		std::string_view word = ReadWord();
		Ret value = 0;
		std::from_chars( word.data(), word.data() + word.size(), value );

		return value;
	}

	const char* Tell() const
	{
		return m_current;
	}

	void Seek( const char* pos )
	{
		m_current = pos;
	}

	void Seek( int64 offset )
	{
		auto low = static_cast<int64>( m_begin - m_current );
		auto high = static_cast<int64>( m_end - m_current );
		offset = std::clamp( offset, low, high );
		m_current += offset;
	}

	uint32 GetLineNumber() const
	{
		return m_line;
	}

private:
	const char* m_begin = nullptr;
	const char* m_end = nullptr;
	const char* m_current = nullptr;

	uint32 m_line = 1;
};