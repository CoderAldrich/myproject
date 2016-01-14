#include "stdafx.h"
#include "PublicFun.h"

#include <Psapi.h>
#include <winternl.h>
#pragma comment(lib,"Psapi.lib")
/*
���ܣ���INI�ļ��л�ȡ�ַ������ݣ�����Ҫ�������ݵĳ���
������
LPCWSTR lpAppName  [lpAppName]������
LPCWSTR lpKeyName   lpKeyName=������
LPCWSTR lpDefault   Ĭ��ֵ
LPCWSTR lpFileName  �ļ�·��
����ֵ����ȡ�����ַ���
*/
CString GetIniString(
					 LPCWSTR lpAppName,
					 LPCWSTR lpKeyName,
					 LPCWSTR lpDefault,
					 LPCWSTR lpFileName
					 )
{
	CString strTemp;
	DWORD dwBufferLen = 1024;
	DWORD dwReturnLen = 0;
	
	do
	{
		dwBufferLen*=2;
		dwReturnLen = GetPrivateProfileStringW(lpAppName,lpKeyName,lpDefault,strTemp.GetBuffer(dwBufferLen),dwBufferLen,lpFileName);
		strTemp.ReleaseBuffer();
	}while(dwReturnLen > 0 && dwReturnLen == dwBufferLen - 1 /*˵�滺����������*/);

	return strTemp;
}


/*
����:��ȡĬ�������·��
����:
LPWSTR pszBrowPath    ������ָ��
DWORD dwBufferSize    ��������С
����ֵ:TRUE ��ʾ�ɹ���FALSE ��ʾʧ��
*/
BOOL WINAPI GetDefaultBrowserPath(LPWSTR pszBrowPath,DWORD dwBufferSize)
{
	CRegKey hKey,subKey;;
	unsigned long lSize = MAX_PATH;
	WCHAR temp[MAX_PATH]={0,0};
	if(hKey.Open(HKEY_CLASSES_ROOT,L"http\\shell",KEY_READ)==ERROR_SUCCESS) 
	{
		hKey.QueryStringValue(NULL,temp,&lSize);//open\\command

		if(temp[0]!=L'\0')//�����������
		{
			wcscat_s(temp,MAX_PATH,L"\\command");
			if(subKey.Open(hKey,temp,KEY_READ)==ERROR_SUCCESS || subKey.Open(hKey,L"open\\command",KEY_READ)==ERROR_SUCCESS)
			{
				lSize = dwBufferSize;
				subKey.QueryStringValue(NULL,pszBrowPath,&lSize);
				subKey.Close();
			}
		}
		else//û�����������
		{
			hKey.Close();
			if(hKey.Open(HKEY_CLASSES_ROOT,L"http\\shell\\open\\command",KEY_READ)==ERROR_SUCCESS)
			{
				lSize = dwBufferSize;
				LONG res = hKey.QueryStringValue(NULL,pszBrowPath,&lSize);
				hKey.Close();
			}
		}

		hKey.Close();
		return TRUE;
	}
	return FALSE;
} 



BOOL IsWow64()
{
#ifdef _WIN64
	return TRUE;
#endif   
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

	LPFN_ISWOW64PROCESS fnIsWow64Process;
	BOOL bIsWow64 = FALSE;    
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		GetModuleHandle(TEXT("kernel32")),"IsWow64Process");      
	if (NULL != fnIsWow64Process)    
	{        
		if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
		{ 
			// handle error
		}
	}
	return bIsWow64;
}
/*
���ܣ���ȡϵͳ�汾��Ϣ��Ŀǰ��֧��ϵͳ����  win7_x64/x86  winxp_x86
����ֵ��ϵͳ�汾
*/
//
SYSTEM_VERSION GetSystemVersion()
{
	SYSTEM_VERSION version = VERSION_ERROR;
	SYSTEM_INFO info;                //��SYSTEM_INFO�ṹ�ж�64λAMD������
	GetSystemInfo(&info);            //����GetSystemInfo�������ṹ
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX); 
	if(GetVersionEx((OSVERSIONINFO *)&os))                  
	{
		//������ݰ汾��Ϣ�жϲ���ϵͳ����
		switch(os.dwMajorVersion)
		{
		case 5:
			{
				version = VERSION_XP;
			}
			break;
		case 6:
			{
#if WIN64 || _WIN64
				version = VERSION_WIN7_X64;
#else
				if(IsWow64())
				{
					version = VERSION_WIN7_X64;
				}
				else
				{
					version = VERSION_WIN7;
				}
#endif
			}
			break;
		}
	}

	return version;
}

