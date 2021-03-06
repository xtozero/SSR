#include <cassert>
#include <filesystem>
#include <fstream>
#include <ostream>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>

#include <iostream>

namespace JSON
{
	namespace
	{
		static std::string CodePointToUTF8( unsigned int cp )
		{
			// https://en.wikipedia.org/wiki/UTF-8
			std::string result;

			if ( cp <= 0x7F )
			{
				result.resize( 1 );
				result[0] = static_cast<char>( cp );
			}
			else if ( cp <= 0x7FF )
			{
				result.resize( 2 );
				result[0] = static_cast<char>( 0xC0 | ( 0x1F & ( cp >> 6 ) ) );
				result[1] = static_cast<char>( 0x80 | ( 0x3F & cp ) );
			}
			else if ( cp <= 0xFFFF )
			{
				result.resize( 3 );
				result[0] = static_cast<char>( 0xE0 | ( 0xF & ( cp >> 12 ) ) );
				result[1] = static_cast<char>( 0x80 | ( 0x3F & ( cp >> 6 ) ) );
				result[2] = static_cast<char>( 0x80 | ( 0x3F & cp ) );
			}
			else if ( cp <= 0x10FFFF )
			{
				result.resize( 4 );
				result[0] = static_cast<char>( 0xF0 | ( 0x7 & ( cp >> 18 ) ) );
				result[1] = static_cast<char>( 0x80 | ( 0x3F & ( cp >> 12 ) ) );
				result[2] = static_cast<char>( 0x80 | ( 0x3F & ( cp >> 6 ) ) );
				result[3] = static_cast<char>( 0x80 | ( 0x3F & cp ) );
			}

			return result;
		}
	}

	enum class DataType
	{
		STRING = 0,
		INTERGER,
		REAL,
		OBJECT,
		ARRAY,
		BOOLEAN,
		EMPTY	// null
	};

	class Value;
	using ObjectType = std::unordered_map<std::string, Value>;

	template <typename T>
	class ValueIteratorBase
	{
	public:
		using pointer = T*;
		using reference = T&;

		std::string Key( ) const
		{
			return m_otherType ? "" : m_mapType->first;
		}

		friend bool operator==( const ValueIteratorBase<T>& lhs, const ValueIteratorBase<T>& rhs )
		{
			if ( lhs.m_otherType )
			{
				return lhs.m_otherType == rhs.m_otherType;
			}

			return lhs.m_mapType == rhs.m_mapType;
		}

		friend bool operator!=( const ValueIteratorBase<T>& lhs, const ValueIteratorBase<T>& rhs )
		{
			return !( lhs == rhs );
		}

	protected:
		void Increment( )
		{
			if ( m_otherType == nullptr )
			{
				++m_mapType;
			}
		}

		void Decrement( )
		{
			if ( m_otherType == nullptr )
			{
				--m_mapType;
			}
		}

		reference Dereference( )
		{
			if ( m_otherType )
			{
				return *m_otherType;
			}

			return m_mapType->second;
		}

		ObjectType::iterator m_mapType;
		pointer m_otherType = nullptr;
	};

	template <typename T>
	class ValueIterator : public ValueIteratorBase<T>
	{
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using difference_type = size_t;

		explicit ValueIterator( ObjectType::iterator iterator ) noexcept
		{
			m_mapType = iterator;
		}

		explicit ValueIterator( pointer pValue ) noexcept
		{
			m_otherType = pValue;
		}

		ValueIterator& operator++( )
		{
			Increment( );
			return *this;
		}

		ValueIterator& operator--( )
		{
			Decrement( );
			return *this;
		}

		ValueIterator operator++( int )
		{
			ValueIterator temp = *this;
			++( *this );
			return temp;
		}

		ValueIterator operator--( int )
		{
			ValueIterator temp = *this;
			--( *this );
			return temp;
		}

		reference operator*( )
		{
			return Dereference( );
		}

		pointer operator->( )
		{
			return &Dereference( );
		}
	};

	template <typename T>
	class ConstValueIterator : public ValueIteratorBase<const T>
	{
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = const T;
		using pointer = const T*;
		using reference = const T&;
		using difference_type = size_t;

		explicit ConstValueIterator( ObjectType::iterator iterator ) noexcept
		{
			m_mapType = iterator;
		}

