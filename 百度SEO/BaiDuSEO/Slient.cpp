#include "stdafx.h"
#include <winternl.h>
#include <mmsystem.h>
#include <WinIoCtl.h>
#include <Shlwapi.h>

#include "DSOUND.H"
#include "AudioVolume.h"
#pragma comment(lib,"winmm.lib")

#ifdef _WIN64
#include <detours64.h>
#pragma comment(lib,"detours64.lib")
#pragma comment(lib,"dsound64.lib")
#else
#include <detours.h>
#pragma comment(lib,"detours.lib")
#pragma comment(lib,"dsound.lib")
#endif

#include "BrowserHelpFun.h"


#define STATUS_ACCESS_DENIED             ((NTSTATUS)0xC0000022L)



typedef NTSTATUS (*TypeZwCreateProcessEx)(OUT PHANDLE ProcessHandle,IN ACCESS_MASK DesiredAccess,IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
									   IN HANDLE ParentProcess,IN BOOLEAN InheritObjectTable,IN HANDLE SectionHandle OPTIONAL,IN HANDLE DebugPort OPTIONAL,IN HANDLE ExceptionPort OPTIONAL,IN ULONG JobMemberLevel);
typedef NTSTATUS (*TypeZwCreateProcess)(OUT PHANDLE ProcessHandle,IN ACCESS_MASK DesiredAccess,IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
									 IN HANDLE ParentProcess,IN BOOLEAN InheritObjectTable,IN HANDLE SectionHandle OPTIONAL,IN HANDLE DebugPort OPTIONAL,IN HANDLE ExceptionPort OPTIONAL);

TypeZwCreateProcessEx	RealZwCreateProcessEx=NULL;
TypeZwCreateProcess	RealZwCreateProcess=NULL;
MMRESULT (WINAPI *pwaveOutWrite)(HWAVEOUT hwo,LPWAVEHDR pwh,UINT cbwh)=waveOutWrite;
MMRESULT WINAPI MywaveOutWrite(HWAVEOUT hwo,LPWAVEHDR pwh,UINT cbwh)
{
	memset(pwh->lpData,0,pwh->dwBufferLength);
	return pwaveOutWrite(hwo,pwh,cbwh);//MMSYSERR_NOERROR;
}
MMRESULT (WINAPI *pmidiStreamOut)( __in HMIDISTRM hms, __out_bcount(cbmh) LPMIDIHDR pmh, __in UINT cbmh)=midiStreamOut;
MMRESULT WINAPI MymidiStreamOut( __in HMIDISTRM hms, __out_bcount(cbmh) LPMIDIHDR pmh, __in UINT cbmh)
{
	memset(pmh->lpData,0,pmh->dwBufferLength);
	return pmidiStreamOut(hms,pmh,cbmh);//MMSYSERR_NOERROR;
}
typedef HRESULT (WINAPI *pDirectSoundCreate)(LPCGUID lpcGuidDevice,LPDIRECTSOUND* /*LPVOID**/ ppDS8,LPUNKNOWN pUnkOuter);
HRESULT WINAPI MyDirectSoundCreate(LPCGUID lpcGuidDevice,LPDIRECTSOUND* /*LPVOID**/ ppDS8,LPUNKNOWN pUnkOuter)
{
	return S_FALSE;
}
typedef HRESULT (WINAPI *pDirectSoundCreate8)(__in_opt LPCGUID pcGuidDevice, __deref_out LPDIRECTSOUND8 *ppDS8, __null LPUNKNOWN pUnkOuter);
HRESULT WINAPI MyDirectSoundCreate8(__in_opt LPCGUID pcGuidDevice, __deref_out LPDIRECTSOUND8 *ppDS8, __null LPUNKNOWN pUnkOuter)
{
	return S_FALSE;
}
NTSTATUS  MyCreateProcess(  //��������  
						  __out PHANDLE ProcessHandle,   //���̾����ָ��  
						  __in ACCESS_MASK DesiredAccess,  
						  __in_opt POBJECT_ATTRIBUTES ObjectAttributes,  
						  __in HANDLE ParentProcess,    //�����̾��  
						  __in BOOLEAN InheritObjectTable,  //�Ƿ�̳о����  
						  __in_opt HANDLE SectionHandle,  
						  __in_opt HANDLE DebugPort,         //���Զ˿�  
						  __in_opt HANDLE ExceptionPort     //�쳣�˿�  
						  )  
{
	return STATUS_ACCESS_DENIED;
}

