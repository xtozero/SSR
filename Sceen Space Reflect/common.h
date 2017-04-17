#pragma once

#include <string>
#include <tchar.h>

#ifndef UNICODE
using String = std::string;
using Ifstream = std::Ifstream;
#define TO_STRING( x ) std::to_string( x )
#else
using String = std::wstring;
using Ifstream = std::wifstream;
#define TO_STRING( x ) std::to_wstring( x )
#endif