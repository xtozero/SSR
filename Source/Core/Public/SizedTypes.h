#pragma once

// Check windows
#if _WIN32 || _WIN64
#if _WIN64
using int8 = signed char;
using int16 = signed short;
using int32 = signed int;
using int64 = signed long long;

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

using uptrint = uint64;
using size_t = uptrint;
#else
using int8 = signed char;
using int16 = signed short;
using int32 = signed int;
using int64 = signed long long;

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

using uptrint = uint32;
using size_t = uptrint;
#endif
#endif

// Check GCC
//#if __GNUC__
//#if __x86_64__ || __ppc64__
//#else
//#endif
//#endif