#include "stdafx.h"
#include <Windows.h>

typedef void* (__stdcall *LPFN_KernelBaseGetGlobalData)(void);

typedef HANDLE (WINAPI *LPFN_CreateRemoteThreadEx)(
							_In_      HANDLE                       hProcess,
							_In_opt_  LPSECURITY_ATTRIBUTES        lpThreadAttributes,
							_In_      SIZE_T                       dwStackSize,
							_In_      LPTHREAD_START_ROUTINE       lpStartAddress,
							_In_opt_  LPVOID                       lpParameter,
							_In_      DWORD                        dwCreationFlags,
							_In_opt_  LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
							_Out_opt_ LPDWORD                      lpThreadId
							);

HANDLE LibCreateRemoteThread( __in HANDLE hProcess, 
 __in LPTHREAD_START_ROUTINE lpStartAddress,
 __in_opt LPVOID lpParameter, 
 __in DWORD dwCreationFlags,
 __out_opt LPDWORD lpThreadId )
{    
	OSVERSIONINFOEX          stOSVersionInfoEx={0};
	LPFN_CreateRemoteThreadEx    pCreateRemoteThreadEx=NULL;
	LPFN_KernelBaseGetGlobalData  pKernelBaseGetGlobalData=NULL;
	UCHAR*              pCreateRemoteThread=NULL;
	UCHAR*              pGlobalData=NULL;
	UCHAR*              pMisc=NULL;
	HMODULE              hKernelBase=NULL;
	HMODULE              hKernel32=NULL;
	HANDLE              hNewThread=NULL;
	ULONG              ulIndex=0;
	WORD              wCode=0;

	do 
	{
		stOSVersionInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if (!GetVersionEx((OSVERSIONINFO*)&stOSVersionInfoEx))
		{
			break;
		}

		//vista以前的系统不存在这个问题
		if ( (stOSVersionInfoEx.dwMajorVersion < 6) || (GetCurrentProcess() == hProcess) )
		{
			hNewThread = CreateRemoteThread(hProcess,NULL,0,lpStartAddress,lpParameter,dwCreationFlags,lpThreadId);
			break;
		}

		if ( (stOSVersionInfoEx.dwMajorVersion == 6) && (0 == stOSVersionInfoEx.dwMinorVersion) )
		{
			//vista
			hKernel32 = LoadLibraryA("Kernel32.dll");
			pCreateRemoteThread = (UCHAR*)GetProcAddress(hKernel32,"CreateRemoteThread");

			for (ulIndex=0; ulIndex < 0x300;ulIndex += 1)
			{
				wCode = *((USHORT*)(pCreateRemoteThread + ulIndex));

#ifdef _WIN64
				if (0x3D80 == wCode )
				{
					pMisc = (*((ULONG*)(pCreateRemoteThread + ulIndex + 2))) + (pCreateRemoteThread + ulIndex + 7);
					break;
				}
#else
				if (0x1D38 == wCode )
				{
					pMisc =  (UCHAR*)(*((ULONG*)(pCreateRemoteThread + ulIndex + 2)));
					break;
				}
#endif
			}
		}
		else if ( (stOSVersionInfoEx.dwMajorVersion == 6) && (1 == stOSVersionInfoEx.dwMinorVersion) )
		{
			//win7
			hKernelBase = LoadLibraryW(L"KernelBase.dll");
			if (NULL == hKernelBase)
			{
				break;
			}
			pKernelBaseGetGlobalData = (LPFN_KernelBaseGetGlobalData)GetProcAddress(hKernelBase,"KernelBaseGetGlobalData");
			if (NULL == pKernelBaseGetGlobalData)
			{
				break;
			}

			pGlobalData = (UCHAR*)pKernelBaseGetGlobalData();
			if (NULL == pGlobalData)
			{
				break;
			}

#ifdef _WIN64
			pMisc =  pGlobalData + 0x5C;
#else
			pMisc =  pGlobalData + 0x30;
#endif
		}
		else
		{
			//手上的win8 Build 8250 没有session 隔离
		}

		//////////////////////////////////////////////////////////////////////////
		if (NULL == pMisc)
		{
			break;
		}

		//Patch
		*pMisc = 1;

		//xx
		hNewThread = CreateRemoteThread(hProcess,NULL,0,lpStartAddress,lpParameter,dwCreationFlags,lpThreadId);

		//UnPatch
		*pMisc = 0;

	} while (FALSE);

	if (NULL != hKernelBase)
	{
		FreeLibrary(hKernelBase);
		hKernelBase = NULL;
	}

	return hNewThread;
}