HMODULE ModuleHandleByAddr(const void* ptrAddr)   
{   
   MEMORY_BASIC_INFORMATION info;   
    ::VirtualQuery(ptrAddr, &info, sizeof(info));   
    return (HMODULE)info.AllocationBase;   
}   
/*  
���ܣ���ȡ��ǰģ����
����ֵ����ǰģ����
*/  
HMODULE ThisModuleHandle()   
{   
    static HMODULE sInstance = ModuleHandleByAddr((void*)&ThisModuleHandle);   
    return sInstance;   
}
/*
���ܣ�����Ȩ�޵�DebugȨ��
����ֵ����Ȩ�Ƿ�ɹ�
*/
BOOL EnableDebugPrivilege()
{   
	HANDLE hToken;   
	LUID sedebugnameValue;   
	TOKEN_PRIVILEGES tkp;   
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{   
		return   FALSE;   
	}   
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))  
	{   
		CloseHandle(hToken);   
		return FALSE;   
	}   
	tkp.PrivilegeCount = 1;   
	tkp.Privileges[0].Luid = sedebugnameValue;   
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;   
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL)) 
	{   
		CloseHandle(hToken);   
		return FALSE;   
	}   
	return TRUE;   
}


/*
���ܣ��ϲ��������ģ���ļ�
������
LPCWSTR *ppszArrayIniFile     //������Ҫ�ϲ���INI�ļ�����
int nIniCount                 //INI�ļ�����
LPCWSTR pszOutIniFile         //����ļ�·��
����ֵ���ϲ��Ƿ�ɹ�
*/
#include <map>
using namespace std;



