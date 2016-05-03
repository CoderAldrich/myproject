#pragma once
#include <atlstr.h>
class CCookieParser
{
public:
	CStringA m_strCookieName;
	CStringA m_strCookieValue;
	CStringA m_strDomain;
	CStringA m_strPath;
	SYSTEMTIME m_tmExpires;
	BOOL     m_bSecure;
	BOOL     m_bHttpOnly;
	BOOL     m_bSessionCookie;
public:
	CCookieParser();
	~CCookieParser(void);

	BOOL ParserCookieString(LPCSTR pchUrl,LPCSTR pszCookie);
	static int CompareTime(SYSTEMTIME *pTime1,SYSTEMTIME *pTime2);
	static BOOL ParseExpiresTime(LPCSTR pchExpires,SYSTEMTIME *ptmExpires);

};
