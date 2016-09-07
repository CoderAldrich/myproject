#include "stdafx.h"

#include "XunLeiDownloader.h"


CXunLeiDownloader::CXunLeiDownloader( LPCWSTR pszName,LPCWSTR pszPid )
{
	m_strName = pszName;
	m_strPid = pszPid;
}
CXunLeiDownloader::~CXunLeiDownloader()
{

}
CString CXunLeiDownloader::GetName(void)
{
	return m_strName;
}

CString CXunLeiDownloader::GetPid(void)
{
	return m_strPid;
}
