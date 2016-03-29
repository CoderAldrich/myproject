
//=--------------------------------------------------------------------------=
//  (C) Copyright 1996-1999 Microsoft Corporation. All Rights Reserved.
//=--------------------------------------------------------------------------=


// 
// NOTE: 
// Some of the code in this file is MFC implementation specific.
// Changes in future versions of MFC implementation may require
// the code to be changed. Please check the readme of this
// sample for more information 
// 

#include "stdafx.h"

#undef AFX_DATA
#define AFX_DATA AFX_DATA_IMPORT


// NOTE: THis line is a hardcoded reference to an MFC header file
//  this path may need to be changed to refer to the location of VC5 install
//  for successful compilation.


#undef AFX_DATA
#define AFX_DATA AFX_DATA_EXPORT

#include "IIEOleClientSite.h"
#include <urlmon.h>




BEGIN_INTERFACE_MAP(IIEOleControlSite, COleControlSite)
 	INTERFACE_PART(IIEOleControlSite, IID_IInternetSecurityManager, InternetSecurityManager)
 	INTERFACE_PART(IIEOleControlSite,IID_INewWindowManager,NewWindowManager)
 	INTERFACE_PART(IIEOleControlSite, IID_IHTMLOMWindowServices, HTMLOMWindowServices)
	INTERFACE_PART(IIEOleControlSite, IID_IOleCommandTarget, OleCommandTarget)
	INTERFACE_PART(IIEOleControlSite, IID_IDocHostUIHandler, DocHostUIHandler)
	INTERFACE_PART(IIEOleControlSite, IID_IDocHostUIHandler2, DocHostUIHandler2)
	INTERFACE_PART(IIEOleControlSite, IID_IServiceProvider, ServiceProvider)
	INTERFACE_PART(IIEOleControlSite, IID_IDownloadManager, DownloadManager)
END_INTERFACE_MAP()

ULONG FAR EXPORT  IIEOleControlSite::XDocHostUIHandler::AddRef() 
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler)
		return pThis->ExternalAddRef();
}


ULONG FAR EXPORT  IIEOleControlSite::XDocHostUIHandler::Release()
{                            
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler)
		return pThis->ExternalRelease();
}

HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler::QueryInterface(REFIID riid, void **ppvObj)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler)
	HRESULT hr = (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);
	return hr;
}

// * CImpIDocHostUIHandler::GetHostInfo
// *
// * Purpose: Called at initialization
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler::GetHostInfo( DOCHOSTUIINFO* pInfo )
{

	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler)
	return pThis->m_pView->OnGetHostInfo(pInfo);
}

// * CImpIDocHostUIHandler::ShowUI
// *
// * Purpose: Called when MSHTML.DLL shows its UI
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler::ShowUI(
	DWORD dwID, 
	IOleInPlaceActiveObject * pActiveObject,
	IOleCommandTarget * pCommandTarget,
	IOleInPlaceFrame * pFrame,
	IOleInPlaceUIWindow * pDoc)
{

	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler)
	return pThis->m_pView->OnShowUI(dwID, pActiveObject, pCommandTarget, pFrame, pDoc);
}

// * CImpIDocHostUIHandler::HideUI
// *
// * Purpose: Called when MSHTML.DLL hides its UI
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler::HideUI(void)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler)
	return pThis->m_pView->OnHideUI();
}

// * CImpIDocHostUIHandler::UpdateUI
// *
// * Purpose: Called when MSHTML.DLL updates its UI
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler::UpdateUI(void)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler)
	return pThis->m_pView->OnUpdateUI();
}

// * CImpIDocHostUIHandler::EnableModeless
// *
// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::EnableModeless
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler::EnableModeless(BOOL fEnable)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler)
	return pThis->m_pView->OnEnableModeless(fEnable);
}

// * CImpIDocHostUIHandler::OnDocWindowActivate
// *
// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::OnDocWindowActivate
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler::OnDocWindowActivate(BOOL fActivate)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler)
	return pThis->m_pView->OnDocWindowActivate(fActivate);
}

