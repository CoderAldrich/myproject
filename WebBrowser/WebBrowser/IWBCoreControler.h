#pragma once;

#include "IWBCoreNotifyer.h"
//������ں���Ҫʵ�����½ӿڣ���ʹ�ⲿ���Բ����ں�
class IWBCoreControler
{
public:
	//�����ں��ڲ������仯ʱ��֪ͨ�ӿ�ָ�룬�豣���ָ����ں�״̬�����仯��֪ͨ�ⲿ
	virtual unsigned long SetNotifyPtr( IWBCoreNotifyer *pNotifyer ) = 0;

	//��ȡ֪ͨ�ӿ�ָ��
	virtual unsigned long GetNotifyPtr( IWBCoreNotifyer **ppNotifyer ) = 0;

	//����ҳ���ʶ  ��Ҫ�����PageID
	virtual unsigned long SetPageID( PAGEID nPageID ) = 0;

	//��ȡ  PageID
	virtual unsigned long GetPageID( PAGEID *pPageID) = 0;

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
	virtual unsigned long ControlGotoUrl( const wchar_t *pszTargetUrl )=0;

	//�����ں˽��к��˲���
	virtual unsigned long ControlGoBack( )=0;

	//�����ں˽���ǰ������
	virtual unsigned long ControlGoForward( )=0;

	virtual unsigned long ControlQueryBackForwardStatus(BOOL *pbCanBack,BOOL *pbCanForward) = 0;

	//�����ں�ˢ��
	virtual unsigned long ControlRefresh( )=0;

	//�����ں�֪ͨ����ҳ��
	virtual unsigned long ControlStopLoading()=0;

	//�����ں˽��м��в���
	virtual unsigned long ControlEditCut()=0;
	
	//�����ں˽��и��Ʋ���
	virtual unsigned long ControlEditCopy()=0;

	//�����ں˽���ճ������
	virtual unsigned long ControlEditPaste()=0;

	//�����ں˽���ȫѡ����
	virtual unsigned long ControlEditSelectAll()=0;

	//�����ں˽��в��Ҳ���
	virtual unsigned long ControlEditFind()=0;
};