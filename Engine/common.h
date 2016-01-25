#pragma once

#include <string>

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL __declspec(dllexport)
#else
#define ENGINE_DLL __declspec(dllimport)
#endif

#ifdef ENGINE_EXPORTS
#define ENGINE_FUNC_DLL extern "C" __declspec(dllexport)
#else
#define ENGINE_FUNC_DLL extern "C" __declspec(dllimport)
#endif

#ifndef UNICODE  
typedef std::string String;
typedef std::ifstream Ifstream;
#else
typedef std::wstring String;
typedef std::wifstream Ifstream;
#endif