		explicit ConstValueIterator( pointer pValue ) noexcept
		{
			m_otherType = pValue;
		}

		ConstValueIterator& operator++( )
		{
			Increment( );
			return *this;
		}

		ConstValueIterator& operator--( )
		{
			Decrement( );
			return *this;
		}

		ConstValueIterator operator++( int )
		{
			ConstValueIterator temp = *this;
			++( *this );
			return temp;
		}

		ConstValueIterator operator--( int )
		{
			ConstValueIterator temp = *this;
			--( *this );
			return temp;
		}

		reference operator*( )
		{
			return Dereference( );
		}

		pointer operator->( )
		{
			return &Dereference( );
		}
	};

	class Value
	{
	public:
		DataType Type( ) const { return m_type; }

		const std::string AsString( ) const
		{
			switch ( Type( ) )
			{
			case DataType::STRING:
				return *( m_data.m_string );
				break;
			case DataType::INTERGER:
				return std::to_string( m_data.m_integer );
				break;
			case DataType::REAL:
				return std::to_string( m_data.m_real );
				break;
			case DataType::BOOLEAN:
				return m_data.m_boolean ? "true" : "false";
			case DataType::EMPTY:
				return "null";
			default:
				break;
			}

			__debugbreak( );
			return "";
		}

		int AsInt( ) const
		{
			switch ( Type( ) )
			{
			case DataType::STRING:
				return std::stoi( *( m_data.m_string ) );
			case DataType::INTERGER:
				return m_data.m_integer;
			case DataType::REAL:
				return static_cast<int>( m_data.m_real );
			case DataType::BOOLEAN:
				return m_data.m_boolean ? 1 : 0;
			case DataType::EMPTY:
				return 0;
			default:
				break;
			}

			__debugbreak( );
			return 0;
		}

		bool AsBool( ) const
		{
			switch ( Type( ) )
			{
			case DataType::INTERGER:
				return m_data.m_integer ? true : false;
			case DataType::REAL:
				return ( m_data.m_real != 0. ) ? true : false;
			case DataType::BOOLEAN:
				return m_data.m_boolean;
			case DataType::EMPTY:
				return false;
			default:
				break;
			}

			__debugbreak( );
			return 0;
		}

		double AsReal( ) const
		{
			switch ( Type( ) )
			{
			case DataType::STRING:
				return std::stod( *( m_data.m_string ) );
				break;
			case DataType::INTERGER:
				return static_cast<double>( m_data.m_integer );
				break;
			case DataType::REAL:
				return m_data.m_real;
				break;
			case DataType::BOOLEAN:
				return m_data.m_boolean ? 1. : 0.;
			case DataType::EMPTY:
				return 0.;
			default:
				break;
			}

			__debugbreak( );
			return 0.;
		}

		const Value* Find( const std::string& key ) const
		{
			assert( Type( ) == DataType::OBJECT || Type( ) == DataType::EMPTY );
			if ( Type( ) == DataType::EMPTY )
			{
				return nullptr;
			}
			
			auto found = m_data.m_object->find( key );
			return ( found == m_data.m_object->end( ) ) ? nullptr : &found->second;
		}

		size_t Size( ) const
		{
			switch ( Type( ) )
			{
			case DataType::OBJECT:
				[[fallthrough]];
			case DataType::ARRAY:
				return m_data.m_object->size( );
			case DataType::EMPTY:
				return 0;
			default:
				return 1;
			}
		}

		size_t QueryRequiredContiguousBufferSize(  ) const
		{
			size_t size = 0;

			switch ( Type( ) )
			{
			case DataType::STRING:
				size += 2; // ""
			case DataType::INTERGER:
				[[fallthrough]];
			case DataType::REAL:
				[[fallthrough]];
			case DataType::BOOLEAN:
				[[fallthrough]];
			case DataType::EMPTY:
				size += AsString( ).size( );
				break;
			case DataType::OBJECT:
				{
					size += 2; // {}
					const ObjectType& map = *m_data.m_object;
					for ( const auto& iter : map )
					{
						size += iter.first.size( ) + 3; // include "":
						size += iter.second.QueryRequiredContiguousBufferSize( );
					}

					if ( size_t mapSize = map.size( ) )
					{
						size += ( mapSize - 1 );
					}
				}
				break;
			case DataType::ARRAY:
				{
					size += 2; // []
					const ObjectType& array = *m_data.m_object;
					for ( const auto& iter : array )
					{
						size += iter.second.QueryRequiredContiguousBufferSize( );
					}

					if ( size_t arraySize = array.size( ) )
					{
						size += ( arraySize - 1 );
					}
				}
				break;
			default:
				break;
			}

			return size;
		}

