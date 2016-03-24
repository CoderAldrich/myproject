
// SuperYueCheDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SuperYueChe.h"
#include "SuperYueCheDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSuperYueCheDlg �Ի���

CSuperYueCheDlg *g_pThis = NULL;

VOID OutputMessage(LPCWSTR pszMessage)
{
	if (g_pThis)
	{
		g_pThis->AddMessageShow(pszMessage);
	}
}

VOID OutputMessageA(LPCSTR pszMessage)
{
	OutputMessage(CString(pszMessage));
}


CSuperYueCheDlg::CSuperYueCheDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSuperYueCheDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	InitializeCriticalSection(&m_cs);
	g_pThis = this;
	m_hYueCheThread = NULL;
}

CSuperYueCheDlg::~CSuperYueCheDlg()
{
	DeleteCriticalSection(&m_cs);
}

void CSuperYueCheDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX,IDC_EDIT1,m_strID);
	DDX_Text(pDX,IDC_EDIT2,m_strPassWord);
	DDX_Control(pDX,IDC_DATETIMEPICKER1,m_wndDataTime);

	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT3, m_wndMsgEdit);
}

BEGIN_MESSAGE_MAP(CSuperYueCheDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1, &CSuperYueCheDlg::OnDtnDatetimechangeDatetimepicker1)
	ON_BN_CLICKED(IDC_BUTTON1, &CSuperYueCheDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CSuperYueCheDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CSuperYueCheDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CSuperYueCheDlg::OnBnClickedButton4)
	ON_MESSAGE(WM_USER+1234,OnYueCheThreadExit)
END_MESSAGE_MAP()


DWORD WINAPI CSuperYueCheDlg::YueCheThread(PVOID pParam)
{
	CSuperYueCheDlg *pThis = (CSuperYueCheDlg *)pParam;

	SYSTEMTIME DataTime;
	pThis->GetWorkTime(&DataTime);

	CString strValue;
	switch (DataTime.wDayOfWeek)
	{
	case 0:
		strValue = L"��";
		break;
	case 1:
		strValue = L"һ";
		break;
	case 2:
		strValue = L"��";
		break;
	case 3:
		strValue = L"��";
		break;
	case 4:
		strValue = L"��";
		break;
	case 5:
		strValue = L"��";
		break;
	case 6:
		strValue = L"��";
		break;

	}
	CString strWeek;
	strWeek.Format(L"����%s",strValue);

	CString strSelDay;
	strSelDay.Format(L"%04d-%02d-%02d %s",DataTime.wYear,DataTime.wMonth,DataTime.wDay,strWeek);

	OutputMessage(L"ָ��Լ�����ڣ�"+strSelDay);

	BOOL bSelTime[12];
	pThis->GetSelTime(bSelTime);

	for (int i=0;i<12;i++)
	{
		if (bSelTime[i])
		{
			CString strSelTime = CServerClient::IndexToTimeString(i);
			OutputMessage(L"ָ��Լ��ʱ�� ��"+strSelTime);
		}
	}

	pThis->Client.DoYueChe(&DataTime,bSelTime);
	
	pThis->PostMessage(WM_USER+1234,0,0);

	return 0;
}



int DivisionString(CString strSeparate, CString strSourceString, CString * pStringArray, int nArrayCount)
{

	if(
		strSeparate.GetLength() == 0 ||
		strSourceString.GetLength() == 0||
		pStringArray == NULL || 
		nArrayCount <=1 
		)
	{
		return 0;
	}

	int nCount = 0;
	while(true)
	{
		int nEnd = strSourceString.Find(strSeparate,0);
		if ( nEnd >= 0 )
		{
			pStringArray[nCount] = strSourceString.Left(nEnd);
			nCount++;

			strSourceString = strSourceString.Right(strSourceString.GetLength() - nEnd-strSeparate.GetLength());
		}
		else
		{
			pStringArray[nCount] = strSourceString;
			nCount++;
			strSourceString = L"";

			break;
		}

		if ( nCount >= nArrayCount)
		{
			break;
		}
	}

	return nCount;
}

// CSuperYueCheDlg ��Ϣ�������

BOOL CSuperYueCheDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	LoadConfig();

	Client.LoadLoginSession();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSuperYueCheDlg::OnPaint()
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
HCURSOR CSuperYueCheDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSuperYueCheDlg::OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);

	OnWeekChange();

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}

void CSuperYueCheDlg::OnWeekChange(void)
{
	SYSTEMTIME SystemTime;
	m_wndDataTime.GetTime(&SystemTime);

	CString strValue;
	switch (SystemTime.wDayOfWeek)
	{
	case 0:
		strValue = L"��";
		break;
	case 1:
		strValue = L"һ";
		break;
	case 2:
		strValue = L"��";
		break;
	case 3:
		strValue = L"��";
		break;
	case 4:
		strValue = L"��";
		break;
	case 5:
		strValue = L"��";
		break;
	case 6:
		strValue = L"��";
		break;

	}
	CString strWeek;
	strWeek.Format(L"����%s",strValue);


	CWnd *pWnd = GetDlgItem(IDC_STATIC_WEEK);
	pWnd->SetWindowText(strWeek);

}


void CSuperYueCheDlg::OnBnClickedButton1()
{
	SaveConfig();

	CString strErrorText;
	
	BOOL bLoginRes = Client.DoLoginServer(m_strID,m_strPassWord,strErrorText);
	if (bLoginRes)
	{
		Client.SaveLoginSession();
		GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	}


	

}
VOID CSuperYueCheDlg::GetWorkTime(LPSYSTEMTIME pTime)
{
	m_wndDataTime.GetTime(pTime);
}

