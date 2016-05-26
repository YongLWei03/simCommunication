#if !defined(AFX_NEWSOCKET_H__5F715000_BF7C_42F4_992A_F02905BE34D3__INCLUDED_)
#define AFX_NEWSOCKET_H__5F715000_BF7C_42F4_992A_F02905BE34D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SocketSer.h : header file
//

//�������˿� 1500
//��ط������ݽ����˿� 1501
//ͨѶ�������ݽ����˿� 1502
//�����������ݽ����˿� 1503

class CNewSocket;
class CNewClientSocketThread;

extern CMutex  g_SendSOEReportListMutex;
extern CPtrList g_SendSOEReportList;
extern BOOL g_EnableStudentExitOperate;

extern CNewSocket g_NetworkManageClientSocket;  //��������

extern BYTE  g_Frame;

struct TagSOEReportStruct;

class CNewClientSocketThread : public CWinThread
{
	DECLARE_DYNCREATE(CNewClientSocketThread)
public:
	CNewClientSocketThread();
	virtual ~CNewClientSocketThread();

  CNewSocket * m_pClientSocket;

  BOOL SendTestLinkInfo();//������·����
  BOOL SendAllData();//����ȫ����
  BOOL SendSOEData(TagSOEReportStruct *pSOEReportStruct);//����SOE

// Attributes
public:
  BOOL m_Run;       //���б�־
  BOOL m_Running;   //�������б�־
  CTime m_Time;     //ˢ��ʱ��
  void KillThread();//�����߳�

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

  BOOL SendRemoteOnOff(WORD UnitID,WORD VarID,WORD Value);//����ң�طֺ�բ
  BOOL SendRemoteRevert(WORD UnitID);//����ң�ظ���
  BOOL CallParameter(WORD UnitID);//�ٻ���ֵ
  BOOL SendParameter(WORD UnitID);//���Ͷ�ֵ

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
