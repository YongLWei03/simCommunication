// NewComputer.h: interface for the CNewComputer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEWCOMPUTER_H__E8CD0860_5D59_11D3_9D51_D85FC20A0395__INCLUDED_)
#define AFX_NEWCOMPUTER_H__E8CD0860_5D59_11D3_9D51_D85FC20A0395__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNewComputer;
class CNewSocket;

#define COMPUTER_COUNT 256

extern CNewComputer g_ComputerList[COMPUTER_COUNT];

void WINAPI LoadComputerFile();

class CNewComputer
{
// Construction
public:
  CNewComputer();	// standard constructor

	~CNewComputer();	// standard constructor
  
  CString m_ComputerName;
  CString m_ComputerIPAddress;
  BOOL    m_ComputerEnable;

  CTime   m_AccessTime[4];
  WORD m_ComputerState[4];  //运行状态 0:在线 1:离线 
  CNewSocket *m_pComputerSocket[4];

  WORD m_EnableRemoteState;     //监控单元遥控状态 0:禁止 1:允许
  WORD m_SimMode;               //仿真模式 0:自习模式 1:教学模式 
};

#endif // !defined(AFX_NEWCOMPUTER_H__E8CD0860_5D59_11D3_9D51_D85FC20A0395__INCLUDED_)
