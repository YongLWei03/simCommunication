#if !defined(AFX_PROTOCOLTH104_H__57477DE4_233C_11D4_BA64_0080C8D77339__INCLUDED_)
#define AFX_PROTOCOLTH104_H__57477DE4_233C_11D4_BA64_0080C8D77339__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProtocolTH104.h : header file
//

//---------------------变量类型-----------------------//
#define  VT_FLOAT         0  //遥测
#define  VT_BIT           1  //遥信
#define  VT_LONG          2  //电度
#define  VT_MEMVAR        3  //内存变量
#define  VT_MEMFLOAT      3  //内存模拟量
#define  VT_MEMBIT        4  //内存开关量
#define  VT_PARAEMTER     5  //参数设定
#define  VT_REMOTECONCTRL 6  //遥控
#define  VT_REMOTEUPDOWN  7  //升降
#define  VT_SOE           8  //SOE
#define  VT_GROUP         10 //变量组

#define  VT_REMOTESELECT  21 //遥控选择
#define  VT_REMOTEPERFORM 22 //遥控执行
#define  VT_REMOTEREPEAL  23 //遥控撤销
#define  VT_REMOTEREVERT  24 //遥控复归

#define  VT_STANDARTVAR   30  //标准变量

#define  VT_EXTBIT        30  //扩展遥信
#define  VT_EXTSOE        40  //扩展SOE
#define  VT_EXTFLOAT      50  //扩展遥测
#define  VT_EXTLONG       60  //扩展电度

//----------------------------------------------------//

class CNewSocketTH104;
class CNewServerSocketTH104Thread;
class CProtocolTH104;
class CCommThread;

extern CNewSocketTH104 g_NewSocketTH104;
extern CNewServerSocketTH104Thread *g_pServerSocketTH104Thread;
extern CProtocolTH104  g_ProtocolTH104[COMPUTER_COUNT];

extern g_DistributionOperateMode;

/////////////////////////////////////////////////////////////////////////////
// CProtocolTH104 thread

struct TagSOEReportStruct;
class CProtocolTH104
{
public:
	CProtocolTH104();
	virtual ~CProtocolTH104();
  
  WORD    m_ComputerID;
  WORD    m_OperateEnable;
  class CNewSocketTH104 *m_pSocketTH104;
  CCommThread *m_pCommThread;

  CMutex    m_SendVarFrameMutex;
  CPtrList  m_SendSOEReportList[UNITCOUNT];

  //通讯缓冲
	int       m_TotalLen;
	int       m_ReceiveLen;
	BYTE      m_TotalBuf[4096];
	BYTE      m_ReceiveBuf[4096];

  BOOL      m_StartSendDataFrame;

  CTime     m_SendAllFloatDataTime;
  CTime     m_SendAllBitDataTime;
  CTime     m_SendAllLongDataTime;

  int       m_InterCountVk;//最大发送编号
  int       m_InterCountVw;//最大确认编号

  int       m_InterCountConfirmVR;//内部接受确认计数器V(R)
  int       m_InterCountConfirmVS;//内部发送确认计数器V(R)

  int       m_InterCountVR;//内部接受计数器V(R)
  int       m_InterCountVS;//内部发送计数器V(S)

  WORD      m_YCAddress;
  WORD      m_YXAddress;
  WORD      m_DDAddress;
  WORD      m_SOEAddress;
  WORD      m_YKAddress;
  
  float     m_ChangeFloatDValue;
  float     m_ChangeFloatHValue;
  float     m_ChangeFloatLValue;

  int       m_FloatSendTypeID;
  int       m_BitSendTypeID;
  int       m_SOESendTypeID;

  BOOL SendAllFloatData(WORD UnitID);
  BOOL SendAllBitData(WORD UnitID);
  BOOL SendAllLongData(WORD UnitID);
  BOOL SendChangeFloatData(WORD UnitID);
  BOOL SendChangeBitData(WORD UnitID);
  BOOL SendChangeSOEData(WORD UnitID);
  
  BOOL SendParameterData(WORD UnitID,WORD GroupID);
  BOOL SendParameterGroupID(WORD UnitID);

  BOOL SendActionValue(WORD UnitID,TagSOEReportStruct *pSOE);

  BOOL SendRecordWaveInfo(WORD UnitID,WORD GroupID);
  BOOL SendRecordWaveData(WORD UnitID,WORD GroupID);

  void ProcessCallAllDataBegin(BYTE CallType);  //召唤全数据
  void ProcessCallAllDataStop(BYTE CallType);   //召唤数据停止
  void ProcessCallLongDataBegin(BYTE CallType); //召唤电度数据
  void ProcessCallLongDataStop(BYTE CallType);  //召唤电度数据停止

  //----------------辅助成员函数-----------------//
  CString GetTextHexFromBuffer(BYTE * Buf,int nLen);
  BOOL PhysicalReceive(BYTE *pReceive, int & nLen );
  BOOL PhysicalSend(BYTE * buf, int nLen );
  //----------------辅助成员函数-----------------//

  //----------------入口成员函数-----------------//
  BOOL UpdateComm();                                //通讯刷新
  BOOL RemoteSelect(TagVarStruct *pVar);					//遥控选择
  BOOL RemotePerform(TagVarStruct *pVar);					//遥控执行
  BOOL RemoteRepeal(TagVarStruct *pVar);					//遥控撤销
  BOOL SendVarFrame(WORD UnitID,TagVarStruct *pVar);					//变量发送
  //----------------入口成员函数-----------------//

  //----------------专用成员函数-----------------//
	BOOL SendTestFrame();								              //发送测试启动帧
	BOOL ProcessDataFrame(BYTE * DataBuf);						//处理子站响应的数据

};

class CNewSocketTH104
{
// Attributes
public:

public:
	CNewSocketTH104();
	virtual ~CNewSocketTH104();

  SOCKET m_hSocket;
  SOCKADDR_IN m_sockAddr;

  BOOL   m_Open;
  CString m_LocalIPAddress;
  UINT    m_SocketPort;

  CString m_ReceiveText;
  CString m_SendText;

  int m_ReceiveFrameCount;
  int m_SendFrameCount;

  CMutex    m_SocketMutex;
  CPtrList  m_SocketMessageReceiveList;

  BOOL AcceptSocket();

	BOOL PhysicalReceive(BYTE *pReceiveBuf,int &ReceiveLen);
	BOOL PhysicalSend(BYTE *pSendBuf,int &SendLen);

  BOOL CreateSocket();
  BOOL CloseSocket();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSocketSer)
	public:
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CNewSocketTH104)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
};

class CNewServerSocketTH104Thread : public CWinThread
{
	DECLARE_DYNCREATE(CNewServerSocketTH104Thread)
public:
	CNewServerSocketTH104Thread();
	virtual ~CNewServerSocketTH104Thread();

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

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROTOCOLTH104_H__57477DE4_233C_11D4_BA64_0080C8D77339__INCLUDED_)