// * CImpIDocHostUIHandler::OnFrameWindowActivate
// *
// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::OnFrameWindowActivate
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler::OnFrameWindowActivate(BOOL fActivate)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler)
	return pThis->m_pView->OnFrameWindowActivate(fActivate);
}

// * CImpIDocHostUIHandler::ResizeBorder
// *
// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::ResizeBorder
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler::ResizeBorder(
	LPCRECT prcBorder, 
	IOleInPlaceUIWindow* pUIWindow,
	BOOL fRameWindow)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler)
	return pThis->m_pView->OnResizeBorder(prcBorder, pUIWindow, fRameWindow);
}

// * CImpIDocHostUIHandler::ShowContextMenu
// *
// * Purpose: Called when MSHTML.DLL would normally display its context menu
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler::ShowContextMenu(
	DWORD dwID, 
	POINT* pptPosition,
	IUnknown* pCommandTarget,
	IDispatch* pDispatchObjectHit)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler)
	return pThis->m_pView->OnShowContextMenu(dwID, pptPosition, pCommandTarget, pDispatchObjectHit);
}

// * CImpIDocHostUIHandler::TranslateAccelerator
// *
// * Purpose: Called from MSHTML.DLL's TranslateAccelerator routines
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler::TranslateAccelerator(LPMSG lpMsg,
																				/* [in] */ const GUID __RPC_FAR *pguidCmdGroup,
																				/* [in] */ DWORD nCmdID)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler)
	return pThis->m_pView->OnTranslateAccelerator(lpMsg, pguidCmdGroup, nCmdID);
}

// * CImpIDocHostUIHandler::GetOptionKeyPath
// *
// * Purpose: Called by MSHTML.DLL to find where the host wishes to store 
// *	its options in the registry
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler::GetOptionKeyPath(BSTR* pbstrKey, DWORD dwReserved)
{

	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler)
	return pThis->m_pView->OnGetOptionKeyPath(pbstrKey, dwReserved);
}

STDMETHODIMP IIEOleControlSite::XDocHostUIHandler::GetDropTarget( 
	/* [in] */ IDropTarget __RPC_FAR *pDropTarget,
	/* [out] */ IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler)
	return pThis->m_pView->OnGetDropTarget(pDropTarget, ppDropTarget);
}	

STDMETHODIMP IIEOleControlSite::XDocHostUIHandler::GetExternal( 
	/* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler)
	return pThis->m_pView->OnGetExternal(ppDispatch);
}


STDMETHODIMP IIEOleControlSite::XDocHostUIHandler::TranslateUrl( 
	/* [in] */ DWORD dwTranslate,
	/* [in] */ OLECHAR __RPC_FAR *pchURLIn,
	/* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler)
	return pThis->m_pView->OnTranslateUrl(dwTranslate, pchURLIn, ppchURLOut);
}

STDMETHODIMP IIEOleControlSite::XDocHostUIHandler::FilterDataObject( /* [in] */ IDataObject __RPC_FAR *pDO,
	/* [out] */ IDataObject __RPC_FAR *__RPC_FAR *ppDORet)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler)
	return pThis->m_pView->OnFilterDataObject(pDO, ppDORet);
}
//////////////////////////////////////////////////////////////////////////////
//IDocHostUIHandler2 Methods

HRESULT FAR EXPORT IIEOleControlSite::XDocHostUIHandler2
::QueryInterface(REFIID riid, void** ppvObj)
{

	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler2)
		HRESULT hr = (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);
	return hr;
}

ULONG FAR EXPORT IIEOleControlSite::XDocHostUIHandler2::AddRef()
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler2)
		return pThis->ExternalAddRef();
}

ULONG FAR EXPORT IIEOleControlSite::XDocHostUIHandler2::Release()
{                            

	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler2)
		return pThis->ExternalRelease();
}

HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler2::GetHostInfo( DOCHOSTUIINFO* pInfo )
{

	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler2)
		return pThis->m_pView->OnGetHostInfo(pInfo);
}

