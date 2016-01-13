#pragma once

class CDeskIcon
{
protected:
	HICON m_hIcon;
public:
	CDeskIcon(void);
	~CDeskIcon(void);
	
	static HICON GetFileIcon(LPCWSTR pszFilePath);
	static CDeskIcon * CreateDeskIcon( LPCWSTR pszTargetFile );

	BOOL SetDeskIcon(HICON hIcon);
	HICON GetDeskIcon();
};