		ConstValueIterator<Value> begin() const noexcept
		{
			switch ( Type( ) )
			{
			case JSON::DataType::OBJECT:
				[[fallthrough]];
			case JSON::DataType::ARRAY:
				return ConstValueIterator<Value>( m_data.m_object->begin() );
			default:
				return ConstValueIterator<Value>( this );
			}
		}

		ConstValueIterator<Value> end( ) const noexcept
		{
			switch ( Type( ) )
			{
			case JSON::DataType::OBJECT:
				[[fallthrough]];
			case JSON::DataType::ARRAY:
				return ConstValueIterator<Value>( m_data.m_object->end( ) );
			default:
				return ConstValueIterator<Value>( nullptr );
			}
		}

		ValueIterator<Value> begin( ) noexcept
		{
			switch ( Type( ) )
			{
			case JSON::DataType::OBJECT:
				[[fallthrough]];
			case JSON::DataType::ARRAY:
				return ValueIterator<Value>( m_data.m_object->begin( ) );
			default:
				return ValueIterator<Value>( this );
			}
		}

		ValueIterator<Value> end( ) noexcept
		{
			switch ( Type( ) )
			{
			case JSON::DataType::OBJECT:
				[[fallthrough]];
			case JSON::DataType::ARRAY:
				return ValueIterator<Value>( m_data.m_object->end( ) );
			default:
				return ValueIterator<Value>( nullptr );
			}
		}

		ConstValueIterator<Value> cbegin( ) const noexcept
		{
			switch ( Type( ) )
			{
			case JSON::DataType::OBJECT:
				[[fallthrough]];
			case JSON::DataType::ARRAY:
				return ConstValueIterator<Value>( m_data.m_object->begin( ) );
			default:
				return ConstValueIterator<Value>( this );
			}
		}

		ConstValueIterator<Value> cend( ) const noexcept
		{
			switch ( Type( ) )
			{
			case JSON::DataType::OBJECT:
				[[fallthrough]];
			case JSON::DataType::ARRAY:
				return ConstValueIterator<Value>( m_data.m_object->end( ) );
			default:
				return ConstValueIterator<Value>( nullptr );
			}
		}

		Value& operator=( const std::string& string )
		{
			switch ( Type( ) )
			{
			case DataType::STRING:
				*( m_data.m_string ) = string;
				break;
			default:
				__debugbreak( );
				break;
			}

			return *this;
		}

		Value& operator=( std::string&& string )
		{
			switch ( Type( ) )
			{
			case DataType::STRING:
				*( m_data.m_string ) = std::move( string );
				break;
			default:
				__debugbreak( );
				break;
			}

			return *this;
		}
		
		Value& operator=( int integer )
		{
			switch ( Type( ) )
			{
			case DataType::STRING:
				*( m_data.m_string ) = std::to_string( integer );
				break;
			case DataType::INTERGER:
				m_data.m_integer = integer;
				break;
			case DataType::REAL:
				m_data.m_real = static_cast<float>( integer );
				break;
			case DataType::BOOLEAN:
				m_data.m_boolean = integer ? true : false;
				break;
			default:
				__debugbreak( );
				break;
			}

			return *this;
		}

		Value& operator=( double real )
		{
			switch ( Type( ) )
			{
			case DataType::STRING:
				*( m_data.m_string ) = std::to_string( real );
				break;
			case DataType::INTERGER:
				m_data.m_integer = static_cast<int>( real );
				break;
			case DataType::REAL:
				m_data.m_real = real;
				break;
			case DataType::BOOLEAN:
				m_data.m_boolean = ( real != 1. ) ? true : false;
				break;
			default:
				__debugbreak( );
				break;
			}

			return *this;
		}

		Value& operator[]( size_t index ) const
		{
			assert( ( Type() == DataType::ARRAY ) );
			ObjectType& array = *m_data.m_object;
			auto iter = array.emplace( std::to_string( index ), Value( DataType::EMPTY ) );
			return iter.first->second;
		}

