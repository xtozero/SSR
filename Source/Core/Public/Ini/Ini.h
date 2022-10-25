#pragma once

#include "Misc/TextTokenaizer.h"
#include "NameTypes.h"
#include "SizedTypes.h"

#include <map>
#include <optional>
#include <string>
#include <string_view>

namespace ini
{
	class Section
	{
	public:
		const std::string* GetValue( const Name& key ) const;

	private:
		std::map<Name, std::string> m_properties;

		friend class IniReader;
	};

	class Ini
	{
	public:
		const Section* GetSection( const Name& name ) const;

	private:
		std::map<Name, Section> m_sections;

		friend class IniReader;
	};

	class IniReader : public TextTokenaizer
	{
	public:
		std::optional<Ini> Parse();

		IniReader( const char* contents, size_t size );

	private:
		enum class TokenType : uint8
		{
			Comments = 0,
			EqualsSign,
			SquareBracketClose,
			SquareBracketOpen,
			String,
		};

		struct Token
		{
			TokenType m_type;
		};

		Token ReadToken();

		std::optional<std::string_view> ReadSectionName();
		std::optional<std::string_view> ReadKeyName();
		std::optional<std::string_view> ReadValue();
	};
}
