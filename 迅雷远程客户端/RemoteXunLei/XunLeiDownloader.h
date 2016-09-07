#pragma once

class CXunLeiDownloader
{
public:
	CXunLeiDownloader( LPCWSTR pszName,LPCWSTR pszPid );
	~CXunLeiDownloader();

protected:
	CString m_strName;
	CString m_strPid;
	CString m_strPath;
public:


	CString GetName(void);
	CString GetPid(void);
};