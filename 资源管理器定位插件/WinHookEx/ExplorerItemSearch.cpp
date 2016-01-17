#include "stdafx.h"
#include "ExplorerItemSearch.h"

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

UINT GetObjectRole(IAccessible* pacc, VARIANT* pvarChild, LPSTR lpszRole, UINT cchRole)
{
	HRESULT hr;
	VARIANT varRetVal;

	*lpszRole = 0;

	VariantInit(&varRetVal);

	hr = pacc->get_accRole(*pvarChild, &varRetVal);

	if (!SUCCEEDED(hr))
		return(0);

	if (varRetVal.vt == VT_I4)
	{
		//函数GetRoleText用来将整型的角色表示转换为字符串表示
		GetRoleTextA(varRetVal.lVal, lpszRole, cchRole);
	}
	else if (varRetVal.vt == VT_BSTR)
	{
		WideCharToMultiByte(CP_ACP, 0, varRetVal.bstrVal, -1, lpszRole,
			cchRole, NULL, NULL);
	}

	VariantClear(&varRetVal);

	return(CString(lpszRole).GetLength());
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



BOOL FindChild (IAccessible* paccParent,LIST_SEARCH_RESULT *pListRes)
{
	HRESULT hr;
	long numChildren;
	unsigned long numFetched;
	VARIANT varChild;
	int index;
	IAccessible * paccChild = NULL;
	IEnumVARIANT* pEnum = NULL;
	IDispatch* pDisp = NULL;

	char szObjName[256];
	char szObjRole[256];

	//得到父亲支持的IEnumVARIANT接口
	hr = paccParent -> QueryInterface(IID_IEnumVARIANT, (PVOID*) & pEnum);

	if(pEnum)
		pEnum -> Reset();

	//取得父亲拥有的可访问的子的数目
	paccParent -> get_accChildCount(&numChildren);

	//搜索并比较每一个子ID，找到名字、角色、类与输入相一致的。
	for(index = 1; index <= numChildren; index++)
	{
		paccChild = NULL;		
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
			hr = pDisp->QueryInterface(IID_IAccessible, (void**)&paccChild);
			hr = pDisp->Release();
		}

		if(paccChild)
		{
			VariantInit(&varChild);
			varChild.vt = VT_I4;
			varChild.lVal = CHILDID_SELF;

			GetObjectName(paccChild, &varChild, szObjName, sizeof(szObjName));
			GetObjectRole(paccChild, &varChild, szObjRole, sizeof(szObjRole));
		}
		else
		{
			VariantInit(&varChild);
			varChild.vt = VT_I4;
			varChild.lVal = index;

			GetObjectName(paccParent, &varChild, szObjName, sizeof(szObjName));
			GetObjectRole(paccParent, &varChild, szObjRole, sizeof(szObjRole));
		}

		BOOL bAdd = FALSE;

		if ( CString(szObjRole) == L"列表项目")
		{
			SEARCH_RESULT_NODE ERes;
			ERes.paccParent = paccParent;
			ERes.paccChild = paccChild;
			ERes.nChildIndex = index;
			ERes.strItemName = szObjName;
			if(ERes.strItemName.GetLength() > 0)
			{
				pListRes->push_back(ERes);
				bAdd = TRUE;
			}
		}

		if( paccChild)
		{
			FindChild(paccChild,pListRes);

// 			if( FALSE == bAdd )
// 			{
// 				paccChild->Release();
// 			}
		}
	}

	if(pEnum)
		pEnum -> Release();

	return 0;
}


BOOL SearchExplorerItem(HWND hWnd,LIST_SEARCH_RESULT *pListRes)
{
	IAccessible *paccMainWindow = NULL;
	HRESULT hr = AccessibleObjectFromWindow(hWnd,OBJID_WINDOW,IID_IAccessible,(void **)&paccMainWindow);

	if (hr == S_OK && paccMainWindow)
	{
		FindChild(paccMainWindow,pListRes);
		paccMainWindow->Release();

		return TRUE;
	}

	return FALSE;
}

BOOL SetSelectExplorerItem( SEARCH_RESULT_NODE *pNode )
{
	HRESULT hr = S_FALSE;
	if (pNode)
	{
		CComVariant vtNull;
		if (pNode->paccChild)
		{
			vtNull = CHILDID_SELF;
			hr = pNode->paccChild->accSelect((SELFLAG_TAKEFOCUS | SELFLAG_TAKESELECTION),vtNull);
		}
		else if(pNode->paccParent)
		{
			vtNull = pNode->nChildIndex;
			hr = pNode->paccParent->accSelect((SELFLAG_TAKEFOCUS | SELFLAG_TAKESELECTION),vtNull);
		}
	}

	return hr == S_OK;
}

VOID ReleaseSearchResult(LIST_SEARCH_RESULT *pListRes)
{
	for (LIST_SEARCH_RESULT_PTR it = pListRes->begin();it!=pListRes->end();it++)
	{
		if(it->paccChild)
		{
			it->paccChild->Release();
		}
	}
}