// * CImpIDocHostUIHandler2::ShowUI
// *
// * Purpose: Called when MSHTML.DLL shows its UI
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler2::ShowUI(
	DWORD dwID, 
	IOleInPlaceActiveObject * pActiveObject,
	IOleCommandTarget * pCommandTarget,
	IOleInPlaceFrame * pFrame,
	IOleInPlaceUIWindow * pDoc)
{

	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler2)
		return pThis->m_pView->OnShowUI(dwID, pActiveObject, pCommandTarget, pFrame, pDoc);
}

// * CImpIDocHostUIHandler2::HideUI
// *
// * Purpose: Called when MSHTML.DLL hides its UI
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler2::HideUI(void)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler2)
		return pThis->m_pView->OnHideUI();
}

// * CImpIDocHostUIHandler2::UpdateUI
// *
// * Purpose: Called when MSHTML.DLL updates its UI
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler2::UpdateUI(void)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler2)
		return pThis->m_pView->OnUpdateUI();
}

// * CImpIDocHostUIHandler2::EnableModeless
// *
// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::EnableModeless
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler2::EnableModeless(BOOL fEnable)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler2)
		return pThis->m_pView->OnEnableModeless(fEnable);
}

// * CImpIDocHostUIHandler2::OnDocWindowActivate
// *
// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::OnDocWindowActivate
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler2::OnDocWindowActivate(BOOL fActivate)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler2)
		return pThis->m_pView->OnDocWindowActivate(fActivate);
}

// * CImpIDocHostUIHandler2::OnFrameWindowActivate
// *
// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::OnFrameWindowActivate
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler2::OnFrameWindowActivate(BOOL fActivate)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler2)
		return pThis->m_pView->OnFrameWindowActivate(fActivate);
}

// * CImpIDocHostUIHandler2::ResizeBorder
// *
// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::ResizeBorder
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler2::ResizeBorder(
	LPCRECT prcBorder, 
	IOleInPlaceUIWindow* pUIWindow,
	BOOL fRameWindow)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler2)
		return pThis->m_pView->OnResizeBorder(prcBorder, pUIWindow, fRameWindow);
}

// * CImpIDocHostUIHandler2::ShowContextMenu
// *
// * Purpose: Called when MSHTML.DLL would normally display its context menu
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler2::ShowContextMenu(
	DWORD dwID, 
	POINT* pptPosition,
	IUnknown* pCommandTarget,
	IDispatch* pDispatchObjectHit)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler2)
		return pThis->m_pView->OnShowContextMenu(dwID, pptPosition, pCommandTarget, pDispatchObjectHit);
}

// * CImpIDocHostUIHandler2::TranslateAccelerator
// *
// * Purpose: Called from MSHTML.DLL's TranslateAccelerator routines
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler2::TranslateAccelerator(LPMSG lpMsg,
																				/* [in] */ const GUID __RPC_FAR *pguidCmdGroup,
																				/* [in] */ DWORD nCmdID)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler2)
		return pThis->m_pView->OnTranslateAccelerator(lpMsg, pguidCmdGroup, nCmdID);
}

// * CImpIDocHostUIHandler2::GetOptionKeyPath
// *
// * Purpose: Called by MSHTML.DLL to find where the host wishes to store 
// *	its options in the registry
// *
HRESULT FAR EXPORT  IIEOleControlSite::XDocHostUIHandler2::GetOptionKeyPath(BSTR* pbstrKey, DWORD dwReserved)
{

	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler2)
		return pThis->m_pView->OnGetOptionKeyPath(pbstrKey, dwReserved);
}

STDMETHODIMP IIEOleControlSite::XDocHostUIHandler2::GetDropTarget( 
	/* [in] */ IDropTarget __RPC_FAR *pDropTarget,
	/* [out] */ IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler2)
		return pThis->m_pView->OnGetDropTarget(pDropTarget, ppDropTarget);
}	

