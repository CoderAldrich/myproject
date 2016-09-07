#include "StdAfx.h"
#include "DownloadItem.h"

CDownloadItem::CDownloadItem(LPCWSTR pszFileName , LPCWSTR pszLoadUrl)
{
	m_strFileName = pszFileName;
	m_strLoadUrl = pszLoadUrl;
}

CDownloadItem::~CDownloadItem(void)
{
}
