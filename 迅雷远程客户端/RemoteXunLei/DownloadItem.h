#pragma once

class CDownloadItem
{
public:
	CDownloadItem( LPCWSTR pszFileName , LPCWSTR pszLoadUrl );
	~CDownloadItem(void);
protected:
	CString m_strFileName;
	CString m_strLoadUrl;
};
