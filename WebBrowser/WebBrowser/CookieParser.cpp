#include "StdAfx.h"
#include "CookieParser.h"
#include "UrlParser.h"
#include "HelpFun.h"


CCookieParser::CCookieParser()
{
	m_bSecure = FALSE;
	m_bHttpOnly = FALSE;
	m_bSessionCookie = TRUE;
}
CCookieParser::~CCookieParser(void)
{

}

int CCookieParser::CompareTime(SYSTEMTIME *pTime1,SYSTEMTIME *pTime2)
{
	if ( pTime1 && pTime2 )
	{
		if ( pTime1->wYear > pTime2->wYear )
		{
			return 1;
		}

		if ( pTime1->wYear < pTime2->wYear )
		{
			return -1;
		}

		if ( pTime1->wMonth > pTime2->wMonth )
		{
			return 1;
		}

		if ( pTime1->wMonth < pTime2->wMonth )
		{
			return -1;
		}

		if ( pTime1->wDay > pTime2->wDay )
		{
			return 1;
		}

		if ( pTime1->wDay < pTime2->wDay )
		{
			return -1;
		}

		if ( pTime1->wHour > pTime2->wHour )
		{
			return 1;
		}

		if ( pTime1->wHour < pTime2->wHour )
		{
			return -1;
		}

		if ( pTime1->wMinute > pTime2->wMinute )
		{
			return 1;
		}

		if ( pTime1->wMinute < pTime2->wMinute )
		{
			return -1;
		}

		if ( pTime1->wSecond > pTime2->wSecond )
		{
			return 1;
		}

		if ( pTime1->wSecond < pTime2->wSecond )
		{
			return -1;
		}

		return 0;
	}

	return 0;
}

BOOL CCookieParser::ParseExpiresTime(LPCSTR pchExpires,SYSTEMTIME *ptmExpires)
{
	BOOL bParserRes = FALSE;
	do 
	{
		if ( NULL == ptmExpires )
		{
			break;
		}

		ZeroMemory(ptmExpires,sizeof(SYSTEMTIME));

		CStringA strExpires;
		strExpires = pchExpires;

		static LPCSTR pchArrWeek[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
		static LPCSTR pchArrMonth[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

		CStringA strExpiresParts[10];
		int     nCurIndex = 0;
		CStringA strNode;
		int curPos = 0;
		strNode= strExpires.Tokenize(": , -",curPos);
		while (strNode != _T(""))
		{
			if (nCurIndex >= _countof(strExpiresParts))
			{
				break;
			}
			strExpiresParts[nCurIndex] = strNode;
			nCurIndex++;
			strNode = strExpires.Tokenize(": , -", curPos);
		};

		if ( nCurIndex != 8 )
		{
			break;
		}


		BOOL bWeekOk = FALSE;
		for (int i=0;i<_countof(pchArrWeek);i++)
		{
			if (strExpiresParts[0].CompareNoCase(pchArrWeek[i]) == 0 )
			{
				bWeekOk = TRUE;
				ptmExpires->wDayOfWeek = i;
				break;
			}
		}

		if(FALSE == bWeekOk) break;


		ptmExpires->wDay = atoi(strExpiresParts[1]);

		BOOL bMonthOk = FALSE;
		for (int i=0;i<_countof(pchArrMonth);i++)
		{
			if (strExpiresParts[2].CompareNoCase(pchArrMonth[i]) == 0 )
			{
				bMonthOk = TRUE;
				ptmExpires->wMonth = i+1;
				break;
			}
		}

		if(FALSE == bMonthOk) break;

		ptmExpires->wYear = atoi(strExpiresParts[3]);
		ptmExpires->wHour = atoi(strExpiresParts[4]);
		ptmExpires->wMinute = atoi(strExpiresParts[5]);
		ptmExpires->wSecond = atoi(strExpiresParts[6]);
		ptmExpires->wMilliseconds = 0;

		bParserRes = strExpiresParts[7].CompareNoCase("GMT")==0;

	} while (FALSE);

	return bParserRes;
}

BOOL CCookieParser::ParserCookieString(LPCSTR pchUrl,LPCSTR pszCookie)
{
	CStringA strCookieData;
	strCookieData = pszCookie;

	CStringA strArrayCookieParts[10];
	int nPartCount = DivisionStringA(";",strCookieData,strArrayCookieParts,10);

	for (int i=0;i<nPartCount;i++)
	{
		CStringA strArrayParts[2];
		DivisionStringA("=",strArrayCookieParts[i].Trim(),strArrayParts,2);

		if (strArrayParts[0].CompareNoCase("domain") == 0)
		{
			m_strDomain = strArrayParts[1];
			if (m_strDomain.GetAt(0) != '.')
			{
				m_strDomain = "."+m_strDomain;
			}
			continue;
		}

		if (strArrayParts[0].CompareNoCase("path") == 0)
		{
			m_strPath = strArrayParts[1];
			continue;
		}

		if (strArrayParts[0].CompareNoCase("secure") == 0)
		{
			m_bSecure = TRUE;
			continue;
		}
		
		if (strArrayParts[0].CompareNoCase("expires") == 0)
		{
			CStringA strExpires;
			strExpires=strArrayParts[1];

			BOOL bRes = ParseExpiresTime(strExpires,&m_tmExpires);
			if( bRes )
			{
				m_bSessionCookie = FALSE;
			}
			continue;
		}

		if (strArrayParts[0].CompareNoCase("httponly") == 0)
		{
			m_bHttpOnly = TRUE;
			continue;
		}

		if ( m_strCookieName.GetLength() == 0 )
		{
			int nCookieNameEnd = strArrayCookieParts[i].Find("=");
			if ( nCookieNameEnd > 0 )
			{
				m_strCookieName = strArrayCookieParts[i].Left(nCookieNameEnd);
				m_strCookieValue = strArrayCookieParts[i].Right(strArrayCookieParts[i].GetLength() - nCookieNameEnd-1);
			}
		}
	}

	CUrlParser urlParser;
	urlParser.ParseUrl(pchUrl);

	if( m_strDomain.GetLength() == 0 )
	{
		m_strDomain = urlParser.GetDomain();
		m_strDomain = "."+m_strDomain;
	}

	if ( m_strPath.GetLength() == 0 )
	{
		m_strPath = urlParser.GetPath();
	}

	return TRUE;
}
BOOL CCookieParser::CheckUrlMatch(LPCSTR pchUrl,BOOL bJsCheck)
{
	BOOL bMatch = FALSE;
	do 
	{
		// get domain
		CUrlParser urlParser;
		urlParser.ParseUrl(pchUrl);


		CStringA strDomain = urlParser.GetDomain();
		CStringA strOrgDomain = m_strDomain;
		CStringA strPath = urlParser.GetPath();
		CStringA strProtocol = urlParser.GetProtocol();

		strDomain = "."+strDomain;
		strDomain.MakeReverse();
		strOrgDomain.MakeReverse();
		
		if (strDomain.Find(strOrgDomain) != 0)
		{
			//域名不匹配
			break;
		}

		if(strPath.Find(m_strPath) != 0 )
		{
			//路径不匹配
			break;
		}
		
		if ( ( m_bSecure && strProtocol.CompareNoCase("https") != 0 ) ||  ( FALSE == m_bSecure  && strProtocol.CompareNoCase("https") == 0 ) )
		{
			//安全属性不匹配
			break;
		}
		
		if ( m_bHttpOnly && bJsCheck )
		{
			// HttpOnly属性不匹配
			break;
		}

		bMatch = TRUE;

	} while (FALSE);

	return bMatch;
}