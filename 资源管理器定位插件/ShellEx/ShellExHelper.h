// ShellExHelper.h : CShellExHelper ������

#pragma once
#include "resource.h"       // ������

#include "ShellEx_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
#endif



// CShellExHelper

class ATL_NO_VTABLE CShellExHelper :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CShellExHelper, &CLSID_ShellExHelper>,
	public IDispatchImpl<IShellExHelper, &IID_IShellExHelper, &LIBID_ShellExLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
	IShellExtInit
public:
	CShellExHelper()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SHELLEXHELPER)


BEGIN_COM_MAP(CShellExHelper)
	COM_INTERFACE_ENTRY(IShellExHelper)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

};

OBJECT_ENTRY_AUTO(__uuidof(ShellExHelper), CShellExHelper)