VOID CSuperYueCheDlg::GetSelTime(BOOL * pTime)
{
	int nIndex = 0;
	for (UINT i = IDC_CHECK1;i<=IDC_CHECK12;i++)
	{
		CButton *pButton = (CButton *)GetDlgItem(i);
		if (pButton)
		{
			pTime[nIndex] = pButton->GetCheck();
		}
		nIndex++;
	}
	return VOID();
}
VOID CSuperYueCheDlg::LoadConfig()
{
	CString strConfigPath;
	GetModuleFileNameW(NULL,strConfigPath.GetBuffer(MAX_PATH),MAX_PATH);
	strConfigPath.ReleaseBuffer();
	strConfigPath+=L".cfg";

	GetPrivateProfileStringW(L"Login",L"ID",L"",m_strID.GetBuffer(MAX_PATH),MAX_PATH,strConfigPath);
	GetPrivateProfileStringW(L"Login",L"PWD",L"",m_strPassWord.GetBuffer(MAX_PATH),MAX_PATH,strConfigPath);
	m_strID.ReleaseBuffer();
	m_strPassWord.ReleaseBuffer();

	UINT nRemberPassWord = GetPrivateProfileIntW(L"Config",L"RemeberPassWord",1,strConfigPath);
	CButton *pButton = (CButton *)GetDlgItem(IDC_CHECK_REMBER_PWD);
	if (pButton)
	{
		pButton->SetCheck(nRemberPassWord);
	}

	UpdateData(FALSE);

	SYSTEMTIME SystemTime;
	m_wndDataTime.GetTime(&SystemTime);
	SystemTime.wDay += 4;
	m_wndDataTime.SetTime(&SystemTime);


	CString strSelTime;
	GetPrivateProfileStringW(L"Config",L"SelTime",L"",strSelTime.GetBuffer(MAX_PATH),MAX_PATH,strConfigPath);
	strSelTime.ReleaseBuffer();

	CString strSelTimeArray[20];
	DivisionString(L"|",strSelTime,strSelTimeArray,20);

	int nIndex = 0;
	for (UINT i = IDC_CHECK1;i<=IDC_CHECK12;i++)
	{
		CButton *pButton = (CButton *)GetDlgItem(i);
		if (pButton)
		{
			pButton->SetCheck(strSelTimeArray[nIndex] == L"1");
		}
		nIndex++;
	}

	OnWeekChange();
}
VOID CSuperYueCheDlg::SaveConfig()
{
	UpdateData();

	BOOL bCheck = TRUE;
	CButton *pButton = (CButton *)GetDlgItem(IDC_CHECK_REMBER_PWD);
	if (pButton)
	{
		bCheck = pButton->GetCheck();
	}

	CString strConfigPath;
	GetModuleFileNameW(NULL,strConfigPath.GetBuffer(MAX_PATH),MAX_PATH);
	strConfigPath.ReleaseBuffer();
	strConfigPath+=L".cfg";

	if(bCheck)
	{
		WritePrivateProfileStringW(L"Login",L"ID",m_strID,strConfigPath);
		WritePrivateProfileStringW(L"Login",L"PWD",m_strPassWord,strConfigPath);
	}

	WritePrivateProfileStringW(L"Config",L"RemberPassWord",bCheck?L"1":L"0",strConfigPath);


	CString strSelTime;
	//int nIndex = 0;
	for (UINT i = IDC_CHECK1;i<=IDC_CHECK12;i++)
	{
		BOOL bCheck = FALSE;

		CButton *pButton = (CButton *)GetDlgItem(i);
		if (pButton)
		{
			bCheck = pButton->GetCheck();
		}

		strSelTime += bCheck?L"1":L"0";
		strSelTime += L"|";

	}
	WritePrivateProfileStringW(L"Config",L"SelTime",strSelTime,strConfigPath);
}

void CSuperYueCheDlg::OnBnClickedButton2()
{
	if ( NULL == m_hYueCheThread )
	{
		m_hYueCheThread = CreateThread(NULL,0,YueCheThread,this,0,NULL);

		GetDlgItem(IDC_BUTTON2)->SetWindowText(L"ֹͣԼ��");
	}
	else
	{
		Client.bQueryDataThreadWait = TRUE;
		TerminateThread(m_hYueCheThread,0);
		m_hYueCheThread = NULL;

		GetDlgItem(IDC_BUTTON2)->SetWindowText(L"��ʼԼ��");
	}
	
}

void CSuperYueCheDlg::OnBnClickedButton3()
{
	EnterCriticalSection(&m_cs);
	m_wndMsgEdit.SetWindowText(L"");
	LeaveCriticalSection(&m_cs);
}

void CSuperYueCheDlg::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CSuperYueCheDlg::AddMessageShow(CString strMessage)
{
	EnterCriticalSection(&m_cs);

	CString strText;
	m_wndMsgEdit.GetWindowText(strText);

	m_wndMsgEdit.SetSel(strText.GetLength(),strText.GetLength());

	SYSTEMTIME Time;
	GetLocalTime(&Time);
	
	CString strTimeTag;
	strTimeTag.Format(L"[%02d:%02d:%02d.%04d]",Time.wHour,Time.wMinute,Time.wSecond,Time.wMilliseconds);

	m_wndMsgEdit.ReplaceSel(strTimeTag+strMessage+L"\r\n");

	LeaveCriticalSection(&m_cs);
}

LRESULT CSuperYueCheDlg::OnYueCheThreadExit(WPARAM wParam,LPARAM lParam)
{
	m_hYueCheThread = NULL;
	GetDlgItem(IDC_BUTTON2)->SetWindowText(L"��ʼԼ��");

	OutputMessage(L"Լ�����");
	return 0;
}