NTSTATUS MyZwCreateProcessEx(OUT PHANDLE ProcessHandle,IN ACCESS_MASK DesiredAccess,IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
							 IN HANDLE ParentProcess,IN BOOLEAN InheritObjectTable,IN HANDLE SectionHandle OPTIONAL,IN HANDLE DebugPort OPTIONAL,IN HANDLE ExceptionPort OPTIONAL,IN ULONG JobMemberLevel)
{
	return STATUS_ACCESS_DENIED;
}

BOOL g_bIsIE6Ver = FALSE;

BOOL (WINAPI *pShowWindow)(HWND hWnd,int nCmdShow)=ShowWindow;
BOOL WINAPI MyShowWindow(HWND hWnd,int nCmdShow)
{
	//return TRUE;
 	char temp[50];
 	GetWindowTextA(hWnd,temp,50);
 	if(strstr(temp,"��Ϊ")!=NULL||strstr(temp,"����")!=NULL||strstr(temp,"���")!=NULL||strstr(temp,"��ӡ")!=NULL
 		||strstr(temp,"����")!=NULL||strstr(temp,"Internet Explorer")!=NULL
 		||strstr(temp,"������ҳ")!=NULL)
 	{
 		PostMessageW(hWnd,WM_CLOSE,0,0);
 		PostMessageW(hWnd,WM_SYSCOMMAND,SC_CLOSE,0);
 		return FALSE;
 	}
#ifdef DEBUG
 	return pShowWindow(hWnd,nCmdShow);
#else
	if (g_bIsIE6Ver)
	{
		if (::IsWindow(hWnd))
		{
			CString szClassName;
			GetClassName(hWnd,szClassName.GetBuffer(MAX_CLASS_NAME),MAX_CLASS_NAME);
			szClassName.ReleaseBuffer();
			if (!szClassName.IsEmpty())
			{
				szClassName.MakeLower();
				if (szClassName == _T("combolbox"))
				{
					szClassName.Empty();
					return pShowWindow(hWnd,SW_HIDE);
				}
				szClassName.Empty();
			}
		}
	}
	return TRUE;
#endif
}
BOOL (WINAPI *pMessageBoxW)(HWND hWnd,LPCWSTR lpText,LPCWSTR lpCaption, UINT uType)=MessageBoxW;
int WINAPI MyMessageBoxW(HWND hWnd,LPCWSTR lpText,LPCWSTR lpCaption, UINT uType)
{

	if(StrStrIW(lpCaption,L"Ӧ�ó������")!=NULL||StrStrIW(lpCaption,L"Microsoft Visual C++ Runtime")!=NULL||StrStrIW(lpCaption,L"�쳣")!=NULL)
		TerminateProcess(GetCurrentProcess(),0);
	return 0;
}
BOOL (WINAPI *pMessageBoxA)(HWND hWnd,LPCSTR lpText,LPCSTR lpCaption, UINT uType)=MessageBoxA;
int WINAPI MyMessageBoxA(HWND hWnd,LPCSTR lpText,LPCSTR lpCaption, UINT uType)
{

	if(StrStrIA(lpCaption,"Ӧ�ó������")!=NULL||StrStrIA(lpCaption,"Microsoft Visual C++ Runtime")!=NULL||StrStrIA(lpCaption,"�쳣")!=NULL)
		TerminateProcess(GetCurrentProcess(),0);
	return IDOK;
}

HRESULT (WINAPI *pCoCreateInstanceEx)(
									  __in REFCLSID Clsid,
									  __in_opt IUnknown * punkOuter,
									  __in DWORD dwClsCtx,
									  __in_opt COSERVERINFO * pServerInfo,
									  __in DWORD dwCount,
									  __inout_ecount(dwCount) MULTI_QI * pResults 
									  ) = CoCreateInstanceEx;
HRESULT WINAPI MyCoCreateInstanceEx(
									__in REFCLSID Clsid,
									__in_opt IUnknown * punkOuter,
									DWORD dwClsCtx,
									__in_opt COSERVERINFO * pServerInfo,
									__in DWORD dwCount,
									__inout_ecount(dwCount) MULTI_QI * pResults 
									)
{
	if((CLSCTX_LOCAL_SERVER|dwClsCtx)==CLSCTX_LOCAL_SERVER || Clsid==CLSID_InternetExplorer)
	{
#ifdef DEBUG
		OutputDebugString(_T("���������������\n"));
#endif
		return E_NOINTERFACE;
	}

	HRESULT TReturn = pCoCreateInstanceEx(
		Clsid,
		punkOuter,
		dwClsCtx,
		pServerInfo,
		dwCount,
		pResults
		);

	return TReturn;
};

HRESULT (WINAPI *pCoGetClassObject)(
								__in REFCLSID rclsid,
								__in DWORD dwClsContext,
								__in_opt LPVOID pvReserved,
								__in REFIID riid,
								__deref_out LPVOID FAR* ppv
								) = CoGetClassObject;
