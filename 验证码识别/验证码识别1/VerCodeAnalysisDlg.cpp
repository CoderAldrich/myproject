
// VerCodeAnalysisDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "��֤��ʶ��1.h"
#include "VerCodeAnalysisDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CVerCodeAnalysisDlg �Ի���




CVerCodeAnalysisDlg::CVerCodeAnalysisDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVerCodeAnalysisDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVerCodeAnalysisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editCode);
}

BEGIN_MESSAGE_MAP(CVerCodeAnalysisDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CVerCodeAnalysisDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CVerCodeAnalysisDlg ��Ϣ�������

BOOL CVerCodeAnalysisDlg::OnInitDialog()
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

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CVerCodeAnalysisDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CVerCodeAnalysisDlg::OnPaint()
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
HCURSOR CVerCodeAnalysisDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


CStringA GetFileTextA(LPCWSTR pszFilePath)
{
	CStringA strFileText;
	HANDLE hFile = CreateFile(pszFilePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if ( INVALID_HANDLE_VALUE != hFile)
	{
		char chFileData[4096];
		DWORD dwReadLen = 0;

		while (ReadFile(hFile,chFileData,4090,&dwReadLen,NULL) && dwReadLen != 0 )
		{
			chFileData[dwReadLen] = 0;
			strFileText+=chFileData;
		}

		CloseHandle(hFile);
	}

	return strFileText;
}

CString GetTempFilePath()
{
	//����Ψһ��ʱ�ļ�
	WCHAR  szTempFile[MAX_PATH];
	WCHAR  szTempPath[MAX_PATH];
	GetTempPathW(MAX_PATH,szTempPath);
	GetTempFileNameW(szTempPath,L"tmp", 0,szTempFile);

	return szTempFile;
}


CStringA AnalysisVerCodePic( LPCWSTR pszVerCodePic )
{

	CStringA strCode;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si,sizeof(si));
	ZeroMemory(&pi,sizeof(pi));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	CString strResultCacheFile;
	strResultCacheFile = GetTempFilePath();

	CString strCmdLine;
	strCmdLine.Format(L" \"%s\" \"%s\" -l eng -psm 7",pszVerCodePic,strResultCacheFile);

	BOOL bRes = CreateProcess(L"C:\\Program Files (x86)\\Tesseract-OCR\\tesseract.exe",strCmdLine.GetBuffer(),NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&si,&pi);
	if (bRes)
	{
		WaitForSingleObject(pi.hProcess,5000);
		TerminateProcess(pi.hProcess,0);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		strCode = GetFileTextA(strResultCacheFile+L".txt");
		strCode.Replace(" ","");
		strCode.Replace("\r","");
		strCode.Replace("\n","");

		CString strTempCode;
		for (int i=0;i<strCode.GetLength();i++)
		{
			WCHAR wchChar = strCode.GetAt(i);
			if ( 
				(wchChar >=L'0' && wchChar <=L'9')
				|| (wchChar >=L'A' && wchChar <=L'Z')
				)
			{
				strTempCode +=wchChar;
			}
		}

		strCode = strTempCode;
	}

	DeleteFile(strResultCacheFile);
	DeleteFile(strResultCacheFile+L".txt");

	return strCode;
}

void CVerCodeAnalysisDlg::OnBnClickedOk()
{
	DeleteFile(L"C:\\tmp.png");
	URLDownloadToFileW( NULL,L"http://www.baizhongsou.com/yanzhengma.aspx",L"C:\\tmp.png",0,NULL );
	//URLDownloadToFileW( NULL,L"http://seo.jiding51.cn/yanzhengma.aspx",L"C:\\tmp.png",0,NULL );
	

	int nZoom = 4;
	CImage img;
	img.Load(L"C:\\tmp.png");
	img.Draw(GetDC()->m_hDC,0,0,img.GetWidth()*nZoom,img.GetHeight()*nZoom);
	
	//if ( hr == S_OK )
	{
		int nFixCount = 0;
		do
		{
			nFixCount = 0;

			for (int x = 0;x<img.GetWidth();x++)
			{
				for (int y = 0;y<img.GetHeight();y++)
				{
					int nOffsetHeight = 5;
					int nOffsetWidth = 10;
					if ( x <= nOffsetWidth || x >= img.GetWidth()-nOffsetWidth-1 || y <= nOffsetHeight || y >= img.GetHeight()-nOffsetHeight-1)
					{
						img.SetPixel(x,y,0x00ffffff);
						continue;
					}
					COLORREF clrRef = img.GetPixel(x,y);
					if ( 0x00ffffff != clrRef )
					{
						int nclrCount = 0;
						for (int m = -1;m<2;m++)
						{
							for (int n = -1;n<2;n++)
							{
								if ( m==0 && n == 0 )
								{

								}
								else
								{
									COLORREF clrTmpRef = img.GetPixel(x+m,y+n);
									if (clrTmpRef != 0x00ffffff)
										//if (clrTmpRef == clrRef)
									{
										nclrCount++;
									}
								}

							}
						}

						if (nclrCount <= 2)
						{
							nFixCount++;
							img.SetPixel(x,y,0x00ffffff);
						}
					}

				}
			}

			//img.Draw(GetDC()->m_hDC,0,img.GetHeight()*nZoom,img.GetWidth()*nZoom,img.GetHeight()*nZoom);

			//Sleep(500);
		}
		while( nFixCount > 0 );
	}

	
	img.Draw(GetDC()->m_hDC,0,img.GetHeight()*nZoom,img.GetWidth()*nZoom,img.GetHeight()*nZoom);

	img.Save(L"C:\\tmp11.bmp");

	CStringA strCode = AnalysisVerCodePic( L"C:\\tmp11.bmp" );
	m_editCode.SetWindowText(CString(strCode));
	int a=0;	
}
