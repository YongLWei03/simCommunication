#if !defined(AFX_PROTOCOLTH104_H__57477DE4_233C_11D4_BA64_0080C8D77339__INCLUDED_)
#define AFX_PROTOCOLTH104_H__57477DE4_233C_11D4_BA64_0080C8D77339__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProtocolTH104.h : header file
//

//---------------------��������-----------------------//
#define  VT_FLOAT         0  //ң��
#define  VT_BIT           1  //ң��
#define  VT_LONG          2  //���
#define  VT_MEMVAR        3  //�ڴ����
#define  VT_MEMFLOAT      3  //�ڴ�ģ����
#define  VT_MEMBIT        4  //�ڴ濪����
#define  VT_PARAEMTER     5  //�����趨
#define  VT_REMOTECONCTRL 6  //ң��
#define  VT_REMOTEUPDOWN  7  //����
#define  VT_SOE           8  //SOE
#define  VT_GROUP         10 //������

#define  VT_REMOTESELECT  21 //ң��ѡ��
#define  VT_REMOTEPERFORM 22 //ң��ִ��
#define  VT_REMOTEREPEAL  23 //ң�س���
#define  VT_REMOTEREVERT  24 //ң�ظ���

#define  VT_STANDARTVAR   30  //��׼����

#define  VT_EXTBIT        30  //��չң��
#define  VT_EXTSOE        40  //��չSOE
#define  VT_EXTFLOAT      50  //��չң��
#define  VT_EXTLONG       60  //��չ���

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

  //ͨѶ����
	int       m_TotalLen;
	int       m_ReceiveLen;
	BYTE      m_TotalBuf[4096];
	BYTE      m_ReceiveBuf[4096];

  BOOL      m_StartSendDataFrame;

  CTime     m_SendAllFloatDataTime;
  CTime     m_SendAllBitDataTime;
  CTime     m_SendAllLongDataTime;

  int       m_InterCountVk;//����ͱ��
  int       m_InterCountVw;//���ȷ�ϱ��

  int       m_InterCountConfirmVR;//�ڲ�����ȷ�ϼ�����V(R)
  int       m_InterCountConfirmVS;//�ڲ�����ȷ�ϼ�����V(R)

  int       m_InterCountVR;//�ڲ����ܼ�����V(R)
  int       m_InterCountVS;//�ڲ����ͼ�����V(S)

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

  void ProcessCallAllDataBegin(BYTE CallType);  //�ٻ�ȫ����
  void ProcessCallAllDataStop(BYTE CallType);   //�ٻ�����ֹͣ
  void ProcessCallLongDataBegin(BYTE CallType); //�ٻ��������
  void ProcessCallLongDataStop(BYTE CallType);  //�ٻ��������ֹͣ

  //----------------������Ա����-----------------//
  CString GetTextHexFromBuffer(BYTE * Buf,int nLen);
  BOOL PhysicalReceive(BYTE *pReceive, int & nLen );
  BOOL PhysicalSend(BYTE * buf, int nLen );
  //----------------������Ա����-----------------//

  //----------------��ڳ�Ա����-----------------//
  BOOL UpdateComm();                                //ͨѶˢ��
  BOOL RemoteSelect(TagVarStruct *pVar);					//ң��ѡ��
  BOOL RemotePerform(TagVarStruct *pVar);					//ң��ִ��
  BOOL RemoteRepeal(TagVarStruct *pVar);					//ң�س���
  BOOL SendVarFrame(WORD UnitID,TagVarStruct *pVar);					//��������
  //----------------��ڳ�Ա����-----------------//

  //----------------ר�ó�Ա����-----------------//
	BOOL SendTestFrame();								              //���Ͳ�������֡
	BOOL ProcessDataFrame(BYTE * DataBuf);						//������վ��Ӧ������

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

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROTOCOLTH104_H__57477DE4_233C_11D4_BA64_0080C8D77339__INCLUDED_)