STDMETHODIMP IIEOleControlSite::XDocHostUIHandler2::GetExternal( 
	/* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler2)
		return pThis->m_pView->OnGetExternal(ppDispatch);
}


STDMETHODIMP IIEOleControlSite::XDocHostUIHandler2::TranslateUrl( 
	/* [in] */ DWORD dwTranslate,
	/* [in] */ OLECHAR __RPC_FAR *pchURLIn,
	/* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler2)
		return pThis->m_pView->OnTranslateUrl(dwTranslate, pchURLIn, ppchURLOut);
}

STDMETHODIMP IIEOleControlSite::XDocHostUIHandler2::FilterDataObject( /* [in] */ IDataObject __RPC_FAR *pDO,
																	 /* [out] */ IDataObject __RPC_FAR *__RPC_FAR *ppDORet)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler2)
		return pThis->m_pView->OnFilterDataObject(pDO, ppDORet);
}

STDMETHODIMP IIEOleControlSite::XDocHostUIHandler2::GetOverrideKeyPath( LPOLESTR *pchKey,DWORD dw)
{
	METHOD_PROLOGUE(IIEOleControlSite, DocHostUIHandler2)
		return pThis->m_pView->OnGetOverrideKeyPath(pchKey, dw);
}


//////////////////////////////////////////////////////////////////////////////
//InternetSecurityManager Methods
HRESULT FAR EXPORT IIEOleControlSite::XInternetSecurityManager
										::QueryInterface(REFIID riid, void** ppvObj)
{

	METHOD_PROLOGUE(IIEOleControlSite, InternetSecurityManager)
   HRESULT hr = (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);
	return hr;
}

ULONG FAR EXPORT IIEOleControlSite::XInternetSecurityManager::AddRef()
{
	METHOD_PROLOGUE(IIEOleControlSite, InternetSecurityManager)
	return pThis->ExternalAddRef();
}

ULONG FAR EXPORT IIEOleControlSite::XInternetSecurityManager::Release()
{                            

	METHOD_PROLOGUE(IIEOleControlSite, InternetSecurityManager)
	return pThis->ExternalRelease();
}

HRESULT IIEOleControlSite::XInternetSecurityManager
							::SetSecuritySite (IInternetSecurityMgrSite *pSite)
{
	METHOD_PROLOGUE(IIEOleControlSite, InternetSecurityManager)
	return INET_E_DEFAULT_ACTION;
}

HRESULT IIEOleControlSite::XInternetSecurityManager
							::GetSecuritySite(IInternetSecurityMgrSite **ppSite)
{

	METHOD_PROLOGUE(IIEOleControlSite, InternetSecurityManager)
	return INET_E_DEFAULT_ACTION;
}

HRESULT IIEOleControlSite::XInternetSecurityManager
							::MapUrlToZone(LPCWSTR pwszUrl,DWORD *pdwZone,DWORD dwFlags)
{
	METHOD_PROLOGUE(IIEOleControlSite, InternetSecurityManager)
	return INET_E_DEFAULT_ACTION;
}

#ifdef _WIN64
HRESULT  FAR EXPORT IIEOleControlSite::XInternetSecurityManager::GetSecurityId(LPCWSTR pwszUrl,
																				BYTE *pbSecurityId,
																				DWORD *pcbSecurityId, 
																				DWORD_PTR pdwReserved)
{

	METHOD_PROLOGUE(IIEOleControlSite, InternetSecurityManager)
		return INET_E_DEFAULT_ACTION;
}
#else
HRESULT  FAR EXPORT IIEOleControlSite::XInternetSecurityManager::GetSecurityId(LPCWSTR pwszUrl,
																				BYTE *pbSecurityId,
																				DWORD *pcbSecurityId, 
																				DWORD dwReserved)
{

	METHOD_PROLOGUE(IIEOleControlSite, InternetSecurityManager)
		return INET_E_DEFAULT_ACTION;
}
#endif


