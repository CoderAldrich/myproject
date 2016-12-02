
// VerCodeAnalysisDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "��֤��ʶ��1.h"
#include "VerCodeAnalysisDlg.h"

#include <map>
using namespace std;
typedef map<COLORREF,int> MAP_COLORS;
typedef MAP_COLORS::iterator MAP_COLORS_PTR;

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
	strCmdLine.Format(L" \"%s\" \"%s\" ",pszVerCodePic,strResultCacheFile);

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

VOID Test( HDC hDC,CImage *pimg,COLORREF clrRef,int x,int y)
{
	pimg->SetPixel(x,y,0x00ffffff);

 	int nZoom = 4;
 	pimg->Draw(hDC,0,pimg->GetHeight()*nZoom,pimg->GetWidth()*nZoom,pimg->GetHeight()*nZoom);
 	
 	Sleep(10);

	for (int xx=-1;xx<2;xx++)
	{
		for (int yy=-1;yy<2;yy++)
		{
			if ( !( xx == 0 && yy == 0 ))
			{
				if (x+xx < pimg->GetWidth() && x+xx >= 0 && y+yy < pimg->GetHeight() && y+yy>=0)
				{
					COLORREF clrTemp = pimg->GetPixel(x+xx,y+yy);
					if ( /*clrTemp != 0x00ffffff &&*/ clrTemp == clrRef )
					{
						Test( hDC,pimg,clrRef,x+xx,y+yy);
					}
				}
			}
		}
	}
}




void CVerCodeAnalysisDlg::OnBnClickedOk()
{
	DeleteFile(L"C:\\tmp.png");
	URLDownloadToFileW( NULL,L"http://www.baizhongsou.com/yanzhengma.aspx",L"C:\\tmp.png",0,NULL );
	//URLDownloadToFileW( NULL,L"http://seo.jiding51.cn/yanzhengma.aspx",L"C:\\tmp.png",0,NULL );
	
	int nTopPadding = 5;
	int nBottomPadding = 9;
	int nLeftPadding = 10;
	int nRightPadding = 10;

	int nZoom = 4;
	CImage img;
	img.Load(L"C:\\tmp.png");
	img.Draw(GetDC()->m_hDC,0,0,img.GetWidth()*nZoom,img.GetHeight()*nZoom);
	

	const int xxx[] = {10,30,50,70,90};
	for (int i = 0;i<=4;i++)
	{
		int x=xxx[i];
		for (int y=0;y<img.GetHeight();y++ )
		{
			COLORREF clrRef = img.GetPixel(x,y);
			if ( 0x00ffffff != clrRef )
			{
				Test( GetDC()->m_hDC, &img,clrRef ,x,y );
			}
		}
	}


	for (int x = 0;x<img.GetWidth();x++)
	{
		for (int y = 0;y<img.GetHeight();y++)
		{
			if ( x <= nLeftPadding || x >= img.GetWidth()-nRightPadding-1 || y <= nTopPadding || y >= img.GetHeight()-nBottomPadding-1)
			{
				img.SetPixel(x,y,0x00ffffff);
			}
		}
	}

	
	int nBound = 50;
	for (int i = 0;false && i<4;i++)
	{
		MAP_COLORS tmpColors;

		for(int x=xxx[i];x<xxx[i+1];x++)
		{
			for (int y=nTopPadding+1;y<img.GetHeight()-nBottomPadding-1;y++ )
			{
				COLORREF clrRef = img.GetPixel(x,y);
				int nRed = GetRValue(clrRef);
				int nGreen = GetGValue(clrRef);
				int nBlue = GetBValue(clrRef);
				if ( !(nRed >= nBound && nGreen >=nBound && nBlue >= nBound) )
				{
					MAP_COLORS_PTR it = tmpColors.find(clrRef);
					if ( it == tmpColors.end() )
					{
						tmpColors.insert(make_pair(clrRef,1));
					}
					else
					{
						it->second++;
					}
				}
			}
		}

		COLORREF maxColor = 0;
		int nMaxCount = 0;
		for ( MAP_COLORS_PTR it = tmpColors.begin();it!=tmpColors.end();it++ )
		{
			if (it->second > nMaxCount)
			{
				nMaxCount = it->second;
				maxColor = it->first;
			}
		}

		for(int x=xxx[i];x<xxx[i+1];x++)
		{
			for (int y=nTopPadding+1;y<img.GetHeight()-nBottomPadding-1;y++ )
			{
				COLORREF clrRef = img.GetPixel(x,y);

				int nRed = GetRValue(clrRef);
				int nGreen = GetGValue(clrRef);
				int nBlue = GetBValue(clrRef);

				if ( !(nRed >= nBound && nGreen >=nBound && nBlue >= nBound) && 0x00ffffff != clrRef && maxColor !=clrRef )
				{
					img.SetPixel(x,y,0x00ffffff);

					img.Draw(GetDC()->m_hDC,0,img.GetHeight()*nZoom,img.GetWidth()*nZoom,img.GetHeight()*nZoom);

					Sleep(50);
				}
			}
		}

	}


	img.Draw(GetDC()->m_hDC,0,img.GetHeight()*nZoom,img.GetWidth()*nZoom,img.GetHeight()*nZoom);

	img.Save(L"C:\\tmp11.bmp");

	CStringA strCode = AnalysisVerCodePic( L"C:\\tmp11.bmp" );
	m_editCode.SetWindowText(CString(strCode));
	int a=0;	
}
