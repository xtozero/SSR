#include "StringUtility.h"

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
