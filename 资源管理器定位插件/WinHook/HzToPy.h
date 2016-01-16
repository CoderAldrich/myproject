#pragma once
#include <atlstr.h>
class CHzToPy
{
public:
	CHzToPy(void);
	~CHzToPy(void);
	char convert(wchar_t n);
	bool In(wchar_t start, wchar_t end, wchar_t code);
	CString HzToPinYin(CString str);
	CString PinYin;
};