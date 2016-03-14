
// DJMasterDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DJMaster.h"
#include "DJMasterDlg.h"
#include "AddMusicDlg.h"
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


// CDJMasterDlg �Ի���




CDJMasterDlg::CDJMasterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDJMasterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDJMasterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDJMasterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CDJMasterDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDJMasterDlg::OnBnClickedCancel)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CDJMasterDlg ��Ϣ�������

BOOL CDJMasterDlg::OnInitDialog()
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

	m_wndMusicDisplay.Create(NULL,NULL,WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,CRect(10,10,200,500),this,0);

	m_wndMusicDisplay.AddMusic(L"",L"����");
	m_wndMusicDisplay.AddMusic(L"",L"�������ϳ�");
	m_wndMusicDisplay.AddMusic(L"",L"�α��ϳ�");
	m_wndMusicDisplay.AddMusic(L"",L"�����˻���");
	m_wndMusicDisplay.AddMusic(L"",L"�Ŷ�չʾ");
	m_wndMusicDisplay.AddMusic(L"",L"�Ŷ��³�");
	

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CDJMasterDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDJMasterDlg::OnPaint()
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
HCURSOR CDJMasterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDJMasterDlg::OnBnClickedOk()
{
	
}

void CDJMasterDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CDJMasterDlg::OnDropFiles(HDROP hDropInfo)
{
	UINT unFileCount;            
	WCHAR szFilePath[200];              
	unFileCount = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);   //ϵͳ��API����         
	for( int i=0;i < unFileCount;i++)             
	{  
		int pathLen = DragQueryFileW(hDropInfo, i, szFilePath, _countof(szFilePath));  //API���� 

		CAddMusicDlg addDlg;
		addDlg.InitDisplay(szFilePath);
		if ( IDOK == addDlg.DoModal() )
		{
			CString strMusicDesc;
			strMusicDesc = addDlg.GetMusicDescription();

			m_wndMusicDisplay.AddMusic(szFilePath,strMusicDesc);
		}
	}
	DragFinish(hDropInfo);   //API����

	CDialog::OnDropFiles(hDropInfo);
}
