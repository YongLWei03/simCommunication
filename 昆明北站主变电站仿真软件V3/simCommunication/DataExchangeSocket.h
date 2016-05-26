#if !defined(AFX_DATAEXCHANGESOCKET_H__5F715000_BF7C_42F4_992A_F02905BE34D3__INCLUDED_)
#define AFX_DATAEXCHANGESOCKET_H__5F715000_BF7C_42F4_992A_F02905BE34D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SocketSer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDataExchangeSocket command target

#define SOCKET_PORT_RECEIVE_POWER_STATE   6101
#define SOCKET_PORT_SEND_POWER_STATE      6102

#define SOCKET_PORT_RECEIVE_TRAIN_STATE   6201
#define SOCKET_PORT_SEND_TRAIN_STATE      6202

#define SOCKET_PORT_RECEIVE_VOLTAGE_VALUE 6301
#define SOCKET_PORT_SEND_VOLTAGE_VALUE    6302

#define SOCKET_PORT_RECEIVE_CURRENT_VALUE 6401
#define SOCKET_PORT_SEND_CURRENT_VALUE    6402

class CDataExchangeSocket;
extern CDataExchangeSocket g_DataExchangePowerStateSocket;
extern CDataExchangeSocket g_DataExchangeTrainStateSocket;

struct TagDataExchangeStruct;
extern TagDataExchangeStruct g_DataExchangeList[256];//数据交换列表

extern WORD    g_EnableReceivePowerState;
extern WORD    g_EnableSendPowerState;
extern WORD    g_EnableReceiveTrainState;
extern WORD    g_EnableSendTrainState;

class CDataExchangeSocket : public CAsyncSocket
{
// Attributes
public:

public:
	CDataExchangeSocket();
	virtual ~CDataExchangeSocket();

  BOOL   m_Open;
  WORD   m_FrameReceiveCount;
  WORD   m_FrameSendCount;
  CString m_LocalIPAddress;
  UINT    m_LocalSocketPort;
  CString m_OtherIPAddress;
  UINT    m_OtherSocketPort;

  BYTE m_DataBuffer[1024];	
  int  m_DataLen;	

  int PhysicalSend(BYTE * pSendBuf, int nSendLen );
	BOOL PhysicalReceive(BYTE *pReceive, int & nLen );

  BOOL CreateSocket();
  BOOL CloseSocket();

  void ProcessReceivePowerState();
  void ProcessSendPowerState();
  void ProcessReceiveTrainState();
  void ProcessSendTrainState();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSocketSer)
	public:
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CDataExchangeSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
};

/////////////////////////////////////////////////////////////////////////////


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATAEXCHANGESOCKET_H__5F715000_BF7C_42F4_992A_F02905BE34D3__INCLUDED_)
