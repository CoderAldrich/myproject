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

	//�õ�����֧�ֵ�IEnumVARIANT�ӿ�
	hr = paccParent -> QueryInterface(IID_IEnumVARIANT, (PVOID*) & pEnum);

	if(pEnum)
		pEnum -> Reset();

	//ȡ�ø���ӵ�еĿɷ��ʵ��ӵ���Ŀ
	paccParent -> get_accChildCount(&numChildren);

	//�������Ƚ�ÿһ����ID���ҵ����֡���ɫ������������һ�µġ�
	for(index = 1; index <= numChildren; index++)
	{
		pCAcc = NULL;		
		// ���֧��IEnumVARIANT�ӿڣ��õ���һ����ID
		//�Լ����Ӧ�� IDispatch �ӿ�
		if (pEnum)
			hr = pEnum -> Next(1, &varChild, &numFetched);	
		else
		{
			//���һ�����ײ�֧��IEnumVARIANT�ӿڣ���ID�����������
			varChild.vt = VT_I4;
			varChild.lVal = index;
		}

		// �ҵ�����ID��Ӧ�� IDispatch �ӿ�
		if (varChild.vt == VT_I4)
		{
			//ͨ����ID��ŵõ���Ӧ�� IDispatch �ӿ�
			pDisp = NULL;
			hr = paccParent -> get_accChild(varChild, &pDisp);
		}
		else
			//�����֧��IEnumVARIANT�ӿڿ���ֱ�ӵõ���IDispatch �ӿ�
			pDisp = varChild.pdispVal;

		// ͨ�� IDispatch �ӿڵõ��ӵ� IAccessible �ӿ� pCAcc
		if (pDisp)
		{
			hr = pDisp->QueryInterface(IID_IAccessible, (void**)&pCAcc);
			hr = pDisp->Release();
		}

		// Get information about the child
		if(pCAcc)
		{
			//�����֧��IAccessible �ӿڣ���ô��ID����CHILDID_SELF
			VariantInit(&varChild);
			varChild.vt = VT_I4;
			varChild.lVal = CHILDID_SELF;

			paccChild = pCAcc;
		}
		else
			//����Ӳ�֧��IAccessible �ӿ�
			paccChild = paccParent;

		GetObjectName(paccChild, &varChild, szObjName, sizeof(szObjName));

		if (bCollect)
		{
			SEARCH_RESULT_NODE ERes;
			ERes.pAcc = paccChild;
			ERes.strItemName = szObjName;
			if (ERes.strItemName.GetLength() > 0 && ERes.strItemName != L"����")
			{
				pListRes->push_back(ERes);
			}
		}

		CString strObjectName;
		strObjectName = szObjName;

		if (strObjectName==L"��Ŀ��ͼ" && FALSE == bCollect )
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