#pragma once;

#include "IWBCoreNotifyer.h"
//������ں���Ҫʵ�����½ӿڣ���ʹ�ⲿ���Բ����ں�
class IWBCoreControler
{
public:

	virtual IWebBrowser2 *GetSafeWebBrowser2() = 0;
	virtual IWebBrowser *GetSafeWebBrowser() = 0;
	virtual HWND QueryIEServerWnd() = 0;

	//�ر��ں�
	virtual unsigned long ControlClose() = 0;

	//��ȡ�ں�չʾ���ھ��
	virtual unsigned long ControlQueryWnd(HWND *phWnd) = 0;
	
	//���ػ�����ʾ���� 
	virtual unsigned long ControlShowWindow(BOOL bShow) = 0;
	
	//���ý���
	virtual unsigned long ControlSetFocus() = 0;

	//��������λ��
	virtual unsigned long ControlMoveWindow(int nLeft,int nTop,int nWidth,int nHeight) = 0;

	//��ȡ�ں˵�ǰ������
	virtual unsigned long ControlQueryUrl(LPTSTR pszUrl,UINT nLen) = 0;

	//��ȡ�ں˵�ǰ�ı���
	virtual unsigned long ControlQueryTitle(LPTSTR pszTitle,UINT nLen) = 0;

	//�����ں˵�����ĳһ������
	virtual unsigned long ControlGotoUrl( const wchar_t *pszTargetUrl , const wchar_t *pszReferer )=0;

	//�����ں˽��к��˲���
	virtual unsigned long ControlGoBack( )=0;

	//�����ں˽���ǰ������
	virtual unsigned long ControlGoForward( )=0;

	virtual unsigned long ControlQueryBackForwardStatus(BOOL *pbCanBack,BOOL *pbCanForward) = 0;

	//�����ں�ˢ��
	virtual unsigned long ControlRefresh( )=0;

	//�����ں�֪ͨ����ҳ��
	virtual unsigned long ControlStopLoading()=0;

	virtual unsigned long ControlWaitDocumentComplete( DWORD dwTimeOut ) = 0;

	virtual unsigned long ControlWaitNewWindow( IWBCoreControler **ppWBControl,LPCWSTR *pszAllowUrls,int nAllowCount,DWORD dwTimeOut ) = 0;
	virtual unsigned long ControlWaitDownloadFile( LPWSTR pszFileUrl, DWORD cchFileUrl ,DWORD dwTimeOut ) = 0;
};