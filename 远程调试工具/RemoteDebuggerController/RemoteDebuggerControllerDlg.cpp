
// RemoteDebuggerControllerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RemoteDebuggerController.h"
#include "RemoteDebuggerControllerDlg.h"

#include "..\公共\MemIni.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CRemoteDebuggerControllerDlg 对话框




CRemoteDebuggerControllerDlg::CRemoteDebuggerControllerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRemoteDebuggerControllerDlg::IDD, pParent)
	, m_strCmdLine(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRemoteDebuggerControllerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strCmdLine);
	DDX_Control(pDX, IDC_EDIT2, m_wndMsgShow);
	DDX_Control(pDX, IDC_LIST1, m_wndOnLineClient);
}

BEGIN_MESSAGE_MAP(CRemoteDebuggerControllerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CRemoteDebuggerControllerDlg::OnBnClickedOk)
	ON_MESSAGE(WM_USER+2222,OnHandleRecvMsg)
END_MESSAGE_MAP()

HANDLE hClientHandle = NULL;

DWORD WINAPI RecvDataThread( PVOID pParam )
{
	CRemoteDebuggerControllerDlg *pThis = (CRemoteDebuggerControllerDlg *)pParam;
	char chRecvBuffer[40960];
	while (TRUE)
	{
		int nRecvRes = pThis->m_tcpSock.RecvData(chRecvBuffer,40940);
		if (nRecvRes <= 0)
		{
			break;
		}
		chRecvBuffer[nRecvRes] = 0;
		chRecvBuffer[nRecvRes+1] = 0;
		
 		CString strMsgOut;
 		strMsgOut = (WCHAR *)chRecvBuffer;
 
 		int nLen = strMsgOut.GetLength();
 		WCHAR *pszMsgBuf = new WCHAR[nLen+1];
 		wcscpy_s(pszMsgBuf,nLen+1,strMsgOut.GetBuffer());
 
 		pThis->PostMessage(WM_USER+2222,(WPARAM)pszMsgBuf,0);

	}


	return 0;
}

LRESULT CRemoteDebuggerControllerDlg::OnHandleRecvMsg(WPARAM wParam,LPARAM lParam)
{
	LPCWSTR pszMsgOut = (LPCWSTR)wParam;
	if (pszMsgOut)
	{
		CString strRecvData;
		strRecvData = (WCHAR *)pszMsgOut;

		OutputDebugStringW(strRecvData+L"\r\n");

		BOOL bBreak = FALSE;

		while ( FALSE == bBreak )
		{
			CString strTempRecvData;
			
			int nStart = strRecvData.Find(L"[]\r\n");
			if (nStart >= 0)
			{
				int nEnd = strRecvData.Find(L"[]\r\n",nStart+1);

				if ( nEnd < 0 )
				{
					bBreak = TRUE;
					strTempRecvData = strRecvData;
				}
				else
				{
					strTempRecvData = strRecvData.Mid(nStart,nEnd-nStart);
					strRecvData.Delete(0,nEnd);
				}
			}
			else
			{
				bBreak = TRUE;
				strTempRecvData = strRecvData;
			}
			CMemIniFile Ini;
			Ini.ParseMemoryDataW((BYTE *)strTempRecvData.GetBuffer(),strTempRecvData.GetLength()*sizeof(WCHAR));

			CString strCmd;
			strCmd = Ini.GetIniString(L"",L"cmd",L"");

			if (strCmd == "getonlineclient")
			{
				while( m_wndOnLineClient.GetCount() > 0 )
				{
					m_wndOnLineClient.DeleteString(0);
				}
				int nCount = Ini.GetIniUint( L"" , L"clientcount",0 );
				for ( int i = 0;i<nCount;i++)
				{
					CString strTempKeyName;
					strTempKeyName.Format(L"client%d",i);
					HANDLE hHandle = (HANDLE)Ini.GetIniUint(L"",strTempKeyName,0);

					CString strListItem;
					strListItem.Format(L"%d",hHandle);
					m_wndOnLineClient.AddString( strListItem );

					int a=0;
				}
			}

			if ( strCmd == L"runcmd" )
			{
				CString strCmdResult;
				strCmdResult = Ini.GetIniString(L"",L"result",L"");
				strCmdResult.Replace(L"\\r",L"\r");
				strCmdResult.Replace(L"\\n",L"\n");

				int nTextLen = m_wndMsgShow.GetWindowTextLengthW();
				m_wndMsgShow.SetSel(nTextLen,nTextLen);
				m_wndMsgShow.ReplaceSel(strCmdResult);
			}
		}


		delete pszMsgOut;
		
	}
	return 0;
}

BOOL CRemoteDebuggerControllerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	Sleep(1000);

	BOOL bRes = m_tcpSock.CreateTcpSocket();
	bRes = m_tcpSock.Connect("gz8912.jios.org",8890);

	if (bRes)
	{
		CreateThread(NULL,0,RecvDataThread,this,0,NULL);

		CMemIniFile Ini;
		Ini.WriteIniString(L"",L"cmd",L"getonlineclient");
		CString strSendData;
		strSendData = Ini.BuildData();
		m_tcpSock.SendData((BYTE *)strSendData.GetBuffer(),strSendData.GetLength()*sizeof(WCHAR));
	}

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
	
	int nSelIndex = m_wndOnLineClient.GetCurSel();
	if (nSelIndex >= 0)
	{
		CString strTargetHandle;
		m_wndOnLineClient.GetText(nSelIndex,strTargetHandle);

		if (strTargetHandle.GetLength() > 0 )
		{
			CMemIniFile Ini;
			Ini.WriteIniString(L"",L"cmd",L"runcmd");
			Ini.WriteIniString(L"",L"data",m_strCmdLine);
			Ini.WriteIniString(L"",L"target",strTargetHandle);


			CString strSendData;
			strSendData = Ini.BuildData();
			m_tcpSock.SendData((BYTE *)strSendData.GetBuffer(),strSendData.GetLength()*sizeof(WCHAR));
			int a=0;
		}

	}
}
