#include "stdafx.h"
#include <atlbase.h>

BOOL RunAsFastMode(CString strExeName,DWORD dwMode)
{
	BOOL bRes = FALSE;
	CRegKey setval;
	if(setval.Open(HKEY_LOCAL_MACHINE,TEXT("Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl"),KEY_ALL_ACCESS) == ERROR_SUCCESS)//
	{
		if(setval.Create(setval,TEXT("FEATURE_BROWSER_EMULATION")) == ERROR_SUCCESS)
		{
			bRes = setval.SetDWORDValue(strExeName,dwMode);
		}
		setval.Close();
	}
	return bRes == ERROR_SUCCESS;
}

BOOL SetBrowserFeature(HKEY hkey,LPCTSTR pszFeatureName,LPCTSTR pszBrowserName,DWORD dwValue,LPCTSTR pszBrowserName2 = NULL,LPCTSTR pszBrowserName3 = NULL)
{
    CRegKey setval;

    if(setval.Open(hkey,TEXT("Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl"),KEY_ALL_ACCESS) == ERROR_SUCCESS)
    {
        setval.Close();
    }
    else
    {
        if(setval.Open(hkey,TEXT("Software\\Microsoft\\Internet Explorer\\Main"),KEY_ALL_ACCESS) == ERROR_SUCCESS)
        {
            setval.Create(setval,TEXT("FeatureControl"));
        }
    }
    setval.Close();

    if(setval.Open(hkey,TEXT("Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl"),KEY_ALL_ACCESS) == ERROR_SUCCESS)//
    {
        if(setval.Create(setval,pszFeatureName) == ERROR_SUCCESS)
        {
            setval.SetDWORDValue(pszBrowserName,dwValue);
            if(pszBrowserName2!=NULL)
            {
                setval.SetDWORDValue(pszBrowserName2,dwValue);
            }
            if(pszBrowserName3!=NULL)
            {
                setval.SetDWORDValue(pszBrowserName3,dwValue);
            }
        }
        setval.Close();
    }

    return TRUE;
}

// 设置IE属性
BOOL SetInternetAttribute()
{
    CRegKey setval;

    //允许提交非加密表单数据
    if(setval.Open(HKEY_CURRENT_USER,TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),KEY_ALL_ACCESS) == ERROR_SUCCESS)//
    {
        setval.SetDWORDValue(TEXT("1601"),0);
        setval.Close();
    }

    if(setval.Open(HKEY_LOCAL_MACHINE,TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),KEY_ALL_ACCESS) == ERROR_SUCCESS)//
    {
        setval.SetDWORDValue(TEXT("1601"),0);
        setval.Close();
    }

    return TRUE;
}

DWORD GetIEVersion(void)
{
	HKEY hKEY;  	

	long ret0=(::RegOpenKeyExW(HKEY_LOCAL_MACHINE,TEXT("SOFTWARE\\Microsoft\\Internet Explorer\\"), 
		0, KEY_READ, &hKEY)); 
	if(ret0!=ERROR_SUCCESS)
	{
		DWORD errorcode=::GetLastError();
		return 0;
	} 

	CHAR  chData[1024]; 
	DWORD dwDataType=REG_SZ;
	DWORD cbData=1024;

	ZeroMemory(chData,1024);

	LONG  ret = ::RegQueryValueExA(hKEY, "svcVersion", NULL, &dwDataType, (LPBYTE)chData, &cbData); 
	if(ret != ERROR_SUCCESS)//8以后的版本
	{
		ZeroMemory(chData,1024);
		dwDataType=REG_SZ;
		cbData = 1024;
		ret = ::RegQueryValueExA(hKEY, "Version", NULL, &dwDataType, (LPBYTE)chData, &cbData); 	
	}

	int i=0;
	for (;i< 1024;i++)
	{
		if (chData[i] == '.')
		{
			chData[i] = 0;
			break;
		}
	}

	DWORD dwMajarVersion = 0;

	if ( i<=2 && i > 0 )
	{
		dwMajarVersion = atoi(chData);
	}

	return dwMajarVersion;
}

