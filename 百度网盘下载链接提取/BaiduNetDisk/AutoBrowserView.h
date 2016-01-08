#pragma once
#include "IECoreView.h"

class CAutoBrowserView : public CIECoreView
{
public: 
	CAutoBrowserView();
	DECLARE_DYNCREATE(CAutoBrowserView)
	virtual ~CAutoBrowserView();
public:
	VOID AutoStartWork();
	virtual BOOL StartWork();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL m_bVirtualMoveing;
	UINT m_nVirtualMove;
	UINT m_nMaxVirtualMove;
	UINT m_nAfterMoveTimeID;
	UINT m_nAfterTimeDelay;

	BOOL m_bAutoStartWork;
	BOOL m_bAutoStartWorkDo;

protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
public:
	virtual void NewWindow3( IDispatch **ppDisp,VARIANT_BOOL *Cancel,DWORD dwFlags,BSTR bstrUrlContext,BSTR bstrUrl);
	virtual void OnNewWindow3( IDispatch **ppDisp,VARIANT_BOOL *Cancel,DWORD dwFlags,BSTR bstrUrlContext,BSTR bstrUrl);
	virtual void OnMainDocumentComplete(LPDISPATCH pDisp, VARIANT* URL);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};



