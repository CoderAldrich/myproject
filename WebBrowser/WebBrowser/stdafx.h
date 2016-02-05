// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#define _ATL_APARTMENT_THREADED 
#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS


#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����

#include <afxinet.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxhtml.h>                    // MFC HTML ��ͼ֧��


#include ".\UI\UIPublic.h"
#include ".\UI\UIResource.h"
#include ".\UI\IUIControler.h"
#include ".\UI\IUINotifyer.h"
#include ".\resource.h"
#include "IWBCoreControler.h"
#include "IWBCoreNotifyer.h"


IUIControler * NewMainFrame( IUINotifyer *pUINotifyer,UINT UIType, HWND *phWnd ,BOOL bMutiTab = TRUE,BOOL bMenuBar = TRUE,BOOL bToolBar = TRUE,BOOL bCommandBar = TRUE );
IWBCoreControler * NewWBCore(IWBCoreNotifyer *pWBCoreNotifyer,PVOID *ppPageRef,HANDLE *pThreadHandle,LPCTSTR pszUrl,CRect rcClient,LONG * nNewPageID=NULL);
bool NewBrowserInstance(IUIControler *pUIControl,PVOID *ppPageRef,CString strUrl,LONG * nNewPageID=NULL);
bool AttachWBCoreToFrame(IWBCoreControler *pWBCoreControler,IWBCoreNotifyer *pNewWBCoreNotifyer,IUIControler *pUIControler);
bool DettachWBCoreFrameFrame(IWBCoreControler *pWBCoreControler,IUIControler *pUIControler);
UINT __cdecl ShadowFrameThread(PVOID pParam);


extern UINT nHtmlMsg;
#define WM_TAB_SEL_CHANGE WM_USER+1111
#define WM_TAB_CLOSE      WM_USER+1112
#define WM_TAB_DRAG       WM_USER+1113

#define WM_STATUS_CHANGE WM_USER+1116

#define WM_TAB_BTN_CLOSE WM_USER+1117
#define WM_TAB_BTN_ADD   WM_USER+1118

#define WM_TAB_NOTIFY_ICON_CHANGE WM_USER+1119

#define WM_KEY_CHANGE_TAB WM_USER+1121

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>

