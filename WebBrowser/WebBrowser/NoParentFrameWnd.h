#pragma once


// CNoParentFrameWnd ���

class CNoParentFrameWnd : public CFrameWnd
{
	DECLARE_DYNCREATE(CNoParentFrameWnd)
public:
	CNoParentFrameWnd();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CNoParentFrameWnd();

protected:
	DECLARE_MESSAGE_MAP()
};