BOOL RegisterBrowserEmulationMode(BOOL bFocusMode)
{
	int nMajarVersion = GetIEVersion();
#ifdef DEBUG
	CString msgout;
	msgout.Format(L"当前IE主版本号为%d\n",nMajarVersion);
	OutputDebugStringW(msgout);
#endif
	if (nMajarVersion >=6 && nMajarVersion <= 12)
	{
		DWORD dwValue = 0;
		if ( nMajarVersion < 10 )
		{
			if(bFocusMode)
				dwValue = 1111*nMajarVersion;
			else
				dwValue = 1000*nMajarVersion;
		}
		else
		{
			if(bFocusMode)
				dwValue = 1000*nMajarVersion+1;
			else
				dwValue = 1000*nMajarVersion;
		}

		CString strExeName;
		GetModuleFileNameW(NULL,strExeName.GetBuffer(MAX_PATH),MAX_PATH);
		strExeName.ReleaseBuffer();

        strExeName = strExeName.Right(strExeName.GetLength() - strExeName.ReverseFind(L'\\')-1);
        RunAsFastMode(strExeName,dwValue);
        
        LPCTSTR pszName2 = strExeName.GetBuffer();
        LPCTSTR pszName3 = NULL;


        if(nMajarVersion>=7)
        {
            SetBrowserFeature(HKEY_CURRENT_USER,L"FEATURE_MAXCONNECTIONSPER1_0SERVER",strExeName,10,pszName2,pszName3);
            SetBrowserFeature(HKEY_CURRENT_USER,L"FEATURE_MAXCONNECTIONSPERSERVER",strExeName,10,pszName2,pszName3);
            SetBrowserFeature(HKEY_LOCAL_MACHINE,L"FEATURE_ACTIVEX_REPURPOSEDETECTION",strExeName,1,pszName2,pszName3);
            SetBrowserFeature(HKEY_LOCAL_MACHINE,L"FEATURE_BLOCK_LMZ_IMG",strExeName,1,pszName2,pszName3);
            SetBrowserFeature(HKEY_LOCAL_MACHINE,L"FEATURE_BLOCK_LMZ_OBJECT",strExeName,1,pszName2,pszName3);
            SetBrowserFeature(HKEY_LOCAL_MACHINE,L"FEATURE_BLOCK_LMZ_SCRIPT",strExeName,1,pszName2,pszName3);
            SetBrowserFeature(HKEY_LOCAL_MACHINE,L"FEATURE_Cross_Domain_Redirect_Mitigation",strExeName,1,pszName2,pszName3);
            SetBrowserFeature(HKEY_LOCAL_MACHINE,L"FEATURE_ENABLE_SCRIPT_PASTE_URLACTION_IF_PROMPT",strExeName,1,pszName2,pszName3);
            SetBrowserFeature(HKEY_LOCAL_MACHINE,L"FEATURE_GPU_RENDERING",strExeName,1,pszName2,pszName3);
            SetBrowserFeature(HKEY_LOCAL_MACHINE,L"FEATURE_LOCALMACHINE_LOCKDOWN",strExeName,1,pszName2,pszName3);
            SetBrowserFeature(HKEY_LOCAL_MACHINE,L"FEATURE_MAXCONNECTIONSPER1_0SERVER",strExeName,10,pszName2,pszName3);
            SetBrowserFeature(HKEY_LOCAL_MACHINE,L"FEATURE_MAXCONNECTIONSPERSERVER",strExeName,10,pszName2,pszName3);
        }
	}
	return TRUE;
}

BOOL BrowserFix()
{
    SetInternetAttribute();

    CString strExeName;
    GetModuleFileNameW(NULL,strExeName.GetBuffer(MAX_PATH),MAX_PATH);
    strExeName.ReleaseBuffer();

    strExeName = strExeName.Right(strExeName.GetLength() - strExeName.ReverseFind(L'\\')-1);
    SetBrowserFeature(HKEY_LOCAL_MACHINE,L"FEATURE_SCRIPTURL_MITIGATION",strExeName,1);

    return TRUE;
}