#include "stdafx.h"

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
	LPOLESTR pOleStr = NULL;
	StringFromCLSID(rclsid,&pOleStr);

	CString strClsID;
	strClsID = pOleStr;
	if (strClsID == L"{488A4255-3236-44B3-8F27-FA1AECAA8844}")
	{
		int a=0;
	}

	if (strClsID == L"{B568F111-DFE4-4944-B67F-0728AB2AB30F}")
	{
		int a=0;
	}

	OutputDebugStringW(strClsID+L"\n");
	CoTaskMemFree(pOleStr);

	HRESULT TReturn = pCoGetClassObject(
		rclsid,
		dwClsContext,
		pvReserved,
		riid,
		ppv
		);


	return TReturn;
};