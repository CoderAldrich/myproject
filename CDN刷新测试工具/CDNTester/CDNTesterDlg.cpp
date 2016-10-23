
// CDNTesterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CDNTester.h"
#include "CDNTesterDlg.h"

#include "PublicFun.h"
#include <WinInet.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HWND hMsgWnd = NULL;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCDNTesterDlg 对话框




CCDNTesterDlg::CCDNTesterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCDNTesterDlg::IDD, pParent)
	, m_strTestUrl(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_phWorkThreads = NULL;
	m_hWatchThread = NULL;
}

void CCDNTesterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strTestUrl);
	DDX_Control(pDX, IDC_EDIT2, m_wndReqAppendHeaders);
	DDX_Control(pDX, IDC_EDIT3, m_wndTestIps);
	DDX_Control(pDX, IDC_EDIT4, m_wndMsgOut);
	DDX_Control(pDX, IDC_EDIT5, m_wndHeadMsg);
	DDX_Control(pDX, IDC_EDIT6, m_editSavePath);
	DDX_Control(pDX, IDC_EDIT7, m_editFileExt);
	DDX_Control(pDX, IDC_CHECK1, m_chkSaveToFile);
}

BEGIN_MESSAGE_MAP(CCDNTesterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CCDNTesterDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CCDNTesterDlg::OnBnClickedButton1)
	ON_MESSAGE(WM_USER+1111,OnMsgOut)
	ON_MESSAGE(WM_USER+1112,OnWorkDone)
	ON_BN_CLICKED(IDC_CHECK1, &CCDNTesterDlg::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CCDNTesterDlg 消息处理程序

BOOL CCDNTesterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_strTestUrl=L"http://www.qq.com/";
	//m_wndTestIps.SetWindowText(L"120.24.17.132\r\n120.52.20.58\r\n");
	//m_wndTestIps.SetWindowText(L"123.126.113.42\r\n");
	m_wndReqAppendHeaders.SetWindowText(L"Accept-Encoding: gzip\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.22 Safari/537.36 SE 2.X MetaSr 1.0");
	UpdateData(FALSE);

	hMsgWnd = m_hWnd;

	m_editFileExt.EnableWindow(FALSE);
	m_editSavePath.EnableWindow(FALSE);
	m_chkSaveToFile.SetCheck(FALSE);

	m_editSavePath.SetWindowText(L"C:\\test\\dltest\\");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCDNTesterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCDNTesterDlg::OnPaint()
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
HCURSOR CCDNTesterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




//临界区互斥锁
class CCSLock
{
private:
	CRITICAL_SECTION m_cs;
public:
	CCSLock()
	{
		InitializeCriticalSection(&m_cs);
	}
	~CCSLock()
	{
		DeleteCriticalSection(&m_cs);
	}
	VOID Lock()
	{
		EnterCriticalSection(&m_cs);
	}

	VOID UnLock()
	{
		LeaveCriticalSection(&m_cs);
	}

};

CCSLock     g_strLstRemoteInfoLock;
CStringList g_strLstRemoteInfo;
CStringList g_strLstAppendHead;
CString     g_strCheckUrl;

BOOL        g_bSaveToPath;
CString     g_strSavePath;
CString     g_strFileExt;


VOID DebugMsgOut( LPCWSTR pszMsg,BOOL bHeadMsg = FALSE )
{
	int nLen = wcslen(pszMsg);
	WCHAR *pszMsgBuf = new WCHAR[nLen+1];
	wcscpy_s(pszMsgBuf,nLen+1,pszMsg);
	::PostMessage(hMsgWnd,WM_USER+1111,(WPARAM)pszMsgBuf,bHeadMsg);
}

BOOL CheckCreateDirectory( LPCWSTR pszPath )
{
	CString strPath;
	strPath = pszPath;
	if (strPath.GetAt(strPath.GetLength()-1) != L'\\')
	{
		strPath+=L"\\";
	}

	int nPathEnd = 0;
	while ( (nPathEnd = strPath.Find(L"\\",nPathEnd)) > 0 )
	{
		CString strTempPath;
		strTempPath = strPath.Left(nPathEnd+1);
		nPathEnd++;
		
		CreateDirectoryW(strTempPath,NULL);
	}

	return TRUE;
}

DWORD WINAPI DownloadThread(PVOID pParam)
{
	BOOL bBreak = FALSE;
	

	while ( FALSE == bBreak )
	{
		CString strRemoteInfo;

		g_strLstRemoteInfoLock.Lock();
		
		if (g_strLstRemoteInfo.GetCount() == 0 )
		{
			bBreak = TRUE;
		}
		else
		{
			strRemoteInfo = g_strLstRemoteInfo.GetHead();
			g_strLstRemoteInfo.RemoveHead();
		}

		g_strLstRemoteInfoLock.UnLock();

		if ( FALSE == strRemoteInfo.IsEmpty() )
		{
			BOOL bRequestRes = FALSE;

			CStringA strDataMd5;
			CStringA strResponseHead;

			BYTE *pRecvBuf = NULL;
			LONGLONG llRecvDataLen = 0;
			int   nContentStart = 0;
			
			for ( int i=0;i<3;i++)
			{
				pRecvBuf = NULL;
				llRecvDataLen = 0;
				nContentStart = 0;
				
				USHORT usRemotePort = 0;
				CString strTempParts[2];
				int nPartCount = DivisionString(L":",strRemoteInfo,strTempParts,2);
				if ( nPartCount == 2 )
				{
					usRemotePort = _ttoi(strTempParts[1]);
				}

				bRequestRes = RequestData(strTempParts[0],usRemotePort,g_strCheckUrl,&g_strLstAppendHead,&pRecvBuf,&llRecvDataLen,&nContentStart);

				GetDataMd5(pRecvBuf+nContentStart,llRecvDataLen-nContentStart,strDataMd5.GetBuffer(50),50);
				strDataMd5.ReleaseBuffer();


				if ( bRequestRes && pRecvBuf )
				{
					if (g_bSaveToPath)
					{
						CheckCreateDirectory(g_strSavePath);

						CString strFileName;
						strFileName = g_strCheckUrl.Right(g_strCheckUrl.GetLength() - g_strCheckUrl.ReverseFind(L'/') - 1 );
						strFileName = strFileName.Left(strFileName.ReverseFind(L'.'));

						CString strFilePath;
						strFilePath.Format(L"%s%s_%s_%s.%s",g_strSavePath,strFileName,CString(strDataMd5),strRemoteInfo,g_strFileExt);
						HANDLE hFile = CreateFile(strFilePath,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
						if ( INVALID_HANDLE_VALUE != hFile )
						{
							DWORD dwWriteLen = 0;
							WriteFile(hFile,pRecvBuf,llRecvDataLen,&dwWriteLen,NULL);
							CloseHandle(hFile);
						}
					}

					char *pHeadBuf = strResponseHead.GetBuffer(1000);
					memcpy_s(pHeadBuf,1000,pRecvBuf,nContentStart);
					pHeadBuf[nContentStart] = 0;
					strResponseHead.ReleaseBuffer();

					free(pRecvBuf);
					pRecvBuf = NULL;
				}

				if (bRequestRes)
				{
					break;
				}
			}

			CString strMsgOut;
			strMsgOut.Format(L"Md5: %s RemoteIp: %s Result:%d ContentLen:%d",CString(strDataMd5),strRemoteInfo,bRequestRes,(DWORD)(llRecvDataLen-nContentStart));
			DebugMsgOut( strMsgOut ,0 );
			
			strMsgOut.Format(L"RemoteIp: %s\r\n%s",strRemoteInfo,CString(strResponseHead));
			DebugMsgOut( strMsgOut , 1 );
		}
	}

	return 0;
}

DWORD WINAPI ThreadWatch( PVOID pParam )
{
	HANDLE *pWorkThread = (HANDLE *)pParam;
	HWND hWndNotify = (HWND)pWorkThread[0];
	WaitForMultipleObjects((DWORD)pWorkThread[1],pWorkThread+2,TRUE,INFINITE);

	SendMessage(hWndNotify,WM_USER+1112,0,0);
	

	return 0;	
}

void CCDNTesterDlg::OnBnClickedOk()
{
	if( NULL == m_hWatchThread )
	{
		UpdateData();

		g_bSaveToPath = m_chkSaveToFile.GetCheck();
		m_editSavePath.GetWindowText(g_strSavePath);
		m_editFileExt.GetWindowText(g_strFileExt);

		if( g_bSaveToPath )
		{
			if ( g_strSavePath .GetLength() == 0 )
			{
				AfxMessageBox(L"请输入保存路径");
				m_editSavePath.SetFocus();
				return;
			}

			if ( g_strFileExt.GetLength() == 0 )
			{
				AfxMessageBox(L"请输入扩展名");
				m_editFileExt.SetFocus();
				return;
			}

			if(g_strSavePath.Right(1) != L"\\")
			{
				g_strSavePath+=L"\\";
			}

		}

		g_strCheckUrl = m_strTestUrl;

		WCHAR szLineText[4000];

		g_strLstAppendHead.RemoveAll();
		int nLineCount = m_wndReqAppendHeaders.GetLineCount();
		for ( int i=0;i< nLineCount;i++)
		{
			int nRetLen = m_wndReqAppendHeaders.GetLine(i,szLineText,4000);
			if ( nRetLen >= 0 )
			{
				szLineText[nRetLen] = 0;
				if ( wcslen(szLineText) > 0 )
				{
					g_strLstAppendHead.AddTail(szLineText);
				}
			}
		}

		g_strLstRemoteInfo.RemoveAll();

		nLineCount = m_wndTestIps.GetLineCount();
		for ( int i=0;i< nLineCount;i++)
		{
			int nRetLen = m_wndTestIps.GetLine(i,szLineText,4000);
			if (nRetLen >= 0)
			{
				szLineText[nRetLen] = 0;

				if ( wcslen(szLineText) > 0 )
				{
					if ( g_strLstRemoteInfo.Find(szLineText) == 0 )
					{
						g_strLstRemoteInfo.AddTail(szLineText);
					}
				}
			}
		}

		if ( g_strLstRemoteInfo.GetCount() == 0 )
		{
			WSADATA wsaData;
			WSAStartup(MAKEWORD(2,2),&wsaData);
			CString   strHostName;
		
			URL_COMPONENTSW UrlComp;
			ZeroMemory(&UrlComp,sizeof(UrlComp));
			UrlComp.dwStructSize = sizeof(UrlComp);
			UrlComp.lpszHostName = strHostName.GetBuffer( MAX_PATH );
			UrlComp.dwHostNameLength = MAX_PATH;
		
			BOOL bCrackRes = InternetCrackUrlW( m_strTestUrl , m_strTestUrl.GetLength() , 0 , &UrlComp );
			strHostName.ReleaseBuffer();

			SOCKADDR_IN sockAddr;
			memset(&sockAddr,0,sizeof(sockAddr));

			sockAddr.sin_family = AF_INET;

			LPHOSTENT lphost;
			lphost = gethostbyname(CStringA(strHostName));
			if (lphost != NULL)
			{
				sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
				g_strLstRemoteInfo.AddTail(CString(inet_ntoa(sockAddr.sin_addr)));
			}
		}

#define WORK_THREAD_NUM 5
		m_phWorkThreads = new HANDLE[WORK_THREAD_NUM+2];

		m_phWorkThreads[0] = (HANDLE)m_hWnd;
		m_phWorkThreads[1] = (HANDLE)WORK_THREAD_NUM;

		for ( int i=0;i<WORK_THREAD_NUM;i++)
		{
			m_phWorkThreads[i+2] = CreateThread(NULL,0,DownloadThread,NULL,0,NULL);
		}

		m_hWatchThread = CreateThread(NULL,0,ThreadWatch,m_phWorkThreads,0,NULL);

		GetDlgItem(IDOK)->SetWindowText(L"停止测试");
	}
	else
	{
		if ( m_phWorkThreads )
		{
			DWORD dwThreadCount = (DWORD)m_phWorkThreads[1];

			for( int i=0;i<dwThreadCount ;i++ )
			{
				TerminateThread(m_phWorkThreads[2+i],0);
			}
		}


	}

}

void CCDNTesterDlg::OnBnClickedButton1()
{
	m_wndMsgOut.SetWindowText(L"");
	m_wndHeadMsg.SetWindowText(L"");
}


LRESULT CCDNTesterDlg::OnMsgOut(WPARAM wParam,LPARAM lParam)
{
	LPCWSTR pszMsgOut = (LPCWSTR)wParam;
	if (pszMsgOut)
	{
		if( 0 == lParam )
		{
			int nTextLen = m_wndMsgOut.GetWindowTextLengthW();
			m_wndMsgOut.SetSel(nTextLen,nTextLen);
			m_wndMsgOut.ReplaceSel(CString(pszMsgOut)+L"\r\n");
			delete pszMsgOut;
		}
		else
		{
			int nTextLen = m_wndHeadMsg.GetWindowTextLengthW();
			m_wndHeadMsg.SetSel(nTextLen,nTextLen);
			m_wndHeadMsg.ReplaceSel(CString(pszMsgOut)+L"\r\n");
			delete pszMsgOut;
		}

	}
	return 0;
}

LRESULT CCDNTesterDlg::OnWorkDone(WPARAM wParam,LPARAM lParam)
{
	DebugMsgOut( L"所有IP均已测试完毕" );

	TerminateThread(m_hWatchThread,0);
	m_hWatchThread = NULL;

	if (m_phWorkThreads)
	{
		delete m_hWatchThread;
		m_hWatchThread = NULL;
	}

	GetDlgItem(IDOK)->SetWindowText(L"开始测试");

	return 0;
}
void CCDNTesterDlg::OnBnClickedCheck1()
{
	if (m_chkSaveToFile.GetCheck())
	{
		UpdateData();

		CString strFileName;
		strFileName = m_strTestUrl.Right(m_strTestUrl.GetLength() - m_strTestUrl.ReverseFind(L'/') - 1 );

		//有效的扩展名
		CString strFileExt;
		strFileExt = strFileName.Right(strFileName.GetLength() - strFileName.ReverseFind(L'.') - 1 );

		m_editFileExt.SetWindowText(strFileExt);

		m_editFileExt.EnableWindow(TRUE);
		m_editSavePath.EnableWindow(TRUE);
	}
	else
	{
		m_editFileExt.EnableWindow(FALSE);
		m_editSavePath.EnableWindow(FALSE);
	}
}
