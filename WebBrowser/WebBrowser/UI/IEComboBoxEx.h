
#include "IEAddrEdit.h"
class CIEComboBoxEx:public CComboBoxEx
{
public:
	CIEComboBoxEx();
	~CIEComboBoxEx();
private:
	CImageList  comboexlist;
	CIEAddrEdit  m_Edit;
public:
	
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	bool AddOrChoiceUrlItem(CString strUrl, CString strIcoFillPath=TEXT(""));
	bool UpdateItemIcon(CString strUrl,CString strIcoFillPath);
	int FindString(LPCTSTR lpszText);
	afx_msg void OnCbnSelchange();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};