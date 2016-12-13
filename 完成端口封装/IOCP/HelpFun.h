#include <atlstr.h>

#if defined(DEBUG) || defined(_DEBUG)
#define DebugStringEx(a,b) DebugStringA("%s",__FUNCTION__); DebugString(a, b);

#ifdef UNICODE
#define DebugString DebugStringW
#else
#define DebugString DebugStringA
#endif

VOID DebugStringW(const WCHAR* fmt, ...);
VOID DebugStringA(const CHAR* fmt, ...);

#else
#define DebugString(a,b) 
#define DebugStringW(a,b)
#define DebugStringA(a,b)
#define DebugStringEx(a,b)

#endif

int DivisionString(CStringA strSeparate, CStringA strSourceString, CStringA * pStringArray, int nArrayCount);
int GetRandValue(int nMin ,int nMax);