STDMETHODIMP IIEOleControlSite::XInternetSecurityManager::ProcessUrlAction(	/* [in] */ LPCWSTR pwszUrl,
									/* [in] */ DWORD dwAction,
									/* [size_is][out] */ BYTE __RPC_FAR *pPolicy,
									/* [in] */ DWORD cbPolicy,
									/* [in] */ BYTE __RPC_FAR *pContext,
									/* [in] */ DWORD cbContext,
									/* [in] */ DWORD dwFlags,
									/* [in] */ DWORD dwReserved)
{
	METHOD_PROLOGUE(IIEOleControlSite, InternetSecurityManager)
	return URLPOLICY_ALLOW;
}

HRESULT IIEOleControlSite::XInternetSecurityManager
							::QueryCustomPolicy(LPCWSTR pwszUrl,
												REFGUID guidKey,
												BYTE **ppPolicy,
												DWORD *pcbPolicy,
												BYTE *pContext,
												DWORD cbContext,
												DWORD dwReserved)
{
	METHOD_PROLOGUE(IIEOleControlSite, InternetSecurityManager)
	return INET_E_DEFAULT_ACTION;
}

HRESULT IIEOleControlSite::XInternetSecurityManager
							::SetZoneMapping(DWORD dwZone,
											LPCWSTR lpszPattern,
											DWORD dwFlags)
{
	METHOD_PROLOGUE(IIEOleControlSite, InternetSecurityManager)
	return INET_E_DEFAULT_ACTION;
}

HRESULT IIEOleControlSite::XInternetSecurityManager
							::GetZoneMappings(DWORD dwZone,
											IEnumString **ppenumString, 
											DWORD dwFlags)
{ 
	METHOD_PROLOGUE(IIEOleControlSite, InternetSecurityManager)
	return INET_E_DEFAULT_ACTION;
}
//////////////////////////////////////////////////////////////////////////
//INEW WINDOW
ULONG IIEOleControlSite::XNewWindowManager::AddRef()
{
	METHOD_PROLOGUE(IIEOleControlSite, NewWindowManager);
	return pThis->ExternalAddRef();
}
ULONG IIEOleControlSite::XNewWindowManager::Release()
{
	METHOD_PROLOGUE(IIEOleControlSite, NewWindowManager);
	return pThis->ExternalRelease();
}
HRESULT IIEOleControlSite::XNewWindowManager::QueryInterface(REFIID riid, void ** ppvObj)
{
	METHOD_PROLOGUE(IIEOleControlSite, NewWindowManager);
	return pThis->ExternalQueryInterface( &riid, ppvObj );
}
HRESULT IIEOleControlSite::XNewWindowManager::EvaluateNewWindow(
	LPCWSTR pszUrl,
	LPCWSTR pszName,
	LPCWSTR pszUrlContext,
	LPCWSTR pszFeatures,
	BOOL fReplace,
	DWORD dwFlags,
	DWORD dwUserActionTime
	)
{
	return S_OK;
}

/////////////////////////////////////////////////////////////
// ServiceProvider Methods
ULONG FAR EXPORT IIEOleControlSite::XServiceProvider::AddRef()
{
	METHOD_PROLOGUE(IIEOleControlSite, ServiceProvider)
	return pThis->ExternalAddRef();
}

ULONG FAR EXPORT IIEOleControlSite::XServiceProvider::Release()
{                            
	METHOD_PROLOGUE(IIEOleControlSite, ServiceProvider)
	return pThis->ExternalRelease();
}

HRESULT FAR EXPORT IIEOleControlSite::XServiceProvider
										::QueryInterface(REFIID riid, 
														void** ppvObj)
{
	METHOD_PROLOGUE(IIEOleControlSite, ServiceProvider)
    HRESULT hr = (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);
	return hr;
}
STDMETHODIMP IIEOleControlSite::XServiceProvider::QueryService(REFGUID guidService,  
												REFIID riid,
												void** ppvObject)
{
	if (riid == IID_IDownloadManager)
	{
		int a=0;
	}

	if (riid == IID_IHTMLOMWindowServices || riid == IID_INewWindowManager|| riid==IID_IInternetSecurityManager || riid == IID_IDownloadManager )
	{
		METHOD_PROLOGUE(IIEOleControlSite, ServiceProvider);
		HRESULT hr = (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObject);
		return hr;
	} 
	else 
	{
		*ppvObject = NULL;
	}
	return E_NOINTERFACE;
}


