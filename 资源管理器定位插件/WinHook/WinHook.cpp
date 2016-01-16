// WinHook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <Windows.h>
#include <WinUser.h>
#include <atlstr.h>
#include "RecordBaseT.h"
#include "ExplorerItemSearch.h"


//获取当前模块句柄
HMODULE ModuleHandleByAddr(const void* ptrAddr)  
{  
	MEMORY_BASIC_INFORMATION info;  
	::VirtualQuery(ptrAddr, &info, sizeof(info));  
	return (HMODULE)info.AllocationBase;  
}  
/*  
功能：获取当前模块句柄
返回值：当前模块句柄
*/  
HMODULE ThisModuleHandle()  
{  
	static HMODULE sInstance = ModuleHandleByAddr((void*)&ThisModuleHandle);  
	return sInstance;  
}



HHOOK g_hCbtHook = NULL;

typedef CRecordBaseT<HWND,LONG_PTR> COldWndProcRecord;
typedef struct tagSEARCH_INPUT_INFO
{
	CString strInputText;
	DWORD dwLastInput;
}SEARCH_INPUT_INFO;

COldWndProcRecord g_WndRecord;

void pinyin_gb2312(char * inbuf, char * outbuf, bool m_blnSimaple = false, bool polyphone_support = false, bool m_blnFirstBig = false, bool m_blnAllBiG = false, bool m_LetterEnd = false, bool m_unknowSkip = true, bool m_filterPunc = true);

LRESULT CALLBACK NewWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	WNDPROC pWndProc = (WNDPROC)GetPropW(hWnd,L"OldWndProc");
	if (pWndProc)
	{
		if ( nMsg == WM_CHAR )
		{
			SEARCH_INPUT_INFO *pInput = (SEARCH_INPUT_INFO *)GetPropW(hWnd,L"Input");
			if ( NULL == pInput)
			{
				pInput = new SEARCH_INPUT_INFO;
				pInput->dwLastInput = GetTickCount();
				SetPropW(hWnd,L"Input",(HANDLE)pInput);
			}

			if ( GetTickCount() - pInput->dwLastInput > 700)
			{
				pInput->strInputText=L"";
			}

			pInput->dwLastInput = GetTickCount();
			pInput->strInputText += (char)wParam;
			pInput->strInputText.MakeLower();

#if defined(DEBUG) || defined(_DEBUG)
			OutputDebugStringW(L"当前搜索："+pInput->strInputText);
#endif

 			if ( ('a' <= wParam && wParam <= 'z') || ( 'A' <= wParam && wParam <= 'Z '))
 			{
		 		LIST_SEARCH_RESULT SearchRes;
		 		SearchExplorerItem(hWnd,&SearchRes);
		 		for (LIST_SEARCH_RESULT_PTR it = SearchRes.begin();it!=SearchRes.end();it++)
		 		{
		 			CString strShortPinYin;
					CString strFullPinYin;
					CStringA strTempPinYin;

					pinyin_gb2312(CStringA(it->strItemName).GetBuffer(), strTempPinYin.GetBuffer(200), false, false, true, true, true);
					strTempPinYin.ReleaseBuffer();
					strFullPinYin = strTempPinYin;
					strFullPinYin.MakeLower();

					pinyin_gb2312(CStringA(it->strItemName).GetBuffer(), strTempPinYin.GetBuffer(200), true, false, true, true, true);
					strTempPinYin.ReleaseBuffer();
					strShortPinYin = strTempPinYin;
					strShortPinYin.MakeLower();

		 			if ( strShortPinYin.Find(pInput->strInputText) >=0 || strFullPinYin.Find(pInput->strInputText) >= 0)
		 			{
		 				CComVariant vtNull;
		 				vtNull = CHILDID_SELF;
		 				it->pAcc->accSelect((SELFLAG_TAKEFOCUS | SELFLAG_TAKESELECTION),vtNull);

						break;
		 			}
		 		}

				ReleaseSearchResult(&SearchRes);


 				return 0;
 			}
		}

		return pWndProc(hWnd,nMsg,wParam,lParam);
	}
	return DefWindowProcW(hWnd,nMsg,wParam,lParam);
}

extern BOOL g_bHookThisProcess;
LRESULT CALLBACK CbtHookProc(int code, WPARAM wParam, LPARAM lParam)
{
	if( g_bHookThisProcess && (HCBT_ACTIVATE == code || HCBT_CREATEWND == code || HCBT_SETFOCUS == code))
	{
		HWND hWnd = (HWND)wParam;
		CString strClassName;
		GetClassName(hWnd,strClassName.GetBuffer(MAX_CLASS_NAME),MAX_CLASS_NAME);
		strClassName.ReleaseBuffer();

		if (strClassName.CompareNoCase(L"DirectUIHWND") == 0)
		{
			HWND hWndParent = GetParent(hWnd);
			GetClassName(hWndParent,strClassName.GetBuffer(MAX_CLASS_NAME),MAX_CLASS_NAME);
			strClassName.ReleaseBuffer();
			if (strClassName.CompareNoCase(L"SHELLDLL_DefView") == 0 )
			{
				LONG_PTR lOldWndProc = GetWindowLongPtrW(hWnd,(-4));
				if (lOldWndProc != (LONG_PTR)NewWndProc)
				{
					SetPropW(hWnd,L"OldWndProc",(HANDLE)lOldWndProc);
					SetWindowLongPtrW(hWnd,(-4),(LONG_PTR)NewWndProc);

					g_WndRecord.AddRecord(hWnd,lOldWndProc);
				}
			}


		}
	}
	return CallNextHookEx(g_hCbtHook,code,wParam,lParam);
}

VOID UnHookWindow()
{
	g_WndRecord.WLock();
	COldWndProcRecord::RECORD_MAP *pMap = g_WndRecord.GetInternalMap();
	
	for (COldWndProcRecord::RECORD_MAP::iterator it = pMap->begin();it!=pMap->end();it++)
	{
		if(IsWindow(it->first))
		{
			SetWindowLongPtrW(it->first,(-4),it->second);
		}
	}
	g_WndRecord.UnLock();
}

BOOL InstallHook()
{
	g_hCbtHook = SetWindowsHookExW(WH_CBT,CbtHookProc,ThisModuleHandle(),0);

	return TRUE;
}