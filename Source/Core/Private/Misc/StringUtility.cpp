#include "StringUtility.h"

#include <algorithm>
#include <regex>
#include <sstream>

std::vector<std::string> SplitString( const char* str, char delim )
{
    std::vector<std::string> splited;

    std::stringstream ss( str );
    std::string item;

    while ( std::getline( ss, item, delim ) )
    {
        if ( item.empty() == false )
        {
            splited.emplace_back( item );
        }
    }

    return splited;
}

std::vector<std::string> SplitString( const std::string& str, char delim )
{
    return SplitString( str.c_str(), delim );
}

void LTrim( std::string& s )
{
    s.erase( std::begin( s ),
             std::ranges::find_if( s,
                                   []( char c )
                                   {
                                       return !std::isspace( c );
                                   } ) );
}

void RTrim( std::string& s )
{
    s.erase( std::find_if( std::rbegin( s ), std::rend( s ),
                           []( char c )
                           {
                               return !std::isspace( c );
                           } ).base(),
             std::end( s ) );
}

void Trim( std::string& s )
{
    LTrim( s );
    RTrim( s );
}

void RemoveSpace( std::string& s )
{
    const auto [begin, end] = std::ranges::remove_if( s,
                            []( char c )
                            {
                                return std::isspace( c );
                            } );
    s.erase( begin, end );
}

std::string ToLower( const std::string& str )
{
    std::string lowercase = str;
    std::ranges::transform( lowercase, std::begin( lowercase ),
                            []( const unsigned char c )
                            {
                                return static_cast<char>( std::tolower( c ) );
                            } );

    return lowercase;
}

std::string ToUpper( const std::string& str )
{
    std::string uppercase = str;
    std::ranges::transform( uppercase, std::begin( uppercase ),
                            []( const unsigned char c )
                            {
                                return static_cast<char>( std::toupper( c ) );
                            } );

    return uppercase;
}

bool ToWideChar( wchar_t* dest, size_t destSize, const char* source )
{
#if _WIN32
    size_t numConverted = 0;
    return mbstowcs_s( &numConverted, dest, destSize, source, destSize ) == 0;
#else
	return std::mbstowcs( dest, source, destSize ) != static_cast<std::size_t>( -1 );
#endif
}

void RegexReplace( std::string& str, const std::string& from, const std::string& to )
{
    if ( from.empty() )
    {
        return;
    }

    str = std::regex_replace( str, std::regex(from), to );
}
