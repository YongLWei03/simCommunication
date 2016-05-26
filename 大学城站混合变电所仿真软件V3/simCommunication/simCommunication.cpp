// simCommunication.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "simCommunication.h"
#include "MainFrm.h"
#include "simCommunicationSOEListView.h"
#include "simCommunicationVarListView.h"
#include "simCommunicationUnitListView.h"
#include "DlgMessageBox.h"
#include "DlgPowerMonitor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMainFrame  *g_pMainFrame = NULL;

void WINAPI ShowPaneTextMessage(CString msg)
{
  g_pMainFrame->SendMessage(WM_COMMAND,(WPARAM) PM_SET_PANE_TEXT,(LPARAM) &msg);
}

/////////////////////////////////////////////////////////////////////////////
// CSimCommunicationUnitApp

BEGIN_MESSAGE_MAP(CSimCommunicationUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CSimCommunicationUnitApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimCommunicationUnitApp construction

CSimCommunicationUnitApp::CSimCommunicationUnitApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSimCommunicationUnitApp object

CSimCommunicationUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSimCommunicationUnitApp initialization

BOOL CSimCommunicationUnitApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

  CString FileName = "C:\\simPower2000\\simPower.exit";
  CFile File;
	CFileStatus FileStatus;
  if(File.GetStatus(FileName,FileStatus)==TRUE)
    File.Remove(FileName);

//---------------------------判断进程------------------------------//
  CString AppName = ::AfxGetAppName();
  int ProcessCount=0;
  HANDLE hProcessSnap = NULL;
	PROCESSENTRY32 pe32 = {0};
	//获得句柄
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hProcessSnap != (HANDLE)-1)
	{
    pe32.dwSize = sizeof(PROCESSENTRY32);

	  //列举所有进程名称
	  if (Process32First(hProcessSnap, &pe32))
	  {
		  do
		  {
			  CString ProcessName = pe32.szExeFile;
        if(AppName.Find(ProcessName)==0)
          ProcessCount ++;
        else if(ProcessName.Find(AppName)==0)
          ProcessCount ++;
      }
		  while (Process32Next(hProcessSnap, &pe32));//直到列举完毕
	  }
  }
  if(ProcessCount>1)
    return FALSE;
//---------------------------判断进程------------------------------//

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

  g_ProjectFilePath = m_pszHelpFilePath;
  g_ProjectFilePath.Replace(m_pszExeName,"");
  g_ProjectFilePath.Replace(".HLP","");

  ::CreateShareMemory();
  ::LoadComputerFile();
  ::LoadUnitList();
  ::LoadPowerPath();

  g_pPowerThread = (CPowerThread*)AfxBeginThread(RUNTIME_CLASS(CPowerThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
  g_pPowerThread->ResumeThread();

  g_pDlgPowerMonitor = new CDlgPowerMonitor;
  g_pDlgPowerMonitor->Create(IDD_DLG_POWER_MONITOR);

  g_pDlgMessageBox = new CDlgMessageBox;
  g_pDlgMessageBox->Create(IDD_DLG_MESSAGE_BOX);

	g_pMainFrame = new CMainFrame;
	m_pMainWnd = g_pMainFrame;

	// create and load the frame with its resources

	g_pMainFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);


	// The one and only window has been initialized, so show and update it.
  g_pMainFrame->m_pUnitListView->MakeListView();
	g_pMainFrame->UpdateWindow();

  char buf[1024];
  ::GetPrivateProfileString("setting","通讯仿真窗口模式","0",buf,1024,g_SystemFilePath + "simPower.ini");
	g_pMainFrame->ShowWindow(atoi(buf));

  g_ProtocolTH104[0].m_OperateEnable = TRUE;
  g_ProtocolTH104[255].m_OperateEnable = TRUE;

  g_DataExchangePowerStateSocket.m_LocalSocketPort = SOCKET_PORT_SEND_POWER_STATE;
  g_DataExchangePowerStateSocket.m_OtherSocketPort = SOCKET_PORT_RECEIVE_POWER_STATE;
  g_DataExchangePowerStateSocket.CreateSocket();
  g_DataExchangeTrainStateSocket.m_LocalSocketPort = SOCKET_PORT_RECEIVE_TRAIN_STATE;
  g_DataExchangeTrainStateSocket.m_OtherSocketPort = SOCKET_PORT_RECEIVE_TRAIN_STATE;
  g_DataExchangeTrainStateSocket.CreateSocket();
  g_DataExchangePowerStateSocketOther.m_LocalSocketPort = SOCKET_PORT_RECEIVE_POWER_STATE;
  g_DataExchangePowerStateSocketOther.m_OtherSocketPort = SOCKET_PORT_RECEIVE_POWER_STATE;
  g_DataExchangePowerStateSocketOther.CreateSocket();

  g_NetworkManageClientSocket.CreateClientSocket();

  g_NewSocketTH104.m_LocalIPAddress = g_ComputerList[0].m_ComputerIPAddress;
  g_NewSocketTH104.CreateSocket();
  if(g_NewSocketTH104.m_Open==FALSE)
  {
    ::AfxMessageBox("TCP.TH104Server 建立失败");
	  return FALSE;
  }
  g_pServerSocketTH104Thread = (CNewServerSocketTH104Thread*)AfxBeginThread(RUNTIME_CLASS(CNewServerSocketTH104Thread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
  g_pServerSocketTH104Thread->ResumeThread();

  return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CSimCommunicationUnitApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CSimCommunicationUnitApp message handlers