ULONG FAR EXPORT IIEOleControlSite::XHTMLOMWindowServices::AddRef()   
{   
	METHOD_PROLOGUE(IIEOleControlSite, HTMLOMWindowServices)   
		return pThis->ExternalAddRef();   
}   
ULONG FAR EXPORT IIEOleControlSite::XHTMLOMWindowServices::Release()   
{   
	METHOD_PROLOGUE(IIEOleControlSite, HTMLOMWindowServices)   
		return pThis->ExternalRelease();   
}   
HRESULT FAR EXPORT IIEOleControlSite::XHTMLOMWindowServices::QueryInterface(REFIID riid, void **ppvObj)   
{   
	METHOD_PROLOGUE(IIEOleControlSite, HTMLOMWindowServices) 
		HRESULT hr = (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);   
	return hr;   
}
 HRESULT IIEOleControlSite::XHTMLOMWindowServices::moveTo(LONG x,LONG y)
 {
 	return E_NOTIMPL;
 }
 HRESULT IIEOleControlSite::XHTMLOMWindowServices::moveBy( LONG x,LONG y)
 {
 	return E_NOTIMPL;
 }
 HRESULT IIEOleControlSite::XHTMLOMWindowServices::resizeTo( LONG x,LONG y)
 {
	 return E_NOTIMPL;
 }
 HRESULT IIEOleControlSite::XHTMLOMWindowServices::resizeBy( LONG x,LONG y)
 {
 	return E_NOTIMPL;
 }
 

ULONG FAR EXPORT IIEOleControlSite::XOleCommandTarget::AddRef()   
{   
	METHOD_PROLOGUE(IIEOleControlSite, OleCommandTarget)   
		return pThis->ExternalAddRef();   
}
ULONG FAR EXPORT IIEOleControlSite::XOleCommandTarget::Release()   
{
	METHOD_PROLOGUE(IIEOleControlSite, OleCommandTarget)   
		return pThis->ExternalRelease();   
}   
HRESULT FAR EXPORT IIEOleControlSite::XOleCommandTarget::QueryInterface(REFIID riid, void **ppvObj)   
{   
	METHOD_PROLOGUE(IIEOleControlSite, OleCommandTarget)   
		HRESULT hr = (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);   
	return hr;   

}  
STDMETHODIMP IIEOleControlSite::XOleCommandTarget::QueryStatus(   
	/* [unique][in] */ const GUID __RPC_FAR *pguidCmdGroup,   
	/* [in] */ ULONG cCmds,   
	/* [out][in][size_is] */ OLECMD __RPC_FAR prgCmds[ ],   
	/* [unique][out][in] */ OLECMDTEXT __RPC_FAR *pCmdText   
	)   
{
	METHOD_PROLOGUE(IIEOleControlSite, OleCommandTarget)   
		return OLECMDERR_E_NOTSUPPORTED;   
}


