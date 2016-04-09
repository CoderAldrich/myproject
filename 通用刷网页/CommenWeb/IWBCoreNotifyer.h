#pragma once

typedef  DWORD PAGEID;
//�ں��ڲ�״̬�����仯ʱ���֪ͨ�ӿ�
class IWBCoreNotifyer
{

public:
	IWBCoreNotifyer(){};
	~IWBCoreNotifyer(){};
public:

	//�ں�������´���
	virtual unsigned long NotifyNewWindow(PVOID *ppPageRef,LPCTSTR pszUrl,BOOL *bCancel,BOOL bNewFrame,LONG * nNewPageID=NULL)=0;

	//�ں˽�Ҫ������ĳһ��URLʱ
	virtual unsigned long NotifyBeforeMainNavigate(PAGEID nPageID,LPCTSTR pszUrl,BOOL *bCancel) = 0;

	//�ں��ӿ�ܽ�Ҫ������ĳһ��URLʱ
	virtual unsigned long NotifyBeforeSubNavigate(PAGEID nPageID,LPCTSTR pszUrl,BOOL *bCancel) = 0;
	
	//���ĵ��������ʱ
	virtual unsigned long NotifyMainDocumentComplete(PAGEID nPageID,LPCTSTR pszUrl) = 0;

	//�ں˱��ⷢ���仯
	virtual unsigned long NotifyTitleChange(PAGEID nPageID,LPCTSTR pszNewTitle)=0;

	//�ں����ӷ����仯
	virtual unsigned long NotifyUrlChange(PAGEID nPageID,LPCTSTR pszNewUrl)=0;

	//ǰ������״̬�����仯
	virtual unsigned long NotifyStatusCommand(PAGEID nPageID,BOOL bCanBack,BOOL bCanForward) = 0;
	
	//״̬���������ݷ����仯
	virtual unsigned long NotifyStatusTextChange(PAGEID nPageID,LPCTSTR pszNewStatusText) = 0;

	//���õ�ǰ��չʾ��ҳ��
	virtual unsigned long NotifySetCurrentPage(PAGEID nPageID)=0;

	//��ҳ����Ҫ��ر�
	virtual unsigned long NotifyCloseThis(PAGEID nPageID)=0;


};