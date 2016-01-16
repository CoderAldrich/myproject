// WinHook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <Windows.h>
#include <WinUser.h>
#include <atlstr.h>
#include "HzToPy.h"
#include "RecordBaseT.h"

#include <OleAcc.h>
#pragma comment(lib,"Oleacc.lib")

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


UINT GetObjectState(IAccessible* pacc, 
					VARIANT* pvarChild, 
					LPTSTR lpszState, 
					UINT cchState);


// --------------------------------------------------------------------------
//
//  GetObjectName()
//
// --------------------------------------------------------------------------
UINT GetObjectName(IAccessible* pacc, VARIANT* pvarChild, LPSTR lpszName, UINT cchName)
{
	HRESULT hr;
	BSTR bstrName;

	*lpszName = 0;
	bstrName = NULL;

	hr = pacc->get_accName(*pvarChild, &bstrName);

	if (SUCCEEDED(hr) && bstrName)
	{
		WideCharToMultiByte(CP_ACP, 0, bstrName, -1, lpszName, cchName, NULL, NULL);
		SysFreeString(bstrName);
	}

	return(CString(lpszName).GetLength());
} 


// --------------------------------------------------------------------------
//
//  GetObjectClass()
//
//  This gets the Class of an object.
//
// --------------------------------------------------------------------------
UINT GetObjectClass(IAccessible* pacc, LPSTR lpszClass, UINT cchClass)
{
	HWND hWnd;
	if(S_OK == WindowFromAccessibleObject(pacc,  &hWnd))
	{
		if(hWnd)
			GetClassNameA(hWnd, lpszClass, cchClass);
		else
			strcpy(lpszClass, "No window");
	}

	return 1;
}

#include <list>
using namespace std;

typedef struct tagENUM_RES
{
	CString strItemName;
	IAccessible* pAcc;
}ENUM_RES;
typedef list<ENUM_RES> LIST_ENUM_RES;
typedef LIST_ENUM_RES::iterator LIST_ENUM_RES_PTR;

BOOL FindChild (IAccessible* paccParent,
				BOOL bCollect,LIST_ENUM_RES *pListRes)
{
	HRESULT hr;
	long numChildren;
	unsigned long numFetched;
	VARIANT varChild;
	int index;
	IAccessible* pCAcc = NULL;
	IAccessible* paccChild = NULL;
	IEnumVARIANT* pEnum = NULL;
	IDispatch* pDisp = NULL;

	char szObjName[256];

	//得到父亲支持的IEnumVARIANT接口
	hr = paccParent -> QueryInterface(IID_IEnumVARIANT, (PVOID*) & pEnum);

	if(pEnum)
		pEnum -> Reset();

	//取得父亲拥有的可访问的子的数目
	paccParent -> get_accChildCount(&numChildren);

	//搜索并比较每一个子ID，找到名字、角色、类与输入相一致的。
	for(index = 1; index <= numChildren; index++)
	{
		pCAcc = NULL;		
		// 如果支持IEnumVARIANT接口，得到下一个子ID
		//以及其对应的 IDispatch 接口
		if (pEnum)
			hr = pEnum -> Next(1, &varChild, &numFetched);	
		else
		{
			//如果一个父亲不支持IEnumVARIANT接口，子ID就是它的序号
			varChild.vt = VT_I4;
			varChild.lVal = index;
		}

		// 找到此子ID对应的 IDispatch 接口
		if (varChild.vt == VT_I4)
		{
			//通过子ID序号得到对应的 IDispatch 接口
			pDisp = NULL;
			hr = paccParent -> get_accChild(varChild, &pDisp);
		}
		else
			//如果父支持IEnumVARIANT接口可以直接得到子IDispatch 接口
			pDisp = varChild.pdispVal;

		// 通过 IDispatch 接口得到子的 IAccessible 接口 pCAcc
		if (pDisp)
		{
			hr = pDisp->QueryInterface(IID_IAccessible, (void**)&pCAcc);
			hr = pDisp->Release();
		}

		// Get information about the child
		if(pCAcc)
		{
			//如果子支持IAccessible 接口，那么子ID就是CHILDID_SELF
			VariantInit(&varChild);
			varChild.vt = VT_I4;
			varChild.lVal = CHILDID_SELF;

			paccChild = pCAcc;
		}
		else
			//如果子不支持IAccessible 接口
			paccChild = paccParent;

		GetObjectName(paccChild, &varChild, szObjName, sizeof(szObjName));

// #ifdef DEBUG
// 		OutputDebugStringW(L"ObjName ");
// 		OutputDebugStringA(szObjName);
// 		OutputDebugStringW(L"\n");
// #endif

		if (bCollect)
		{
			ENUM_RES ERes;
			ERes.pAcc = paccChild;
			ERes.strItemName = szObjName;
			pListRes->push_back(ERes);
		}

		CString strObjectName;
		strObjectName = szObjName;

		if (strObjectName==L"项目视图" && FALSE == bCollect )
		{
			if( pCAcc)
			{
				FindChild(pCAcc,TRUE,pListRes);

				if(paccChild != pCAcc)
					pCAcc->Release();
			}
			break;
		}


	}

	if(pEnum)
		pEnum -> Release();

	return 0;
}