HRESULT IIEOleControlSite::XOleCommandTarget::Exec(const GUID* pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANTARG* pvaIn, VARIANTARG* pvaOut )  
{  
	HRESULT hr = S_OK;

	if (pguidCmdGroup && IsEqualGUID(*pguidCmdGroup, CGID_DocHostCommandHandler))
	{

		switch (nCmdID) 
		{

		case OLECMDID_SHOWSCRIPTERROR:
			{
				IHTMLDocument2*             pDoc = NULL;
				IHTMLWindow2*               pWindow = NULL;
				IHTMLEventObj*              pEventObj = NULL;
				BSTR                        rgwszNames[5] = 
				{ 
					SysAllocString(L"errorLine"),
					SysAllocString(L"errorCharacter"),
					SysAllocString(L"errorCode"),
					SysAllocString(L"errorMessage"),
					SysAllocString(L"errorUrl")
				};
				DISPID                      rgDispIDs[5];
				VARIANT                     rgvaEventInfo[5];
				DISPPARAMS                  params;
				BOOL                        fContinueRunningScripts = true;
				int	                        i;
				params.cArgs = 0;
				params.cNamedArgs = 0;
				
				__try
				{
					if (pvaIn->punkVal)
					{

						// Get the document that is currently being viewed.
						hr = pvaIn->punkVal->QueryInterface(IID_IHTMLDocument2, (void **) &pDoc);
						if (hr == S_OK && pDoc)
						{
							// Get document.parentWindow.
							hr = pDoc->get_parentWindow(&pWindow);
							pDoc->Release();
							if(hr == S_OK && pWindow)
							{
								// Get the window.event object.
								hr = pWindow->get_event(&pEventObj);
								if (hr == S_OK && pEventObj)
								{
									// Get the error info from the window.event object.
									for (i = 0; i < 5; i++) 
									{  
										if(
											rgwszNames[i] && 
											rgDispIDs[i] 
										)
										{
											// Get the property's dispID.
											hr = pEventObj->GetIDsOfNames(IID_NULL, &rgwszNames[i], 1, 
												LOCALE_SYSTEM_DEFAULT, &rgDispIDs[i]);
											// Get the value of the property.
											hr = pEventObj->Invoke(rgDispIDs[i], IID_NULL,
												LOCALE_SYSTEM_DEFAULT,
												DISPATCH_PROPERTYGET, &params, &rgvaEventInfo[i],
												NULL, NULL);
											SysFreeString(rgwszNames[i]);
										}

									}

									// At this point, you would normally alert the user with 
									// the information about the error, which is now contained
									// in rgvaEventInfo[]. Or, you could just exit silently.

									(*pvaOut).vt = VT_BOOL;

									if (fContinueRunningScripts)
									{
										// Continue running scripts on the page.
										(*pvaOut).boolVal = VARIANT_TRUE;
									}
									else
									{
										// Stop running scripts on the page.
										(*pvaOut).boolVal = VARIANT_FALSE;            
									}
								}

							}


						}

					}
				}
				__except(1)
				{
					hr = OLECMDERR_E_UNKNOWNGROUP;
				}


	
				break;
			}
		default:
			hr = OLECMDERR_E_NOTSUPPORTED;
			break;
		}
	}
	else
	{
		hr = OLECMDERR_E_UNKNOWNGROUP;
	}
	return (hr);

}  


ULONG FAR EXPORT  IIEOleControlSite::XDownloadManager::AddRef()
{
	METHOD_PROLOGUE(IIEOleControlSite, DownloadManager)
		return pThis->ExternalAddRef();
}


ULONG FAR EXPORT  IIEOleControlSite::XDownloadManager::Release()
{                            
	METHOD_PROLOGUE(IIEOleControlSite, DownloadManager)
		return pThis->ExternalRelease();
}

HRESULT FAR EXPORT  IIEOleControlSite::XDownloadManager::QueryInterface(REFIID riid, void **ppvObj)
{
	METHOD_PROLOGUE(IIEOleControlSite, DownloadManager)
		HRESULT hr = (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);
	return hr;
}


HRESULT FAR EXPORT IIEOleControlSite::XDownloadManager::Download(
	/* [in] */ IMoniker __RPC_FAR *pmk,
	/* [in] */ IBindCtx __RPC_FAR *pbc,
	/* [in] */ DWORD dwBindVerb,
	/* [in] */ LONG grfBINDF,
	/* [in] */ BINDINFO __RPC_FAR *pBindInfo,
	/* [in] */ LPCOLESTR pszHeaders,
	/* [in] */ LPCOLESTR pszRedir,
	/* [in] */ UINT uiCP)
{

	CString strFileUrl;

	BSTR bsFileUrl;
	pmk->GetDisplayName( pbc , NULL , &bsFileUrl) ;
	strFileUrl=bsFileUrl;
	CoTaskMemFree(bsFileUrl);
	//fixit: free bsFileUrl

	return S_FALSE;
}