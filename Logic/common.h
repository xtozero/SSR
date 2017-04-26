#pragma once

#include <cstdio>
#include <cstdarg>
#include <iostream>
#include <DirectXMath.h>

#ifdef LOGIC_EXPORTS
#define LOGIC_DLL __declspec(dllexport)
#define LOGIC_FUNC_DLL extern "C" __declspec(dllexport)
#else
#define LOGIC_DLL __declspec(dllimport)
#define LOGIC_FUNC_DLL extern "C" __declspec(dllimport)
#endif

#ifndef UNICODE
using String = std::string;
#else
using String = std::wstring;
#endif
