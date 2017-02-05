#pragma once

#include <string>
#include <tchar.h>

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL __declspec(dllexport)
#define ENGINE_FUNC_DLL extern "C" __declspec(dllexport)
#else
#define ENGINE_DLL __declspec(dllimport)
#define ENGINE_FUNC_DLL extern "C" __declspec(dllimport)
#endif

#ifndef UNICODE
using String = std::string;
using Ifstream = std::Ifstream;
#define TO_STRING( x ) std::to_string( x )
#else
using String = std::wstring;
using Ifstream = std::wifstream;
#define TO_STRING( x ) std::to_wstring( x )
#endif