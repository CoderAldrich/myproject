
//=--------------------------------------------------------------------------=
//  (C) Copyright 1996-1999 Microsoft Corporation. All Rights Reserved.
//=--------------------------------------------------------------------------=
#ifndef __CUSTOMSITEH__
#define __CUSTOMSITEH__

#include <mshtmhst.h>
#include <..\src\mfc\occimpl.h>
// 
// NOTE: 
// Some of the code in this file is MFC implementation specific.
// Changes in future versions of MFC implementation may require
// the code to be changed. Please check the readme of this
// sample for more information 
// 


//#include "MainFrm.h"
#include "IECoreView.h"

//#include "downloadmgr.h"


class IIEOleControlSite: public COleControlSite
{
public:
	IIEOleControlSite(COleControlContainer *pCnt,CIECoreView *frame):COleControlSite(pCnt)
	{
		m_pView=frame;
	}
	~IIEOleControlSite()
	{
		int a=0;
	}
	CIECoreView* m_pView;
protected:

	DECLARE_INTERFACE_MAP();

	////////////////////////////////
	//Implement IDocHostUIHandler
	
BEGIN_INTERFACE_PART(DocHostUIHandler, IDocHostUIHandler)
		STDMETHOD(ShowContextMenu)( DWORD dwID,
		 POINT __RPC_FAR *ppt,
		 IUnknown __RPC_FAR *pcmdtReserved,
		 IDispatch __RPC_FAR *pdispReserved);
		STDMETHOD(GetHostInfo)( DOCHOSTUIINFO __RPC_FAR *pInfo);
		STDMETHOD(ShowUI)( 
			 DWORD dwID,
			 IOleInPlaceActiveObject __RPC_FAR *pActiveObject,
			 IOleCommandTarget __RPC_FAR *pCommandTarget,
			 IOleInPlaceFrame __RPC_FAR *pFrame,
			 IOleInPlaceUIWindow __RPC_FAR *pDoc);
		STDMETHOD(HideUI)(void);
		STDMETHOD(UpdateUI)(void);
		STDMETHOD(EnableModeless)( BOOL fEnable);
		STDMETHOD(OnDocWindowActivate)( BOOL fEnable);
		STDMETHOD(OnFrameWindowActivate)( BOOL fEnable);
		STDMETHOD(ResizeBorder)( 
			 LPCRECT prcBorder,
			 IOleInPlaceUIWindow __RPC_FAR *pUIWindow,
			 BOOL fRameWindow);
		STDMETHOD(TranslateAccelerator)( 
			 LPMSG lpMsg,
			 const GUID __RPC_FAR *pguidCmdGroup,
			 DWORD nCmdID);
		STDMETHOD(GetOptionKeyPath)( 
			 LPOLESTR __RPC_FAR *pchKey,
			 DWORD dw);
		STDMETHOD(GetDropTarget)(
			 IDropTarget __RPC_FAR *pDropTarget,
			 IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget);
		STDMETHOD(GetExternal)( 
			 IDispatch __RPC_FAR *__RPC_FAR *ppDispatch);
		STDMETHOD(TranslateUrl)( 
			 DWORD dwTranslate,
			 OLECHAR __RPC_FAR *pchURLIn,
			 OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut);
		STDMETHOD(FilterDataObject)( 
			 IDataObject __RPC_FAR *pDO,
			 IDataObject __RPC_FAR *__RPC_FAR *ppDORet);
END_INTERFACE_PART(DocHostUIHandler)
///////////////////////////////////////
// Implement IInternetSecurityManager
BEGIN_INTERFACE_PART(DocHostUIHandler2, IDocHostUIHandler2)
		STDMETHOD(ShowContextMenu)( DWORD dwID,
		POINT __RPC_FAR *ppt,
		IUnknown __RPC_FAR *pcmdtReserved,
		IDispatch __RPC_FAR *pdispReserved);
		STDMETHOD(GetHostInfo)( DOCHOSTUIINFO __RPC_FAR *pInfo);
		STDMETHOD(ShowUI)( 
						  DWORD dwID,
						  IOleInPlaceActiveObject __RPC_FAR *pActiveObject,
						  IOleCommandTarget __RPC_FAR *pCommandTarget,
						  IOleInPlaceFrame __RPC_FAR *pFrame,
						  IOleInPlaceUIWindow __RPC_FAR *pDoc);
		STDMETHOD(HideUI)(void);
		STDMETHOD(UpdateUI)(void);
		STDMETHOD(EnableModeless)( BOOL fEnable);
		STDMETHOD(OnDocWindowActivate)( BOOL fEnable);
		STDMETHOD(OnFrameWindowActivate)( BOOL fEnable);
		STDMETHOD(ResizeBorder)( 
								LPCRECT prcBorder,
								IOleInPlaceUIWindow __RPC_FAR *pUIWindow,
								BOOL fRameWindow);
		STDMETHOD(TranslateAccelerator)( 
										LPMSG lpMsg,
										const GUID __RPC_FAR *pguidCmdGroup,
										DWORD nCmdID);
		STDMETHOD(GetOptionKeyPath)( 
									LPOLESTR __RPC_FAR *pchKey,
									DWORD dw);
		STDMETHOD(GetDropTarget)(
								 IDropTarget __RPC_FAR *pDropTarget,
								 IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget);
		STDMETHOD(GetExternal)( 
							   IDispatch __RPC_FAR *__RPC_FAR *ppDispatch);
		STDMETHOD(TranslateUrl)( 
								DWORD dwTranslate,
								OLECHAR __RPC_FAR *pchURLIn,
								OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut);
		STDMETHOD(FilterDataObject)( 
									IDataObject __RPC_FAR *pDO,
									IDataObject __RPC_FAR *__RPC_FAR *ppDORet);
		STDMETHOD(GetOverrideKeyPath)(LPOLESTR *pchKey,DWORD dw);
END_INTERFACE_PART(DocHostUIHandler2)

///////////////////////////////////////
// Implement IInternetSecurityManager
BEGIN_INTERFACE_PART(InternetSecurityManager, IInternetSecurityManager)
		STDMETHOD(SetSecuritySite)(IInternetSecurityMgrSite*);
		STDMETHOD(GetSecuritySite)(IInternetSecurityMgrSite**);
		STDMETHOD(MapUrlToZone)(LPCWSTR,DWORD*,DWORD);
		