		Value& operator[]( const std::string& key ) const
		{
			assert( ( Type( ) == DataType::OBJECT ) );
			ObjectType& map = *m_data.m_object;
			auto iter = map.emplace( key, Value( DataType::EMPTY ) );
			return iter.first->second;
		}

		Value& operator=( const Value& rhs )
		{
			ChangeValueType( rhs.Type( ) );

			switch ( Type( ) )
			{
			case DataType::STRING:
				*( m_data.m_string ) = *( rhs.m_data.m_string );
				break;
			case DataType::ARRAY:
				[[fallthrough]];
			case DataType::OBJECT:
				*( m_data.m_object ) = *( rhs.m_data.m_object );
				break;
			default:
				m_data = rhs.m_data;
				break;
			}

			return *this;
		}

		Value& operator=( Value&& rhs )
		{
			if ( Type( ) != rhs.Type( ) )
			{
				FreeDataByType( Type( ) );
			}

			m_type = rhs.Type( );
			rhs.m_type = DataType::EMPTY;

			switch ( Type( ) )
			{
			case DataType::STRING:
				m_data.m_string = rhs.m_data.m_string;
				rhs.m_data.m_string = nullptr;
				break;
			case DataType::INTERGER:
				m_data.m_integer = rhs.m_data.m_integer;
				rhs.m_data.m_integer = 0;
				break;
			case DataType::REAL:
				m_data.m_real = rhs.m_data.m_real;
				rhs.m_data.m_real = 0.f;
				break;
			case DataType::OBJECT:
				[[fallthrough]];
			case DataType::ARRAY:
				m_data.m_object = rhs.m_data.m_object;
				rhs.m_data.m_object = nullptr;
				break;
			case DataType::BOOLEAN:
				m_data.m_boolean = rhs.m_data.m_boolean;
				rhs.m_data.m_boolean = false;
				break;
			}

			return *this;
		}

		explicit Value( DataType type ) : m_type( type ) 
		{
			AllocDataByType( Type( ) );
		}

		Value( const Value& other )
		{
			( *this ) = other;
		}

		Value( Value&& other )
		{
			( *this ) = std::move( other );
		}

		~Value( )
		{
			FreeDataByType( Type( ) );
		}

	private:
		void ChangeValueType( DataType newType )
		{
			if ( Type( ) == newType )
			{
				return;
			}

			FreeDataByType( Type( ) );
			m_type = newType;
			AllocDataByType( Type( ) );
		}

		void AllocDataByType( DataType type )
		{
			switch ( type )
			{
			case DataType::STRING:
				m_data.m_string = new std::string( );
				break;
			case DataType::OBJECT:
				[[fallthrough]];
			case DataType::ARRAY:
				m_data.m_object = new ObjectType( );
				break;
			default:
				break;
			}
		}

		void FreeDataByType( DataType type )
		{
			switch ( type )
			{
			case DataType::STRING:
				delete m_data.m_string;
				break;
			case DataType::OBJECT:
				[[fallthrough]];
			case DataType::ARRAY:
				delete m_data.m_object;
				break;
			default:
				break;
			}
		}

		void PrintArrayTypeValue( std::ostream& os ) const
		{
			assert( ( Type( ) == DataType::ARRAY ) );
			ObjectType& array = *m_data.m_object;
			os << '[';
			for ( size_t i = 0; i < array.size(); ++i )
			{
				if ( i != 0 )
				{
					os << ',';
				}

				auto iter = array.find( std::to_string( i ) );
				assert( iter != array.end( ) );
				os << iter->second;
			}
			os << ']';
		}

		void PrintObjectTypeValue( std::ostream& os ) const
		{
			assert( ( Type( ) == DataType::OBJECT ) );
			const ObjectType& map = *m_data.m_object;
			os << '{';
			for ( auto iter = map.begin( ); iter != map.end( ); ++iter )
			{
				if ( iter != map.begin( ) )
				{
					os << ',';
				}

				os << '"' << iter->first << "\":" << iter->second;
			}
			os << '}';
		}

		DataType m_type = DataType::EMPTY;
		union Data
		{
			int m_integer;
			double m_real;
			bool m_boolean;
			std::string* m_string;
			ObjectType* m_object;
		} m_data;

