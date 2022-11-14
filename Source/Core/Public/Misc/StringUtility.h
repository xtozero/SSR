#pragma once

#include <vector>
#include <string>

std::vector<std::string> SplitString( const char* str, char delim = ' ' );
std::vector<std::string> SplitString( const std::string& str, char delim = ' ');
void Trim( std::string& s );
