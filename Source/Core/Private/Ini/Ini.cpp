#include "Ini/Ini.h"

#include <cassert>
#include <iostream>

namespace ini
{
	struct IniReaderContext
	{
		Section* m_curSection = nullptr;
	};

	std::optional<Ini> Reader::Parse()
	{
		Ini ini;
		IniReaderContext context;

		while ( CanRead() )
		{
			Token token = ReadToken();
			switch ( token.m_type )
			{
			case TokenType::Comments:
			{
				SkipLine();
				break;
			}
			case TokenType::SquareBracketOpen:
			{
				std::optional<std::string_view> sectionName = ReadSectionName();
				if ( sectionName.has_value() == false )
				{
					return {};
				}

				auto result = ini.m_sections.emplace( sectionName.value(), Section() );
				context.m_curSection = &result.first->second;
				break;
			}
			case TokenType::String:
			{
				if ( context.m_curSection == nullptr )
				{
					return {};
				}

				std::optional<std::string_view> keyName = ReadKeyName();
				if ( keyName.has_value() == false )
				{
					return {};
				}

				token = ReadToken();
				if ( token.m_type != TokenType::EqualsSign )
				{
					return {};
				}

				std::optional<std::string_view> value = ReadValue();
				if ( value.has_value() == false )
				{
					return {};
				}

				context.m_curSection->m_properties.emplace( keyName.value(), value.value() );

				SkipWhiteSpace();
				break;
			}
			default:
				break;
			}
		}

		return ini;
	}

	Reader::Reader( const char* contents, size_t size )
	{
		TextTokenaizer::Parse( contents, size );
	}

	Reader::Token Reader::ReadToken()
	{
		SkipWhiteSpace();

		Token token;
		token.m_type = TokenType::String;

		switch ( PeekNextChar() )
		{
		case ';':
			token.m_type = TokenType::Comments;
			break;
		case '=':
			token.m_type = TokenType::EqualsSign;
			break;
		case ']':
			token.m_type = TokenType::SquareBracketClose;
			break;
		case '[':
			token.m_type = TokenType::SquareBracketOpen;
			break;
		default:
			break;
		}

		if ( token.m_type != TokenType::String )
		{
			GetNextChar();
		}

		return token;
	}
	
	std::optional<std::string_view> Reader::ReadSectionName()
	{
		const char* start = Tell();

		while ( CanRead() && ( PeekNextChar() != ']' ) )
		{
			GetNextChar();
		}

		const char* end = Tell();
		if ( PeekNextChar() == ']' )
		{
			return std::string_view( start, end );
		}

		return {};
	}

	std::optional<std::string_view> Reader::ReadKeyName()
	{
		const char* start = Tell();

		while ( CanRead() && ( PeekNextChar() != '=' ) )
		{
			char c = GetNextChar();
			if ( c == ';' )
			{
				return {};
			}
		}

		const char* end = Tell();
		if ( PeekNextChar() == '=' )
		{
			std::string_view ret( start, end );
			size_t trimPos = ret.find_last_not_of( ' ' );
			if ( trimPos != std::string_view::npos )
			{
				ret.remove_suffix( ret.size() - trimPos - 1 );
			}

			return ret;
		}

		return {};
	}

	std::optional<std::string_view> Reader::ReadValue()
	{
		SkipWhiteSpace();

		const char* start = Tell();

		while ( CanRead() 
			&& (( PeekNextChar() != '\n' ) && ( PeekNextChar() != '\r' )) )
		{
			char c = GetNextChar();
			if ( c == ';' )
			{
				assert( false && "the comment must be at the beginning of the line." );
				return {};
			}
		}

		const char* end = Tell();
		if ( ( PeekNextChar() == '\n' ) || ( PeekNextChar() == '\r' ) || ( CanRead() == false ) )
		{
			std::string_view ret( start, end );
			size_t trimPos = ret.find_last_not_of( ' ' );
			if ( trimPos != std::string_view::npos )
			{
				ret.remove_suffix( ret.size() - trimPos - 1 );
			}

			return ret;
		}

		return {};
	}

	bool Section::HasValue() const
	{
		return m_properties.empty() == false;
	}

	const std::string* Section::GetValue( const Name& key ) const
	{
		auto found = m_properties.find( key );

		if ( found == std::end( m_properties ) )
		{
			return nullptr;
		}

		return &found->second;
	}

	const Section* Ini::GetSection( const Name& name ) const
	{
		auto found = m_sections.find( name );
		
		if ( found == std::end(m_sections) )
		{
			return nullptr;
		}
		
		return &found->second;
	}

	void Ini::AddSection( const Name& name, const Section& section )
	{
		m_sections.emplace( name, section );
	}

	std::string Writer::ToString( const Ini& ini )
	{
		std::string str;
		constexpr size_t MB = 1024 * 1024;
		str.reserve( 1 * MB );

		for ( const auto& [name, section] : ini.m_sections )
		{
			if ( section.HasValue() == false )
			{
				continue;
			}

			str += "[";
			str += name.Str();
			str += "]\n";

			for ( const auto& [propertyName, value] : section.m_properties )
			{
				str += propertyName.Str();
				str += "=";
				str += value;
				str += "\n";
			}

			str += "\n";
		}

		return str;
	}
}
