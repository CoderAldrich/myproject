
// CDNTesterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CDNTester.h"
#include "CDNTesterDlg.h"

#include <WinInet.h>
#pragma comment(lib,"wininet.lib")

#include "TcpSocket.h"
#include "HttpRecvParser.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
}

void CCDNTesterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strTestUrl);
	DDX_Control(pDX, IDC_EDIT2, m_wndReqAppendHeaders);
	DDX_Control(pDX, IDC_EDIT3, m_wndTestIps);
}

BEGIN_MESSAGE_MAP(CCDNTesterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CCDNTesterDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CCDNTesterDlg::OnBnClickedButton1)
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

	m_strTestUrl=L"http://ini.58qz.com/aaa/abcdefg.g.0.ini";
	UpdateData(FALSE);

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

#include <WinCrypt.h>

BOOL GetFileMd5(LPCWSTR FileDirectory,char *pchFileMd5,int nBufLen)
{
	BOOL bRes = FALSE;
	HANDLE hFile = NULL;
	BYTE *pbHash = NULL;
	HCRYPTPROV hProv=NULL;
	HCRYPTPROV hHash=NULL;

	HANDLE hFileMap = NULL;
	LPVOID pFileMapBuf = NULL;
	do
	{
		if( NULL == pchFileMd5 || nBufLen <= 32 )
		{
			break;
		}

		pchFileMd5[0] = 0;

		hFile = CreateFile(FileDirectory,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
		if ( hFile==INVALID_HANDLE_VALUE || hFile == NULL )                                        //如果CreateFile调用失败
		{
			break;
		}

		if(CryptAcquireContext(&hProv,NULL,NULL,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT)==FALSE)       //获得CSP中一个密钥容器的句柄
		{
			break;
		}

		if(CryptCreateHash(hProv,CALG_MD5,0,0,&hHash)==FALSE)     //初始化对数据流的hash，创建并返回一个与CSP的hash对象相关的句柄。这个句柄接下来将被CryptHashData调用。
		{
			break;
		}

		DWORD dwFileSizeHigh = 0;
		DWORD dwFileSizeLow=GetFileSize(hFile,&dwFileSizeHigh);    //获取文件的大小
		if (dwFileSizeLow==0xFFFFFFFF)               //如果获取文件大小失败
		{
			break;
		}

		hFileMap = CreateFileMappingW(hFile,NULL,PAGE_READONLY,dwFileSizeHigh,dwFileSizeLow,NULL);
		if( NULL == hFileMap ||  INVALID_HANDLE_VALUE == hFileMap)
		{
			break;
		}

		pFileMapBuf = MapViewOfFile(hFileMap,FILE_MAP_READ,0,0,dwFileSizeLow);
		if(NULL == pFileMapBuf)
		{
			break;
		}

		if(CryptHashData(hHash,(const BYTE *)pFileMapBuf,dwFileSizeLow,0)==FALSE)      //hash文件
		{
			break;
		}

		DWORD dwHashLen=sizeof(DWORD);

		if (CryptGetHashParam(hHash,HP_HASHVAL,NULL,&dwHashLen,0))      //我也不知道为什么要先这样调用CryptGetHashParam，这块是参照的msdn       
		{
		}
		else
		{
			break;
		}

		pbHash=new BYTE[dwHashLen];

		if( NULL == pbHash )
		{
			break;
		}
		if(CryptGetHashParam(hHash,HP_HASHVAL,pbHash,&dwHashLen,0))            //获得md5值
		{
			if( dwHashLen >= (nBufLen+1)/2 )
			{
				break;
			}
			for(DWORD i=0;i<dwHashLen;i++)         //输出md5值
			{
				sprintf_s(pchFileMd5+i*2,nBufLen-i*2,"%02x",pbHash[i]);
			}
		}
		else
		{
			break;
		}

		bRes = TRUE;
	}
	while(FALSE);

	if (pbHash)
	{
		delete pbHash;
	}



	if(hHash)          //销毁hash对象
	{
		CryptDestroyHash(hHash);
	}

	if( hProv )
	{
		CryptReleaseContext(hProv,0);
	}

	if (pFileMapBuf)
	{
		UnmapViewOfFile(pFileMapBuf);
	}

	if( NULL != hFileMap &&  INVALID_HANDLE_VALUE != hFileMap)
	{
		CloseHandle(hFileMap);
	}

	if (hFile)
	{
		BOOL bRes = CloseHandle(hFile);
		int a=0;
	}

	return bRes;
}

//////////////////////////////////////////////////////////////////////
//获取当前模块句柄
HMODULE ModuleHandleByAddr(const void* ptrAddr)  
{  
	MEMORY_BASIC_INFORMATION info;  
	::VirtualQuery(ptrAddr, &info, sizeof(info));  
	return (HMODULE)info.AllocationBase;  
}  
/*  
功能：获取当前模块句柄
返回值：当前模块句柄
*/  
HMODULE ThisModuleHandle()  
{  
	static HMODULE sInstance = ModuleHandleByAddr((void*)&ThisModuleHandle);  
	return sInstance;  
}
//////////////////////////////////////////////////////////////////////


BOOL RequestData( LPCWSTR pszRemoteIP, USHORT nPort, LPCWSTR pszHostName, LPCWSTR pszPath ,CStringList *plstAppendHead )
{
	WSADATA wsd;
	if( WSAStartup( MAKEWORD(2,2),&wsd) != 0 )
	{

	}

	BOOL bRequestRes = FALSE;


	CString strAppendHeads;
	if (plstAppendHead)
	{
		POSITION pos = plstAppendHead->GetHeadPosition();
		while (pos)
		{
			CString strTemp;
			strTemp = plstAppendHead->GetNext(pos);
			strAppendHeads+=strTemp;
			strAppendHeads+=L"\r\n";
		}
	}

	CString strRequestData;
	strRequestData.Format(L"GET %s HTTP/1.1\r\nHost: %s\r\n%s\r\n",pszPath,pszHostName,strAppendHeads);

	CStringA straRequestData;
	straRequestData = strRequestData;

	CTcpSocket tcpSock;
	BOOL bRes = tcpSock.CreateTcpSocket();
	bRes = tcpSock.Connect( CStringA(pszRemoteIP) , nPort );

	int nSendLen = tcpSock.SendData(straRequestData.GetBuffer(),straRequestData.GetLength());

	BYTE *pRecvBuf = (BYTE *)malloc(4096);
	int   nRecvBufTotalLen = 4096;
	int   nRecvTotalLen = 0;

	int nContentLen = 0;
	int nContentStart = 0;
	int nRecvLen = 0;
	char chRecvBuf[4096];
	while ( TRUE )
	{
		if( (nRecvLen = tcpSock.RecvData(chRecvBuf,4096)) <= 0)
		{
			if ( GetLastError() == WSAETIMEDOUT )
			{
				Sleep(100);
				continue;
			}

			break;
		}

		//缓冲区不够
		if ( nRecvBufTotalLen < nRecvTotalLen+nRecvLen )
		{
			pRecvBuf = (BYTE *)realloc(pRecvBuf,nRecvTotalLen+nRecvLen);
		}

		memcpy_s(pRecvBuf+nRecvTotalLen,nRecvBufTotalLen,chRecvBuf,nRecvLen);
		nRecvTotalLen+=nRecvLen;


		CHttpRecvParser recvparser;
		if( 0 == nContentLen && recvparser.ParseData((const char *)pRecvBuf,nRecvTotalLen))
		{
			CStringA strContentLen;
			strContentLen = recvparser.GetValueByName("Content-Length");
			
			nContentStart = recvparser.GetContentStart();
			nContentLen = _ttoi(CString(strContentLen));
		}
		
		if ( nContentLen && (nRecvTotalLen - nContentStart) >=nContentLen  )
		{
			bRequestRes = TRUE;
			break;
		}

	}

	if (bRequestRes)
	{
		//获取当前路径
		WCHAR szLocalPath[MAX_PATH]={0};
		GetModuleFileNameW(ThisModuleHandle()  ,szLocalPath,MAX_PATH);
		WCHAR *pPathEnd = (WCHAR *)szLocalPath+wcslen(szLocalPath);
		while (pPathEnd != szLocalPath && *pPathEnd != L'\\') pPathEnd--;
		*(pPathEnd+1) = 0;
		wcscat_s(szLocalPath,MAX_PATH,L"DownloadFiles\\");

		CreateDirectory(szLocalPath,NULL);

		CString strFileName;
		strFileName.Format(L"%s[md5]_%s.txt",szLocalPath,pszRemoteIP);

		HANDLE hFileContentWrite = CreateFile(strFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
		if ( INVALID_HANDLE_VALUE != hFileContentWrite )
		{
			DWORD dwWriteLen = 0;
			WriteFile(hFileContentWrite,pRecvBuf+nContentStart,nRecvTotalLen-nContentStart,&dwWriteLen,NULL);
			CloseHandle(hFileContentWrite);

			CStringA strFileMd5;
			GetFileMd5(strFileName,strFileMd5.GetBuffer(50),50);
			strFileMd5.ReleaseBuffer();
			
			CString strNewFileName;
			strNewFileName = strFileName;
			strNewFileName.Replace(L"[md5]",CString(strFileMd5));
			MoveFileW(strFileName,strNewFileName);
			int a=0;
		}

		wcscat_s(szLocalPath,MAX_PATH,L"Headers\\");
		CreateDirectory(szLocalPath,NULL);

		strFileName.Format(L"%s%s_head.txt",szLocalPath,pszRemoteIP);
		HANDLE hFileHeadWrite = CreateFile(strFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
		if ( INVALID_HANDLE_VALUE != hFileContentWrite )
		{
			DWORD dwWriteLen = 0;
			WriteFile(hFileHeadWrite,pRecvBuf,nContentStart,&dwWriteLen,NULL);
			CloseHandle(hFileHeadWrite);
		}


	}

	free(pRecvBuf);
	tcpSock.CloseTcpSocket();

	return bRequestRes;
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

CCSLock     g_strLstIpsLock;
CStringList g_strLstIps;
CStringList g_strLstAppendHead;
CString     g_strHostName;
CString     g_strPath;
DWORD       g_dwPort;

VOID DebugMsgOut( LPCWSTR pszMsg )
{
	
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

			for ( int i=0;i<3;i++)
			{
				bRequestRes = RequestData(strTestIp,g_dwPort,g_strHostName,g_strPath,&g_strLstAppendHead);
				if (bRequestRes)
				{
					break;
				}
			}

			CString strMsgOut;
			strMsgOut.Format(L"RemoteIp: %s Result:%d\r\n",strTestIp,bRequestRes);
			OutputDebugStringW(strMsgOut);
		}
	}

	return 0;
}

void CCDNTesterDlg::OnBnClickedOk()
{
	UpdateData();

	CString   strHostName;
	CString   strUrlPath;
	INTERNET_PORT nPort = 80;

	URL_COMPONENTSW UrlComp;
	ZeroMemory(&UrlComp,sizeof(UrlComp));
	UrlComp.dwStructSize = sizeof(UrlComp);
	UrlComp.lpszHostName = strHostName.GetBuffer( MAX_PATH );
	UrlComp.dwHostNameLength = MAX_PATH;
	UrlComp.lpszUrlPath = strUrlPath.GetBuffer(2000);
	UrlComp.dwUrlPathLength = 2000;

	BOOL bCrackRes = InternetCrackUrlW( m_strTestUrl , m_strTestUrl.GetLength() , 0 , &UrlComp );

	nPort = UrlComp.nPort;
	strHostName.ReleaseBuffer();
	strUrlPath.ReleaseBuffer();

	g_strHostName = strHostName;
	g_strPath = strUrlPath;
	g_dwPort = nPort;

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
	// TODO: 在此添加控件通知处理程序代码
}
