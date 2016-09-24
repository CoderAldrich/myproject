
// CDNTesterDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CDNTester.h"
#include "CDNTesterDlg.h"

#include "PublicFun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HWND hMsgWnd = NULL;

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CCDNTesterDlg �Ի���




CCDNTesterDlg::CCDNTesterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCDNTesterDlg::IDD, pParent)
	, m_strTestUrl(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCDNTesterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strTestUrl);
	DDX_Control(pDX, IDC_EDIT2, m_wndReqAppendHeaders);
	DDX_Control(pDX, IDC_EDIT3, m_wndTestIps);
	DDX_Control(pDX, IDC_EDIT4, m_wndMsgOut);
}

BEGIN_MESSAGE_MAP(CCDNTesterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CCDNTesterDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CCDNTesterDlg::OnBnClickedButton1)
	ON_MESSAGE(WM_USER+1111,OnMsgOut)
END_MESSAGE_MAP()


// CCDNTesterDlg ��Ϣ�������

BOOL CCDNTesterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	m_strTestUrl=L"http://dw.xj6x.com/fn/0/hlock.zip";
	m_wndTestIps.SetWindowText(L"120.24.17.132");
	UpdateData(FALSE);

	hMsgWnd = m_hWnd;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCDNTesterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CCDNTesterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




//�ٽ���������
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

CCSLock     g_strLstIpsLock;
CStringList g_strLstIps;
CStringList g_strLstAppendHead;
CString     g_strCheckUrl;

VOID DebugMsgOut( LPCWSTR pszMsg )
{
	int nLen = wcslen(pszMsg);
	WCHAR *pszMsgBuf = new WCHAR[nLen+1];
	wcscpy_s(pszMsgBuf,nLen+1,pszMsg);
	::PostMessage(hMsgWnd,WM_USER+1111,(WPARAM)pszMsgBuf,NULL);
}

DWORD WINAPI DownloadThread(PVOID pParam)
{
	BOOL bBreak = FALSE;
	

	while ( FALSE == bBreak )
	{
		CString strTestIp;


		g_strLstIpsLock.Lock();
		
		if (g_strLstIps.GetCount() == 0 )
		{
			DebugMsgOut( L"����IP���������" );
			bBreak = TRUE;
		}
		else
		{
			strTestIp = g_strLstIps.GetHead();
			g_strLstIps.RemoveHead();
		}

		g_strLstIpsLock.UnLock();

		if ( FALSE == strTestIp.IsEmpty() )
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

				bRequestRes = RequestData(strTestIp,g_strCheckUrl,&g_strLstAppendHead,&pRecvBuf,&llRecvDataLen,&nContentStart);

				GetDataMd5(pRecvBuf+nContentStart,llRecvDataLen-nContentStart,strDataMd5.GetBuffer(50),50);
				strDataMd5.ReleaseBuffer();


				if ( bRequestRes && pRecvBuf )
				{
					char *pHeadBuf = strResponseHead.GetBuffer(500);
					memcpy_s(pHeadBuf,500,pRecvBuf,nContentStart);
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
			strMsgOut.Format(L"RemoteIp: %s\r\nResult:%d\r\nContentLen:%d\r\nMd5: %s\r\nHeader:\r\n%s\r\n------------------------------------------------",strTestIp,bRequestRes,(DWORD)(llRecvDataLen-nContentStart),CString(strDataMd5),CString(strResponseHead));
			//OutputDebugStringW(strMsgOut);

			DebugMsgOut( strMsgOut );
		}
	}

	return 0;
}

void CCDNTesterDlg::OnBnClickedOk()
{
	UpdateData();

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

	g_strLstIps.RemoveAll();

	nLineCount = m_wndTestIps.GetLineCount();
	for ( int i=0;i< nLineCount;i++)
	{
		int nRetLen = m_wndTestIps.GetLine(i,szLineText,4000);
		if (nRetLen >= 0)
		{
			szLineText[nRetLen] = 0;

			if ( wcslen(szLineText) > 0 )
			{
				if ( g_strLstIps.Find(szLineText) == 0 )
				{
					g_strLstIps.AddTail(szLineText);
				}
			}
		}
	}


	for ( int i=0;i<5;i++)
	{
		CreateThread(NULL,0,DownloadThread,NULL,0,NULL);
	}

}

void CCDNTesterDlg::OnBnClickedButton1()
{
	m_wndMsgOut.SetWindowText(L"");
}


LRESULT CCDNTesterDlg::OnMsgOut(WPARAM wParam,LPARAM lParam)
{
	LPCWSTR pszMsgOut = (LPCWSTR)wParam;
	if (pszMsgOut)
	{
		int nTextLen = m_wndMsgOut.GetWindowTextLengthW();
		m_wndMsgOut.SetSel(nTextLen,nTextLen);
		m_wndMsgOut.ReplaceSel(CString(pszMsgOut)+L"\r\n");
		delete pszMsgOut;
	}
	return 0;
}