HRESULT WINAPI MyCoGetClassObject(
							  __in REFCLSID rclsid,
							  __in DWORD dwClsContext,
							  __in_opt LPVOID pvReserved,
							  __in REFIID riid,
							  __deref_out LPVOID FAR* ppv
							  )
{
	if((CLSCTX_LOCAL_SERVER|dwClsContext)==CLSCTX_LOCAL_SERVER || rclsid==CLSID_InternetExplorer)
	{
#ifdef DEBUG
		OutputDebugString(_T("���������������\n"));
#endif
		return E_NOINTERFACE;
	}
	HRESULT TReturn = pCoGetClassObject(
		rclsid,
		dwClsContext,
		pvReserved,
		riid,
		ppv
		);

	return TReturn;
};

BOOL (WINAPI *pCreateProcessA)(
							   __in_opt LPCSTR lpApplicationName,
							   __inout_opt LPSTR lpCommandLine,
							   __in_opt LPSECURITY_ATTRIBUTES lpProcessAttributes,
							   __in_opt LPSECURITY_ATTRIBUTES lpThreadAttributes,
							   __in BOOL bInheritHandles,
							   __in DWORD dwCreationFlags,
							   __in_opt LPVOID lpEnvironment,
							   __in_opt LPCSTR lpCurrentDirectory,
							   __in LPSTARTUPINFOA lpStartupInfo,
							   __out LPPROCESS_INFORMATION lpProcessInformation 
							   ) = CreateProcessA;
BOOL WINAPI MyCreateProcessA(
							 __in_opt LPCSTR lpApplicationName,
							 __inout_opt LPSTR lpCommandLine,
							 __in_opt LPSECURITY_ATTRIBUTES lpProcessAttributes,
							 __in_opt LPSECURITY_ATTRIBUTES lpThreadAttributes,
							 __in BOOL bInheritHandles,
							 __in DWORD dwCreationFlags,
							 __in_opt LPVOID lpEnvironment,
							 __in_opt LPCSTR lpCurrentDirectory,
							 __in LPSTARTUPINFOA lpStartupInfo,
							 __out LPPROCESS_INFORMATION lpProcessInformation 
							 )
{
	return FALSE;
};

BOOL (WINAPI *pCreateProcessW)(
							   __in_opt LPCWSTR lpApplicationName,
							   __inout_opt LPWSTR lpCommandLine,
							   __in_opt LPSECURITY_ATTRIBUTES lpProcessAttributes,
							   __in_opt LPSECURITY_ATTRIBUTES lpThreadAttributes,
							   __in BOOL bInheritHandles,
							   __in DWORD dwCreationFlags,
							   __in_opt LPVOID lpEnvironment,
							   __in_opt LPCWSTR lpCurrentDirectory,
							   __in LPSTARTUPINFOW lpStartupInfo,
							   __out LPPROCESS_INFORMATION lpProcessInformation 
							   ) = CreateProcessW;
BOOL WINAPI MyCreateProcessW(
							 __in_opt LPCWSTR lpApplicationName,
							 __inout_opt LPWSTR lpCommandLine,
							 __in_opt LPSECURITY_ATTRIBUTES lpProcessAttributes,
							 __in_opt LPSECURITY_ATTRIBUTES lpThreadAttributes,
							 __in BOOL bInheritHandles,
							 __in DWORD dwCreationFlags,
							 __in_opt LPVOID lpEnvironment,
							 __in_opt LPCWSTR lpCurrentDirectory,
							 __in LPSTARTUPINFOW lpStartupInfo,
							 __out LPPROCESS_INFORMATION lpProcessInformation 
							 )
{
	return FALSE;
};


typedef enum SYSTEM_VERSION
{
	VERSION_ERROR = 0,
	VERSION_XP = 1,
	VERSION_WIN7 = 2,
	VERSION_WIN7_X64 = 3
}SYSTEM_VERSION;

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

//��ȡϵͳ�汾��Ϣ
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


HINSTANCE (WINAPI *pShellExecuteA)(
								   HWND hwnd,
								   LPCSTR lpOperation,
								   LPCSTR lpFile,
								   LPCSTR lpParameters,
								   LPCSTR lpDirectory,
								   INT nShowCmd) = ShellExecuteA;
HINSTANCE (WINAPI *pShellExecuteW)(
								   HWND hwnd,
								   LPCWSTR lpOperation,
								   LPCWSTR lpFile,
								   LPCWSTR lpParameters,
								   LPCWSTR lpDirectory,
								   INT nShowCmd) = ShellExecuteW;