		friend std::ostream& operator<<( std::ostream& os, const Value& value )
		{
			switch ( value.Type( ) )
			{
			case DataType::STRING:
				os << '"' << value.AsString( ) << '"';
				break;
			case DataType::OBJECT:
				value.PrintObjectTypeValue( os );
				break;
			case DataType::ARRAY:
				value.PrintArrayTypeValue( os );
				break;
			case DataType::INTERGER:
				[[fallthrough]];
			case DataType::REAL:
				[[fallthrough]];
			case DataType::BOOLEAN:
				[[fallthrough]];
			case DataType::EMPTY:
				os << value.AsString( );
				break;
			default:
				break;
			}

			return os;
		}
	};

	class Reader
	{
	public:
		bool Parse( std::filesystem::path filePath, Value& root )
		{
			std::ifstream jsonFile( filePath );

			if ( jsonFile.good( ) )
			{
				return Parse( jsonFile, root );
			}

			return false;
		}

		bool Parse( std::istream& file, Value& root )
		{
			std::string contents;
			std::getline( file, contents, static_cast<char>( EOF ) );

			return Parse( contents.data(), contents.size(), root );
		}

		bool Parse( char* contents, size_t length, Value& root )
		{
			m_begin = contents;
			m_end = contents + length;
			m_current = m_begin;

			while ( m_valueStack.empty( ) == false )
			{
				m_valueStack.pop( );
			}

			m_valueStack.push( &root );

			ReadValue( );

			return true;
		}

		friend std::istream& operator>>( std::istream& is, Value& value )
		{
			Reader reader;
			reader.Parse( is, value );
			return is;
		}

	private:
		enum class TokenType
		{
			BRACE_OPEN = 1,			// {
			BRACE_CLOSE,			// }
			COLON,					
			SQUARE_BRACKET_OPEN,	// [
			SQUARE_BRACKET_CLOSE,	// ]
			COMMA,					// ,
			NUMBER,
			STRING,
			TRUE,					// true
			FALSE,					// false
			EMPTY					// null
		};

		struct Token
		{
			TokenType m_type;
			char* m_begin = nullptr;
			char* m_end = nullptr;
		};

		Token ReadToken( )
		{
			SkipWhiteSpace( );

			Token token;
			token.m_begin = m_current;

			switch ( GetNextChar( ) )
			{
			case '{':
				token.m_type = TokenType::BRACE_OPEN;
				break;
			case '}':
				token.m_type = TokenType::BRACE_CLOSE;
				break;
			case ':':
				token.m_type = TokenType::COLON;
				break;
			case '[':
				token.m_type = TokenType::SQUARE_BRACKET_OPEN;
				break;
			case ']':
				token.m_type = TokenType::SQUARE_BRACKET_CLOSE;
				break;
			case ',':
				token.m_type = TokenType::COMMA;
				break;
			case '0':
				[[fallthrough]];
			case '1':
				[[fallthrough]];
			case '2':
				[[fallthrough]];
			case '3':
				[[fallthrough]];
			case '4':
				[[fallthrough]];
			case '5':
				[[fallthrough]];
			case '6':
				[[fallthrough]];
			case '7':
				[[fallthrough]];
			case '8':
				[[fallthrough]];
			case '9':
				[[fallthrough]];
			case '-':
				token.m_type = TokenType::NUMBER;
				ReadNumber( );
				break;
			case '"':
				token.m_type = TokenType::STRING;
				ReadString( );
				break;
			case 't':
				if ( MatchNextString( "rue", 3 ) )
				{
					token.m_type = TokenType::TRUE;
				}
				break;
			case 'f':
				if ( MatchNextString( "alse", 4 ) )
				{
					token.m_type = TokenType::FALSE;
				}
				break;
			case 'n':
				if ( MatchNextString( "ull", 3 ) )
				{
					token.m_type = TokenType::EMPTY;
				}
				break;
			default:
				__debugbreak( );
				break;
			}

			token.m_end = m_current;
			return token;
		}

		char GetNextChar( )
		{
			return ( m_current == m_end ) ? 0 : *m_current++;
		}

		bool MatchNextString( const char* str, size_t length )
		{
			if ( m_end - m_current < static_cast<int>( length ) )
			{
				return false;
			}

			for ( size_t i = 0; i < length; ++i )
			{
				if ( m_current[i] != str[i] )
				{
					return false;
				}
			}

			m_current += length;
			return true;
		}