BOOL CombineTemplateFile( LPCWSTR pszTemplateDesc,LPCWSTR *ppszArrayIniFile,int nIniCount ,LPCWSTR pszOutIniFile )
{

	typedef map<CString,CString> MAP_SECTION_NODE;
	typedef MAP_SECTION_NODE::iterator MAP_SECTION_NODE_PTR;

	typedef map<CString,MAP_SECTION_NODE> MAP_SECTION_NAMES;
	typedef MAP_SECTION_NAMES::iterator MAP_SECTION_NAMES_PTR;



	MAP_SECTION_NAMES mapSectionNames;

	int nCalcCount = 0;
	MAP_SECTION_NAMES mapCalcCount[10];

	for (int i=0;i<nIniCount;i++)
	{
		WCHAR szSectionNames[2000];
		GetPrivateProfileSectionNamesW(szSectionNames,2000, ppszArrayIniFile[i]);
		int nIndex = 0;
		while ( *(szSectionNames+nIndex) != 0 )
		{
			CString strSectionName;
			strSectionName = szSectionNames+nIndex;
			nIndex+=strSectionName.GetLength()+1;


			BOOL bFoundSection = FALSE;
			MAP_SECTION_NAMES_PTR it;
			for ( it = mapSectionNames.begin();it!=mapSectionNames.end();it++)
			{
				if ( it->first.CompareNoCase(strSectionName) == 0 )
				{
					bFoundSection = TRUE;
					break;
				}
			}

			if (bFoundSection)
			{
				int nNodeIndex = 0;
				WCHAR szNodeNames[2000];
				GetPrivateProfileStringW(strSectionName,NULL,NULL,szNodeNames,2000,ppszArrayIniFile[i]);
				while ( *(szNodeNames+nNodeIndex) != 0 )
				{
					CString strNodeName;
					CString strNodeValue;
					strNodeName = szNodeNames+nNodeIndex;
					strNodeValue = GetIniString(strSectionName,strNodeName,L"",ppszArrayIniFile[i]);

					nNodeIndex+=strNodeName.GetLength()+1;

					BOOL bFoundNode = FALSE;
					MAP_SECTION_NODE_PTR itnote;
					for ( itnote = it->second.begin();itnote!=it->second.end();itnote++)
					{
						if ( itnote->first.CompareNoCase(strNodeName) == 0 )
						{
							bFoundNode = TRUE;
							break;
						}
					}

					if ( FALSE == bFoundNode )
					{
						it->second.insert(make_pair(strNodeName,strNodeValue));
					}
					else
					{
						//���ֿ��ܳ�ͻ��
						CString strNodeAttr = GetIniString(strSectionName,strNodeName,L"00000",pszTemplateDesc);
						if (strNodeAttr.GetLength() == 5)
						{
							if(strNodeAttr.GetAt(1) == L'1')
							{
								//����ϲ�
								if(strNodeAttr.GetAt(0) == L'1')
								{
									//�����������ַ���
									CString strDivChar = strNodeAttr.GetAt(2);
									itnote->second+=strDivChar;
									itnote->second+=strNodeValue;
									int a=0;
								}
							}	
						}
					}
				}
			}
			else
			{
				if ( strSectionName.Find(L"Lock") == 0 )
				{
					MAP_SECTION_NODE Node;
					int nNodeIndex = 0;
					WCHAR szNodeNames[2000];
					GetPrivateProfileStringW(strSectionName,NULL,NULL,szNodeNames,2000,ppszArrayIniFile[i]);
					while ( *(szNodeNames+nNodeIndex) != 0 )
					{
						CString strNodeName;
						CString strNodeValue;
						strNodeName = szNodeNames+nNodeIndex;
						strNodeValue = GetIniString(strSectionName,strNodeName,L"",ppszArrayIniFile[i]);

						nNodeIndex+=strNodeName.GetLength()+1;

						Node.insert(make_pair( strNodeName , strNodeValue ));
					}

					mapCalcCount[nCalcCount].insert(make_pair(L"Lock",Node));
					nCalcCount++;
				}
				else
				{
					MAP_SECTION_NODE Node;

					int nNodeIndex = 0;
					WCHAR szNodeNames[2000];
					GetPrivateProfileStringW(strSectionName,NULL,NULL,szNodeNames,2000,ppszArrayIniFile[i]);
					while ( *(szNodeNames+nNodeIndex) != 0 )
					{
						CString strNodeName;
						CString strNodeValue;
						strNodeName = szNodeNames+nNodeIndex;
						strNodeValue = GetIniString(strSectionName,strNodeName,L"",ppszArrayIniFile[i]);

						nNodeIndex+=strNodeName.GetLength()+1;

						Node.insert(make_pair( strNodeName , strNodeValue ));
					}

					mapSectionNames.insert(make_pair(strSectionName,Node));
				}


				int a=0;
			}


		}
	}

	DeleteFile(pszOutIniFile);

	for ( MAP_SECTION_NAMES_PTR it = mapSectionNames.begin();it!=mapSectionNames.end();it++)
	{
		for ( MAP_SECTION_NODE_PTR itnote = it->second.begin();itnote!=it->second.end();itnote++)
		{
			WritePrivateProfileStringW(it->first,itnote->first,itnote->second,pszOutIniFile);
		}
	}

	if ( nCalcCount > 0 )
	{
		CString strTemp;
		strTemp.Format(L"%d",nCalcCount);
		WritePrivateProfileStringW(L"LockInfo",L"LockCount",strTemp,pszOutIniFile);
		for (int i=0;i<nCalcCount;i++)
		{
			for ( MAP_SECTION_NAMES_PTR it = mapCalcCount[i].begin();it!=mapCalcCount[i].end();it++)
			{
				for ( MAP_SECTION_NODE_PTR itnote = it->second.begin();itnote!=it->second.end();itnote++)
				{
					CString strTempSection;
					strTempSection.Format(L"Lock_%d",i);
					WritePrivateProfileStringW(strTempSection,itnote->first,itnote->second,pszOutIniFile);
				}
			}
		}
	}
	

	return FALSE;
}


//�ָ��ַ���
int DivisionString(CString strSeparate, CString strSourceString, CString * pStringArray, int nArrayCount)
{

	if(
		strSeparate.GetLength() == 0 ||
		strSourceString.GetLength() == 0||
		pStringArray == NULL ||
		nArrayCount <=1
		)
	{
		return 0;
	}

	int nCount = 0;
	while(true)
	{
		int nEnd = strSourceString.Find(strSeparate,0);
		if ( nEnd >= 0 )
		{
			pStringArray[nCount] = strSourceString.Left(nEnd);
			nCount++;

			strSourceString = strSourceString.Right(strSourceString.GetLength() - nEnd-strSeparate.GetLength());
		}
		else
		{
			pStringArray[nCount] = strSourceString;
			nCount++;
			strSourceString = L"";

			break;
		}

		if ( nCount >= nArrayCount)
		{
			break;
		}
	}

	return nCount;
}



CString GetGuidString()
{
	CString strGuid;

	GUID Guid;
	CoCreateGuid(&Guid);

	LPOLESTR polestrGuid = NULL;
	StringFromCLSID(Guid,&polestrGuid);
	strGuid = polestrGuid;
	CoTaskMemFree(polestrGuid);

	strGuid.Replace(L"{",L"");
	strGuid.Replace(L"}",L"");

	return strGuid;
}