		STDMETHOD(ProcessUrlAction)(
            LPCWSTR pwszUrl,
            DWORD dwAction,
            BYTE __RPC_FAR *pPolicy,
            DWORD cbPolicy,
            BYTE __RPC_FAR *pContext,
            DWORD cbContext,
            DWORD dwFlags,
            DWORD dwReserved = 0);
		STDMETHOD(QueryCustomPolicy)(LPCWSTR,REFGUID,BYTE**,DWORD*,BYTE*,DWORD,DWORD);
		STDMETHOD(SetZoneMapping)(DWORD,LPCWSTR,DWORD);
		STDMETHOD(GetZoneMappings)(DWORD,IEnumString**,DWORD);
#ifdef _WIN64
		STDMETHOD(GetSecurityId)( LPCWSTR , BYTE *,DWORD *,DWORD_PTR);
#else
		STDMETHOD(GetSecurityId)(LPCWSTR,BYTE*,DWORD*,DWORD);
#endif
		
END_INTERFACE_PART(InternetSecurityManager)

///////////////////////////////////////
//// Implement INewWindowManager
BEGIN_INTERFACE_PART(NewWindowManager, INewWindowManager)        
	STDMETHOD(EvaluateNewWindow)(LPCWSTR pszUrl,LPCWSTR pszName,LPCWSTR pszUrlContext,LPCWSTR pszFeatures,BOOL fReplace,DWORD dwFlags,DWORD dwUserActionTime);
END_INTERFACE_PART(NewWindowManager)

///////////////////////////////////////
//// Implement IHTMLOMWindowServices
BEGIN_INTERFACE_PART(HTMLOMWindowServices, IHTMLOMWindowServices)        
		STDMETHOD(moveTo)(LONG x,LONG y);
		STDMETHOD(moveBy)( LONG x,LONG y);
		STDMETHOD(resizeTo)( LONG x,LONG y);
		STDMETHOD(resizeBy)( LONG x,LONG y);
END_INTERFACE_PART(HTMLOMWindowServices)



///////////////////////////////////////
//// Implement IServiceProvider
BEGIN_INTERFACE_PART(ServiceProvider, IServiceProvider)
		STDMETHOD(QueryService)(REFGUID,REFIID,void**);
END_INTERFACE_PART(ServiceProvider)


///////////////////////////////////////
//// Implement IOleCommandTarget
BEGIN_INTERFACE_PART(OleCommandTarget, IOleCommandTarget)  
		STDMETHOD(QueryStatus)(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText);  
		STDMETHOD(Exec)(const GUID* pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANTARG* pvaIn, VARIANTARG* pvaOut);  
END_INTERFACE_PART(OleCommandTarget) 

};




#endif