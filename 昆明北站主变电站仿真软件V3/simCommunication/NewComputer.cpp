// NewComputer.cpp: implementation of the CNewComputer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NewComputer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNewComputer g_ComputerList[COMPUTER_COUNT];

CNewComputer::CNewComputer()
{
  m_ComputerName = "";
  m_ComputerIPAddress = "";
  m_ComputerEnable = FALSE;

  for(int no=0;no<4;no++)
  {
    m_AccessTime[no] = NULL;
    m_ComputerState[no] = FALSE;
    m_pComputerSocket[no] = NULL;
  }

  m_EnableRemoteState = 0;     //��ص�Ԫң��״̬ 0:��ֹ 1:����
  m_SimMode = 1;               //����ģʽ 0:��ϰģʽ 1:��ѧģʽ 
}

CNewComputer::~CNewComputer(void)
{
}
  
void WINAPI LoadComputerFile()
{
  CString filename = g_SystemFilePath + "simPower.ini";
  for(int no=0;no<COMPUTER_COUNT;no++)
  {
    CString key;
    key.Format("%03d",no);
    char buf[1024];
    ::GetPrivateProfileString("��������",key,"",buf,1024,filename);
    g_ComputerList[no].m_ComputerName = buf;
    ::GetPrivateProfileString("����IP",key,"",buf,1024,filename);
    g_ComputerList[no].m_ComputerIPAddress = buf;
  
    if((g_ComputerList[no].m_ComputerName!="")&&(g_ComputerList[no].m_ComputerIPAddress!=""))
      g_ComputerList[no].m_ComputerEnable = TRUE;
  }
}
