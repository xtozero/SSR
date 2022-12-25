#include "StringUtility.h"

#include <algorithm>
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
        std::find_if( std::begin( s ), std::end( s ), 
            []( char c )
            {
                return !std::isspace( c );
            }));
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