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
		bool HasValue() const;

		const std::string* GetValue( const Name& key ) const;

		template <typename T>
		void AddValue( Name name, T&& value )
		{
			if constexpr ( std::is_same_v<std::string, T> || std::is_convertible_v<T, std::string> )
			{
				m_properties.emplace( name, value );
			}
			else
			{
				m_properties.emplace( name, std::to_string( value ) );
			}
		}

	private:
		std::map<Name, std::string> m_properties;

		friend class Reader;
		friend class Writer;
	};

	class Ini
	{
	public:
		const Section* GetSection( const Name& name ) const;
		void AddSection( const Name& name, const Section& section );

	private:
		std::map<Name, Section> m_sections;

		friend class Reader;
		friend class Writer;
	};

	class Reader : public TextTokenaizer
	{
	public:
		std::optional<Ini> Parse();

		Reader( const char* contents, size_t size );

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

	class Writer
	{
	public:
		static std::string ToString( const Ini& ini );
	};
}
