#if !defined(AFX_NEWSOCKET_H__5F715000_BF7C_42F4_992A_F02905BE34D3__INCLUDED_)
#define AFX_NEWSOCKET_H__5F715000_BF7C_42F4_992A_F02905BE34D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SocketSer.h : header file
//

//网管主端口 1500
//监控仿真数据交换端口 1501
//通讯仿真数据交换端口 1502
//保护仿真数据交换端口 1503

class CNewSocket;
class CNewClientSocketThread;

extern CMutex  g_SendSOEReportListMutex;
extern CPtrList g_SendSOEReportList;
extern BOOL g_EnableStudentExitOperate;

extern CNewSocket g_NetworkManageClientSocket;  //连接网管

extern BYTE  g_Frame;

struct TagSOEReportStruct;

class CNewClientSocketThread : public CWinThread
{
	DECLARE_DYNCREATE(CNewClientSocketThread)
public:
	CNewClientSocketThread();
	virtual ~CNewClientSocketThread();

  CNewSocket * m_pClientSocket;

  BOOL SendTestLinkInfo();//发送链路测试
  BOOL SendAllData();//发送全数据
  BOOL SendSOEData(TagSOEReportStruct *pSOEReportStruct);//发送SOE

// Attributes
public:
  BOOL m_Run;       //运行标志
  BOOL m_Running;   //正在运行标志
  CTime m_Time;     //刷新时间
  void KillThread();//结束线程

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSocketThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSocketThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

class CNewSocket
{
// Attributes
public:

public:
	CNewSocket();
	CNewSocket(WORD SocketPort);
	virtual ~CNewSocket();

  SOCKET m_hSocket;
  SOCKADDR_IN m_sockAddr;
  WORD m_SocketPort;
  BOOL    m_Create;
  BOOL    m_Connect;
  WORD    m_SocketID;

  CTime   m_AcceptTime[4];
  CTime   m_AccessTime[4];
  CNewSocket *m_pClientSocket[4];

  CMutex  m_SocketMutex;

  BOOL AcceptSocket();
  BOOL Connect(CString ServerIPAddress,WORD SocketPort);

	BOOL PhysicalReceive(BYTE *pReceiveBuf,WORD &ReceiveLen);
	BOOL PhysicalSend(BYTE *pSendBuf,WORD &SendLen);

  void CreateClientSocket();
  void CloseClientSocket();

  CNewClientSocketThread* m_pClientSocketThread;

  BOOL SendRemoteOnOff(WORD UnitID,WORD VarID,WORD Value);//发送遥控分合闸
  BOOL SendRemoteRevert(WORD UnitID);//发送遥控复归
  BOOL CallParameter(WORD UnitID);//召唤定值
  BOOL SendParameter(WORD UnitID);//发送定值

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSocketSer)
	public:
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CNewSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWSOCKET_H__5F715000_BF7C_42F4_992A_F02905BE34D3__INCLUDED_)