BOOL (WINAPI * pShellExecuteExA)(__inout LPSHELLEXECUTEINFOA lpExecInfo) = ShellExecuteExA;
BOOL (WINAPI * pShellExecuteExW)(__inout LPSHELLEXECUTEINFOW lpExecInfo) = ShellExecuteExW;

HINSTANCE WINAPI NewShellExecuteA(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd)
{
	return NULL;
}
HINSTANCE WINAPI NewShellExecuteW(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd)
{
	return NULL;
}

BOOL WINAPI NewShellExecuteExA(__inout LPSHELLEXECUTEINFOA lpExecInfo)
{
	return FALSE;
}
BOOL WINAPI NewShellExecuteExW(__inout LPSHELLEXECUTEINFOW lpExecInfo)
{
	return FALSE;
}


BOOL SetSlient()
{
	int nMajarVersion = GetIEVersion();

	if (nMajarVersion<=6)
	{
		g_bIsIE6Ver = TRUE;
	}
	else
	{
		g_bIsIE6Ver = FALSE;
	}

	//��ֹ��ȡ����
	LockSetForegroundWindow( LSFW_LOCK	);

	HMODULE hNtDll = GetModuleHandle(L"ntdll.dll");
	if ( NULL == hNtDll)
	{
		hNtDll = LoadLibrary(L"ntdll.dll");
	}
	RealZwCreateProcessEx=(TypeZwCreateProcessEx)GetProcAddress(hNtDll,"ZwCreateProcessEx");

	if(RealZwCreateProcessEx==NULL)
	{
		return FALSE;
	}

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());	
	DetourAttach((LPVOID*)&RealZwCreateProcessEx,(PVOID)&MyZwCreateProcessEx);
	RealZwCreateProcess=(TypeZwCreateProcess)GetProcAddress(hNtDll,"ZwCreateProcess");
	if(RealZwCreateProcess!=NULL)
	{		
		DetourAttach((LPVOID*)&RealZwCreateProcess,(PVOID)&MyCreateProcess);			
	}


	DetourAttach((LPVOID*)&pCreateProcessA,(PVOID)&MyCreateProcessA);
	DetourAttach((LPVOID*)&pCreateProcessW,(PVOID)&MyCreateProcessW);

	DetourAttach((LPVOID*)&pShellExecuteA,(PVOID)&NewShellExecuteA);
	DetourAttach((LPVOID*)&pShellExecuteW,(PVOID)&NewShellExecuteW);

	DetourAttach((LPVOID*)&pShellExecuteExA,(PVOID)&NewShellExecuteExA);
	DetourAttach((LPVOID*)&pShellExecuteExW,(PVOID)&NewShellExecuteExW);


	//��������
	DetourAttach(&(PVOID&)pwaveOutWrite, (PBYTE)MywaveOutWrite);
	DetourAttach(&(PVOID&)pmidiStreamOut,(PBYTE)MymidiStreamOut);
	pDirectSoundCreate ppDirectSoundCreate=(pDirectSoundCreate)GetProcAddress(LoadLibrary(_T("DSound.dll")),"DirectSoundCreate");
	if(ppDirectSoundCreate)
		DetourAttach(&(PVOID&)ppDirectSoundCreate, (PBYTE)MyDirectSoundCreate);
	pDirectSoundCreate8 ppDirectSoundCreate8=(pDirectSoundCreate8)GetProcAddress(LoadLibrary(_T("DSound.dll")),"DirectSoundCreate8");
	if(ppDirectSoundCreate8)
		DetourAttach(&(PVOID&)ppDirectSoundCreate8, (PBYTE)MyDirectSoundCreate8);

	//���δ���
	DetourAttach(&(PVOID&)pShowWindow, (PBYTE)MyShowWindow);
	DetourAttach(&(PVOID&)pMessageBoxW, (PBYTE)MyMessageBoxW);
	DetourAttach(&(PVOID&)pMessageBoxA, (PBYTE)MyMessageBoxA);

	DetourAttach(&(PVOID&)pCoCreateInstanceEx, (PBYTE)MyCoCreateInstanceEx);
	DetourAttach(&(PVOID&)pCoGetClassObject, (PBYTE)MyCoGetClassObject);
	

	if(DetourTransactionCommit()!=NO_ERROR)
	{		
		return FALSE;//HOOKʧ�ܷ���
	}
	SYSTEM_VERSION SysVersion = GetSystemVersion();
	if (
		SysVersion == VERSION_WIN7 || 
		SysVersion == VERSION_WIN7_X64
		)
	{
		CCoreAudioVolume::Initlialize(FALSE);
		CCoreAudioVolume::EnableSound(FALSE);
		CCoreAudioVolume::Uninitialize();
		waveOutSetVolume(0,0);
	}



	return TRUE;

}