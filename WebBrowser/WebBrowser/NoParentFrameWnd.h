#pragma once


// CNoParentFrameWnd 框架

class CNoParentFrameWnd : public CFrameWnd
{
	DECLARE_DYNCREATE(CNoParentFrameWnd)
public:
	CNoParentFrameWnd();           // 动态创建所使用的受保护的构造函数
	virtual ~CNoParentFrameWnd();

protected:
	DECLARE_MESSAGE_MAP()
};


