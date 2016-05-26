// CommThread.cpp : implementation file
//
 
#include "stdafx.h"
#include "simCommunication.h"
#include "CommThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCommThread

IMPLEMENT_DYNCREATE(CCommThread, CWinThread)

CCommThread::CCommThread()
{
  m_Run = TRUE;       //运行标志
  m_Running = TRUE;   //正在运行标志
  m_Pause = TRUE;     //暂停标志
  m_Pausing = TRUE;   //正在暂停标志

  m_ComputerID = 0;
}

CCommThread::~CCommThread()
{
}

BOOL CCommThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CCommThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CCommThread, CWinThread)
	//{{AFX_MSG_MAP(CCommThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCommThread message handlers

int CCommThread::Run() 
{
  Sleep(1000);
  while(m_Run)
  {
    Sleep(100);
    
    g_ProtocolTH104[m_ComputerID].UpdateComm();
    g_ProtocolTH104[m_ComputerID].m_pCommThread = this;
  }
	m_Running = FALSE;
  return CWinThread::Run();
}

void CCommThread::KillThread()//结束线程
{
	m_Run = FALSE;
  while(m_Running==TRUE)
    Sleep(100);

  TerminateThread(m_hThread,0);
}
