
// CNZZExportDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CNZZExport.h"
#include "CNZZExportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCNZZExportDlg 对话框

#include "CNZZ.h"

CCNZZExportDlg *g_pThis = NULL;
VOID WINAPI CNZZMsgShowCallBack(LPCWSTR pszMsgShow)
{
	int nTextLen = wcslen(pszMsgShow);
	if ( nTextLen > 0 )
	{
		WCHAR *pszTempMsg = new WCHAR[nTextLen+1];
		wcscpy_s(pszTempMsg,nTextLen+1,pszMsgShow);
		pszTempMsg[nTextLen] = 0;
		g_pThis->PostMessage( WM_USER+2222,(WPARAM)pszTempMsg,0 );
	}
}

CCNZZExportDlg::CCNZZExportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCNZZExportDlg::IDD, pParent)
	, m_strCNZZID(_T(""))
	, m_strCNZZPassWord(_T(""))
	, m_strExportDayTime(_T(""))
	, m_strExportSavePath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCNZZExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strCNZZID);
	DDX_Text(pDX, IDC_EDIT2, m_strCNZZPassWord);
	DDX_Text(pDX, IDC_EDIT3, m_strExportDayTime);
	DDX_Control(pDX, IDC_EDIT4, m_editMsgShow);
	DDX_Text(pDX, IDC_EDIT5, m_strExportSavePath);
}

BEGIN_MESSAGE_MAP(CCNZZExportDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CCNZZExportDlg::OnBnClickedOk)
	ON_MESSAGE(WM_USER+2222,OnShowText)
END_MESSAGE_MAP()


// CCNZZExportDlg 消息处理程序

BOOL CCNZZExportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	g_pThis = this;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCNZZExportDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCNZZExportDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CCNZZExportDlg::OnShowText( WPARAM wParam,LPARAM lParam )
{
	LPCWSTR pszShowText = (LPCWSTR)wParam;

	int nTextLen = m_editMsgShow.GetWindowTextLengthW();
	m_editMsgShow.SetSel(nTextLen,nTextLen);
	m_editMsgShow.ReplaceSel(CString(pszShowText)+L"\r\n");

	delete pszShowText;

	return 0;
}

typedef struct tagEXPORT_PARAM
{
	CString strCNZZID;
	CString strCNZZPassWord;
	CString strExportDayTime;
	CString strSavePath;

}EXPORT_PARAM,*PEXPORT_PARAM;
DWORD WINAPI CNZZDataExportThread(PVOID pParam)
{

	PEXPORT_PARAM pExportParam = (PEXPORT_PARAM)pParam;

	if (pExportParam->strSavePath.Right(1) != L"\\")
	{
		pExportParam->strSavePath+=L"\\";
	}
	CNZZDataExport(pExportParam->strCNZZID,pExportParam->strCNZZPassWord,pExportParam->strExportDayTime,pExportParam->strSavePath,CNZZMsgShowCallBack);

	delete pExportParam;

	return 0;
}

void CCNZZExportDlg::OnBnClickedOk()
{
	UpdateData();

	if ( m_strCNZZID.GetLength() == 0 )
	{
		AfxMessageBox(L"请输入CNZZID");
		return ;
	}

	if ( m_strCNZZPassWord.GetLength() == 0 )
	{
		AfxMessageBox(L"请输入CNZZ密码");
		return ;
	}

	if ( m_strExportDayTime.GetLength() != 10 )
	{
		AfxMessageBox(L"导出日期格式不对，必须为10位，不够用0补充");
		return ;
	}

	
	SHGetSpecialFolderPathW(0,m_strExportSavePath.GetBuffer(MAX_PATH),CSIDL_DESKTOPDIRECTORY,FALSE);
	m_strExportSavePath.ReleaseBuffer();

	if ( FALSE == PathFileExistsW( m_strExportSavePath ))
	{
		AfxMessageBox(L"保存路劲不存在");
		return ;
	}

	PEXPORT_PARAM pExportParam = new EXPORT_PARAM;
	pExportParam->strCNZZID = m_strCNZZID;
	pExportParam->strCNZZPassWord = m_strCNZZPassWord;
	pExportParam->strExportDayTime = m_strExportDayTime;
	pExportParam->strSavePath = m_strExportSavePath;

	HANDLE hThread = CreateThread(NULL,0,CNZZDataExportThread,pExportParam,0,NULL);
	CloseHandle(hThread);

}