HHOOK g_hCbtHook = NULL;

typedef CRecordBaseT<HWND,LONG_PTR> COldWndProcRecord;
typedef struct tagNAVI_INFO
{
	CString strInput;
	DWORD dwLastInput;
}NAVI_INFO;

COldWndProcRecord g_WndRecord;

void pinyin_gb2312(char * inbuf, char * outbuf, bool m_blnSimaple = false, bool polyphone_support = false, bool m_blnFirstBig = false, bool m_blnAllBiG = false, bool m_LetterEnd = false, bool m_unknowSkip = true, bool m_filterPunc = true);

LRESULT CALLBACK NewWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	//OutputDebugStringW(L"NewWndProc Call");

	WNDPROC pWndProc = (WNDPROC)GetPropW(hWnd,L"OldWndProc");
	if (pWndProc)
	{
		if ( nMsg == WM_CHAR )
		{
			NAVI_INFO *pInput = (NAVI_INFO *)GetPropW(hWnd,L"Input");
			if ( NULL == pInput)
			{
				pInput = new NAVI_INFO;
				pInput->dwLastInput = GetTickCount();
				SetPropW(hWnd,L"Input",(HANDLE)pInput);
			}

			if ( GetTickCount() - pInput->dwLastInput > 1000)
			{
				pInput->strInput=L"";
			}

			pInput->dwLastInput = GetTickCount();
			pInput->strInput += (char)wParam;
			pInput->strInput.MakeLower();

			OutputDebugStringW(L"Input: "+pInput->strInput);

 			if ( ('a' <= wParam && wParam <= 'z') && ( 'A' <= wParam && wParam <= 'Z '))
 			{
			 	IAccessible *paccMainWindow = NULL;
			 	HRESULT hr = AccessibleObjectFromWindow(hWnd,OBJID_WINDOW,IID_IAccessible,(void **)&paccMainWindow);
			 
			 	CHzToPy Test;
			 
			 	if (hr == S_OK && paccMainWindow)
			 	{
			 		LIST_ENUM_RES Res;
			 		FindChild(paccMainWindow,FALSE,&Res);
			 		paccMainWindow->Release();
			 
			 		for (LIST_ENUM_RES_PTR it = Res.begin();it!=Res.end();it++)
			 		{
			 			CString strPinYin;
						CString strFullPinYin;

			 			strPinYin = Test.HzToPinYin(it->strItemName);
			 			if (strPinYin.GetLength() == 0 )
			 			{
			 				strPinYin = it->strItemName;
			 			}
			 			strPinYin.MakeLower();

						CStringA strTempPinYin;
						pinyin_gb2312(CStringA(it->strItemName).GetBuffer(), strTempPinYin.GetBuffer(5000), false, false, true, true, true);
						strTempPinYin.ReleaseBuffer();

						strFullPinYin = strTempPinYin;
						strFullPinYin.MakeLower();

			 			if (strPinYin.Find(pInput->strInput) >=0 || strFullPinYin.Find(pInput->strInput) >= 0)
			 			{	
			 				CComVariant vtNull;
			 				vtNull = CHILDID_SELF;
			 				it->pAcc->accSelect(SELFLAG_TAKESELECTION,vtNull);

							break;
			 			}
			 
			 			it->pAcc->Release();
			 			
			 		}
			 
			 	}

 				return 0;
 			}
		}

		return pWndProc(hWnd,nMsg,wParam,lParam);
	}
	return DefWindowProcW(hWnd,nMsg,wParam,lParam);
}

LRESULT CALLBACK CbtHookProc(int code, WPARAM wParam, LPARAM lParam)
{
	if(HCBT_ACTIVATE == code || HCBT_CREATEWND == code || HCBT_SETFOCUS == code)
	{
		HWND hWnd = (HWND)wParam;
		CString strClassName;
		GetClassName(hWnd,strClassName.GetBuffer(MAX_CLASS_NAME),MAX_CLASS_NAME);
		strClassName.ReleaseBuffer();

		if (strClassName.CompareNoCase(L"DirectUIHWND") == 0)
		{
			OutputDebugStringW(L"DirectUIHWND Found");
			LONG_PTR lOldWndProc = GetWindowLongPtrW(hWnd,(-4));
			if (lOldWndProc != (LONG_PTR)NewWndProc)
			{
				SetPropW(hWnd,L"OldWndProc",(HANDLE)lOldWndProc);
				SetWindowLongPtrW(hWnd,(-4),(LONG_PTR)NewWndProc);

				g_WndRecord.AddRecord(hWnd,lOldWndProc);

// 				static BOOL bSetClass = FALSE;
// 				if ( FALSE == bSetClass )
// 				{
// 					bSetClass = TRUE;
// 					SetClassLongPtrW(hWnd,(-24),(LONG_PTR)NewWndProc);
// 				}
				
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