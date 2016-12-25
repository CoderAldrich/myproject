
// RemoteDebuggerControllerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RemoteDebuggerController.h"
#include "RemoteDebuggerControllerDlg.h"

#include "..\����\ProtocolHandler.h"
#include "..\iocp\IOCPExport.h"
#pragma comment(lib,"IOCP.lib")

#include "..\����\Base64.h"

#if defined(DEBUG) || defined(_DEBUG)
#define REMOTE_SERVER_IP "localhost"
#else
#define REMOTE_SERVER_IP "gz8912.jios.org"
#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CRemoteDebuggerControllerDlg �Ի���




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
	DDX_Control(pDX, IDC_EDIT3, m_wndCurClient);
	DDX_Control(pDX, IDC_LIST1, m_wndOnlineClient);
}

BEGIN_MESSAGE_MAP(CRemoteDebuggerControllerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CRemoteDebuggerControllerDlg::OnBnClickedOk)
	ON_MESSAGE(WM_USER+2222,OnHandleRecvMsg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CRemoteDebuggerControllerDlg::OnBnClickedButton1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CRemoteDebuggerControllerDlg::OnNMClickList1)
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
		m_wndOnlineClient.DeleteAllItems();

		int nCount = ptlHandler.GetParamValueInt( "clientcount",0 );
		for ( int i = 0;i<nCount;i++)
		{
			CStringA strTempKeyName;
			strTempKeyName.Format("client%d",i);
			HANDLE hHandle = (HANDLE)ptlHandler.GetParamValueInt(strTempKeyName,0);

			CString strListItem;
			strListItem.Format(L"%d",hHandle);
			
			strTempKeyName.Format("pcname%d",i);
			m_wndOnlineClient.InsertItem(0,CString(ptlHandler.GetParamValueString(strTempKeyName,"null")));

			strTempKeyName.Format("mac%d",i);
			m_wndOnlineClient.SetItemText(0,1,CString(ptlHandler.GetParamValueString(strTempKeyName,"null")));

			strTempKeyName.Format("wip%d",i);
			m_wndOnlineClient.SetItemText(0,2,CString(ptlHandler.GetParamValueString(strTempKeyName,"null")));

			m_wndOnlineClient.SetItemData(0,(DWORD_PTR)hHandle);

		}
	}

	if ( strCmd == "runcmd" )
	{
		CStringA strCmdResult;
		strCmdResult = ptlHandler.GetParamValueString("result","");
		
// #ifdef DEBUG
// 		OutputDebugStringA("���ƶ�\r\n");
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	m_wndMsgShow.SetLimitText(0);

	m_wndOnlineClient.InsertColumn(0,L"�������",0,70);
	m_wndOnlineClient.InsertColumn(1,L"MAC��ַ",0,100);
	m_wndOnlineClient.InsertColumn(2,L"����IP��ַ",0,100);
	m_wndOnlineClient.SetExtendedStyle(LVS_EX_FULLROWSELECT);

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
	

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CRemoteDebuggerControllerDlg::OnPaint()
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


void CRemoteDebuggerControllerDlg::OnBnClickedButton1()
{
	m_wndMsgShow.SetWindowText(L"");
}


void CRemoteDebuggerControllerDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pNMItemActivate->iItem >= 0)
	{
		m_hCurClient = (HANDLE)m_wndOnlineClient.GetItemData(pNMItemActivate->iItem);
		CString strShow;
		strShow.Format(L"���������%s MAC��ַ��%s ����IP��%s",m_wndOnlineClient.GetItemText(pNMItemActivate->iItem,0),m_wndOnlineClient.GetItemText(pNMItemActivate->iItem,1),m_wndOnlineClient.GetItemText(pNMItemActivate->iItem,2));
		m_wndCurClient.SetWindowText(strShow);
	}
	

	*pResult = 0;
}