BOOL GetProcessName(DWORD processID,WCHAR *pszProcessName,DWORD dwBufferLen )
{
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (hProcess == FALSE)
	{
		return FALSE;
	}

	typedef BOOL (WINAPI *TypeQueryFullProcessImageName)(
		HANDLE hProcess,
		DWORD dwFlags,
		LPTSTR lpExeName,
		PDWORD lpdwSize
		);
	static TypeQueryFullProcessImageName pQueryFullProcessImageName = (TypeQueryFullProcessImageName)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"QueryFullProcessImageNameW");
	if (pQueryFullProcessImageName)
	{
		pQueryFullProcessImageName(hProcess,0,pszProcessName,&dwBufferLen);
	}
	else
	{
		DWORD dwRes =  GetModuleFileNameExW(hProcess, NULL, pszProcessName, dwBufferLen);
	}

	CloseHandle(hProcess);

	return TRUE;
}

BOOL GetProcessName(HANDLE hProcess,WCHAR *pszProcessName,DWORD dwBufferLen )
{

	typedef BOOL (WINAPI *TypeQueryFullProcessImageName)(
		HANDLE hProcess,
		DWORD dwFlags,
		LPTSTR lpExeName,
		PDWORD lpdwSize
		);
	static TypeQueryFullProcessImageName pQueryFullProcessImageName = (TypeQueryFullProcessImageName)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"QueryFullProcessImageNameW");
	if (pQueryFullProcessImageName)
	{
		pQueryFullProcessImageName(hProcess,0,pszProcessName,&dwBufferLen);
	}
	else
	{
		DWORD dwRes =  GetModuleFileNameExW(hProcess, NULL, pszProcessName, dwBufferLen);
	}

	return TRUE;
}

typedef NTSTATUS (WINAPI *TypeNtQueryInformationProcess)(
	IN HANDLE ProcessHandle,
	IN PROCESSINFOCLASS ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL 
	);

static TypeNtQueryInformationProcess pNtQueryInformationProcess = (TypeNtQueryInformationProcess)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"NtQueryInformationProcess");

DWORD GetParentPid( DWORD dwPid )
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,dwPid);
	if ( hProcess )
	{
		PROCESS_BASIC_INFORMATION BaseInfo;
		ZeroMemory(&BaseInfo,sizeof(BaseInfo));
		DWORD dwRetLen = 0;
		pNtQueryInformationProcess(hProcess,ProcessBasicInformation,&BaseInfo,sizeof(BaseInfo),&dwRetLen);
		CloseHandle(hProcess);

		return (DWORD)(BaseInfo.Reserved3);
	}

	return 0;
}

DWORD GetParentPid( HANDLE hProcess )
{
	if ( hProcess )
	{
		PROCESS_BASIC_INFORMATION BaseInfo;
		ZeroMemory(&BaseInfo,sizeof(BaseInfo));
		DWORD dwRetLen = 0;
		pNtQueryInformationProcess(hProcess,ProcessBasicInformation,&BaseInfo,sizeof(BaseInfo),&dwRetLen);
		return (DWORD)(BaseInfo.Reserved3);
	}

	return 0;
}

BOOL CheckBrowserExeName(LPCWSTR pszExeName)
{
	CString strExeFileName;
	strExeFileName = pszExeName;
	strExeFileName.MakeLower();

	if (
		strExeFileName.CompareNoCase(L"iexplore.exe")==0 ||
		strExeFileName.CompareNoCase(L"sogouexplorer.exe")==0 ||
		strExeFileName.CompareNoCase(L"firefox.exe")==0 ||
		strExeFileName.CompareNoCase(L"maxthon.exe")==0 ||
		strExeFileName.CompareNoCase(L"theworld.exe")==0 ||
		strExeFileName.CompareNoCase(L"f1browser.exe")==0 ||
		strExeFileName.CompareNoCase(L"360se.exe")==0 ||
		strExeFileName.CompareNoCase(L"360chrome.exe")==0 ||
		strExeFileName.CompareNoCase(L"tango.exe")==0 ||
		strExeFileName.CompareNoCase(L"chrome.exe")==0 ||
		strExeFileName.CompareNoCase(L"ruiying.exe")==0 ||
		strExeFileName.CompareNoCase(L"reying.exe")==0 ||
		strExeFileName.CompareNoCase(L"krbrowser.exe")==0 ||
		strExeFileName.CompareNoCase(L"qqbrowser.exe")==0 ||
		strExeFileName.CompareNoCase(L"liebao.exe")==0 ||
		strExeFileName.CompareNoCase(L"webbrowser.exe")==0 ||
		strExeFileName.CompareNoCase(L"baidubrowser.exe")==0 ||
		strExeFileName.CompareNoCase(L"ucbrowser.exe")==0
		)
	{
		return TRUE;
	}

	return FALSE;

}