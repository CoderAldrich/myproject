
// ShuaClient.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "ShuaClient.h"
#include "MainFrm.h"

#include "VirtualMouse.h"
#include "BrowserHelpFun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CShuaClientApp

BEGIN_MESSAGE_MAP(CShuaClientApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CShuaClientApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CShuaClientApp 构造

CShuaClientApp::CShuaClientApp()
{

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CShuaClientApp 对象

CShuaClientApp theApp;


BOOL SetSlient();

BOOL CShuaClientApp::InitInstance()
{

	SetSlient();
	StartVirtualMouse();
	RegisterBrowserEmulationMode(TRUE);
	BrowserFix();

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	// 分析标准外壳命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	CMainFrame *pMainFrame = new CMainFrame;
	pMainFrame->CreateInstance(L"http://www.130100.prcjx.cn/");

	m_pMainWnd = pMainFrame;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand 之后发生
	return TRUE;
}



// 用于运行对话框的应用程序命令
void CShuaClientApp::OnAppAbout()
{
}

// CShuaClientApp 消息处理程序




int CShuaClientApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
