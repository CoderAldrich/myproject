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



BOOL FindChild (IAccessible* paccParent,
				BOOL bCollect,LIST_SEARCH_RESULT *pListRes)
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

		if (bCollect)
		{
			SEARCH_RESULT_NODE ERes;
			ERes.pAcc = paccChild;
			ERes.strItemName = szObjName;
			if (ERes.strItemName.GetLength() > 0 && ERes.strItemName != L"标题")
			{
				pListRes->push_back(ERes);
			}
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


BOOL SearchExplorerItem(HWND hWnd,LIST_SEARCH_RESULT *pListRes)
{
	IAccessible *paccMainWindow = NULL;
	HRESULT hr = AccessibleObjectFromWindow(hWnd,OBJID_WINDOW,IID_IAccessible,(void **)&paccMainWindow);

	if (hr == S_OK && paccMainWindow)
	{
		FindChild(paccMainWindow,FALSE,pListRes);
		paccMainWindow->Release();

		return TRUE;
	}

	return FALSE;

}

VOID ReleaseSearchResult(LIST_SEARCH_RESULT *pListRes)
{
	for (LIST_SEARCH_RESULT_PTR it = pListRes->begin();it!=pListRes->end();it++)
	{
		it->pAcc->Release();
	}
}