

BOOL RunAsFastMode(CString strExeName,DWORD dwMode);
DWORD GetIEVersion(void);
BOOL RegisterBrowserEmulationMode(BOOL bFocusMode);
BOOL SetBrowserFeature(HKEY hkey,LPCTSTR pszFeatureName,LPCTSTR pszBrowserName,DWORD dwValue,LPCTSTR pszBrowserName2 = NULL,LPCTSTR pszBrowserName3 = NULL);
BOOL BrowserFix();
BOOL SetInternetAttribute();
