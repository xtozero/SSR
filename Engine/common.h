#pragma once

#include <string>

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL __declspec(dllexport)
#else
#define ENGINE_DLL __declspec(dllimport)
#endif

#ifndef UNICODE  
typedef std::string String;
typedef std::ifstream Ifstream;
#define Cout std::cout
#else
typedef std::wstring String;
typedef std::wifstream Ifstream;
#define Cout std::wcout
#endif