		void SkipWhiteSpace( )
		{
			while ( m_current != m_end )
			{
				char c = *m_current;
				if ( c == ' ' || c == '\t' || c == '\n' || c == '\r' )
				{
					++m_current;
				}
				else
				{
					break;
				}
			}
		}

		void ReadValue( )
		{
			Token token = ReadToken( );

			switch ( token.m_type )
			{
			case TokenType::BRACE_OPEN:
				ReadObject( );
				break;
			case TokenType::SQUARE_BRACKET_OPEN:
				ReadArray( );
				break;
			case TokenType::NUMBER:
				CurrentValue( ) = DecodeNumberValue( token );
				break;
			case TokenType::STRING:
				CurrentValue( ) = DecodeStringValue( token );
				break;
			case TokenType::TRUE:
				{
					Value value( DataType::BOOLEAN );
					value = true;
					CurrentValue( ) = value;
				}
				break;
			case TokenType::FALSE:
				{
					Value value( DataType::BOOLEAN );
					value = false;
					CurrentValue( ) = value;
				}
				break;
			case TokenType::EMPTY:
				CurrentValue( ) = Value( DataType::EMPTY );
				break;
			default:
				__debugbreak( );
				break;
			}
		}

		void ReadNumber( )
		{
			char c = *m_current;

			if ( c == '-' )
			{
				c = ( m_current != m_end ) ? *( ++m_current ) : '\0';
			}

			if ( c == '0' )
			{
				c = ( m_current != m_end ) ? *( ++m_current ) : '\0';
				if ( c == '.' )
				{
					c = ( m_current != m_end ) ? *( ++m_current ) : '\0';
					while ( c >= '0' && c <= '9' )
					{
						c = ( m_current != m_end ) ? *( ++m_current ) : '\0';
					}
				}
			}
			else if ( c >= '1' && c <= '9' )
			{
				c = ( m_current != m_end ) ? *( ++m_current ) : '\0';
				while ( c >= '0' && c <= '9' )
				{
					c = ( m_current != m_end ) ? *( ++m_current ) : '\0';
				}
			}

			if ( c == 'e' || c == 'E' )
			{
				c = ( m_current != m_end ) ? *( ++m_current ) : '\0';
				if ( c == '+' || c == '-' )
				{
					c = ( m_current != m_end ) ? *( ++m_current ) : '\0';
				}

				while ( c >= '0' && c <= '9' )
				{
					c = ( m_current != m_end ) ? *( ++m_current ) : '\0';
				}
			}
		}

		void ReadString( )
		{
			char c = *m_current;

			while ( m_current != m_end )
			{
				c = GetNextChar( );
				if ( c == '\\' )
				{
					c = GetNextChar( );
				}
				if ( c == '"' )
				{
					break;
				}
			}
		}

		void ReadObject( )
		{
			Value init( DataType::OBJECT );
			CurrentValue( ) = init;

			SkipWhiteSpace( );

			if ( ( m_current != m_end ) && ( *m_current == '}' ) )
			{
				Token endToken = ReadToken( );
				return;
			}

			while ( true )
			{
				Token keyString = ReadToken( );

				if ( keyString.m_type != TokenType::STRING )
				{
					__debugbreak( );
				}

				Value& value = CurrentValue( )[DecodeString( keyString )];

				Token colon = ReadToken( );

				if ( colon.m_type != TokenType::COLON )
				{
					__debugbreak( );
				}

				m_valueStack.push( &value );

				ReadValue( );

				m_valueStack.pop( );

				Token nextToken = ReadToken( );

				if ( nextToken.m_type != TokenType::BRACE_CLOSE &&
					nextToken.m_type != TokenType::COMMA )
				{
					__debugbreak( );
				}

				if ( nextToken.m_type == TokenType::BRACE_CLOSE )
				{
					break;
				}
			}
		}

		void ReadArray( )
		{
			Value init( DataType::ARRAY );
			CurrentValue( ) = init;
			
			SkipWhiteSpace( );

			if ( ( m_current != m_end ) && ( *m_current == ']' ) )
			{
				Token endToken = ReadToken( );
				return;
			}

			int index = 0;
			while ( true )
			{
				Value& value = CurrentValue( )[index++];
				m_valueStack.push( &value );

				ReadValue( );

				m_valueStack.pop( );

				Token nextToken = ReadToken( );

				if ( nextToken.m_type != TokenType::SQUARE_BRACKET_CLOSE &&
					nextToken.m_type != TokenType::COMMA )
				{
					__debugbreak( );
				}

				if ( nextToken.m_type == TokenType::SQUARE_BRACKET_CLOSE )
				{
					break;
				}
			}
		}

