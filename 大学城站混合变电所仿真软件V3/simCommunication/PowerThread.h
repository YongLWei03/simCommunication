#if !defined(AFX_POWERTHREAD_H__57477DE4_233C_11D4_BA64_0080C8D77339__INCLUDED_)
#define AFX_POWERTHREAD_H__57477DE4_233C_11D4_BA64_0080C8D77339__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PowerThread.h : header file
//

extern struct TagInlineVotlageStruct g_InlineVotlage;  //进线电压
extern struct TagPTVotlageStruct     g_PTVoltage;      //压互补偿
extern struct TagCurrentStruct       g_Current;        //负荷电流
extern struct TagCompensationStruct  g_Compensation[]; //电容补偿

extern WORD  g_StartupDelaySecond;

class CPowerThread;
extern CPowerThread *g_pPowerThread;

struct TagPresetVarStruct;
struct TagFreeTransmissionStruct;   //自由传动
struct TagFreeTrainStruct;          //自由行车
struct TagFaultTransmissionStruct;  //故障传动
struct TagSimTrainStruct;           //仿真行车
struct TagProtectionCalculaterStruct;
struct TagInlineVotlageStruct;

extern CMutex  g_PowerMutex;
extern CMutex  g_PresetVarMutex;
extern CPtrList g_FreeTransmissionList;
extern CPtrList g_FreeTrainList;

extern struct TagFaultTransmissionStruct g_FaultTransmission;//故障传动
extern struct TagSimTrainStruct          g_SimTrain;         //仿真行车

extern BOOL g_SimCommunicationMain;
extern BOOL g_SimProtectionMain;

extern CPtrList g_PresetVarList;

extern CPtrList g_1B35FeedList;
extern CPtrList g_2B35FeedList;
extern CPtrList g_1B400FeedList;
extern CPtrList g_2B400FeedList;
extern CPtrList g_Up750FeedList;
extern CPtrList g_Down750FeedList;
extern CPtrList g_1LPTList;
extern CPtrList g_2LPTList;
extern CPtrList g_1L35AutoModeOnList;
extern CPtrList g_1L35AutoModeOffList;
extern CPtrList g_2L35AutoModeOnList;
extern CPtrList g_2L35AutoModeOffList;
extern CPtrList g_Bus35AutoModeOnList;
extern CPtrList g_Bus35AutoModeOffList;
extern CPtrList g_1L400AutoModeOnList;
extern CPtrList g_1L400AutoModeOffList;
extern CPtrList g_2L400AutoModeOnList;
extern CPtrList g_2L400AutoModeOffList;
extern CPtrList g_Bus400AutoModeOnList;
extern CPtrList g_Bus400AutoModeOffList;

BOOL WINAPI LoadPowerPath();
WORD WINAPI GetFeederIDFromName(CString FeederName);
BOOL WINAPI AddFreeTransmission(TagFreeTransmissionStruct *pFreeTransmission);
BOOL WINAPI AddFreeTrain(TagFreeTrainStruct *pFreeTrain);
BOOL WINAPI ClearPower();

TagPresetVarStruct *WINAPI ReadPresetVar(WORD PresetID);
BOOL WINAPI AddPresetVar(WORD UnitID,WORD VarGroupID,WORD VarID,float PresetValue,CTime PresetTime);
BOOL WINAPI WritePresetVar(TagPresetVarStruct *pPresetVar);
BOOL WINAPI RemovePresetVar(TagPresetVarStruct *pPresetVar);
BOOL WINAPI ClearFresetVar();

/////////////////////////////////////////////////////////////////////////////
// CPowerThread thread

class CPowerThread : public CWinThread
{
	DECLARE_DYNCREATE(CPowerThread)
public:
	CPowerThread();
	virtual ~CPowerThread();
  BOOL m_Run;       //运行标志
  BOOL m_Running;   //正在运行标志
  BOOL m_Pause;     //暂停标志
  BOOL m_Pausing;   //正在暂停标志
  void KillThread();//结束线程

  WORD m_ComputerID;

  WORD m_PowerState;

  void ProcessPowerState();

  void ProcessSampleWave();

  void ProcessPowerNoloadVotlage();
  void ProcessPowerNoloadVotlage35kV();
  void ProcessPowerNoloadVotlage400V();
  void ProcessPowerNoloadVotlage750V();
  void ProcessPowerNoloadCurrent();
  void ProcessPowerCurrent();
  void ProcessPowerCurrent35kV();
  void ProcessPowerCurrent400V();
  void ProcessPowerCurrent750V();
  void ProcessPowerFeedTransmissionAndFreeTrain();
  void ProcessPowerSimTrain();
  void ProcessPowerFaultTransmission();
  void ProcessPowerPTVotlage();

  void ProcessProtectionCalculater();
  void ProcessAutoOnOff35(WORD UnitID);
  void ProcessAutoOnOff400(WORD UnitID);

  CTime m_ProcessFeedTransmissionAndFreeTrainTime;//处理馈线传动及自由行车数据更新时间
  CTime m_ProcessSimTrainTime;        //处理仿真行车负荷数据更新时间

  float PowerCalculaterValue(float SampleValueList[WAVE_SAMPLE_COUNT],WORD BeginPosition);	//计算从BeginPosition开始的32个采样点的基波值

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPowerThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPowerThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POWERTHREAD_H__57477DE4_233C_11D4_BA64_0080C8D77339__INCLUDED_)

