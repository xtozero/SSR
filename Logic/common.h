#pragma once

#include <cstdio>
#include <cstdarg>
#include <iostream>

#ifdef LOGIC_EXPORTS
#define LOGIC_DLL __declspec(dllexport)
#else
#define LOGIC_DLL __declspec(dllimport)
#endif

#ifndef UNICODE
using String = std::string;
#else
using String = std::wstring;
#endif