		Value DecodeNumberValue( const Token& token )
		{
			char* current = token.m_begin;
			bool isNegative = ( *current ) == '-';
			if ( isNegative )
			{
				++current;
			}

			int value = 0;
			while ( current != token.m_end )
			{
				char c = *current++;
				if ( c < '0' || c > '9' )
					return DecodeRealValue( token );

				int digit = static_cast<int>( c - '0' );
				value = value * 10 + digit;
			}

			Value decoded( DataType::INTERGER );
			decoded = isNegative ? -value : value;
			
			return decoded;
		}

		Value DecodeRealValue( const Token& token )
		{
			double value;
			std::string buffer( token.m_begin, token.m_end );
			std::istringstream ss( buffer );

			ss >> value;

			Value decoded( DataType::REAL );
			decoded = value;
			return decoded;
		}

		Value DecodeStringValue( const Token& token )
		{
			Value decoded( DataType::STRING );
			decoded = std::move( DecodeString( token ) );
			return decoded;
		}

		std::string DecodeString( const Token& token )
		{
			std::string decoded;
			decoded.reserve( token.m_end - token.m_begin - 2 );

			char* current = token.m_begin + 1;
			char* end = token.m_end - 2;

			while ( current <= end )
			{
				char c = *current++;

				if ( c == '\\' )
				{
					if ( current <= end )
					{
						c = *current++;
					}

					switch ( c )
					{
					case '"':
						decoded += '\"';
						break;
					case '\\':
						decoded += '\\';
						break;
					case '/':
						decoded += '/';
						break;
					case 'b':
						decoded += '\b';
						break;
					case 'f':
						decoded += '\f';
						break;
					case 'n':
						decoded += '\n';
						break;
					case 'r':
						decoded += '\r';
						break;
					case 't':
						decoded += '\t';
						break;
					case 'u':
						{
							unsigned int unicode = decodeUnicodeCodePoint( current, end );
							decoded += CodePointToUTF8( unicode );
						}
						break;
					default:
						__debugbreak( );
						break;
					}
				}
				else
				{
					decoded += c;
				}
			}

			return decoded;
		}

		unsigned int decodeUnicodeCodePoint( const char* current, const char* end )
		{
			unsigned int unicode = decodeUnicodeEscapeSequence( current, end );

			if ( unicode >= 0xD800 && unicode <= 0xDBFF )
			{
				if ( end - current < 6 )
				{
					__debugbreak( );
				}

				if ( *( current++ ) == '\\' && *( current++ ) == 'u' )
				{
					unsigned int surrogatePair = decodeUnicodeEscapeSequence( current, end );
					assert( surrogatePair >= 0xDC00 && surrogatePair <= 0xDFFF );
					// https://en.wikipedia.org/wiki/UTF-16#U+010000_to_U+10FFFF
					unicode = 0x10000 + ( ( unicode & 0x3FF ) << 10 ) + ( surrogatePair & 0x3FF );
				}
			}

			return unicode;
		}

		unsigned int decodeUnicodeEscapeSequence( const char* current, const char* end )
		{
			if ( end - current < 4 )
			{
				__debugbreak( );
			}

			unsigned int unicode = 0;
			for ( int i = 0; i < 4; ++i )
			{
				char c = *current++;
				unicode *= 16;
				if ( c >= '0' && c <= '9' )
				{
					unicode += c - '0';
				}
				else if ( c >= 'a' && c <= 'f' )
				{
					unicode += c - 'a' + 10;
				}
				else if ( c >= 'A' && c <= 'F' )
				{
					unicode += c - 'A' + 10;
				}
				else
				{
					__debugbreak( );
				}
			}

			return unicode;
		}

		Value& CurrentValue( )
		{
			return *m_valueStack.top( );
		}

		char* m_begin = nullptr;
		char* m_end = nullptr;
		char* m_current = nullptr;

		std::stack<Value*> m_valueStack;
	};
}