// PowerThread.cpp : implementation file
//
 
#include "stdafx.h"
#include "PowerThread.h"
#include "DlgMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPowerThread *g_pPowerThread;

struct TagInlineVotlageStruct g_InlineVotlage;  //进线电压
struct TagPTVotlageStruct     g_PTVoltage;      //压互补偿
struct TagCurrentStruct       g_Current;        //负荷电流
struct TagCompensationStruct  g_Compensation[2];//电容补偿

WORD  g_StartupDelaySecond = 10;

CPtrList g_1B35FeedList;
CPtrList g_2B35FeedList;
CPtrList g_1B400FeedList;
CPtrList g_2B400FeedList;
CPtrList g_Up750FeedList;
CPtrList g_Down750FeedList;
CPtrList g_1LPTList;
CPtrList g_2LPTList;
CPtrList g_1L35AutoModeOnList;
CPtrList g_1L35AutoModeOffList;
CPtrList g_2L35AutoModeOnList;
CPtrList g_2L35AutoModeOffList;
CPtrList g_Bus35AutoModeOnList;
CPtrList g_Bus35AutoModeOffList;
CPtrList g_1L400AutoModeOnList;
CPtrList g_1L400AutoModeOffList;
CPtrList g_2L400AutoModeOnList;
CPtrList g_2L400AutoModeOffList;
CPtrList g_Bus400AutoModeOnList;
CPtrList g_Bus400AutoModeOffList;

BOOL g_Power1T35 = TRUE;
BOOL g_Power2T35 = TRUE;
BOOL g_PowerBus35 = TRUE;

BOOL g_Power1T750 = TRUE;
BOOL g_Power2T750 = TRUE;

BOOL g_Power1T400 = TRUE;
BOOL g_Power2T400 = TRUE;
BOOL g_PowerBus400 = TRUE;

float g_TotalIa1B = 0;
float g_TotalIb1B = 0;
float g_TotalIc1B = 0;
float g_TotalIa2B = 0;
float g_TotalIb2B = 0;
float g_TotalIc2B = 0;

CMutex  g_PowerMutex;
CMutex  g_PresetVarMutex;
CPtrList g_FreeTransmissionList;
CPtrList g_FreeTrainList;

struct TagFaultTransmissionStruct g_FaultTransmission;//故障传动
struct TagSimTrainStruct          g_SimTrain;         //仿真行车

CPtrList g_PresetVarList;

BOOL g_SimCommunicationMain = TRUE; //通讯仿真为主
BOOL g_SimProtectionMain = FALSE;   //保护仿真为主

BOOL WINAPI LoadPowerPath()
{
  g_1B35FeedList.RemoveAll();
  g_2B35FeedList.RemoveAll();
  g_1B400FeedList.RemoveAll();
  g_2B400FeedList.RemoveAll();
  g_Up750FeedList.RemoveAll();
  g_Down750FeedList.RemoveAll();
  g_1L35AutoModeOnList.RemoveAll();
  g_1L35AutoModeOffList.RemoveAll();
  g_2L35AutoModeOnList.RemoveAll();
  g_2L35AutoModeOffList.RemoveAll();
  g_Bus35AutoModeOnList.RemoveAll();
  g_Bus35AutoModeOffList.RemoveAll();
  g_1L400AutoModeOnList.RemoveAll();
  g_1L400AutoModeOffList.RemoveAll();
  g_2L400AutoModeOnList.RemoveAll();
  g_2L400AutoModeOffList.RemoveAll();
  g_Bus400AutoModeOnList.RemoveAll();
  g_Bus400AutoModeOffList.RemoveAll();

  CString filename = g_ProjectFilePath + "PowerPath.ini";

  filename = g_ProjectFilePath + "simPower.ini";
  {
    float Ua1In = (float)66.4;
    float Ub1In = (float)66.4;
    float Uc1In = (float)66.4;
    float Ua2In = (float)66.4;
    float Ub2In = (float)66.4;
    float Uc2In = (float)66.4;
    float VoltageChangeRate = (float)0.025;
    float CurrentMaxValue   = (float)0;
    float VoltageValueK     = (float)1;

    float OnIdCurrentScale =  (float)0.3;
    float OnIgCurrentScale =  (float)0.6;
    float OffIdCurrentScale = (float)0.04;
    float OffIgCurrentScale = (float)0.06;
    float NoloadCurrentChangeMaxValue = (float)1;

    char buf[1024];
    ::GetPrivateProfileString("默认值","进线正常电压值","66.4",buf,1024,filename);
    Ua1In = (float)atof(buf) * 35 / 110;
    Ub1In = (float)atof(buf) * 35 / 110;
    Uc1In = (float)atof(buf) * 35 / 110;
    Ua2In = (float)atof(buf) * 35 / 110;
    Ub2In = (float)atof(buf) * 35 / 110;
    Uc2In = (float)atof(buf) * 35 / 110;

    ::GetPrivateProfileString("默认值","电压抖动值","0.01",buf,1024,filename);
    VoltageChangeRate = (float)atof(buf);
    ::GetPrivateProfileString("默认值","电压补偿电流最大值","0",buf,1024,filename);
    CurrentMaxValue = (float)atoi(buf);
    ::GetPrivateProfileString("默认值","电压补偿系数","1",buf,1024,filename);
    VoltageValueK = (float)atoi(buf);

    ::GetPrivateProfileString("默认值","空载电流抖动值","1",buf,1024,filename);
    NoloadCurrentChangeMaxValue = (float)atof(buf);

    ::GetPrivateProfileString("默认值","数据加载延时时间","10",buf,1024,filename);
	  g_StartupDelaySecond = atoi(buf);

    char Gear[1024];
    char Temp1[1024];
    char Temp2[1024];
    ::GetPrivateProfileString("默认值","主变档位","7",Gear,1024,filename);
    ::GetPrivateProfileString("默认值","主变油温1","35",Temp1,1024,filename);
    ::GetPrivateProfileString("默认值","主变油温2","35",Temp2,1024,filename);

    g_InlineVotlage.Ua1 = Ua1In;
    g_InlineVotlage.Ub1 = Ub1In;
    g_InlineVotlage.Uc1 = Uc1In;
    g_InlineVotlage.Ua2 = Ua2In;
    g_InlineVotlage.Ub2 = Ub2In;
    g_InlineVotlage.Uc2 = Uc2In;
    g_InlineVotlage.T1HiVoltageChangeRate = VoltageChangeRate;
    g_InlineVotlage.T1LoVoltageChangeRate = VoltageChangeRate;
    g_InlineVotlage.T2HiVoltageChangeRate = VoltageChangeRate;
    g_InlineVotlage.T2LoVoltageChangeRate = VoltageChangeRate;
    
    g_PTVoltage.T1HiVoltageValueK = VoltageValueK;
    g_PTVoltage.T1HiCurrentMaxValue = CurrentMaxValue;
    g_PTVoltage.T2HiVoltageValueK = VoltageValueK;
    g_PTVoltage.T2HiCurrentMaxValue = CurrentMaxValue;
    g_PTVoltage.T1LoVoltageValueK = VoltageValueK;
    g_PTVoltage.T1LoCurrentMaxValue = CurrentMaxValue;
    g_PTVoltage.T2LoVoltageValueK = VoltageValueK;
    g_PTVoltage.T2LoCurrentMaxValue = CurrentMaxValue;
    g_PTVoltage.BeginTime = 0;

    g_PTVoltage.T1Gear = atoi(Gear);
    g_PTVoltage.T2Gear = atoi(Gear);
    g_PTVoltage.T1Temp1 = atoi(Temp1);
    g_PTVoltage.T1Temp2 = atoi(Temp2);
    g_PTVoltage.T2Temp1 = atoi(Temp1);
    g_PTVoltage.T2Temp2 = atoi(Temp2);

    g_Current.NoloadCurrentChangeMaxValue = NoloadCurrentChangeMaxValue;
    g_Current.BeginTime = 0;
  }

  filename = g_ProjectFilePath + "PowerPath.ini";

  g_1B35FeedList.AddTail(&g_Unit[3]);
  g_2B35FeedList.AddTail(&g_Unit[4]);
  g_1B400FeedList.AddTail(&g_Unit[21]);
  g_2B400FeedList.AddTail(&g_Unit[22]);
  g_Up750FeedList.AddTail(&g_Unit[14]);
  g_Up750FeedList.AddTail(&g_Unit[16]);
  g_Down750FeedList.AddTail(&g_Unit[15]);
  g_Down750FeedList.AddTail(&g_Unit[13]);

  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("35kV1#进线并列运行合位",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);
      
      g_1L35AutoModeOnList.AddTail(pPowerPath);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("35kV1#进线并列运行分位",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);

      g_1L35AutoModeOffList.AddTail(pPowerPath);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("35kV2#进线并列运行合位",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);

      g_2L35AutoModeOnList.AddTail(pPowerPath);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("35kV2#进线并列运行分位",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);

      g_2L35AutoModeOffList.AddTail(pPowerPath);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("35kV分列运行合位",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);
      
      g_Bus35AutoModeOnList.AddTail(pPowerPath);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("35kV分列运行分位",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);

      g_Bus35AutoModeOffList.AddTail(pPowerPath);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("400V1#进线并列运行合位",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);
      
      g_1L400AutoModeOnList.AddTail(pPowerPath);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("400V1#进线并列运行分位",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);

      g_1L400AutoModeOffList.AddTail(pPowerPath);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("400V2#进线并列运行合位",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);

      g_2L400AutoModeOnList.AddTail(pPowerPath);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("400V2#进线并列运行分位",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);

      g_2L400AutoModeOffList.AddTail(pPowerPath);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("400V分列运行合位",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);
      
      g_Bus400AutoModeOnList.AddTail(pPowerPath);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("400V分列运行分位",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);

      g_Bus400AutoModeOffList.AddTail(pPowerPath);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("供电状态数据交换",key,"",buf,1024,filename);
      if(strlen(buf)>=8)
      {
        CString VarInfo = buf;
        WORD UnitID = atoi(VarInfo.Mid(0,2));
        WORD VarGroupID = atoi(VarInfo.Mid(3,2));
        WORD VarID = atoi(VarInfo.Mid(6,2));
        g_DataExchangeList[no].UnitID = UnitID;
        g_DataExchangeList[no].VarGroupID = VarGroupID;
        g_DataExchangeList[no].VarID = VarID;
      }

      key.Format("%02d",no);
      ::GetPrivateProfileString("行车状态数据交换",key,"",buf,1024,filename);
      if(strlen(buf)>0)
      {
        CNewString ns(buf,";","\r\n");
        g_DataExchangeList[no].AverCurrent = ns.GetFloatValue(0,0);
        g_DataExchangeList[no].AverAngle = ns.GetFloatValue(0,1);
        g_DataExchangeList[no].CurrentChangeRate = ns.GetFloatValue(0,2);
      }
    }
  }

  return TRUE;
}

WORD WINAPI GetFeederIDFromName(CString FeederName)
{
  CPtrList FeedList;
  {
    POSITION pos = g_1B35FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_1B35FeedList.GetNext(pos);
      FeedList.AddTail(pUnit);
    }
  }
  {
    POSITION pos = g_2B35FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_2B35FeedList.GetNext(pos);
      FeedList.AddTail(pUnit);
    }
  }
  {
    POSITION pos = g_1B400FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_1B400FeedList.GetNext(pos);
      FeedList.AddTail(pUnit);
    }
  }
  {
    POSITION pos = g_2B400FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_2B400FeedList.GetNext(pos);
      FeedList.AddTail(pUnit);
    }
  }
  {
    POSITION pos = g_Up750FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_Up750FeedList.GetNext(pos);
      FeedList.AddTail(pUnit);
    }
  }
  {
    POSITION pos = g_Down750FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_Down750FeedList.GetNext(pos);
      FeedList.AddTail(pUnit);
    }
  }

  POSITION pos = FeedList.GetHeadPosition();
  while(pos)
  {
    struct TagUnitStruct *pUnit = (TagUnitStruct*)FeedList.GetNext(pos);
    if(pUnit->UnitName == FeederName)
      return pUnit->UnitID;
  }

  return 0;
}

BOOL WINAPI AddFreeTransmission(TagFreeTransmissionStruct *pFreeTransmission)
{
  CSingleLock sLock(&g_PowerMutex);
  sLock.Lock();

  g_FreeTransmissionList.AddTail(pFreeTransmission);

  return TRUE;
}

BOOL WINAPI AddFreeTrain(TagFreeTrainStruct *pFreeTrain)
{
  CSingleLock sLock(&g_PowerMutex);
  sLock.Lock();

  g_FreeTrainList.AddTail(pFreeTrain);

  return TRUE;
}

BOOL WINAPI ClearPower()
{
  CSingleLock sLock(&g_PowerMutex);
  sLock.Lock();

  while(g_FreeTransmissionList.GetCount())
    delete (TagFreeTransmissionStruct*)g_FreeTransmissionList.RemoveHead();

  while(g_FreeTrainList.GetCount())
    delete (TagFreeTrainStruct*)g_FreeTrainList.RemoveHead();

  g_FaultTransmission.FaultTransmissionState = 0; //故障传动
  g_SimTrain.SimTrainState = 0;                   //仿真行车

  return TRUE;
}

TagPresetVarStruct *WINAPI ReadPresetVar(WORD PresetID)
{
  CSingleLock sLock(&g_PresetVarMutex);
  sLock.Lock();

  TagPresetVarStruct *pPresetVar = NULL;
  POSITION pos = g_PresetVarList.FindIndex(PresetID);
  if(pos)
    pPresetVar = (TagPresetVarStruct *)g_PresetVarList.GetAt(pos);

  return pPresetVar;
}

BOOL WINAPI AddPresetVar(WORD UnitID,WORD VarGroupID,WORD VarID,float PresetValue,CTime PresetTime)
{
  CSingleLock sLock(&g_PresetVarMutex);
  sLock.Lock();

  TagPresetVarStruct *pPresetVar = new TagPresetVarStruct;
  g_PresetVarList.AddTail(pPresetVar);
  pPresetVar->UnitID = UnitID;
  pPresetVar->VarGroupID = VarGroupID;
  pPresetVar->VarID = VarID;
  pPresetVar->PresetValue = PresetValue;
  pPresetVar->PresetTime = PresetTime;

  return TRUE;
}

BOOL WINAPI WritePresetVar(TagPresetVarStruct *pPresetVar)
{
  CSingleLock sLock(&g_PresetVarMutex);
  sLock.Lock();

  g_PresetVarList.AddTail(pPresetVar);

  return TRUE;
}

BOOL WINAPI RemovePresetVar(TagPresetVarStruct *pPresetVar)
{
  POSITION pos = g_PresetVarList.Find(pPresetVar);
  if(pos)
  {
    g_PresetVarList.RemoveAt(pos);
    delete pPresetVar;

    return TRUE;
  }

  return FALSE;
}

BOOL WINAPI ClearFresetVar()
{
  CSingleLock sLock(&g_PresetVarMutex);
  sLock.Lock();

  while(g_PresetVarList.GetCount())
    delete (TagPresetVarStruct*)g_PresetVarList.RemoveHead();

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPowerThread

IMPLEMENT_DYNCREATE(CPowerThread, CWinThread)

CPowerThread::CPowerThread()
{
  m_Run = TRUE;       //运行标志
  m_Running = TRUE;   //正在运行标志
  m_Pause = TRUE;     //暂停标志
  m_Pausing = TRUE;   //正在暂停标志

  m_ComputerID = 0;
  m_PowerState = 0;

  m_ProcessFeedTransmissionAndFreeTrainTime = 0;//处理馈线传动及自由行车数据更新时间
  m_ProcessSimTrainTime = 0;        //处理仿真行车负荷数据更新时间
}

CPowerThread::~CPowerThread()
{
}

BOOL CPowerThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CPowerThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CPowerThread, CWinThread)
	//{{AFX_MSG_MAP(CPowerThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPowerThread message handlers

void CPowerThread::ProcessPowerNoloadVotlage()
{
  float Ua1In = g_InlineVotlage.Ua1+(g_InlineVotlage.Ua1*(float)(rand()%100)/(float)100*g_InlineVotlage.T1HiVoltageChangeRate);
  float Ub1In = g_InlineVotlage.Ub1+(g_InlineVotlage.Ub1*(float)(rand()%100)/(float)100*g_InlineVotlage.T1HiVoltageChangeRate);
  float Uc1In = g_InlineVotlage.Uc1+(g_InlineVotlage.Uc1*(float)(rand()%100)/(float)100*g_InlineVotlage.T1HiVoltageChangeRate);
  float Ua2In = g_InlineVotlage.Ua2+(g_InlineVotlage.Ua2*(float)(rand()%100)/(float)100*g_InlineVotlage.T2HiVoltageChangeRate);
  float Ub2In = g_InlineVotlage.Ub2+(g_InlineVotlage.Ub2*(float)(rand()%100)/(float)100*g_InlineVotlage.T2HiVoltageChangeRate);
  float Uc2In = g_InlineVotlage.Uc2+(g_InlineVotlage.Uc2*(float)(rand()%100)/(float)100*g_InlineVotlage.T2HiVoltageChangeRate);

  float Uab1In = (float)::sqrt(Ua1In*Ua1In + Ub1In*Ub1In + Ua1In*Ub1In);
  float Ubc1In = (float)::sqrt(Ub1In*Ub1In + Uc1In*Uc1In + Ub1In*Uc1In);
  float Uca1In = (float)::sqrt(Uc1In*Uc1In + Ua1In*Ua1In + Uc1In*Ua1In);
  float Uo1In = 0;

  float Uab2In = (float)::sqrt(Ua2In*Ua2In + Ub2In*Ub2In + Ua2In*Ub2In);
  float Ubc2In = (float)::sqrt(Ub2In*Ub2In + Uc2In*Uc2In + Ub2In*Uc2In);
  float Uca2In = (float)::sqrt(Uc2In*Uc2In + Ua2In*Ua2In + Uc2In*Ua2In);
  float Uo2In = 0;

  float Ua1T35 = 0;
  float Ub1T35 = 0;
  float Uc1T35 = 0;
  float Uab1T35 = 0;
  float Ubc1T35 = 0;
  float Uca1T35 = 0;
  float Uo1T35 = 0;

  float Ua2T35 = 0;
  float Ub2T35 = 0;
  float Uc2T35 = 0;
  float Uab2T35 = 0;
  float Ubc2T35 = 0;
  float Uca2T35 = 0;
  float Uo2T35 = 0;

  float Ua1B35 = 0;
  float Ub1B35 = 0;
  float Uc1B35 = 0;
  float Uab1B35 = 0;
  float Ubc1B35 = 0;
  float Uca1B35 = 0;
  float Uo1B35 = 0;

  float Ua2B35 = 0;
  float Ub2B35 = 0;
  float Uc2B35 = 0;
  float Uab2B35 = 0;
  float Ubc2B35 = 0;
  float Uca2B35 = 0;
  float Uo2B35 = 0;

  float Ua1T400 = 0;
  float Ub1T400 = 0;
  float Uc1T400 = 0;
  float Uab1T400 = 0;
  float Ubc1T400 = 0;
  float Uca1T400 = 0;
  float Uo1T400 = 0;

  float Ua2T400 = 0;
  float Ub2T400 = 0;
  float Uc2T400 = 0;
  float Uab2T400 = 0;
  float Ubc2T400 = 0;
  float Uca2T400 = 0;
  float Uo2T400 = 0;

  float Ua1B400 = 0;
  float Ub1B400 = 0;
  float Uc1B400 = 0;
  float Uab1B400 = 0;
  float Ubc1B400 = 0;
  float Uca1B400 = 0;
  float Uo1B400 = 0;

  float Ua2B400 = 0;
  float Ub2B400 = 0;
  float Uc2B400 = 0;
  float Uab2B400 = 0;
  float Ubc2B400 = 0;
  float Uca2B400 = 0;
  float Uo2B400 = 0;

  float UB750 = 0;
  float U1B750 = 0;
  float U2B750 = 0;

  g_Power1T35 = TRUE;
  g_Power2T35 = TRUE;
  g_PowerBus35 = TRUE;

  g_Power1T750 = TRUE;
  g_Power2T750 = TRUE;

  g_Power1T400 = TRUE;
  g_Power2T400 = TRUE;
  g_PowerBus400 = TRUE;

  {
    if(g_Unit[1].VarGroup[1].Var[7].Value==0)//35kV1#进线PT刀闸
    {
      Ua1In = 0;
      Ub1In = 0;
      Uc1In = 0;
      Uab1In = 0;
      Ubc1In = 0;
      Uca1In = 0;
      Uo1In = 0;
    }    
  }
  {
    if(g_Unit[2].VarGroup[1].Var[7].Value==0)//35kV2#进线PT刀闸
    {
      Ua2In = 0;
      Ub2In = 0;
      Uc2In = 0;
      Uab2In = 0;
      Ubc2In = 0;
      Uca2In = 0;
      Uo2In = 0;
    }    
  }
  {
    struct TagUnitStruct *pUnit;
    pUnit = &g_Unit[1];//35kV1#进线
    pUnit->VarGroup[0].Var[0].Value = Uab1In;
    pUnit->VarGroup[0].Var[1].Value = Ubc1In;
    pUnit->VarGroup[0].Var[2].Value = Uca1In;
    pUnit->VarGroup[0].Var[3].Value = Ua1In;
    pUnit->VarGroup[0].Var[4].Value = Ub1In;
    pUnit->VarGroup[0].Var[5].Value = Uc1In;
    pUnit->VarGroup[0].Var[6].Value = Uo1In;

    pUnit = &g_Unit[3];//35kV1#出线
    pUnit->VarGroup[0].Var[0].Value = Uab1In;
    pUnit->VarGroup[0].Var[1].Value = Ubc1In;
    pUnit->VarGroup[0].Var[2].Value = Uca1In;
    pUnit->VarGroup[0].Var[3].Value = Ua1In;
    pUnit->VarGroup[0].Var[4].Value = Ub1In;
    pUnit->VarGroup[0].Var[5].Value = Uc1In;
    pUnit->VarGroup[0].Var[6].Value = Uo1In;

    pUnit = &g_Unit[2];//35kV2#进线
    pUnit->VarGroup[0].Var[0].Value = Uab2In;
    pUnit->VarGroup[0].Var[1].Value = Ubc2In;
    pUnit->VarGroup[0].Var[2].Value = Uca2In;
    pUnit->VarGroup[0].Var[3].Value = Ua2In;
    pUnit->VarGroup[0].Var[4].Value = Ub2In;
    pUnit->VarGroup[0].Var[5].Value = Uc2In;
    pUnit->VarGroup[0].Var[6].Value = Uo2In;

    pUnit = &g_Unit[4];//35kV2#出线
    pUnit->VarGroup[0].Var[0].Value = Uab2In;
    pUnit->VarGroup[0].Var[1].Value = Ubc2In;
    pUnit->VarGroup[0].Var[2].Value = Uca2In;
    pUnit->VarGroup[0].Var[3].Value = Ua2In;
    pUnit->VarGroup[0].Var[4].Value = Ub2In;
    pUnit->VarGroup[0].Var[5].Value = Uc2In;
    pUnit->VarGroup[0].Var[6].Value = Uo2In;

    pUnit = &g_Unit[5];//35kV母联
    pUnit->VarGroup[0].Var[0].Value = 0;
    pUnit->VarGroup[0].Var[1].Value = 0;
    pUnit->VarGroup[0].Var[2].Value = 0;
    pUnit->VarGroup[0].Var[3].Value = 0;
    pUnit->VarGroup[0].Var[4].Value = 0;
    pUnit->VarGroup[0].Var[5].Value = 0;
    pUnit->VarGroup[0].Var[6].Value = 0;
  }

  if(g_Unit[1].VarGroup[1].Var[0].Value==0)//35kV1#进线
    g_Power1T35 = FALSE;
  if(g_Unit[1].VarGroup[1].Var[5].Value==0)
    g_Power1T35 = FALSE;
  if(g_Unit[2].VarGroup[1].Var[0].Value==0)//35kV2#进线
    g_Power2T35 = FALSE;
  if(g_Unit[2].VarGroup[1].Var[5].Value==0)
    g_Power2T35 = FALSE;
  if(g_Unit[5].VarGroup[1].Var[0].Value==0)//35kV母联
    g_PowerBus35 = FALSE;
  if(g_Unit[5].VarGroup[1].Var[5].Value==0)
    g_PowerBus35 = FALSE;
  if(g_Unit[5].VarGroup[1].Var[6].Value==0)
    g_PowerBus35 = FALSE;

//-----------35kV I段----------//
  if(g_Power1T35==TRUE)
  {
    Ua1B35 = Ua1In;
    Ub1B35 = Ub1In;
    Uc1B35 = Uc1In;
    Uab1B35 = Uab1In;
    Ubc1B35 = Ubc1In;
    Uca1B35 = Uca1In;
    Uo1B35 = Uo1In;
  }

//-----------35kV II段----------//
  if(g_Power2T35==TRUE)
  {
    Ua2B35 = Ua2In;
    Ub2B35 = Ub2In;
    Uc2B35 = Uc2In;
    Uab2B35 = Uab2In;
    Ubc2B35 = Ubc2In;
    Uca2B35 = Uca2In;
    Uo2B35 = Uo2In;
  }

  if((g_Power1T35==TRUE)&&(g_Power2T35==TRUE)&&(g_PowerBus35==FALSE))//1#、2#进线分列运行
  {
    ;
  }
  if((g_Power1T35==TRUE)&&(g_Power2T35==FALSE)&&(g_PowerBus35==TRUE))//1#进线并列运行
  {
    Ua2B35 = Ua1B35;
    Ub2B35 = Ub1B35;
    Uc2B35 = Uc1B35;
    Uab2B35 = Uab1B35;
    Ubc2B35 = Ubc1B35;
    Uca2B35 = Uca1B35;
    Uo2B35 = Uo1B35;
  }
  if((g_Power1T35==FALSE)&&(g_Power2T35==TRUE)&&(g_PowerBus35==TRUE))//2#进线并列运行
  {
    Ua1B35 = Ua2B35;
    Ub1B35 = Ub2B35;
    Uc1B35 = Uc2B35;
    Uab1B35 = Uab2B35;
    Ubc1B35 = Ubc2B35;
    Uca1B35 = Uca2B35;
    Uo1B35 = Uo2B35;
  }

  {
    struct TagUnitStruct *pUnit;
    pUnit = &g_Unit[7];//1T高压侧
    pUnit->VarGroup[0].Var[0].Value = Uab1B35;
    pUnit->VarGroup[0].Var[1].Value = Ubc1B35;
    pUnit->VarGroup[0].Var[2].Value = Uca1B35;
    pUnit->VarGroup[0].Var[3].Value = Ua1B35;
    pUnit->VarGroup[0].Var[4].Value = Ub1B35;
    pUnit->VarGroup[0].Var[5].Value = Uc1B35;
    pUnit->VarGroup[0].Var[6].Value = Uo1B35;

    pUnit = &g_Unit[9];//1T整流高压侧
    pUnit->VarGroup[0].Var[0].Value = Uab1B35;
    pUnit->VarGroup[0].Var[1].Value = Ubc1B35;
    pUnit->VarGroup[0].Var[2].Value = Uca1B35;
    pUnit->VarGroup[0].Var[3].Value = Ua1B35;
    pUnit->VarGroup[0].Var[4].Value = Ub1B35;
    pUnit->VarGroup[0].Var[5].Value = Uc1B35;
    pUnit->VarGroup[0].Var[6].Value = Uo1B35;

    pUnit = &g_Unit[8];//2T高压侧
    pUnit->VarGroup[0].Var[0].Value = Uab2B35;
    pUnit->VarGroup[0].Var[1].Value = Ubc2B35;
    pUnit->VarGroup[0].Var[2].Value = Uca2B35;
    pUnit->VarGroup[0].Var[3].Value = Ua2B35;
    pUnit->VarGroup[0].Var[4].Value = Ub2B35;
    pUnit->VarGroup[0].Var[5].Value = Uc2B35;
    pUnit->VarGroup[0].Var[6].Value = Uo2B35;

    pUnit = &g_Unit[10];//2T整流高压侧
    pUnit->VarGroup[0].Var[0].Value = Uab2B35;
    pUnit->VarGroup[0].Var[1].Value = Ubc2B35;
    pUnit->VarGroup[0].Var[2].Value = Uca2B35;
    pUnit->VarGroup[0].Var[3].Value = Ua2B35;
    pUnit->VarGroup[0].Var[4].Value = Ub2B35;
    pUnit->VarGroup[0].Var[5].Value = Uc2B35;
    pUnit->VarGroup[0].Var[6].Value = Uo2B35;
  }


  if(g_Unit[7].VarGroup[1].Var[0].Value==0)//1T高压侧
    g_Power1T400 = FALSE;
  if(g_Unit[7].VarGroup[1].Var[5].Value==0)
    g_Power1T400 = FALSE;
  if(g_Unit[17].VarGroup[1].Var[0].Value==0)//400V1#进线
    g_Power1T400 = FALSE;
  if(g_Unit[17].VarGroup[1].Var[5].Value==0)
    g_Power1T400 = FALSE;
  if(g_Unit[8].VarGroup[1].Var[0].Value==0)//2T高压侧
    g_Power2T400 = FALSE;
  if(g_Unit[8].VarGroup[1].Var[5].Value==0)
    g_Power2T400 = FALSE;
  if(g_Unit[18].VarGroup[1].Var[0].Value==0)//400V2#进线
    g_Power2T400 = FALSE;
  if(g_Unit[18].VarGroup[1].Var[5].Value==0)
    g_Power2T400 = FALSE;
  if(g_Unit[20].VarGroup[1].Var[0].Value==0)//400V母联
    g_PowerBus400 = FALSE;
  if(g_Unit[20].VarGroup[1].Var[5].Value==0)
    g_PowerBus400 = FALSE;

//-----------400V I段----------//
  if(g_Power1T400==TRUE)
  {
    Ua1B400 = Ua1B35 / (float)35 * (float)400;
    Ub1B400 = Ub1B35 / (float)35 * (float)400;
    Uc1B400 = Uc1B35 / (float)35 * (float)400;
    Uab1B400 = Uab1B35 / (float)35 * (float)400;
    Ubc1B400 = Ubc1B35 / (float)35 * (float)400;
    Uca1B400 = Uca1B35 / (float)35 * (float)400;
    Uo1B400 = Uo1B35;
  }

//-----------400V II段----------//
  if(g_Power2T400==TRUE)
  {
    Ua2B400 = Ua2B35 / (float)35 * (float)400;
    Ub2B400 = Ub2B35 / (float)35 * (float)400;
    Uc2B400 = Uc2B35 / (float)35 * (float)400;
    Uab2B400 = Uab2B35 / (float)35 * (float)400;
    Ubc2B400 = Ubc2B35 / (float)35 * (float)400;
    Uca2B400 = Uca2B35 / (float)35 * (float)400;
    Uo2B400 = Uo2B35;
  }

  Ua1B400 = Ua1B400+(Ua1B400*(float)(rand()%100)/(float)100*g_InlineVotlage.T1LoVoltageChangeRate);
  Ub1B400 = Ub1B400+(Ub1B400*(float)(rand()%100)/(float)100*g_InlineVotlage.T1LoVoltageChangeRate);
  Uc1B400 = Uc1B400+(Uc1B400*(float)(rand()%100)/(float)100*g_InlineVotlage.T1LoVoltageChangeRate);
  Uab1B400 = Uab1B400+(Uab1B400*(float)(rand()%100)/(float)100*g_InlineVotlage.T1LoVoltageChangeRate);
  Ubc1B400 = Ubc1B400+(Ubc1B400*(float)(rand()%100)/(float)100*g_InlineVotlage.T1LoVoltageChangeRate);
  Uca1B400 = Uca1B400+(Uca1B400*(float)(rand()%100)/(float)100*g_InlineVotlage.T1LoVoltageChangeRate);

  Ua2B400 = Ua2B400+(Ua2B400*(float)(rand()%100)/(float)100*g_InlineVotlage.T1LoVoltageChangeRate);
  Ub2B400 = Ub2B400+(Ub2B400*(float)(rand()%100)/(float)100*g_InlineVotlage.T1LoVoltageChangeRate);
  Uc2B400 = Uc2B400+(Uc2B400*(float)(rand()%100)/(float)100*g_InlineVotlage.T1LoVoltageChangeRate);
  Uab2B400 = Uab2B400+(Uab2B400*(float)(rand()%100)/(float)100*g_InlineVotlage.T1LoVoltageChangeRate);
  Ubc2B400 = Ubc2B400+(Ubc2B400*(float)(rand()%100)/(float)100*g_InlineVotlage.T1LoVoltageChangeRate);
  Uca2B400 = Uca2B400+(Uca2B400*(float)(rand()%100)/(float)100*g_InlineVotlage.T1LoVoltageChangeRate);

  if((g_Power1T400==TRUE)&&(g_Power2T400==TRUE)&&(g_PowerBus400==FALSE))//1#、2#进线分列运行
  {
    ;
  }
  if((g_Power1T400==TRUE)&&(g_Power2T400==FALSE)&&(g_PowerBus400==TRUE))//1#进线并列运行
  {
    Ua2B400 = Ua1B400;
    Ub2B400 = Ub1B400;
    Uc2B400 = Uc1B400;
    Uab2B400 = Uab1B400;
    Ubc2B400 = Ubc1B400;
    Uca2B400 = Uca1B400;
    Uo2B400 = Uo1B400;

  }
  if((g_Power1T400==FALSE)&&(g_Power2T400==TRUE)&&(g_PowerBus400==TRUE))//2#进线并列运行
  {
    Ua1B400 = Ua2B400;
    Ub1B400 = Ub2B400;
    Uc1B400 = Uc2B400;
    Uab1B400 = Uab2B400;
    Ubc1B400 = Ubc2B400;
    Uca1B400 = Uca2B400;
    Uo1B400 = Uo2B400;
  }

  {
    struct TagUnitStruct *pUnit;
    pUnit = &g_Unit[17];//1T400V侧
    pUnit->VarGroup[0].Var[0].Value = Uab1B400;
    pUnit->VarGroup[0].Var[1].Value = Ubc1B400;
    pUnit->VarGroup[0].Var[2].Value = Uca1B400;
    pUnit->VarGroup[0].Var[3].Value = Ua1B400;
    pUnit->VarGroup[0].Var[4].Value = Ub1B400;
    pUnit->VarGroup[0].Var[5].Value = Uc1B400;
    pUnit->VarGroup[0].Var[6].Value = Uo1B400;

    pUnit = &g_Unit[18];//2T400V侧
    pUnit->VarGroup[0].Var[0].Value = Uab2B400;
    pUnit->VarGroup[0].Var[1].Value = Ubc2B400;
    pUnit->VarGroup[0].Var[2].Value = Uca2B400;
    pUnit->VarGroup[0].Var[3].Value = Ua2B400;
    pUnit->VarGroup[0].Var[4].Value = Ub2B400;
    pUnit->VarGroup[0].Var[5].Value = Uc2B400;
    pUnit->VarGroup[0].Var[6].Value = Uo2B400;

    pUnit = &g_Unit[21];//400VI段出线
    pUnit->VarGroup[0].Var[0].Value = Uab1B400;
    pUnit->VarGroup[0].Var[1].Value = Ubc1B400;
    pUnit->VarGroup[0].Var[2].Value = Uca1B400;
    pUnit->VarGroup[0].Var[3].Value = Ua1B400;
    pUnit->VarGroup[0].Var[4].Value = Ub1B400;
    pUnit->VarGroup[0].Var[5].Value = Uc1B400;
    pUnit->VarGroup[0].Var[6].Value = Uo1B400;
    pUnit->VarGroup[1].Var[17].Value = 0;
    if((pUnit->VarGroup[0].Var[0].Value>0)&&(pUnit->VarGroup[0].Var[1].Value>0)&&(pUnit->VarGroup[0].Var[2].Value>0))
    {
      if(pUnit->VarGroup[1].Var[0].Value!=0)
        pUnit->VarGroup[1].Var[17].Value = 1;//判断有电
    }
    pUnit = &g_Unit[22];//400VII段出线
    pUnit->VarGroup[0].Var[0].Value = Uab2B400;
    pUnit->VarGroup[0].Var[1].Value = Ubc2B400;
    pUnit->VarGroup[0].Var[2].Value = Uca2B400;
    pUnit->VarGroup[0].Var[3].Value = Ua2B400;
    pUnit->VarGroup[0].Var[4].Value = Ub2B400;
    pUnit->VarGroup[0].Var[5].Value = Uc2B400;
    pUnit->VarGroup[0].Var[6].Value = Uo2B400;
    pUnit->VarGroup[1].Var[17].Value = 0;
    if((pUnit->VarGroup[0].Var[0].Value>0)&&(pUnit->VarGroup[0].Var[1].Value>0)&&(pUnit->VarGroup[0].Var[2].Value>0))
    {
      if(pUnit->VarGroup[1].Var[0].Value!=0)
        pUnit->VarGroup[1].Var[17].Value = 1;//判断有电
    }
  }

  if((g_Unit[9].VarGroup[1].Var[0].Value!=0)&&(g_Unit[9].VarGroup[1].Var[5].Value!=0)&&//1T整流高压侧
     (g_Unit[11].VarGroup[1].Var[0].Value!=0)&&(g_Unit[11].VarGroup[1].Var[5].Value!=0)&&(g_Unit[11].VarGroup[1].Var[6].Value!=0))//1T整流低压侧
  {
    U1B750 = 0;
    if(g_Unit[11].VarGroup[0].Var[1].Value<7286)
      U1B750 = (float)826 - (float)0.0076*g_Unit[11].VarGroup[0].Var[1].Value;
    else
      U1B750 = (float)904 - (float)0.0184*g_Unit[11].VarGroup[0].Var[1].Value;
    if(((Ua2B35+Ub2B35+Uc2B35)/3)<10)
      U1B750 = 0;
  }
  if((g_Unit[10].VarGroup[1].Var[0].Value!=0)&&(g_Unit[10].VarGroup[1].Var[5].Value!=0)&&//2T整流高压侧
     (g_Unit[12].VarGroup[1].Var[0].Value!=0)&&(g_Unit[12].VarGroup[1].Var[5].Value!=0)&&(g_Unit[12].VarGroup[1].Var[6].Value!=0))//2T整流低压侧
  {
    U2B750 = 0;
    if(g_Unit[11].VarGroup[0].Var[1].Value<7286)
      U2B750 = (float)826 - (float)0.0076*g_Unit[11].VarGroup[0].Var[1].Value;
    else
      U2B750 = (float)904 - (float)0.0184*g_Unit[11].VarGroup[0].Var[1].Value;
    if(((Ua2B35+Ub2B35+Uc2B35)/3)<10)
      U2B750 = 0;
  }

  if(g_Unit[9].VarGroup[1].Var[0].Value==0)//1T整流
    g_Power1T750 = FALSE;
  if(g_Unit[9].VarGroup[1].Var[5].Value==0)
    g_Power1T750 = FALSE;
  if(g_Unit[11].VarGroup[1].Var[0].Value==0)//1T整流
    g_Power1T750 = FALSE;
  if(g_Unit[11].VarGroup[1].Var[5].Value==0)
    g_Power1T750 = FALSE;
  if(g_Unit[11].VarGroup[1].Var[6].Value==0)
    g_Power1T750 = FALSE;

  if(g_Unit[10].VarGroup[1].Var[0].Value==0)//2T整流
    g_Power2T750 = FALSE;
  if(g_Unit[10].VarGroup[1].Var[5].Value==0)
    g_Power2T750 = FALSE;
  if(g_Unit[12].VarGroup[1].Var[0].Value==0)//2T整流
    g_Power2T750 = FALSE;
  if(g_Unit[12].VarGroup[1].Var[5].Value==0)
    g_Power2T750 = FALSE;
  if(g_Unit[12].VarGroup[1].Var[6].Value==0)
    g_Power2T750 = FALSE;

  if(g_Power1T750==FALSE)
    U1B750 = 0;
  if(g_Power2T750==FALSE)
    U2B750 = 0;

  if(U1B750>0)
    UB750 = U1B750;
  if(U2B750>0)
    UB750 = U2B750;
  
  if(UB750>0)
  {
    if((rand()%100)>50)
      UB750 = UB750 + 1;
  }

  {
    struct TagUnitStruct *pUnit;
    pUnit = &g_Unit[11];//直流进线
    pUnit->VarGroup[0].Var[0].Value = UB750;

    pUnit = &g_Unit[12];//直流进线
    pUnit->VarGroup[0].Var[0].Value = UB750;

    pUnit = &g_Unit[13];//直流出线
    pUnit->VarGroup[0].Var[0].Value = UB750;
    pUnit->VarGroup[1].Var[17].Value = 0;
    if(pUnit->VarGroup[0].Var[0].Value>0)
    {
      if((pUnit->VarGroup[1].Var[0].Value!=0)&&(pUnit->VarGroup[1].Var[5].Value!=0)&&(pUnit->VarGroup[1].Var[6].Value!=0))
        pUnit->VarGroup[1].Var[17].Value = 1;//判断有电
    }

    pUnit = &g_Unit[14];//直流出线
    pUnit->VarGroup[0].Var[0].Value = UB750;
    pUnit->VarGroup[1].Var[17].Value = 0;
    if(pUnit->VarGroup[0].Var[0].Value>0)
    {
      if((pUnit->VarGroup[1].Var[0].Value!=0)&&(pUnit->VarGroup[1].Var[5].Value!=0)&&(pUnit->VarGroup[1].Var[6].Value!=0))
        pUnit->VarGroup[1].Var[17].Value = 1;//判断有电
    }

    pUnit = &g_Unit[15];//直流出线
    pUnit->VarGroup[0].Var[0].Value = UB750;
    pUnit->VarGroup[1].Var[17].Value = 0;
    if(pUnit->VarGroup[0].Var[0].Value>0)
    {
      if((pUnit->VarGroup[1].Var[0].Value!=0)&&(pUnit->VarGroup[1].Var[5].Value!=0)&&(pUnit->VarGroup[1].Var[6].Value!=0))
        pUnit->VarGroup[1].Var[17].Value = 1;//判断有电
    }

    pUnit = &g_Unit[16];//直流出线
    pUnit->VarGroup[0].Var[0].Value = UB750;
    pUnit->VarGroup[1].Var[17].Value = 0;
    if(pUnit->VarGroup[0].Var[0].Value>0)
    {
      if((pUnit->VarGroup[1].Var[0].Value!=0)&&(pUnit->VarGroup[1].Var[5].Value!=0)&&(pUnit->VarGroup[1].Var[6].Value!=0))
        pUnit->VarGroup[1].Var[17].Value = 1;//判断有电
    }
  }

  if(UB750<=1)//大双边供电
  {
    if((g_DataExchangeListOther[25].PowerState==TRUE)&&(g_Unit[13].VarGroup[1].Var[8].Value!=0))//2113纵向隔离开关
    {
      g_Unit[13].VarGroup[1].Var[17].Value = 1;
      g_Unit[15].VarGroup[1].Var[17].Value = 1;
    }

    if((g_DataExchangeListOther[25].PowerState==TRUE)&&
       (g_Unit[13].VarGroup[1].Var[0].Value!=0)&&(g_Unit[13].VarGroup[1].Var[5].Value!=0)&&(g_Unit[13].VarGroup[1].Var[6].Value!=0)&&//211
       (g_Unit[15].VarGroup[1].Var[0].Value!=0)&&(g_Unit[15].VarGroup[1].Var[5].Value!=0)&&(g_Unit[15].VarGroup[1].Var[6].Value!=0)) //213
    {
      if((rand()%100)>50)
        g_Unit[11].VarGroup[0].Var[0].Value = 826;
      else
        g_Unit[11].VarGroup[0].Var[0].Value = 827;

      g_Unit[12].VarGroup[0].Var[0].Value = g_Unit[11].VarGroup[0].Var[0].Value;
      g_Unit[13].VarGroup[0].Var[0].Value = g_Unit[11].VarGroup[0].Var[0].Value;
      g_Unit[14].VarGroup[0].Var[0].Value = g_Unit[11].VarGroup[0].Var[0].Value;
      g_Unit[15].VarGroup[0].Var[0].Value = g_Unit[11].VarGroup[0].Var[0].Value;
      g_Unit[16].VarGroup[0].Var[0].Value = g_Unit[11].VarGroup[0].Var[0].Value;

      g_Unit[13].VarGroup[1].Var[17].Value = 1;
      g_Unit[14].VarGroup[1].Var[17].Value = 1;
      g_Unit[15].VarGroup[1].Var[17].Value = 1;
      g_Unit[16].VarGroup[1].Var[17].Value = 1;
    }

    if((g_DataExchangeListOther[26].PowerState==TRUE)&&(g_Unit[14].VarGroup[1].Var[8].Value!=0))//2114纵向隔离开关
    {
      g_Unit[14].VarGroup[1].Var[17].Value = 1;
      g_Unit[16].VarGroup[1].Var[17].Value = 1;
    }

    if((g_DataExchangeListOther[26].PowerState==TRUE)&&
       (g_Unit[14].VarGroup[1].Var[0].Value!=0)&&(g_Unit[14].VarGroup[1].Var[5].Value!=0)&&(g_Unit[14].VarGroup[1].Var[6].Value!=0)&&//212
       (g_Unit[16].VarGroup[1].Var[0].Value!=0)&&(g_Unit[16].VarGroup[1].Var[5].Value!=0)&&(g_Unit[16].VarGroup[1].Var[6].Value!=0)) //214
    {
      if((rand()%100)>50)
        g_Unit[11].VarGroup[0].Var[0].Value = 826;
      else
        g_Unit[11].VarGroup[0].Var[0].Value = 827;

      g_Unit[12].VarGroup[0].Var[0].Value = g_Unit[11].VarGroup[0].Var[0].Value;
      g_Unit[13].VarGroup[0].Var[0].Value = g_Unit[11].VarGroup[0].Var[0].Value;
      g_Unit[14].VarGroup[0].Var[0].Value = g_Unit[11].VarGroup[0].Var[0].Value;
      g_Unit[15].VarGroup[0].Var[0].Value = g_Unit[11].VarGroup[0].Var[0].Value;
      g_Unit[16].VarGroup[0].Var[0].Value = g_Unit[11].VarGroup[0].Var[0].Value;

      g_Unit[13].VarGroup[1].Var[17].Value = 1;
      g_Unit[14].VarGroup[1].Var[17].Value = 1;
      g_Unit[15].VarGroup[1].Var[17].Value = 1;
      g_Unit[16].VarGroup[1].Var[17].Value = 1;
    }
  }
}

void CPowerThread::ProcessPowerPTVotlage()
{
}

void CPowerThread::ProcessPowerNoloadCurrent()
{
  CPtrList FeedList;
  {
    POSITION pos = g_1B35FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_1B35FeedList.GetNext(pos);
      FeedList.AddTail(pUnit);
    }
  }
  {
    POSITION pos = g_2B35FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_2B35FeedList.GetNext(pos);
      FeedList.AddTail(pUnit);
    }
  }
  {
    POSITION pos = g_1B400FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_1B400FeedList.GetNext(pos);
      FeedList.AddTail(pUnit);
    }
  }
  {
    POSITION pos = g_2B400FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_2B400FeedList.GetNext(pos);
      FeedList.AddTail(pUnit);
    }
  }
  {
    POSITION pos = g_Up750FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_Up750FeedList.GetNext(pos);
      FeedList.AddTail(pUnit);
    }
  }
  {
    POSITION pos = g_Down750FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_Down750FeedList.GetNext(pos);
      FeedList.AddTail(pUnit);
    }
  }

  {
    POSITION pos = FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)FeedList.GetNext(pos);
      switch(pUnit->UnitType)
      {
        case 1://中压线路保护
        case 8://低压线路保护
        {
          float Ia = g_Current.NoloadCurrentChangeMaxValue*(float)(float)(rand()%100)/(float)100;
          float Ib = g_Current.NoloadCurrentChangeMaxValue*(float)(float)(rand()%100)/(float)100;
          float Ic = g_Current.NoloadCurrentChangeMaxValue*(float)(float)(rand()%100)/(float)100;
          float Pa = pUnit->VarGroup[0].Var[0].Value*Ia*(float)cos((float)3.1415926*1/(float)180)*(float)0.01;
          float Pb = pUnit->VarGroup[0].Var[0].Value*Ib*(float)cos((float)3.1415926*1/(float)180)*(float)0.01;
          float Pc = pUnit->VarGroup[0].Var[0].Value*Ic*(float)cos((float)3.1415926*1/(float)180)*(float)0.01;
          float Qa = pUnit->VarGroup[0].Var[0].Value*Ia*(float)sin((float)3.1415926*1/(float)180)*(float)0.01;
          float Qb = pUnit->VarGroup[0].Var[0].Value*Ib*(float)sin((float)3.1415926*1/(float)180)*(float)0.01;
          float Qc = pUnit->VarGroup[0].Var[0].Value*Ic*(float)sin((float)3.1415926*1/(float)180)*(float)0.01;
          if(pUnit->VarGroup[1].Var[17].Value==0)//馈线无电
          {
            Ia = 0;
            Ib = 0;
            Ic = 0;
            Pa = 0;
            Pb = 0;
            Pc = 0;
            Qa = 0;
            Qb = 0;
            Qc = 0;
          }
          float P = Pa + Pb + Pc;
          float Q = Qa + Qb + Qc;

          pUnit->VarGroup[0].Var[7].Value = Ia;
          pUnit->VarGroup[0].Var[8].Value = Ib;
          pUnit->VarGroup[0].Var[9].Value = Ic;
          pUnit->VarGroup[0].Var[10].Value = 0;
          pUnit->VarGroup[0].Var[11].Value = P;
          pUnit->VarGroup[0].Var[12].Value = Q;
          pUnit->VarGroup[0].Var[13].Value = 1;
        
          break;
        }
        case 6://直流馈线保护
        {
          float I = g_Current.NoloadCurrentChangeMaxValue*(float)(float)(rand()%100)/(float)100;
          if(pUnit->VarGroup[1].Var[17].Value==0)//馈线无电
            I = 0;
          pUnit->VarGroup[0].Var[1].Value = I;
        
          break;
        }
      }
    }
  }
}

void CPowerThread::ProcessPowerCurrent()
{
  if(CTime::GetCurrentTime()<g_Current.BeginTime)
    return;

  ProcessPowerCurrent400V();
  ProcessPowerCurrent750V();
  ProcessPowerCurrent35kV();
}

void CPowerThread::ProcessPowerCurrent400V()
{
  WORD UnitID1THi = 7;
  WORD UnitID2THi = 8;
  WORD UnitID1TLo = 17;
  WORD UnitID2TLo = 18;
  WORD UnitIDBus = 20;

//-----------400V母线电压计算----------//
  float Ua1B = 0;
  float Ub1B = 0;
  float Uc1B = 0;
  float Uab1B = 0;
  float Ubc1B = 0;
  float Uca1B = 0;
  float Uo1B = 0;

  float Ua2B = 0;
  float Ub2B = 0;
  float Uc2B = 0;
  float Uab2B = 0;
  float Ubc2B = 0;
  float Uca2B = 0;
  float Uo2B = 0;

  Uab1B = g_Unit[UnitID1TLo].VarGroup[0].Var[0].Value;
  Ubc1B = g_Unit[UnitID1TLo].VarGroup[0].Var[1].Value;
  Uca1B = g_Unit[UnitID1TLo].VarGroup[0].Var[2].Value;
  Ua1B = g_Unit[UnitID1TLo].VarGroup[0].Var[3].Value;
  Ub1B = g_Unit[UnitID1TLo].VarGroup[0].Var[4].Value;
  Uc1B = g_Unit[UnitID1TLo].VarGroup[0].Var[5].Value;
  Uo1B = g_Unit[UnitID1TLo].VarGroup[0].Var[6].Value;

  Uab2B = g_Unit[UnitID2TLo].VarGroup[0].Var[0].Value;
  Ubc2B = g_Unit[UnitID2TLo].VarGroup[0].Var[1].Value;
  Uca2B = g_Unit[UnitID2TLo].VarGroup[0].Var[2].Value;
  Ua2B = g_Unit[UnitID2TLo].VarGroup[0].Var[3].Value;
  Ub2B = g_Unit[UnitID2TLo].VarGroup[0].Var[4].Value;
  Uc2B = g_Unit[UnitID2TLo].VarGroup[0].Var[5].Value;
  Uo2B = g_Unit[UnitID2TLo].VarGroup[0].Var[6].Value;
//-----------400线电压计算----------//

  float TotalIa1B = 0;
  float TotalIb1B = 0;
  float TotalIc1B = 0;
  float TotalP1B = 0;
  float TotalQ1B = 0;
  float TotalS1B = 0;
  float Totalcos1B = 1;

  float TotalIa2B = 0;
  float TotalIb2B = 0;
  float TotalIc2B = 0;
  float TotalP2B = 0;
  float TotalQ2B = 0;
  float TotalS2B = 0;
  float Totalcos2B = 1;

  float TotalIa = 0;
  float TotalIb = 0;
  float TotalIc = 0;
  float TotalP = 0;
  float TotalQ = 0;
  float TotalS = 0;
  float Totalcos = 1;

  float TotalIa_r1B = 0;
  float TotalIb_r1B = 0;
  float TotalIc_r1B = 0;
  float TotalIa_x1B = 0;
  float TotalIb_x1B = 0;
  float TotalIc_x1B = 0;

  float TotalIa_r2B = 0;
  float TotalIb_r2B = 0;
  float TotalIc_r2B = 0;
  float TotalIa_x2B = 0;
  float TotalIb_x2B = 0;
  float TotalIc_x2B = 0;

//-----------1T低压侧电流计算----------//
  {
    float TotalIa_r = 0;
    float TotalIb_r = 0;
    float TotalIc_r = 0;
    float TotalIa_x = 0;
    float TotalIb_x = 0;
    float TotalIc_x = 0;
    POSITION pos = g_1B400FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_1B400FeedList.GetNext(pos);
      if(pUnit->UnitID>0)
      {
        if(pUnit->VarGroup[0].Var[1].Value>g_Current.NoloadCurrentChangeMaxValue)
        {
          float cosI = pUnit->VarGroup[0].Var[13].Value;
          float sinI = (float)::sqrt(1-pUnit->VarGroup[0].Var[13].Value*pUnit->VarGroup[0].Var[13].Value);
          float Iar = pUnit->VarGroup[0].Var[7].Value*cosI;
          float Ibr = pUnit->VarGroup[0].Var[8].Value*cosI;
          float Icr = pUnit->VarGroup[0].Var[9].Value*cosI;
          float Iax = pUnit->VarGroup[0].Var[7].Value*sinI;
          float Ibx = pUnit->VarGroup[0].Var[8].Value*sinI;
          float Icx = pUnit->VarGroup[0].Var[9].Value*sinI;
          TotalIa_r += Iar;
          TotalIb_r += Ibr;
          TotalIc_r += Icr;
          TotalIa_x += Iax;
          TotalIb_x += Ibx;
          TotalIc_x += Icx;

          TotalIa_r1B += Iar;
          TotalIb_r1B += Ibr;
          TotalIc_r1B += Icr;
          TotalIa_x1B += Iax;
          TotalIb_x1B += Ibx;
          TotalIc_x1B += Icx;
        }
        TotalP1B += pUnit->VarGroup[0].Var[11].Value;
        TotalQ1B += pUnit->VarGroup[0].Var[12].Value;
      }
    }

    g_TotalIa1B = TotalIa1B = (float)::sqrt(TotalIa_r*TotalIa_r+TotalIa_x*TotalIa_x);
    g_TotalIb1B = TotalIb1B = (float)::sqrt(TotalIb_r*TotalIb_r+TotalIb_x*TotalIb_x);
    g_TotalIc1B = TotalIc1B = (float)::sqrt(TotalIc_r*TotalIc_r+TotalIc_x*TotalIc_x);
    TotalS1B = (float)::sqrt(TotalP1B*TotalP1B+TotalQ1B*TotalQ1B);
    if((TotalP1B>0)&&(TotalS1B>0))
      Totalcos1B = TotalP1B / TotalS1B;
    if(Totalcos1B<0.1)
      Totalcos1B = 1;
    if(Totalcos1B>=1)
      Totalcos1B = 1;
  }
//-----------1T低压侧电流计算----------//
//-----------2T低压侧电流计算----------//
  {
    float TotalIa_r = 0;
    float TotalIb_r = 0;
    float TotalIc_r = 0;
    float TotalIa_x = 0;
    float TotalIb_x = 0;
    float TotalIc_x = 0;
    POSITION pos = g_2B400FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_2B400FeedList.GetNext(pos);
      if(pUnit->UnitID>0)
      {
        if(pUnit->VarGroup[0].Var[1].Value>g_Current.NoloadCurrentChangeMaxValue)
        {
          float cosI = pUnit->VarGroup[0].Var[13].Value;
          float sinI = (float)::sqrt(1-pUnit->VarGroup[0].Var[13].Value*pUnit->VarGroup[0].Var[13].Value);
          float Iar = pUnit->VarGroup[0].Var[7].Value*cosI;
          float Ibr = pUnit->VarGroup[0].Var[8].Value*cosI;
          float Icr = pUnit->VarGroup[0].Var[9].Value*cosI;
          float Iax = pUnit->VarGroup[0].Var[7].Value*sinI;
          float Ibx = pUnit->VarGroup[0].Var[8].Value*sinI;
          float Icx = pUnit->VarGroup[0].Var[9].Value*sinI;
          TotalIa_r += Iar;
          TotalIb_r += Ibr;
          TotalIc_r += Icr;
          TotalIa_x += Iax;
          TotalIb_x += Ibx;
          TotalIc_x += Icx;

          TotalIa_r2B += Iar;
          TotalIb_r2B += Ibr;
          TotalIc_r2B += Icr;
          TotalIa_x2B += Iax;
          TotalIb_x2B += Ibx;
          TotalIc_x2B += Icx;
        }
        TotalP2B += pUnit->VarGroup[0].Var[11].Value;
        TotalQ2B += pUnit->VarGroup[0].Var[12].Value;
      }
    }

    g_TotalIa2B = TotalIa2B = (float)::sqrt(TotalIa_r*TotalIa_r+TotalIa_x*TotalIa_x);
    g_TotalIb2B = TotalIb2B = (float)::sqrt(TotalIb_r*TotalIb_r+TotalIb_x*TotalIb_x);
    g_TotalIc2B = TotalIc2B = (float)::sqrt(TotalIc_r*TotalIc_r+TotalIc_x*TotalIc_x);
    TotalS2B = (float)::sqrt(TotalP2B*TotalP2B+TotalQ2B*TotalQ2B);
    if((TotalP2B>0)&&(TotalS2B>0))
      Totalcos2B = TotalP2B / TotalS2B;
    if(Totalcos2B<0.1)
      Totalcos2B = 1;
    if(Totalcos2B>=1)
      Totalcos2B = 1;
  }
//-----------2T低压侧电流计算----------//

//-----------高压侧电流计算----------//
  TotalIa = (float)::sqrt((TotalIa_r1B+TotalIa_r2B)*(TotalIa_r1B+TotalIa_r2B)+(TotalIa_x1B+TotalIa_x2B)*(TotalIa_x1B+TotalIa_x2B));
  TotalIb = (float)::sqrt((TotalIb_r1B+TotalIb_r2B)*(TotalIb_r1B+TotalIb_r2B)+(TotalIb_x1B+TotalIb_x2B)*(TotalIb_x1B+TotalIb_x2B));
  TotalIc = (float)::sqrt((TotalIc_r1B+TotalIc_r2B)*(TotalIc_r1B+TotalIc_r2B)+(TotalIc_x1B+TotalIc_x2B)*(TotalIc_x1B+TotalIc_x2B));
  TotalP = TotalP1B + TotalP2B;
  TotalQ = TotalQ1B + TotalQ2B;
  TotalS = (float)::sqrt(TotalP*TotalP+TotalQ*TotalQ);
  if((TotalP>0)&&(TotalS>0))
    Totalcos = TotalP / TotalS;
  else
    Totalcos = 1;
  if(Totalcos<0.1)
    Totalcos = 1;
  if(Totalcos>=1)
    Totalcos = 1;
//-----------高后备电流计算----------//

  if((g_Power1T400==TRUE)&&(g_Power2T400==TRUE)&&(g_PowerBus400==FALSE))//分列运行
  {
    g_Unit[UnitID1THi].VarGroup[0].Var[7].Value = TotalIa1B / (float)35 * (float)0.4;
    g_Unit[UnitID1THi].VarGroup[0].Var[8].Value = TotalIb1B / (float)35 * (float)0.4;
    g_Unit[UnitID1THi].VarGroup[0].Var[9].Value = TotalIc1B / (float)35 * (float)0.4;
    g_Unit[UnitID1THi].VarGroup[0].Var[11].Value = TotalP1B;
    g_Unit[UnitID1THi].VarGroup[0].Var[12].Value = TotalQ1B;
    g_Unit[UnitID1THi].VarGroup[0].Var[13].Value = Totalcos1B;
    g_Unit[UnitID1THi].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);

    g_Unit[UnitID2THi].VarGroup[0].Var[7].Value = TotalIa2B / (float)35 * (float)0.4;
    g_Unit[UnitID2THi].VarGroup[0].Var[8].Value = TotalIb2B / (float)35 * (float)0.4;
    g_Unit[UnitID2THi].VarGroup[0].Var[9].Value = TotalIc2B / (float)35 * (float)0.4;
    g_Unit[UnitID2THi].VarGroup[0].Var[11].Value = TotalP2B;
    g_Unit[UnitID2THi].VarGroup[0].Var[12].Value = TotalQ2B;
    g_Unit[UnitID2THi].VarGroup[0].Var[13].Value = Totalcos2B;
    g_Unit[UnitID2THi].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);
    
    g_Unit[UnitIDBus].VarGroup[0].Var[7].Value = 0;
    g_Unit[UnitIDBus].VarGroup[0].Var[8].Value = 0;
    g_Unit[UnitIDBus].VarGroup[0].Var[9].Value = 0;

    g_Unit[UnitID1TLo].VarGroup[0].Var[7].Value = TotalIa1B;
    g_Unit[UnitID1TLo].VarGroup[0].Var[8].Value = TotalIb1B;
    g_Unit[UnitID1TLo].VarGroup[0].Var[9].Value = TotalIc1B;
    g_Unit[UnitID1TLo].VarGroup[0].Var[11].Value = TotalP1B;
    g_Unit[UnitID1TLo].VarGroup[0].Var[12].Value = TotalQ1B;
    g_Unit[UnitID1TLo].VarGroup[0].Var[13].Value = Totalcos1B;
    g_Unit[UnitID1TLo].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);

    g_Unit[UnitID2TLo].VarGroup[0].Var[7].Value = TotalIa2B;
    g_Unit[UnitID2TLo].VarGroup[0].Var[8].Value = TotalIb2B;
    g_Unit[UnitID2TLo].VarGroup[0].Var[9].Value = TotalIc2B;
    g_Unit[UnitID2TLo].VarGroup[0].Var[11].Value = TotalP2B;
    g_Unit[UnitID2TLo].VarGroup[0].Var[12].Value = TotalQ2B;
    g_Unit[UnitID2TLo].VarGroup[0].Var[13].Value = Totalcos2B;
    g_Unit[UnitID2TLo].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);
  }
  if((g_Power1T400==TRUE)&&(g_Power2T400==FALSE)&&(g_PowerBus400==TRUE))//1T运行
  {
    g_Unit[UnitID1THi].VarGroup[0].Var[7].Value = TotalIa / (float)35 * (float)0.4;
    g_Unit[UnitID1THi].VarGroup[0].Var[8].Value = TotalIb / (float)35 * (float)0.4;
    g_Unit[UnitID1THi].VarGroup[0].Var[9].Value = TotalIc / (float)35 * (float)0.4;
    g_Unit[UnitID1THi].VarGroup[0].Var[11].Value = TotalP;
    g_Unit[UnitID1THi].VarGroup[0].Var[12].Value = TotalQ;
    g_Unit[UnitID1THi].VarGroup[0].Var[13].Value = Totalcos;
    g_Unit[UnitID1THi].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);

    g_Unit[UnitID1TLo].VarGroup[0].Var[7].Value = TotalIa1B;
    g_Unit[UnitID1TLo].VarGroup[0].Var[8].Value = TotalIb1B;
    g_Unit[UnitID1TLo].VarGroup[0].Var[9].Value = TotalIc1B;
    g_Unit[UnitID1TLo].VarGroup[0].Var[11].Value = TotalP1B;
    g_Unit[UnitID1TLo].VarGroup[0].Var[12].Value = TotalQ1B;
    g_Unit[UnitID1TLo].VarGroup[0].Var[13].Value = Totalcos1B;
    g_Unit[UnitID1TLo].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);

    g_Unit[UnitIDBus].VarGroup[0].Var[7].Value = g_TotalIa2B;
    g_Unit[UnitIDBus].VarGroup[0].Var[8].Value = g_TotalIb2B;
    g_Unit[UnitIDBus].VarGroup[0].Var[9].Value = g_TotalIc2B;

    g_Unit[UnitID2THi].VarGroup[0].Var[7].Value = 0;
    g_Unit[UnitID2THi].VarGroup[0].Var[8].Value = 0;
    g_Unit[UnitID2THi].VarGroup[0].Var[9].Value = 0;
    g_Unit[UnitID2THi].VarGroup[0].Var[11].Value = 0;
    g_Unit[UnitID2THi].VarGroup[0].Var[12].Value = 0;
    g_Unit[UnitID2THi].VarGroup[0].Var[13].Value = 0;
    g_Unit[UnitID2THi].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);

    g_Unit[UnitID2TLo].VarGroup[0].Var[7].Value = 0;
    g_Unit[UnitID2TLo].VarGroup[0].Var[8].Value = 0;
    g_Unit[UnitID2TLo].VarGroup[0].Var[9].Value = 0;
    g_Unit[UnitID2TLo].VarGroup[0].Var[11].Value = 0;
    g_Unit[UnitID2TLo].VarGroup[0].Var[12].Value = 0;
    g_Unit[UnitID2TLo].VarGroup[0].Var[13].Value = 0;
    g_Unit[UnitID2TLo].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);
  }
  if((g_Power1T400==FALSE)&&(g_Power2T400==TRUE)&&(g_PowerBus400==TRUE))//2T运行
  {
    g_Unit[UnitID1THi].VarGroup[0].Var[7].Value = 0;
    g_Unit[UnitID1THi].VarGroup[0].Var[8].Value = 0;
    g_Unit[UnitID1THi].VarGroup[0].Var[9].Value = 0;
    g_Unit[UnitID1THi].VarGroup[0].Var[11].Value = 0;
    g_Unit[UnitID1THi].VarGroup[0].Var[12].Value = 0;
    g_Unit[UnitID1THi].VarGroup[0].Var[13].Value = 0;
    g_Unit[UnitID1THi].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);

    g_Unit[UnitID1TLo].VarGroup[0].Var[7].Value = 0;
    g_Unit[UnitID1TLo].VarGroup[0].Var[8].Value = 0;
    g_Unit[UnitID1TLo].VarGroup[0].Var[9].Value = 0;
    g_Unit[UnitID1TLo].VarGroup[0].Var[11].Value = 0;
    g_Unit[UnitID1TLo].VarGroup[0].Var[12].Value = 0;
    g_Unit[UnitID1TLo].VarGroup[0].Var[13].Value = 0;
    g_Unit[UnitID1TLo].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);

    g_Unit[UnitIDBus].VarGroup[0].Var[7].Value = g_TotalIa1B;
    g_Unit[UnitIDBus].VarGroup[0].Var[8].Value = g_TotalIb1B;
    g_Unit[UnitIDBus].VarGroup[0].Var[9].Value = g_TotalIc1B;

    g_Unit[UnitID2THi].VarGroup[0].Var[7].Value = TotalIa / (float)35 * (float)0.4;
    g_Unit[UnitID2THi].VarGroup[0].Var[8].Value = TotalIb / (float)35 * (float)0.4;
    g_Unit[UnitID2THi].VarGroup[0].Var[9].Value = TotalIc / (float)35 * (float)0.4;
    g_Unit[UnitID2THi].VarGroup[0].Var[11].Value = TotalP;
    g_Unit[UnitID2THi].VarGroup[0].Var[12].Value = TotalQ;
    g_Unit[UnitID2THi].VarGroup[0].Var[13].Value = Totalcos;
    g_Unit[UnitID2THi].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);

    g_Unit[UnitID2TLo].VarGroup[0].Var[7].Value = TotalIa1B;
    g_Unit[UnitID2TLo].VarGroup[0].Var[8].Value = TotalIb1B;
    g_Unit[UnitID2TLo].VarGroup[0].Var[9].Value = TotalIc1B;
    g_Unit[UnitID2TLo].VarGroup[0].Var[11].Value = TotalP1B;
    g_Unit[UnitID2TLo].VarGroup[0].Var[12].Value = TotalQ1B;
    g_Unit[UnitID2TLo].VarGroup[0].Var[13].Value = Totalcos1B;
    g_Unit[UnitID2TLo].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);
  }
}

void CPowerThread::ProcessPowerCurrent750V()
{
  WORD UnitID1THi = 9;
  WORD UnitID2THi = 10;
  WORD UnitID1TLo = 11;
  WORD UnitID2TLo = 12;

//-----------750V母线电压计算----------//
  float Ua1B = 0;
  float Ub1B = 0;
  float Uc1B = 0;
  float Uab1B = 0;
  float Ubc1B = 0;
  float Uca1B = 0;
  float Uo1B = 0;

  float Ua2B = 0;
  float Ub2B = 0;
  float Uc2B = 0;
  float Uab2B = 0;
  float Ubc2B = 0;
  float Uca2B = 0;
  float Uo2B = 0;

  float U1B = g_Unit[UnitID1TLo].VarGroup[0].Var[0].Value;
  float U2B = g_Unit[UnitID2TLo].VarGroup[0].Var[0].Value;
//-----------750V母线电压计算----------//

  float TotalI = 0;

//-----------750V侧电流计算----------//
  {
    CPtrList FeedList;
    FeedList.AddTail(&g_Unit[13]);
    FeedList.AddTail(&g_Unit[14]);
    FeedList.AddTail(&g_Unit[15]);
    FeedList.AddTail(&g_Unit[16]);

    POSITION pos = FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)FeedList.GetNext(pos);
      if(pUnit->UnitID>0)
      {
        if(pUnit->VarGroup[0].Var[1].Value>g_Current.NoloadCurrentChangeMaxValue)
          TotalI += pUnit->VarGroup[0].Var[1].Value;
      }
    }
  }
//-----------750V侧电流计算----------//

//-----------高压侧电流计算----------//
  if((g_Power1T750==TRUE)&&(g_Power2T750==TRUE))//同时运行
  {
    float I1L = TotalI / (float)::sqrt((float)6) / 2;
    float I1H = I1L / (float)59.32;
    float I2L  = TotalI / (float)::sqrt((float)6) / 2;
    float I2H = I2L / (float)59.32;
    float P1 = U1B*I1H;
    float P2 = U2B*I2H;
    float Q = 0;
    float cos = 1;
    g_Unit[UnitID1THi].VarGroup[0].Var[7].Value = I1H;
    g_Unit[UnitID1THi].VarGroup[0].Var[8].Value = I1H;
    g_Unit[UnitID1THi].VarGroup[0].Var[9].Value = I1H;
    g_Unit[UnitID1THi].VarGroup[0].Var[11].Value = P1;
    g_Unit[UnitID1THi].VarGroup[0].Var[12].Value = Q;
    g_Unit[UnitID1THi].VarGroup[0].Var[13].Value = cos;
    g_Unit[UnitID1THi].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);

    g_Unit[UnitID2THi].VarGroup[0].Var[7].Value = I2H;
    g_Unit[UnitID2THi].VarGroup[0].Var[8].Value = I2H;
    g_Unit[UnitID2THi].VarGroup[0].Var[9].Value = I2H;
    g_Unit[UnitID2THi].VarGroup[0].Var[11].Value = P2;
    g_Unit[UnitID2THi].VarGroup[0].Var[12].Value = Q;
    g_Unit[UnitID2THi].VarGroup[0].Var[13].Value = cos;
    g_Unit[UnitID2THi].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);
    
    g_Unit[UnitID1TLo].VarGroup[0].Var[1].Value = TotalI / 2;
    g_Unit[UnitID2TLo].VarGroup[0].Var[1].Value = TotalI / 2;
  }
  if((g_Power1T750==TRUE)&&(g_Power2T750==FALSE))//1T运行
  {
    float IL = TotalI / (float)::sqrt((float)6);
    float IH = IL / (float)59.32;
    float P = U1B*IH;
    float Q = 0;
    float cos = 1;
    g_Unit[UnitID1THi].VarGroup[0].Var[7].Value = IH;
    g_Unit[UnitID1THi].VarGroup[0].Var[8].Value = IH;
    g_Unit[UnitID1THi].VarGroup[0].Var[9].Value = IH;
    g_Unit[UnitID1THi].VarGroup[0].Var[11].Value = P;
    g_Unit[UnitID1THi].VarGroup[0].Var[12].Value = Q;
    g_Unit[UnitID1THi].VarGroup[0].Var[13].Value = cos;
    g_Unit[UnitID1THi].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);

    g_Unit[UnitID2THi].VarGroup[0].Var[7].Value = 0;
    g_Unit[UnitID2THi].VarGroup[0].Var[8].Value = 0;
    g_Unit[UnitID2THi].VarGroup[0].Var[9].Value = 0;
    g_Unit[UnitID2THi].VarGroup[0].Var[11].Value = 0;
    g_Unit[UnitID2THi].VarGroup[0].Var[12].Value = 0;
    g_Unit[UnitID2THi].VarGroup[0].Var[13].Value = cos;
    g_Unit[UnitID2THi].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);
    
    g_Unit[UnitID1TLo].VarGroup[0].Var[1].Value = TotalI;
    g_Unit[UnitID2TLo].VarGroup[0].Var[1].Value = 0;
  }
  if((g_Power1T750==FALSE)&&(g_Power2T750==TRUE))//2T运行
  {
    float IL = TotalI / (float)::sqrt((float)6);
    float IH = IL / (float)59.32;
    float P = U1B*IH;
    float Q = 0;
    float cos = 1;

    g_Unit[UnitID1THi].VarGroup[0].Var[7].Value = 0;
    g_Unit[UnitID1THi].VarGroup[0].Var[8].Value = 0;
    g_Unit[UnitID1THi].VarGroup[0].Var[9].Value = 0;
    g_Unit[UnitID1THi].VarGroup[0].Var[11].Value = 0;
    g_Unit[UnitID1THi].VarGroup[0].Var[12].Value = 0;
    g_Unit[UnitID1THi].VarGroup[0].Var[13].Value = cos;
    g_Unit[UnitID1THi].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);
    
    g_Unit[UnitID2THi].VarGroup[0].Var[7].Value = IH;
    g_Unit[UnitID2THi].VarGroup[0].Var[8].Value = IH;
    g_Unit[UnitID2THi].VarGroup[0].Var[9].Value = IH;
    g_Unit[UnitID2THi].VarGroup[0].Var[11].Value = P;
    g_Unit[UnitID2THi].VarGroup[0].Var[12].Value = Q;
    g_Unit[UnitID2THi].VarGroup[0].Var[13].Value = cos;
    g_Unit[UnitID2THi].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);

    g_Unit[UnitID1TLo].VarGroup[0].Var[1].Value = 0;
    g_Unit[UnitID2TLo].VarGroup[0].Var[1].Value = TotalI;
  }
}

void CPowerThread::ProcessPowerCurrent35kV()
{
  WORD UnitID1BIn = 1;
  WORD UnitID2BIn = 2;
  WORD UnitID1BOut = 3;
  WORD UnitID2BOut = 4;
  WORD UnitIDBus = 5;
  WORD UnitID1BOut1 = 7;
  WORD UnitID2BOut1 = 8;
  WORD UnitID2BOut2 = 9;
  WORD UnitID2BOut3 = 10;

//-----------35kV母线电压计算----------//
  float Ua1B = 0;
  float Ub1B = 0;
  float Uc1B = 0;
  float Uab1B = 0;
  float Ubc1B = 0;
  float Uca1B = 0;
  float Uo1B = 0;

  float Ua2B = 0;
  float Ub2B = 0;
  float Uc2B = 0;
  float Uab2B = 0;
  float Ubc2B = 0;
  float Uca2B = 0;
  float Uo2B = 0;

  Uab1B = g_Unit[UnitID1BIn].VarGroup[0].Var[0].Value;
  Ubc1B = g_Unit[UnitID1BIn].VarGroup[0].Var[1].Value;
  Uca1B = g_Unit[UnitID1BIn].VarGroup[0].Var[2].Value;
  Ua1B = g_Unit[UnitID1BIn].VarGroup[0].Var[3].Value;
  Ub1B = g_Unit[UnitID1BIn].VarGroup[0].Var[4].Value;
  Uc1B = g_Unit[UnitID1BIn].VarGroup[0].Var[5].Value;
  Uo1B = g_Unit[UnitID1BIn].VarGroup[0].Var[6].Value;

  Uab2B = g_Unit[UnitID2BIn].VarGroup[0].Var[0].Value;
  Ubc2B = g_Unit[UnitID2BIn].VarGroup[0].Var[1].Value;
  Uca2B = g_Unit[UnitID2BIn].VarGroup[0].Var[2].Value;
  Ua2B = g_Unit[UnitID2BIn].VarGroup[0].Var[3].Value;
  Ub2B = g_Unit[UnitID2BIn].VarGroup[0].Var[4].Value;
  Uc2B = g_Unit[UnitID2BIn].VarGroup[0].Var[5].Value;
  Uo2B = g_Unit[UnitID2BIn].VarGroup[0].Var[6].Value;
//-----------35kV母线电压计算----------//

  float TotalIa1B = 0;
  float TotalIb1B = 0;
  float TotalIc1B = 0;
  float TotalP1B = 0;
  float TotalQ1B = 0;
  float TotalS1B = 0;
  float Totalcos1B = 1;

  float TotalIa2B = 0;
  float TotalIb2B = 0;
  float TotalIc2B = 0;
  float TotalP2B = 0;
  float TotalQ2B = 0;
  float TotalS2B = 0;
  float Totalcos2B = 1;

  float TotalIa = 0;
  float TotalIb = 0;
  float TotalIc = 0;
  float TotalP = 0;
  float TotalQ = 0;
  float TotalS = 0;
  float Totalcos = 1;

  float TotalIa_r1B = 0;
  float TotalIb_r1B = 0;
  float TotalIc_r1B = 0;
  float TotalIa_x1B = 0;
  float TotalIb_x1B = 0;
  float TotalIc_x1B = 0;

  float TotalIa_r2B = 0;
  float TotalIb_r2B = 0;
  float TotalIc_r2B = 0;
  float TotalIa_x2B = 0;
  float TotalIb_x2B = 0;
  float TotalIc_x2B = 0;

//-----------I段出线电流计算----------//
  {
    float TotalIa_r = 0;
    float TotalIb_r = 0;
    float TotalIc_r = 0;
    float TotalIa_x = 0;
    float TotalIb_x = 0;
    float TotalIc_x = 0;
    
    CPtrList FeedList;
    FeedList.AddTail(&g_Unit[UnitID1BOut1]);

    POSITION pos = FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)FeedList.GetNext(pos);
      if(pUnit->UnitID>0)
      {
        if(pUnit->VarGroup[0].Var[1].Value>g_Current.NoloadCurrentChangeMaxValue)
        {
          float cosI = pUnit->VarGroup[0].Var[13].Value;
          float sinI = (float)::sqrt(1-pUnit->VarGroup[0].Var[13].Value*pUnit->VarGroup[0].Var[13].Value);
          float Iar = pUnit->VarGroup[0].Var[7].Value*cosI;
          float Ibr = pUnit->VarGroup[0].Var[8].Value*cosI;
          float Icr = pUnit->VarGroup[0].Var[9].Value*cosI;
          float Iax = pUnit->VarGroup[0].Var[7].Value*sinI;
          float Ibx = pUnit->VarGroup[0].Var[8].Value*sinI;
          float Icx = pUnit->VarGroup[0].Var[9].Value*sinI;
          TotalIa_r += Iar;
          TotalIb_r += Ibr;
          TotalIc_r += Icr;
          TotalIa_x += Iax;
          TotalIb_x += Ibx;
          TotalIc_x += Icx;

          TotalIa_r1B += Iar;
          TotalIb_r1B += Ibr;
          TotalIc_r1B += Icr;
          TotalIa_x1B += Iax;
          TotalIb_x1B += Ibx;
          TotalIc_x1B += Icx;
        }
        TotalP1B += pUnit->VarGroup[0].Var[11].Value;
        TotalQ1B += pUnit->VarGroup[0].Var[12].Value;
      }
    }

    g_TotalIa1B = TotalIa1B = (float)::sqrt(TotalIa_r*TotalIa_r+TotalIa_x*TotalIa_x);
    g_TotalIb1B = TotalIb1B = (float)::sqrt(TotalIb_r*TotalIb_r+TotalIb_x*TotalIb_x);
    g_TotalIc1B = TotalIc1B = (float)::sqrt(TotalIc_r*TotalIc_r+TotalIc_x*TotalIc_x);
    TotalS1B = (float)::sqrt(TotalP1B*TotalP1B+TotalQ1B*TotalQ1B);
    if((TotalP1B>0)&&(TotalS1B>0))
      Totalcos1B = TotalP1B / TotalS1B;
    if(Totalcos1B<0.1)
      Totalcos1B = 1;
    if(Totalcos1B>=1)
      Totalcos1B = 1;
  }
//-----------I段出线电流计算----------//
//-----------II段出线电流计算----------//
  {
    float TotalIa_r = 0;
    float TotalIb_r = 0;
    float TotalIc_r = 0;
    float TotalIa_x = 0;
    float TotalIb_x = 0;
    float TotalIc_x = 0;
    CPtrList FeedList;
    FeedList.AddTail(&g_Unit[UnitID2BOut1]);
    FeedList.AddTail(&g_Unit[UnitID2BOut2]);
    FeedList.AddTail(&g_Unit[UnitID2BOut3]);

    POSITION pos = FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)FeedList.GetNext(pos);
      if(pUnit->UnitID>0)
      {
        if(pUnit->VarGroup[0].Var[1].Value>g_Current.NoloadCurrentChangeMaxValue)
        {
          float cosI = pUnit->VarGroup[0].Var[13].Value;
          float sinI = (float)::sqrt(1-pUnit->VarGroup[0].Var[13].Value*pUnit->VarGroup[0].Var[13].Value);
          float Iar = pUnit->VarGroup[0].Var[7].Value*cosI;
          float Ibr = pUnit->VarGroup[0].Var[8].Value*cosI;
          float Icr = pUnit->VarGroup[0].Var[9].Value*cosI;
          float Iax = pUnit->VarGroup[0].Var[7].Value*sinI;
          float Ibx = pUnit->VarGroup[0].Var[8].Value*sinI;
          float Icx = pUnit->VarGroup[0].Var[9].Value*sinI;
          TotalIa_r += Iar;
          TotalIb_r += Ibr;
          TotalIc_r += Icr;
          TotalIa_x += Iax;
          TotalIb_x += Ibx;
          TotalIc_x += Icx;

          TotalIa_r2B += Iar;
          TotalIb_r2B += Ibr;
          TotalIc_r2B += Icr;
          TotalIa_x2B += Iax;
          TotalIb_x2B += Ibx;
          TotalIc_x2B += Icx;
        }
        TotalP2B += pUnit->VarGroup[0].Var[11].Value;
        TotalQ2B += pUnit->VarGroup[0].Var[12].Value;
      }
    }

    g_TotalIa2B = TotalIa2B = (float)::sqrt(TotalIa_r*TotalIa_r+TotalIa_x*TotalIa_x);
    g_TotalIb2B = TotalIb2B = (float)::sqrt(TotalIb_r*TotalIb_r+TotalIb_x*TotalIb_x);
    g_TotalIc2B = TotalIc2B = (float)::sqrt(TotalIc_r*TotalIc_r+TotalIc_x*TotalIc_x);
    TotalS2B = (float)::sqrt(TotalP2B*TotalP2B+TotalQ2B*TotalQ2B);
    if((TotalP2B>0)&&(TotalS2B>0))
      Totalcos2B = TotalP2B / TotalS2B;
    if(Totalcos2B<0.1)
      Totalcos2B = 1;
    if(Totalcos2B>=1)
      Totalcos2B = 1;
  }
//-----------II段出线电流计算----------//

//-----------进线电流计算----------//
  TotalIa = (float)::sqrt((TotalIa_r1B+TotalIa_r2B)*(TotalIa_r1B+TotalIa_r2B)+(TotalIa_x1B+TotalIa_x2B)*(TotalIa_x1B+TotalIa_x2B));
  TotalIb = (float)::sqrt((TotalIb_r1B+TotalIb_r2B)*(TotalIb_r1B+TotalIb_r2B)+(TotalIb_x1B+TotalIb_x2B)*(TotalIb_x1B+TotalIb_x2B));
  TotalIc = (float)::sqrt((TotalIc_r1B+TotalIc_r2B)*(TotalIc_r1B+TotalIc_r2B)+(TotalIc_x1B+TotalIc_x2B)*(TotalIc_x1B+TotalIc_x2B));
  TotalP = TotalP1B + TotalP2B;
  TotalQ = TotalQ1B + TotalQ2B;
  TotalS = (float)::sqrt(TotalP*TotalP+TotalQ*TotalQ);
  if((TotalP>0)&&(TotalS>0))
    Totalcos = TotalP / TotalS;
  else
    Totalcos = 1;
  if(Totalcos<0.1)
    Totalcos = 1;
  if(Totalcos>=1)
    Totalcos = 1;
//-----------进线电流计算----------//

  if((g_Power1T35==TRUE)&&(g_Power2T35==TRUE)&&(g_PowerBus35==FALSE))//分列运行
  {
    g_Unit[UnitID1BIn].VarGroup[0].Var[7].Value = TotalIa1B;
    g_Unit[UnitID1BIn].VarGroup[0].Var[8].Value = TotalIb1B;
    g_Unit[UnitID1BIn].VarGroup[0].Var[9].Value = TotalIc1B;
    g_Unit[UnitID1BIn].VarGroup[0].Var[11].Value = TotalP1B;
    g_Unit[UnitID1BIn].VarGroup[0].Var[12].Value = TotalQ1B;
    g_Unit[UnitID1BIn].VarGroup[0].Var[13].Value = Totalcos1B;
    g_Unit[UnitID1BIn].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);

    g_Unit[UnitID2BIn].VarGroup[0].Var[7].Value = TotalIa2B;
    g_Unit[UnitID2BIn].VarGroup[0].Var[8].Value = TotalIb2B;
    g_Unit[UnitID2BIn].VarGroup[0].Var[9].Value = TotalIc2B;
    g_Unit[UnitID2BIn].VarGroup[0].Var[11].Value = TotalP2B;
    g_Unit[UnitID2BIn].VarGroup[0].Var[12].Value = TotalQ2B;
    g_Unit[UnitID2BIn].VarGroup[0].Var[13].Value = Totalcos2B;
    g_Unit[UnitID2BIn].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);
    
    g_Unit[UnitIDBus].VarGroup[0].Var[7].Value = 0;
    g_Unit[UnitIDBus].VarGroup[0].Var[8].Value = 0;
    g_Unit[UnitIDBus].VarGroup[0].Var[9].Value = 0;
  }
  if((g_Power1T35==TRUE)&&(g_Power2T35==FALSE)&&(g_PowerBus35==TRUE))//1L并列运行
  {
    g_Unit[UnitID1BIn].VarGroup[0].Var[7].Value = TotalIa;
    g_Unit[UnitID1BIn].VarGroup[0].Var[8].Value = TotalIb;
    g_Unit[UnitID1BIn].VarGroup[0].Var[9].Value = TotalIc;
    g_Unit[UnitID1BIn].VarGroup[0].Var[11].Value = TotalP;
    g_Unit[UnitID1BIn].VarGroup[0].Var[12].Value = TotalQ;
    g_Unit[UnitID1BIn].VarGroup[0].Var[13].Value = Totalcos;
    g_Unit[UnitID1BIn].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);

    g_Unit[UnitIDBus].VarGroup[0].Var[7].Value = g_TotalIa2B;
    g_Unit[UnitIDBus].VarGroup[0].Var[8].Value = g_TotalIb2B;
    g_Unit[UnitIDBus].VarGroup[0].Var[9].Value = g_TotalIc2B;

    g_Unit[UnitID2BIn].VarGroup[0].Var[7].Value = 0;
    g_Unit[UnitID2BIn].VarGroup[0].Var[8].Value = 0;
    g_Unit[UnitID2BIn].VarGroup[0].Var[9].Value = 0;
    g_Unit[UnitID2BIn].VarGroup[0].Var[11].Value = 0;
    g_Unit[UnitID2BIn].VarGroup[0].Var[12].Value = 0;
    g_Unit[UnitID2BIn].VarGroup[0].Var[13].Value = 0;
    g_Unit[UnitID2BIn].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);
  }
  if((g_Power1T35==FALSE)&&(g_Power2T35==TRUE)&&(g_PowerBus35==TRUE))//2L并列运行
  {
    g_Unit[UnitID1BIn].VarGroup[0].Var[7].Value = 0;
    g_Unit[UnitID1BIn].VarGroup[0].Var[8].Value = 0;
    g_Unit[UnitID1BIn].VarGroup[0].Var[9].Value = 0;
    g_Unit[UnitID1BIn].VarGroup[0].Var[11].Value = 0;
    g_Unit[UnitID1BIn].VarGroup[0].Var[12].Value = 0;
    g_Unit[UnitID1BIn].VarGroup[0].Var[13].Value = 0;
    g_Unit[UnitID1BIn].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);

    g_Unit[UnitIDBus].VarGroup[0].Var[7].Value = g_TotalIa1B;
    g_Unit[UnitIDBus].VarGroup[0].Var[8].Value = g_TotalIb1B;
    g_Unit[UnitIDBus].VarGroup[0].Var[9].Value = g_TotalIc1B;

    g_Unit[UnitID2BIn].VarGroup[0].Var[7].Value = TotalIa;
    g_Unit[UnitID2BIn].VarGroup[0].Var[8].Value = TotalIb;
    g_Unit[UnitID2BIn].VarGroup[0].Var[9].Value = TotalIc;
    g_Unit[UnitID2BIn].VarGroup[0].Var[11].Value = TotalP;
    g_Unit[UnitID2BIn].VarGroup[0].Var[12].Value = TotalQ;
    g_Unit[UnitID2BIn].VarGroup[0].Var[13].Value = Totalcos;
    g_Unit[UnitID2BIn].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);
  }
}

void CPowerThread::ProcessPowerFeedTransmissionAndFreeTrain()
{
  if((CTime::GetCurrentTime()-m_ProcessFeedTransmissionAndFreeTrainTime).GetTotalSeconds()<1)
    return;

  m_ProcessFeedTransmissionAndFreeTrainTime = CTime::GetCurrentTime();

  CSingleLock sLock(&g_PowerMutex);
  sLock.Lock();

  float FeedValue[UNITCOUNT][VARCOUNT];
  for(int unitno=0;unitno<UNITCOUNT;unitno++)
  {
    g_Unit[unitno].TransmissionState = FALSE;
    for(int varno=0;varno<VARCOUNT;varno++)
      FeedValue[unitno][varno] = 0;

    FeedValue[unitno][1] = g_Current.NoloadCurrentChangeMaxValue*(float)(float)(rand()%100)/(float)100;
  }
    
  CTime ct = CTime::GetCurrentTime();

  {
    for(int no=0;no<256;no++)
    {
      if((g_DataExchangeList[no].PowerState==TRUE)&&(g_DataExchangeList[no].TrainState==TRUE))
      {
        TagUnitStruct *pUnit = (TagUnitStruct *)&g_Unit[g_DataExchangeList[no].UnitID];

        pUnit->CurrentValue = g_DataExchangeList[no].AverCurrent;
        pUnit->AngleValue = g_DataExchangeList[no].AverAngle;

        switch(pUnit->UnitType)
        {
          case 1://中压线路保护
          case 8://低压线路保护
          {
            float Ia = pUnit->CurrentValue+(pUnit->CurrentValue*(float)(rand()%100)/(float)100*g_DataExchangeList[no].CurrentChangeRate);
            float Ib = pUnit->CurrentValue+(pUnit->CurrentValue*(float)(rand()%100)/(float)100*g_DataExchangeList[no].CurrentChangeRate);
            float Ic = pUnit->CurrentValue+(pUnit->CurrentValue*(float)(rand()%100)/(float)100*g_DataExchangeList[no].CurrentChangeRate);
            float Pa = pUnit->VarGroup[0].Var[3].Value*Ia*(float)cos((float)3.1415926*pUnit->AngleValue/(float)180)*3/1000;
            float Pb = pUnit->VarGroup[0].Var[4].Value*Ib*(float)cos((float)3.1415926*pUnit->AngleValue/(float)180)*3/1000;
            float Pc = pUnit->VarGroup[0].Var[5].Value*Ic*(float)cos((float)3.1415926*pUnit->AngleValue/(float)180)*3/1000;
            float Qa = pUnit->VarGroup[0].Var[3].Value*Ia*(float)sin((float)3.1415926*pUnit->AngleValue/(float)180)*3/1000;
            float Qb = pUnit->VarGroup[0].Var[4].Value*Ib*(float)sin((float)3.1415926*pUnit->AngleValue/(float)180)*3/1000;
            float Qc = pUnit->VarGroup[0].Var[5].Value*Ic*(float)sin((float)3.1415926*pUnit->AngleValue/(float)180)*3/1000;
            float P = Pa + Pb + Pc;
            float Q = Qa + Qb + Qc;
            float S = (float)::sqrt(P*P+Q*Q);
            float cos = 1;
            if((P>0)&&(S>0))
              cos = P / S;

            FeedValue[pUnit->UnitID][7] += Ia;
            FeedValue[pUnit->UnitID][8] += Ib;
            FeedValue[pUnit->UnitID][9] += Ic;
            FeedValue[pUnit->UnitID][11] += P;
            FeedValue[pUnit->UnitID][12] += Q;
            FeedValue[pUnit->UnitID][13]  = cos;
            FeedValue[pUnit->UnitID][14] = 50+((float)(rand()%100)/(float)100000);
            break;
          }
          case 6://直流馈线保护
          {
            float I = pUnit->CurrentValue+(pUnit->CurrentValue*(float)(rand()%100)/(float)100*g_DataExchangeList[no].CurrentChangeRate);
            FeedValue[pUnit->UnitID][1] += I;
            break;
          }
        }
      }
    }
  }
  {
    POSITION pos = g_FreeTransmissionList.GetHeadPosition();
    while(pos)
    {
      struct TagFreeTransmissionStruct *pTransmission = (TagFreeTransmissionStruct *)g_FreeTransmissionList.GetNext(pos);
      struct TagUnitStruct *pUnit = (TagUnitStruct *)&g_Unit[pTransmission->FeederUnitID];
      g_Unit[pTransmission->FeederUnitID].CurrentValue = pTransmission->AverCurrent;
      g_Unit[pTransmission->FeederUnitID].AngleValue = pTransmission->AverAngle;
      g_Unit[pTransmission->FeederUnitID].TransmissionState = TRUE;

      if(ct<pTransmission->BeginTime)  //等待传动
        continue;
      if(ct>pTransmission->FinishTime) //结束传动
      {
        pos = g_FreeTransmissionList.Find(pTransmission);
        delete pTransmission;
        g_FreeTransmissionList.RemoveAt(pos);
        pos = g_FreeTransmissionList.GetHeadPosition();
        continue;
      }

      if(g_Unit[pTransmission->FeederUnitID].VarGroup[1].Var[17].Value>0)//馈线有电压，可以传动
      {
        float CurrentChangeRate[100];
        for(int no=0;no<100;no++)
        {
          CNewString ns(pTransmission->CurrentChangeRateList,";","\r\n");
          CurrentChangeRate[no] = ns.GetFloatValue(0,no);
          if(CurrentChangeRate[no]==0)
            CurrentChangeRate[no] = 1;
        }

        CTimeSpan TimeSpanF = ct-pTransmission->BeginTime;
        CTimeSpan TimeSpanT = pTransmission->FinishTime-pTransmission->BeginTime;
        WORD TimeID = 0;
        if(TimeSpanT.GetTotalSeconds()>0)
          TimeID = (WORD)TimeSpanF.GetTotalSeconds() * 100 / (WORD)TimeSpanT.GetTotalSeconds();
        if(TimeID>=100)
          TimeID = 99;

        switch(pUnit->UnitType)
        {
          case 1://中压线路保护
          case 8://低压线路保护
          {
            float Ia = pUnit->CurrentValue*CurrentChangeRate[TimeID]+(pUnit->CurrentValue*CurrentChangeRate[TimeID]*(float)(rand()%100)/(float)100*pTransmission->CurrentChangeRate);
            float Ib = pUnit->CurrentValue*CurrentChangeRate[TimeID]+(pUnit->CurrentValue*CurrentChangeRate[TimeID]*(float)(rand()%100)/(float)100*pTransmission->CurrentChangeRate);
            float Ic = pUnit->CurrentValue*CurrentChangeRate[TimeID]+(pUnit->CurrentValue*CurrentChangeRate[TimeID]*(float)(rand()%100)/(float)100*pTransmission->CurrentChangeRate);
            float Pa = pUnit->VarGroup[0].Var[3].Value*Ia*(float)cos((float)3.1415926*pUnit->AngleValue/(float)180)*3/1000;
            float Pb = pUnit->VarGroup[0].Var[4].Value*Ib*(float)cos((float)3.1415926*pUnit->AngleValue/(float)180)*3/1000;
            float Pc = pUnit->VarGroup[0].Var[5].Value*Ic*(float)cos((float)3.1415926*pUnit->AngleValue/(float)180)*3/1000;
            float Qa = pUnit->VarGroup[0].Var[3].Value*Ia*(float)sin((float)3.1415926*pUnit->AngleValue/(float)180)*3/1000;
            float Qb = pUnit->VarGroup[0].Var[4].Value*Ib*(float)sin((float)3.1415926*pUnit->AngleValue/(float)180)*3/1000;
            float Qc = pUnit->VarGroup[0].Var[5].Value*Ic*(float)sin((float)3.1415926*pUnit->AngleValue/(float)180)*3/1000;
            float P = Pa + Pb + Pc;
            float Q = Qa + Qb + Qc;
            float S = (float)::sqrt(P*P+Q*Q);
            float cos = 1;
            if((P>0)&&(S>0))
              cos = P / S;

            FeedValue[pUnit->UnitID][7] += Ia;
            FeedValue[pUnit->UnitID][8] += Ib;
            FeedValue[pUnit->UnitID][9] += Ic;
            FeedValue[pUnit->UnitID][11] += P;
            FeedValue[pUnit->UnitID][12] += Q;
            FeedValue[pUnit->UnitID][13]  = cos;
            FeedValue[pUnit->UnitID][14] = 50+((float)(rand()%100)/(float)100000);
            break;
          }
          case 6://直流馈线保护
          {
            float I = pUnit->CurrentValue*CurrentChangeRate[TimeID]+(pUnit->CurrentValue*CurrentChangeRate[TimeID]*(float)(rand()%100)/(float)100*pTransmission->CurrentChangeRate);
            FeedValue[pUnit->UnitID][1] += I;
            break;
          }
        }
      }
      else
      {
        switch(pUnit->UnitType)
        {
          case 1://中压线路保护
          case 8://低压线路保护
          {
            FeedValue[pUnit->UnitID][7] = g_Current.NoloadCurrentChangeMaxValue*(float)(float)(rand()%100)/(float)100;
            FeedValue[pUnit->UnitID][8] = g_Current.NoloadCurrentChangeMaxValue*(float)(float)(rand()%100)/(float)100;
            FeedValue[pUnit->UnitID][9] = g_Current.NoloadCurrentChangeMaxValue*(float)(float)(rand()%100)/(float)100;
            FeedValue[pUnit->UnitID][11] = 0;
            FeedValue[pUnit->UnitID][12] = 0;
            FeedValue[pUnit->UnitID][13] = 1;
            FeedValue[pUnit->UnitID][14] = 50+((float)(rand()%100)/(float)100000);
            break;
          }
          case 6://直流馈线保护
          {
            FeedValue[pUnit->UnitID][1] = 0;
            break;
          }
        }
      
        pos = g_FreeTransmissionList.Find(pTransmission);
        delete pTransmission;
        g_FreeTransmissionList.RemoveAt(pos);
        pos = g_FreeTransmissionList.GetHeadPosition();
        continue;
      }
    }
  }

  {
    POSITION pos = g_FreeTrainList.GetHeadPosition();
    while(pos)
    {
      TagFreeTrainStruct *pFreeTrain = (TagFreeTrainStruct *)g_FreeTrainList.GetNext(pos);
      g_Unit[pFreeTrain->InFeederUnitID].TrainState = TRUE;
      g_Unit[pFreeTrain->OutFeederUnitID].TrainState = TRUE;

      if(ct<pFreeTrain->InBeginTime)  //等待行车
        continue;

      if((ct>=pFreeTrain->InBeginTime)&&(ct<pFreeTrain->InFinishTime))//驶入
      {
        float CurrentChangeRate[100];
        for(int no=0;no<100;no++)
        {
          CNewString ns(pFreeTrain->InCurrentChangeRateList,";","\r\n");
          CurrentChangeRate[no] = ns.GetFloatValue(0,no);
          if(CurrentChangeRate[no]==0)
            CurrentChangeRate[no] = 1;
        }

        CTimeSpan TimeSpanF = ct-pFreeTrain->InBeginTime;
        CTimeSpan TimeSpanT = pFreeTrain->InFinishTime-pFreeTrain->InBeginTime;
        WORD TimeID = 0;
        if(TimeSpanT.GetTotalSeconds()>0)
          TimeID = (WORD)TimeSpanF.GetTotalSeconds() * 100 / (WORD)TimeSpanT.GetTotalSeconds();
        if(TimeID>=100)
          TimeID = 99;

        WORD UnitID = pFreeTrain->InFeederUnitID;

        if(g_Unit[UnitID].VarGroup[1].Var[17].Value>0)//馈线有电压，可以驶入
        {
          float I = pFreeTrain->AverCurrent*CurrentChangeRate[TimeID]+(pFreeTrain->AverCurrent*CurrentChangeRate[TimeID]*(float)(rand()%100)/(float)100*pFreeTrain->CurrentChangeRate);
          float P = g_Unit[UnitID].VarGroup[0].Var[0].Value*I*(float)cos((float)3.1415926*pFreeTrain->AverAngle/(float)180);
          float Q = g_Unit[UnitID].VarGroup[0].Var[0].Value*I*(float)sin((float)3.1415926*pFreeTrain->AverAngle/(float)180);
          float S = (float)::sqrt(g_Unit[UnitID].VarGroup[0].Var[2].Value*g_Unit[UnitID].VarGroup[0].Var[2].Value+g_Unit[UnitID].VarGroup[0].Var[3].Value*g_Unit[UnitID].VarGroup[0].Var[3].Value);
          float cos = 1;
          if((P>0)&&(S>0))
            cos = P / S;

          FeedValue[UnitID][1] += I;
          FeedValue[UnitID][2] += P;
          FeedValue[UnitID][3] += Q;
          FeedValue[UnitID][4] = cos;
          FeedValue[UnitID][5] = 50+((float)(rand()%100)/(float)100000);
        }
        else
        {
          pos = g_FreeTrainList.Find(pFreeTrain);
          delete pFreeTrain;
          g_FreeTrainList.RemoveAt(pos);
          pos = g_FreeTrainList.GetHeadPosition();
          continue;
        }
      }
      else if((ct>=pFreeTrain->ChangeBeginTime)&&(ct<pFreeTrain->ChangeFinishTime))//换相
      {
        WORD InFeederUnitID = pFreeTrain->InFeederUnitID;
        WORD OutFeederUnitID = pFreeTrain->OutFeederUnitID;
      }
      else if((ct>=pFreeTrain->OutBeginTime)&&(ct<pFreeTrain->OutFinishTime))//驶出
      {
        float CurrentChangeRate[100];
        for(int no=0;no<100;no++)
        {
          CNewString ns(pFreeTrain->OutCurrentChangeRateList,";","\r\n");
          CurrentChangeRate[no] = ns.GetFloatValue(0,no);
          if(CurrentChangeRate[no]==0)
            CurrentChangeRate[no] = 1;
        }

        CTimeSpan TimeSpanF = ct-pFreeTrain->OutBeginTime;
        CTimeSpan TimeSpanT = pFreeTrain->OutFinishTime-pFreeTrain->OutBeginTime;
        WORD TimeID = (WORD)TimeSpanF.GetTotalSeconds() * 100 / (WORD)TimeSpanT.GetTotalSeconds();
        if(TimeID>=100)
          TimeID = 99;

        WORD UnitID = pFreeTrain->OutFeederUnitID;
        if(g_Unit[UnitID].VarGroup[1].Var[17].Value>0)//馈线有电压，可以驶出
        {
          float I = pFreeTrain->AverCurrent*CurrentChangeRate[TimeID]+(pFreeTrain->AverCurrent*CurrentChangeRate[TimeID]*(float)(rand()%100)/(float)100*pFreeTrain->CurrentChangeRate);
          float P = g_Unit[UnitID].VarGroup[0].Var[0].Value*I*(float)cos((float)3.1415926*pFreeTrain->AverAngle/(float)180);
          float Q = g_Unit[UnitID].VarGroup[0].Var[0].Value*I*(float)sin((float)3.1415926*pFreeTrain->AverAngle/(float)180);
          float S = (float)::sqrt(g_Unit[UnitID].VarGroup[0].Var[2].Value*g_Unit[UnitID].VarGroup[0].Var[2].Value+g_Unit[UnitID].VarGroup[0].Var[3].Value*g_Unit[UnitID].VarGroup[0].Var[3].Value);
          float cos = 1;
          if((P>0)&&(S>0))
            cos = P / S;

          FeedValue[UnitID][1] += I;
          FeedValue[UnitID][2] += P;
          FeedValue[UnitID][3] += Q;
          FeedValue[UnitID][4] = cos;
          FeedValue[UnitID][5] = 50+((float)(rand()%100)/(float)100000);
        }
        else//行车结束
        {
          pos = g_FreeTrainList.Find(pFreeTrain);
          delete pFreeTrain;
          g_FreeTrainList.RemoveAt(pos);
          pos = g_FreeTrainList.GetHeadPosition();
          continue;
        }
      }
      else//行车结束
      {
        WORD InFeederUnitID = pFreeTrain->InFeederUnitID;
        WORD OutFeederUnitID = pFreeTrain->OutFeederUnitID;

        pos = g_FreeTrainList.Find(pFreeTrain);
        delete pFreeTrain;
        g_FreeTrainList.RemoveAt(pos);
        pos = g_FreeTrainList.GetHeadPosition();
        continue;
      }
    }
  }

  CPtrList TotalFeedList;

  {
    POSITION pos = g_Up750FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_Up750FeedList.GetNext(pos);
      TotalFeedList.AddTail(pUnit);
    }
  }
  {
    POSITION pos = g_Down750FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_Down750FeedList.GetNext(pos);
      TotalFeedList.AddTail(pUnit);
    }
  }
  {
    POSITION pos = g_1B400FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_1B400FeedList.GetNext(pos);
      TotalFeedList.AddTail(pUnit);
    }
  }
  {
    POSITION pos = g_2B400FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_2B400FeedList.GetNext(pos);
      TotalFeedList.AddTail(pUnit);
    }
  }
//-----------全部馈线----------//
  {
    POSITION pos = TotalFeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)TotalFeedList.GetNext(pos);
      switch(pUnit->UnitType)
      {
        case 1://中压线路保护
        case 8://低压线路保护
        {
          pUnit->VarGroup[0].Var[7].Value = FeedValue[pUnit->UnitID][7];
          pUnit->VarGroup[0].Var[8].Value = FeedValue[pUnit->UnitID][8];
          pUnit->VarGroup[0].Var[9].Value = FeedValue[pUnit->UnitID][9];
          pUnit->VarGroup[0].Var[11].Value = FeedValue[pUnit->UnitID][11];
          pUnit->VarGroup[0].Var[12].Value = FeedValue[pUnit->UnitID][12];
          pUnit->VarGroup[0].Var[13].Value = FeedValue[pUnit->UnitID][13];
          pUnit->VarGroup[0].Var[14].Value = FeedValue[pUnit->UnitID][14];
          break;
        }
        case 6://直流馈线保护
        {
          pUnit->VarGroup[0].Var[1].Value = FeedValue[pUnit->UnitID][1];
          break;
        }
      }
    }
  }
}

void CPowerThread::ProcessPowerSimTrain()
{
  return;

  if(CTime::GetCurrentTime()<g_SimTrain.BeginTime)  //等待行车
    return;
  
//-----------馈线行车----------//
  CSingleLock sLock(&g_PowerMutex);
  sLock.Lock();

  if((CTime::GetCurrentTime()-m_ProcessSimTrainTime).GetTotalSeconds()<3)
    return;

  m_ProcessSimTrainTime = CTime::GetCurrentTime();

  for(int unitno=1;unitno<UNITCOUNT;unitno++)
  {
    for(int varno=0;varno<32;varno++)
    {
      float Value = g_SimTrainData[unitno][varno][g_SimTrain.SimTrainDataCurrentPos];
      g_Unit[unitno].VarGroup[0].Var[varno].Value = Value;
    }
  }

  for(unitno=1;unitno<UNITCOUNT;unitno++)
  {
    switch(g_Unit[unitno].UnitType)
    {
      case 6://接地变保护
      case 7://综合测控
      case 8://备自投
      {
        break;
      }
      case 1://变压器差动保护
        break;
      case 2://变压器高后备保护
      {
        float ValueUa = g_Unit[unitno].VarGroup[0].Var[0].Value;
        float ValueUb = g_Unit[unitno].VarGroup[0].Var[1].Value;
        float ValueUc = g_Unit[unitno].VarGroup[0].Var[2].Value;
        float ValueIa = g_Unit[unitno].VarGroup[0].Var[6].Value;
        float ValueIb = g_Unit[unitno].VarGroup[0].Var[7].Value;
        float ValueIc = g_Unit[unitno].VarGroup[0].Var[8].Value;

        if((ValueIa>0)||(ValueIb>0)||(ValueIc>0))
        {
          if(g_Unit[unitno].VarGroup[1].Var[0].Value==0)
          {
            ::ClearPower();
            return;
          }
        }
        break;
      }
      case 3://变压器低后备保护
      {
        float ValueUa = g_Unit[unitno].VarGroup[0].Var[0].Value;
        float ValueUb = g_Unit[unitno].VarGroup[0].Var[1].Value;
        float ValueIa = g_Unit[unitno].VarGroup[0].Var[2].Value;
        float ValueIb = g_Unit[unitno].VarGroup[0].Var[3].Value;
        if(ValueIa>0)
        {
          if(g_Unit[unitno].VarGroup[1].Var[0].Value==0)
          {
            ::ClearPower();
            return;
          }
        }
        if(ValueIb>0)
        {
          if(g_Unit[unitno].VarGroup[1].Var[2].Value==0)
          {
            ::ClearPower();
            return;
          }
        }
        break;
      }
      case 4://线路保护
      case 5://电容器保护
      {
        float ValueU = g_Unit[unitno].VarGroup[0].Var[0].Value;
        float ValueI = g_Unit[unitno].VarGroup[0].Var[1].Value;
        if(ValueI>0)
        {
          if((g_Unit[unitno].VarGroup[1].Var[0].Value==0)||(g_Unit[unitno].VarGroup[1].Var[2].Value==0))
          {
            ::ClearPower();
            return;
          }
        }
        break;
      }
    }
  }  
  
  g_SimTrain.SimTrainDataCurrentPos++;
  if(g_SimTrain.SimTrainDataCurrentPos>=g_SimTrain.SimTrainDataCount)
    ::ClearPower();
}

void CPowerThread::ProcessPowerFaultTransmission()
{
  if(CTime::GetCurrentTime()<g_FaultTransmission.BeginTime)  //等待传动
    return;
  
  for(int unitno=0;unitno<UNITCOUNT;unitno++)
  {
    for(int ch=0;ch<12;ch++)
    {
      for(int samp=0;samp<320;samp++)
        g_Unit[unitno].SampleWaveValue[ch][samp] = g_Unit[unitno].FaultWaveValue[ch][samp];
    }
  }

  ProcessProtectionCalculater();

  for(unitno=0;unitno<UNITCOUNT;unitno++)
  {
    for(int ch=0;ch<12;ch++)
    {
      for(int samp=0;samp<320;samp++)
        g_Unit[unitno].FaultWaveValue[ch][samp] = 0;
    }
  }

  ::ClearPower();
}

void CPowerThread::ProcessAutoOnOff35(WORD UnitID)
{
  static BOOL  LostVotlageStartupState1;     //1#失压启动状态
	static DWORD LostVotlageStartupTickCount1; //1#失压启动计数
  static BOOL  HasVotlageStartupState1;      //1#有压启动状态
	static DWORD HasVotlageStartupTickCount1;  //1#有压启动计数
  static BOOL  LostVotlageStartupState2;     //2#失压启动状态
	static DWORD LostVotlageStartupTickCount2; //2#失压启动计数
  static BOOL  HasVotlageStartupState2;      //2#有压启动状态
	static DWORD HasVotlageStartupTickCount2;  //2#有压启动计数

  BOOL ChargeState1L = TRUE;             //1L1T充电
  BOOL ChargeState2L = TRUE;             //2L2T充电
  BOOL ChargeStateBus  = TRUE;             //Bus充电

  static BOOL  ChargeStartupState1L;     //1L1T充电启动状态
	static DWORD ChargeStartupTickCount1L; //1L1T充电启动计数
  static BOOL  ChargeStartupState2L;     //2L2T充电启动状态
	static DWORD ChargeStartupTickCount2L; //2L2T充电启动计数
  static BOOL  ChargeStartupStateBus;      //Bus充电启动状态
	static DWORD ChargeStartupTickCountBus;  //Bus充电启动计数

  WORD CurrentParamaterGroupID = g_Unit[UnitID].CurrentParamaterGroupID;

  BOOL To1T1L = FALSE;
  BOOL To2T2L = FALSE;
  BOOL ToBus = FALSE;

  //读取保护压板的投退
  
  //读取定值数值
	float LostVotlageDelay = g_Unit[UnitID].ParameterGroup[CurrentParamaterGroupID].Var[32].Value*1000;//进线失压延时定值
	float HasVotlageDelay = g_Unit[UnitID].ParameterGroup[CurrentParamaterGroupID].Var[33].Value*1000;//进线有压延时定值
	float LostVotlageValue = g_Unit[UnitID].ParameterGroup[CurrentParamaterGroupID].Var[49].Value;//检进线无压保护定值
	float HasVotlageValue = g_Unit[UnitID].ParameterGroup[CurrentParamaterGroupID].Var[50].Value;//检进线有压保护定值
  if(LostVotlageValue>=HasVotlageValue)
  {
    LostVotlageValue = 10;
    HasVotlageValue = 40;
  }

	g_Unit[UnitID].VarGroup[0].Var[0].Value = g_InlineVotlage.Ua1;
	g_Unit[UnitID].VarGroup[0].Var[1].Value = g_InlineVotlage.Ub1;
	g_Unit[UnitID].VarGroup[0].Var[2].Value = g_InlineVotlage.Uc1;
	g_Unit[UnitID].VarGroup[0].Var[3].Value = g_InlineVotlage.Ua2;
	g_Unit[UnitID].VarGroup[0].Var[4].Value = g_InlineVotlage.Ub2;
	g_Unit[UnitID].VarGroup[0].Var[5].Value = g_InlineVotlage.Uc2;

  float Ua1 = g_Unit[UnitID].VarGroup[0].Var[0].Value;
  float Ub1 = g_Unit[UnitID].VarGroup[0].Var[1].Value;
  float Uc1 = g_Unit[UnitID].VarGroup[0].Var[2].Value;
  float Ua2 = g_Unit[UnitID].VarGroup[0].Var[3].Value;
  float Ub2 = g_Unit[UnitID].VarGroup[0].Var[4].Value;
  float Uc2 = g_Unit[UnitID].VarGroup[0].Var[5].Value;

  float U1 = 1E+10;
  float U2 = 1E+10;
  if(Ua1<U1)
    U1 = Ua1;
  if(Ub1<U1)
    U1 = Ub1;
  if(Uc1<U1)
    U1 = Uc1;
  if(Ua2<U2)
    U2 = Ua2;
  if(Ub2<U2)
    U2 = Ub2;
  if(Uc2<U2)
    U2 = Uc2;

  if(U1<=LostVotlageValue)//判断1#进线失压
  {
    if(g_Unit[1].VarGroup[1].Var[30].Value!=0)//失压且跳闸
	    g_Unit[UnitID].VarGroup[1].Var[13].Value = 1;
    else
	    g_Unit[UnitID].VarGroup[1].Var[13].Value = 0;
  }

  if(U2<=LostVotlageValue)//判断2#进线失压
  {
    if(g_Unit[2].VarGroup[1].Var[30].Value!=0)//失压且跳闸
	    g_Unit[UnitID].VarGroup[1].Var[14].Value = 1;
    else
	    g_Unit[UnitID].VarGroup[1].Var[14].Value = 0;
  }

  if((g_Unit[UnitID].VarGroup[1].Var[1].Value!=0)&&(g_Unit[UnitID].VarGroup[1].Var[2].Value!=0))//判断自投压板
  {
//-------------------1#进线故障-----------------//
    if(g_Unit[UnitID].VarGroup[1].Var[18].Value!=0)//2回2T已充电
    {
      if(g_Unit[UnitID].VarGroup[1].Var[22].Value!=0)//分列运行方式
      {
        if(g_Unit[UnitID].VarGroup[1].Var[13].Value!=0)//1#进线故障
          To2T2L = TRUE;
      }
    }
//-------------------2#进线故障-----------------//
    if(g_Unit[UnitID].VarGroup[1].Var[17].Value!=0)//1回1T已充电
    {
      if(g_Unit[UnitID].VarGroup[1].Var[22].Value!=0)//分列运行方式
      {
        if(g_Unit[UnitID].VarGroup[1].Var[14].Value!=0)//2#进线故障
          To1T1L = TRUE;
      }
    }
  }

  if(To1T1L==TRUE)//自投到1L
  {
    float ActionValue[PROTECTION_ACTION_VALUE_COUNT];
    for(int varno=0;varno<6;varno++)
      ActionValue[varno] = g_Unit[UnitID].VarGroup[0].Var[varno].Value;

    BOOL VarLock = FALSE;

    if((VarLock==FALSE)&&(g_Unit[18].VarLock[0]==FALSE))
    {
      g_Unit[2].VarGroup[1].Var[0].Value = 0; //2#进线跳闸 合位
      g_Unit[2].VarGroup[1].Var[1].Value = 1; //2#进线跳闸 分位
    }
    else
      VarLock = TRUE;

    if((VarLock==FALSE)&&(g_Unit[5].VarLock[0]==FALSE))
    {
      ::AddPresetVar(5,1,0,1,CTime::GetCurrentTime()+CTimeSpan(5)); //35kV母联合闸 合位
      ::AddPresetVar(5,1,1,0,CTime::GetCurrentTime()+CTimeSpan(5)); //35kV母联合闸 分位
      ::AddPresetVar(5,1,3,0,CTime::GetCurrentTime()+CTimeSpan(5)); //35kV母联储能
      ::AddPresetVar(5,1,3,1,CTime::GetCurrentTime()+CTimeSpan(15));
    }
    else if(VarLock==FALSE)
    {
      ::ProductSOEReport(UnitID,6,CTime::GetCurrentTime() + CTimeSpan(0,0,0,5),(WORD)::GetTickCount()%1000,1,ActionValue);
      VarLock = TRUE;
    }

    g_Unit[UnitID].VarGroup[1].Var[17].Value = 0; //1回自投放电
    g_Unit[UnitID].VarGroup[1].Var[18].Value = 0; //2回自投放电
    g_Unit[UnitID].VarGroup[1].Var[19].Value = 0; //Bus自投放电
    ChargeStartupState1L = FALSE;
    ChargeStartupTickCount1L = 0;
    ChargeStartupState2L = FALSE;
    ChargeStartupTickCount2L = 0;
    ChargeStartupStateBus = FALSE;
    ChargeStartupTickCountBus = 0;
    
    if(VarLock==FALSE)
      ::ProductSOEReport(UnitID,2,CTime::GetCurrentTime(),(WORD)::GetTickCount()%1000,1,ActionValue);
  }
  if(To2T2L==TRUE)//自投到2L
  {
    float ActionValue[PROTECTION_ACTION_VALUE_COUNT];
    for(int varno=0;varno<6;varno++)
      ActionValue[varno] = g_Unit[UnitID].VarGroup[0].Var[varno].Value;

    BOOL VarLock = FALSE;

    if((VarLock==FALSE)&&(g_Unit[17].VarLock[0]==FALSE))
    {
      g_Unit[1].VarGroup[1].Var[0].Value = 0; //1#进线跳闸 合位
      g_Unit[1].VarGroup[1].Var[1].Value = 1; //1#进线跳闸 分位
    }
    else
      VarLock = TRUE;

    if((VarLock==FALSE)&&(g_Unit[5].VarLock[0]==FALSE))
    {
      ::AddPresetVar(5,1,0,1,CTime::GetCurrentTime()+CTimeSpan(5)); //35kV母联合闸 合位
      ::AddPresetVar(5,1,1,0,CTime::GetCurrentTime()+CTimeSpan(5)); //35kV母联合闸 分位
      ::AddPresetVar(5,1,3,0,CTime::GetCurrentTime()+CTimeSpan(5)); //35kV母联储能
      ::AddPresetVar(5,1,3,1,CTime::GetCurrentTime()+CTimeSpan(15));
    }
    else if(VarLock==FALSE)
    {
      ::ProductSOEReport(UnitID,5,CTime::GetCurrentTime() + CTimeSpan(0,0,0,5),(WORD)::GetTickCount()%1000,1,ActionValue);
      VarLock = TRUE;
    }

    g_Unit[UnitID].VarGroup[1].Var[17].Value = 0; //1回自投放电
    g_Unit[UnitID].VarGroup[1].Var[18].Value = 0; //2回自投放电
    g_Unit[UnitID].VarGroup[1].Var[19].Value = 0; //Bus自投放电
    ChargeStartupState1L = FALSE;
    ChargeStartupTickCount1L = 0;
    ChargeStartupState2L = FALSE;
    ChargeStartupTickCount2L = 0;
    ChargeStartupStateBus = FALSE;
    ChargeStartupTickCountBus = 0;

    if(VarLock==FALSE)
      ::ProductSOEReport(UnitID,1,CTime::GetCurrentTime(),(WORD)::GetTickCount()%1000,1,ActionValue);
  }

	g_Unit[UnitID].VarGroup[0].Var[0].Value = g_Unit[1].VarGroup[0].Var[3].Value;
	g_Unit[UnitID].VarGroup[0].Var[1].Value = g_Unit[1].VarGroup[0].Var[4].Value;
	g_Unit[UnitID].VarGroup[0].Var[2].Value = g_Unit[1].VarGroup[0].Var[5].Value;
	g_Unit[UnitID].VarGroup[0].Var[3].Value = g_Unit[2].VarGroup[0].Var[3].Value;
	g_Unit[UnitID].VarGroup[0].Var[4].Value = g_Unit[2].VarGroup[0].Var[4].Value;
	g_Unit[UnitID].VarGroup[0].Var[5].Value = g_Unit[2].VarGroup[0].Var[5].Value;

	BOOL b1L1TMode = TRUE;
	BOOL b2L2TMode = TRUE;
	BOOL bBusMode = TRUE;

  {
    POSITION pos = g_1L35AutoModeOnList.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath  = (TagPowerPathStruct *)g_1L35AutoModeOnList.GetNext(pos);
      if(g_Unit[pPowerPath->UnitID].VarGroup[1].Var[pPowerPath->VarID].Value==0)//合列表中遥信非合
      {
        b1L1TMode = FALSE;
        break;
      }
    }
  }
  {
    POSITION pos = g_1L35AutoModeOffList.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath  = (TagPowerPathStruct *)g_1L35AutoModeOffList.GetNext(pos);
      if(g_Unit[pPowerPath->UnitID].VarGroup[1].Var[pPowerPath->VarID].Value!=0)//分列表中遥信非分
      {
        b1L1TMode = FALSE;
        break;
      }
    }
  }
  {
    POSITION pos = g_2L35AutoModeOnList.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath  = (TagPowerPathStruct *)g_2L35AutoModeOnList.GetNext(pos);
      if(g_Unit[pPowerPath->UnitID].VarGroup[1].Var[pPowerPath->VarID].Value==0)//合列表中遥信非合
      {
        b2L2TMode = FALSE;
        break;
      }
    }
  }
  {
    POSITION pos = g_2L35AutoModeOffList.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath  = (TagPowerPathStruct *)g_2L35AutoModeOffList.GetNext(pos);
      if(g_Unit[pPowerPath->UnitID].VarGroup[1].Var[pPowerPath->VarID].Value!=0)//分列表中遥信非分
      {
        b2L2TMode = FALSE;
        break;
      }
    }
  }
  {
    POSITION pos = g_Bus35AutoModeOnList.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath  = (TagPowerPathStruct *)g_Bus35AutoModeOnList.GetNext(pos);
      if(g_Unit[pPowerPath->UnitID].VarGroup[1].Var[pPowerPath->VarID].Value==0)//合列表中遥信非合
      {
        bBusMode = FALSE;
        break;
      }
    }
  }
  {
    POSITION pos = g_Bus35AutoModeOffList.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath  = (TagPowerPathStruct *)g_Bus35AutoModeOffList.GetNext(pos);
      if(g_Unit[pPowerPath->UnitID].VarGroup[1].Var[pPowerPath->VarID].Value!=0)//分列表中遥信非分
      {
        bBusMode = FALSE;
        break;
      }
    }
  }  

  g_Unit[UnitID].VarGroup[1].Var[20].Value = (float)b1L1TMode;
  g_Unit[UnitID].VarGroup[1].Var[21].Value = (float)b2L2TMode;
  g_Unit[UnitID].VarGroup[1].Var[22].Value = (float)bBusMode;

//-----------------//-----------------进线跳闸，清除充电--------------//
  if((g_Unit[UnitID].VarGroup[1].Var[13].Value!=0)||(g_Unit[UnitID].VarGroup[1].Var[14].Value!=0))
  {
    ChargeState1L = FALSE;
    ChargeState2L = FALSE;
    ChargeStateBus = FALSE;
  }

//-----------------无运行方式，清除充电--------------//
  if((g_Unit[UnitID].VarGroup[1].Var[20].Value==0)&&(g_Unit[UnitID].VarGroup[1].Var[21].Value==0)&&(g_Unit[UnitID].VarGroup[1].Var[22].Value==0))
  {
    ChargeState1L = FALSE;
    ChargeState2L = FALSE;
    ChargeStateBus = FALSE;
  }

  if(g_Unit[UnitID].VarGroup[1].Var[20].Value!=0)
    ChargeState1L = FALSE;
  if(g_Unit[UnitID].VarGroup[1].Var[21].Value!=0)
    ChargeState2L = FALSE;
  if(g_Unit[UnitID].VarGroup[1].Var[22].Value!=0)
    ChargeStateBus = FALSE;

  if(ChargeState1L==FALSE)
  {
    g_Unit[UnitID].VarGroup[1].Var[17].Value = 0;
    ChargeStartupState1L = FALSE;
    ChargeStartupTickCount1L = 0;
  }
  if(ChargeState2L==FALSE)
  {
    g_Unit[UnitID].VarGroup[1].Var[18].Value = 0;
    ChargeStartupState2L = FALSE;
    ChargeStartupTickCount1L = 0;
  }
  if(ChargeStateBus==FALSE)
  {
    g_Unit[UnitID].VarGroup[1].Var[19].Value = 0;
    ChargeStartupStateBus = FALSE;
    ChargeStartupTickCountBus = 0;
  }

  if((ChargeState1L==TRUE)&&(g_Unit[UnitID].VarGroup[1].Var[17].Value==0))
  {
    if(ChargeStartupState1L==FALSE)
    {
      ChargeStartupState1L = TRUE;
      ChargeStartupTickCount1L = ::GetTickCount();
    }
    else
    {
      if((::GetTickCount()-ChargeStartupTickCount1L)>=10000)
      {
        ChargeStartupState1L = FALSE;
        ChargeStartupTickCount1L = 0;
        g_Unit[UnitID].VarGroup[1].Var[17].Value = 1;//1T1L设置充电
      }
    }
  }
  if((ChargeState2L==TRUE)&&(g_Unit[UnitID].VarGroup[1].Var[18].Value==0))
  {
    if(ChargeStartupState2L==FALSE)
    {
      ChargeStartupState2L = TRUE;
      ChargeStartupTickCount2L = ::GetTickCount();
    }
    else
    {
      if((::GetTickCount()-ChargeStartupTickCount2L)>=10000)
      {
        ChargeStartupState2L = FALSE;
        ChargeStartupTickCount2L = 0;
        g_Unit[UnitID].VarGroup[1].Var[18].Value = 1;//2T2L设置充电
      }
    }
  }
  if((ChargeStateBus==TRUE)&&(g_Unit[UnitID].VarGroup[1].Var[19].Value==0))
  {
    if(ChargeStartupStateBus==FALSE)
    {
      ChargeStartupStateBus = TRUE;
      ChargeStartupTickCountBus = ::GetTickCount();
    }
    else
    {
      if((::GetTickCount()-ChargeStartupTickCountBus)>=10000)
      {
        ChargeStartupStateBus = FALSE;
        ChargeStartupTickCountBus = 0;
        g_Unit[UnitID].VarGroup[1].Var[19].Value = 1;//Bus设置充电
      }
    }
  }
}

void CPowerThread::ProcessAutoOnOff400(WORD UnitID)
{
  static BOOL  LostVotlageStartupState1;     //1#失压启动状态
	static DWORD LostVotlageStartupTickCount1; //1#失压启动计数
  static BOOL  HasVotlageStartupState1;      //1#有压启动状态
	static DWORD HasVotlageStartupTickCount1;  //1#有压启动计数
  static BOOL  LostVotlageStartupState2;     //2#失压启动状态
	static DWORD LostVotlageStartupTickCount2; //2#失压启动计数
  static BOOL  HasVotlageStartupState2;      //2#有压启动状态
	static DWORD HasVotlageStartupTickCount2;  //2#有压启动计数

  BOOL ChargeState1L = TRUE;             //1L1T充电
  BOOL ChargeState2L = TRUE;             //2L2T充电
  BOOL ChargeStateBus  = TRUE;             //Bus充电

  static BOOL  ChargeStartupState1L;     //1L1T充电启动状态
	static DWORD ChargeStartupTickCount1L; //1L1T充电启动计数
  static BOOL  ChargeStartupState2L;     //2L2T充电启动状态
	static DWORD ChargeStartupTickCount2L; //2L2T充电启动计数
  static BOOL  ChargeStartupStateBus;      //Bus充电启动状态
	static DWORD ChargeStartupTickCountBus;  //Bus充电启动计数

  WORD CurrentParamaterGroupID = g_Unit[UnitID].CurrentParamaterGroupID;

  BOOL To1T1L = FALSE;
  BOOL To2T2L = FALSE;
  BOOL ToBus = FALSE;

  //读取保护压板的投退
  
  //读取定值数值
	float LostVotlageDelay = g_Unit[UnitID].ParameterGroup[CurrentParamaterGroupID].Var[32].Value*1000;//进线失压延时定值
	float HasVotlageDelay = g_Unit[UnitID].ParameterGroup[CurrentParamaterGroupID].Var[33].Value*1000;//进线有压延时定值
	float LostVotlageValue = g_Unit[UnitID].ParameterGroup[CurrentParamaterGroupID].Var[49].Value;//检进线无压保护定值
	float HasVotlageValue = g_Unit[UnitID].ParameterGroup[CurrentParamaterGroupID].Var[50].Value;//检进线有压保护定值
  if(LostVotlageValue>=HasVotlageValue)
  {
    LostVotlageValue = 10;
    HasVotlageValue = 220;
  }


	g_Unit[UnitID].VarGroup[0].Var[0].Value = g_Unit[17].VarGroup[0].Var[3].Value;
	g_Unit[UnitID].VarGroup[0].Var[1].Value = g_Unit[17].VarGroup[0].Var[4].Value;
	g_Unit[UnitID].VarGroup[0].Var[2].Value = g_Unit[17].VarGroup[0].Var[5].Value;
	g_Unit[UnitID].VarGroup[0].Var[3].Value = g_Unit[18].VarGroup[0].Var[3].Value;
	g_Unit[UnitID].VarGroup[0].Var[4].Value = g_Unit[18].VarGroup[0].Var[4].Value;
	g_Unit[UnitID].VarGroup[0].Var[5].Value = g_Unit[18].VarGroup[0].Var[5].Value;

  float Ua1 = g_Unit[UnitID].VarGroup[0].Var[0].Value;
  float Ub1 = g_Unit[UnitID].VarGroup[0].Var[1].Value;
  float Uc1 = g_Unit[UnitID].VarGroup[0].Var[2].Value;
  float Ua2 = g_Unit[UnitID].VarGroup[0].Var[3].Value;
  float Ub2 = g_Unit[UnitID].VarGroup[0].Var[4].Value;
  float Uc2 = g_Unit[UnitID].VarGroup[0].Var[5].Value;

  float U1 = 1E+10;
  float U2 = 1E+10;
  if(Ua1<U1)
    U1 = Ua1;
  if(Ub1<U1)
    U1 = Ub1;
  if(Uc1<U1)
    U1 = Uc1;
  if(Ua2<U2)
    U2 = Ua2;
  if(Ub2<U2)
    U2 = Ub2;
  if(Uc2<U2)
    U2 = Uc2;

  if(U1<=LostVotlageValue)//判断1#进线失压
  {
    if(g_Unit[17].VarGroup[1].Var[30].Value!=0)//失压且跳闸
	    g_Unit[UnitID].VarGroup[1].Var[13].Value = 1;
    else
	    g_Unit[UnitID].VarGroup[1].Var[13].Value = 0;
  }

  if(U2<=LostVotlageValue)//判断2#进线失压
  {
    if(g_Unit[18].VarGroup[1].Var[30].Value!=0)//失压且跳闸
	    g_Unit[UnitID].VarGroup[1].Var[14].Value = 1;
    else
	    g_Unit[UnitID].VarGroup[1].Var[14].Value = 0;
  }

  if((g_Unit[UnitID].VarGroup[1].Var[1].Value!=0)&&(g_Unit[UnitID].VarGroup[1].Var[2].Value!=0))//判断自投压板
  {
//-------------------1#进线故障-----------------//
    if(g_Unit[UnitID].VarGroup[1].Var[18].Value!=0)//2回2T已充电
    {
      if(g_Unit[UnitID].VarGroup[1].Var[22].Value!=0)//分列运行方式
      {
        if(g_Unit[UnitID].VarGroup[1].Var[13].Value!=0)//1#进线故障
          To2T2L = TRUE;
      }
    }
//-------------------2#进线故障-----------------//
    if(g_Unit[UnitID].VarGroup[1].Var[17].Value!=0)//1回1T已充电
    {
      if(g_Unit[UnitID].VarGroup[1].Var[22].Value!=0)//分列运行方式
      {
        if(g_Unit[UnitID].VarGroup[1].Var[14].Value!=0)//2#进线故障
          To1T1L = TRUE;
      }
    }
  }

  if(To1T1L==TRUE)//自投到1L
  {
    float ActionValue[PROTECTION_ACTION_VALUE_COUNT];
    for(int varno=0;varno<6;varno++)
      ActionValue[varno] = g_Unit[UnitID].VarGroup[0].Var[varno].Value;

    BOOL VarLock = FALSE;

    if((VarLock==FALSE)&&(g_Unit[18].VarLock[0]==FALSE))
    {
      g_Unit[18].VarGroup[1].Var[0].Value = 0; //2#进线跳闸 合位
      g_Unit[18].VarGroup[1].Var[1].Value = 1; //2#进线跳闸 分位
    }
    else
      VarLock = TRUE;

    if((VarLock==FALSE)&&(g_Unit[21].VarLock[0]==FALSE))
    {
      g_Unit[21].VarGroup[1].Var[0].Value = 0; //I出线跳闸 合位
      g_Unit[21].VarGroup[1].Var[1].Value = 1; //I出线跳闸 分位
    }
    else
      VarLock = TRUE;

    if((VarLock==FALSE)&&(g_Unit[22].VarLock[0]==FALSE))
    {
      g_Unit[22].VarGroup[1].Var[0].Value = 0; //II出线跳闸 合位
      g_Unit[22].VarGroup[1].Var[1].Value = 1; //II出线跳闸 分位
    }
    else
      VarLock = TRUE;

    if((VarLock==FALSE)&&(g_Unit[20].VarLock[0]==FALSE))
    {
      ::AddPresetVar(20,1,0,1,CTime::GetCurrentTime()+CTimeSpan(5)); //400V母联合闸 合位
      ::AddPresetVar(20,1,1,0,CTime::GetCurrentTime()+CTimeSpan(5)); //400V母联合闸 分位
      ::AddPresetVar(20,1,3,0,CTime::GetCurrentTime()+CTimeSpan(5)); //400V母联储能
      ::AddPresetVar(20,1,3,1,CTime::GetCurrentTime()+CTimeSpan(15));
    }
    else if(VarLock==FALSE)
    {
      ::ProductSOEReport(UnitID,6,CTime::GetCurrentTime() + CTimeSpan(0,0,0,5),(WORD)::GetTickCount()%1000,1,ActionValue);
      VarLock = TRUE;
    }

    g_Unit[UnitID].VarGroup[1].Var[17].Value = 0; //1回自投放电
    g_Unit[UnitID].VarGroup[1].Var[18].Value = 0; //2回自投放电
    g_Unit[UnitID].VarGroup[1].Var[19].Value = 0; //Bus自投放电
    ChargeStartupState1L = FALSE;
    ChargeStartupTickCount1L = 0;
    ChargeStartupState2L = FALSE;
    ChargeStartupTickCount2L = 0;
    ChargeStartupStateBus = FALSE;
    ChargeStartupTickCountBus = 0;
    
    if(VarLock==FALSE)
      ::ProductSOEReport(UnitID,2,CTime::GetCurrentTime(),(WORD)::GetTickCount()%1000,1,ActionValue);
  }
  if(To2T2L==TRUE)//自投到2L
  {
    float ActionValue[PROTECTION_ACTION_VALUE_COUNT];
    for(int varno=0;varno<6;varno++)
      ActionValue[varno] = g_Unit[UnitID].VarGroup[0].Var[varno].Value;

    BOOL VarLock = FALSE;

    if((VarLock==FALSE)&&(g_Unit[17].VarLock[0]==FALSE))
    {
      g_Unit[17].VarGroup[1].Var[0].Value = 0; //1#进线跳闸 合位
      g_Unit[17].VarGroup[1].Var[1].Value = 1; //1#进线跳闸 分位
    }
    else
      VarLock = TRUE;

    if((VarLock==FALSE)&&(g_Unit[21].VarLock[0]==FALSE))
    {
      g_Unit[21].VarGroup[1].Var[0].Value = 0; //I出线跳闸 合位
      g_Unit[21].VarGroup[1].Var[1].Value = 1; //I出线跳闸 分位
    }
    else
      VarLock = TRUE;

    if((VarLock==FALSE)&&(g_Unit[22].VarLock[0]==FALSE))
    {
      g_Unit[22].VarGroup[1].Var[0].Value = 0; //II出线跳闸 合位
      g_Unit[22].VarGroup[1].Var[1].Value = 1; //II出线跳闸 分位
    }
    else
      VarLock = TRUE;

    if((VarLock==FALSE)&&(g_Unit[20].VarLock[0]==FALSE))
    {
      ::AddPresetVar(20,1,0,1,CTime::GetCurrentTime()+CTimeSpan(5)); //400V母联合闸 合位
      ::AddPresetVar(20,1,1,0,CTime::GetCurrentTime()+CTimeSpan(5)); //400V母联合闸 分位
      ::AddPresetVar(20,1,3,0,CTime::GetCurrentTime()+CTimeSpan(5)); //400V母联储能
      ::AddPresetVar(20,1,3,1,CTime::GetCurrentTime()+CTimeSpan(15));
    }
    else if(VarLock==FALSE)
    {
      ::ProductSOEReport(UnitID,5,CTime::GetCurrentTime() + CTimeSpan(0,0,0,5),(WORD)::GetTickCount()%1000,1,ActionValue);
      VarLock = TRUE;
    }

    g_Unit[UnitID].VarGroup[1].Var[17].Value = 0; //1回自投放电
    g_Unit[UnitID].VarGroup[1].Var[18].Value = 0; //2回自投放电
    g_Unit[UnitID].VarGroup[1].Var[19].Value = 0; //Bus自投放电
    ChargeStartupState1L = FALSE;
    ChargeStartupTickCount1L = 0;
    ChargeStartupState2L = FALSE;
    ChargeStartupTickCount2L = 0;
    ChargeStartupStateBus = FALSE;
    ChargeStartupTickCountBus = 0;

    if(VarLock==FALSE)
      ::ProductSOEReport(UnitID,1,CTime::GetCurrentTime(),(WORD)::GetTickCount()%1000,1,ActionValue);
  }

	BOOL b1L1TMode = TRUE;
	BOOL b2L2TMode = TRUE;
	BOOL bBusMode = TRUE;

  {
    POSITION pos = g_1L400AutoModeOnList.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath  = (TagPowerPathStruct *)g_1L400AutoModeOnList.GetNext(pos);
      if(g_Unit[pPowerPath->UnitID].VarGroup[1].Var[pPowerPath->VarID].Value==0)//合列表中遥信非合
      {
        b1L1TMode = FALSE;
        break;
      }
    }
  }
  {
    POSITION pos = g_1L400AutoModeOffList.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath  = (TagPowerPathStruct *)g_1L400AutoModeOffList.GetNext(pos);
      if(g_Unit[pPowerPath->UnitID].VarGroup[1].Var[pPowerPath->VarID].Value!=0)//分列表中遥信非分
      {
        b1L1TMode = FALSE;
        break;
      }
    }
  }
  {
    POSITION pos = g_2L400AutoModeOnList.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath  = (TagPowerPathStruct *)g_2L400AutoModeOnList.GetNext(pos);
      if(g_Unit[pPowerPath->UnitID].VarGroup[1].Var[pPowerPath->VarID].Value==0)//合列表中遥信非合
      {
        b2L2TMode = FALSE;
        break;
      }
    }
  }
  {
    POSITION pos = g_2L400AutoModeOffList.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath  = (TagPowerPathStruct *)g_2L400AutoModeOffList.GetNext(pos);
      if(g_Unit[pPowerPath->UnitID].VarGroup[1].Var[pPowerPath->VarID].Value!=0)//分列表中遥信非分
      {
        b2L2TMode = FALSE;
        break;
      }
    }
  }
  {
    POSITION pos = g_Bus400AutoModeOnList.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath  = (TagPowerPathStruct *)g_Bus400AutoModeOnList.GetNext(pos);
      if(g_Unit[pPowerPath->UnitID].VarGroup[1].Var[pPowerPath->VarID].Value==0)//合列表中遥信非合
      {
        bBusMode = FALSE;
        break;
      }
    }
  }
  {
    POSITION pos = g_Bus400AutoModeOffList.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath  = (TagPowerPathStruct *)g_Bus400AutoModeOffList.GetNext(pos);
      if(g_Unit[pPowerPath->UnitID].VarGroup[1].Var[pPowerPath->VarID].Value!=0)//分列表中遥信非分
      {
        bBusMode = FALSE;
        break;
      }
    }
  }  

  g_Unit[UnitID].VarGroup[1].Var[20].Value = (float)b1L1TMode;
  g_Unit[UnitID].VarGroup[1].Var[21].Value = (float)b2L2TMode;
  g_Unit[UnitID].VarGroup[1].Var[22].Value = (float)bBusMode;

//-----------------//-----------------进线跳闸，清除充电--------------//
  if((g_Unit[UnitID].VarGroup[1].Var[13].Value!=0)||(g_Unit[UnitID].VarGroup[1].Var[14].Value!=0))
  {
    ChargeState1L = FALSE;
    ChargeState2L = FALSE;
    ChargeStateBus = FALSE;
  }

//-----------------无运行方式，清除充电--------------//
  if((g_Unit[UnitID].VarGroup[1].Var[20].Value==0)&&(g_Unit[UnitID].VarGroup[1].Var[21].Value==0)&&(g_Unit[UnitID].VarGroup[1].Var[22].Value==0))
  {
    ChargeState1L = FALSE;
    ChargeState2L = FALSE;
    ChargeStateBus = FALSE;
  }

  if(g_Unit[UnitID].VarGroup[1].Var[20].Value!=0)
    ChargeState1L = FALSE;
  if(g_Unit[UnitID].VarGroup[1].Var[21].Value!=0)
    ChargeState2L = FALSE;
  if(g_Unit[UnitID].VarGroup[1].Var[22].Value!=0)
    ChargeStateBus = FALSE;

  if(ChargeState1L==FALSE)
  {
    g_Unit[UnitID].VarGroup[1].Var[17].Value = 0;
    ChargeStartupState1L = FALSE;
    ChargeStartupTickCount1L = 0;
  }
  if(ChargeState2L==FALSE)
  {
    g_Unit[UnitID].VarGroup[1].Var[18].Value = 0;
    ChargeStartupState2L = FALSE;
    ChargeStartupTickCount1L = 0;
  }
  if(ChargeStateBus==FALSE)
  {
    g_Unit[UnitID].VarGroup[1].Var[19].Value = 0;
    ChargeStartupStateBus = FALSE;
    ChargeStartupTickCountBus = 0;
  }

  if((ChargeState1L==TRUE)&&(g_Unit[UnitID].VarGroup[1].Var[17].Value==0))
  {
    if(ChargeStartupState1L==FALSE)
    {
      ChargeStartupState1L = TRUE;
      ChargeStartupTickCount1L = ::GetTickCount();
    }
    else
    {
      if((::GetTickCount()-ChargeStartupTickCount1L)>=10000)
      {
        ChargeStartupState1L = FALSE;
        ChargeStartupTickCount1L = 0;
        g_Unit[UnitID].VarGroup[1].Var[17].Value = 1;//1T1L设置充电
      }
    }
  }
  if((ChargeState2L==TRUE)&&(g_Unit[UnitID].VarGroup[1].Var[18].Value==0))
  {
    if(ChargeStartupState2L==FALSE)
    {
      ChargeStartupState2L = TRUE;
      ChargeStartupTickCount2L = ::GetTickCount();
    }
    else
    {
      if((::GetTickCount()-ChargeStartupTickCount2L)>=10000)
      {
        ChargeStartupState2L = FALSE;
        ChargeStartupTickCount2L = 0;
        g_Unit[UnitID].VarGroup[1].Var[18].Value = 1;//2T2L设置充电
      }
    }
  }
  if((ChargeStateBus==TRUE)&&(g_Unit[UnitID].VarGroup[1].Var[19].Value==0))
  {
    if(ChargeStartupStateBus==FALSE)
    {
      ChargeStartupStateBus = TRUE;
      ChargeStartupTickCountBus = ::GetTickCount();
    }
    else
    {
      if((::GetTickCount()-ChargeStartupTickCountBus)>=10000)
      {
        ChargeStartupStateBus = FALSE;
        ChargeStartupTickCountBus = 0;
        g_Unit[UnitID].VarGroup[1].Var[19].Value = 1;//Bus设置充电
      }
    }
  }
}

void CPowerThread::ProcessProtectionCalculater()
{
  for(int unitno=1;unitno<UNITCOUNT;unitno++)
  {
    TagUnitStruct *pUnit = &g_Unit[unitno];

    if(pUnit->UnitType==2)//中压备自投
    {
      ProcessAutoOnOff35(unitno);
      continue;
    }
    if(pUnit->UnitType==9)//低压备自投
    {
      ProcessAutoOnOff400(unitno);
      continue;
    }

    switch(pUnit->UnitType)
    {
      case 1://中压线路保护
      case 3://配电变压器保护
      case 4://整流机组保护
      case 8://低压线路保护
      {
        if((pUnit->VarGroup[0].Var[0].Value==0)&&(pUnit->VarGroup[0].Var[1].Value==0)&&(pUnit->VarGroup[0].Var[2].Value==0)||
           (pUnit->VarGroup[1].Var[0].Value==0))
        {
          for(int samp=0;samp<WAVE_SAMPLE_COUNT;samp++)
          {
            pUnit->SampleWaveValue[7][samp] = 0;
            pUnit->SampleWaveValue[8][samp] = 0;
            pUnit->SampleWaveValue[9][samp] = 0;
          }
        }
        if(g_Unit[unitno].ParameterGroup[g_Unit[unitno].CurrentParamaterGroupID].Var[31].Value!=0)//判控制回路断线压板
        {
          if(g_Unit[unitno].VarGroup[1].Var[0].Value==g_Unit[unitno].VarGroup[1].Var[1].Value)
            g_Unit[unitno].VarGroup[1].Var[4].Value = 1;
          else
            g_Unit[unitno].VarGroup[1].Var[4].Value = 0;
        }
        else
          g_Unit[unitno].VarGroup[1].Var[4].Value = 0;
        break;
      }
      case 5://直流进线保护
      case 6://直流馈线保护
      {
        if((pUnit->VarGroup[0].Var[0].Value==0)||
           (pUnit->VarGroup[1].Var[0].Value==0))
        {
          for(int samp=0;samp<WAVE_SAMPLE_COUNT;samp++)
          {
            pUnit->SampleWaveValue[1][samp] = 0;
          }
        }
        if(g_Unit[unitno].ParameterGroup[g_Unit[unitno].CurrentParamaterGroupID].Var[31].Value!=0)//判控制回路断线压板
        {
          if(g_Unit[unitno].VarGroup[1].Var[0].Value==g_Unit[unitno].VarGroup[1].Var[1].Value)
            g_Unit[unitno].VarGroup[1].Var[4].Value = 1;
          else
            g_Unit[unitno].VarGroup[1].Var[4].Value = 0;
        }
        else
          g_Unit[unitno].VarGroup[1].Var[4].Value = 0;
        break;
      }
      case 2://中压备自投
      case 7://直流框架保护
      case 9://低压备自投
        break;
    }
  }
  
  float ActionValue[PROTECTION_ACTION_VALUE_COUNT];
  memset(&ActionValue,0x00,sizeof(ActionValue));

  TagProtectionCalculaterStruct ProtectionCalculater[UNITCOUNT];

  for(unitno=1;unitno<UNITCOUNT;unitno++)
  {
    TagUnitStruct *pUnit = &g_Unit[unitno];

    ProtectionCalculater[unitno].ProtectionID = unitno;                     //保护装置编号
    ProtectionCalculater[unitno].ProtectionType = pUnit->UnitType;          //保护装置类型
    ProtectionCalculater[unitno].ProtectionActionType = 0;                  //保护动作类型 0为无动作
    ProtectionCalculater[unitno].ProtectionActionTime = 0;                  //保护动作时间
    ProtectionCalculater[unitno].ProtectionActionBreakerTrip[0] = FALSE;    //保护动作跳闸输出1#
    ProtectionCalculater[unitno].ProtectionActionBreakerTrip[1] = FALSE;    //保护动作跳闸输出2#
    ProtectionCalculater[unitno].ProtectionActionBreakerTrip[2] = FALSE;    //保护动作跳闸输出3#
    ProtectionCalculater[unitno].ProtectionActionBreakerReclose = FALSE;    //保护动作重合闸输出

    if(pUnit->UnitType==0)
      continue;

    for(int ch=0;ch<WAVE_CHANNEL_COUNT;ch++)//保护采样波形数据
    {
      for(int samp=0;samp<WAVE_SAMPLE_COUNT;samp++)
        ProtectionCalculater[unitno].ProtectionSample.ChannelValue[ch][samp] = pUnit->SampleWaveValue[ch][samp];
    }

    for(int no=0;no<PROTECTION_SWITCH_COUNT;no++) //保护压板
    {
      ProtectionCalculater[unitno].ProtectionSwitch.VarName[no] = pUnit->ParameterGroup[pUnit->CurrentParamaterGroupID].Var[no].VarName;
      ProtectionCalculater[unitno].ProtectionSwitch.Value[no] = pUnit->ParameterGroup[pUnit->CurrentParamaterGroupID].Var[no].Value;
    }

    for(no=0;no<PROTECTION_PARAMETER_COUNT;no++) //保护定值
    {
      ProtectionCalculater[unitno].ProtectionParameter.VarName[no] = pUnit->ParameterGroup[pUnit->CurrentParamaterGroupID].Var[no+PROTECTION_SWITCH_COUNT].VarName;
      ProtectionCalculater[unitno].ProtectionParameter.Value[no] = pUnit->ParameterGroup[pUnit->CurrentParamaterGroupID].Var[no+PROTECTION_SWITCH_COUNT].Value;
    }

    for(no=0;no<PROTECTION_ACTION_VALUE_COUNT;no++) //保护动作值
    {
      //ProtectionCalculater.ProtectionActionValue.VarName[no] = g_ProtectionActionValue.VarName[no];
      ProtectionCalculater[unitno].ProtectionActionValue.Value[no] = 0;
    }
  }

  for(unitno=1;unitno<UNITCOUNT;unitno++)
  {
    if(::ProtectionCalculater(&ProtectionCalculater[unitno])==FALSE)//调用保护计算
      return;
  }

  while(TRUE)
  {
    WORD ActionUnitID = 0;
    WORD ProtectionActionTime = 0xffff;
    BOOL ProtectionActionBreakerTrip = FALSE;
    for(unitno=1;unitno<UNITCOUNT;unitno++)
    {
      if(ProtectionCalculater[unitno].ProtectionActionBreakerTrip[0]==TRUE)
        ProtectionActionBreakerTrip = TRUE;
      if(ProtectionCalculater[unitno].ProtectionActionBreakerTrip[1]==TRUE)
        ProtectionActionBreakerTrip = TRUE;
      if(ProtectionCalculater[unitno].ProtectionActionBreakerTrip[2]==TRUE)
        ProtectionActionBreakerTrip = TRUE;

      if(ProtectionCalculater[unitno].ProtectionActionType>0)//保护动作类型
      {
        if(ProtectionActionTime>ProtectionCalculater[unitno].ProtectionActionTime)
        {
          ProtectionActionTime = ProtectionCalculater[unitno].ProtectionActionTime;
          ActionUnitID = unitno;
        }
      }
    }

    if(ActionUnitID==0)//没有保护动作
      return;

    BOOL ProtectionActionTrip = FALSE;//跳闸状态

    ProtectionActionTime = ProtectionCalculater[ActionUnitID].ProtectionActionTime;

    for(unitno=1;unitno<UNITCOUNT;unitno++)
    {
      TagUnitStruct *pUnit = &g_Unit[unitno];

      if(ProtectionCalculater[unitno].ProtectionActionType==0)
        continue;

      if(ProtectionCalculater[unitno].ProtectionActionTime!=ProtectionActionTime)//查询相同的跳闸时间
        continue;

      for(int no=0;no<PROTECTION_ACTION_VALUE_COUNT;no++) //保护动作值
        ActionValue[no] = ProtectionCalculater[unitno].ProtectionActionValue.Value[no];

      for(int ch=0;ch<WAVE_CHANNEL_COUNT;ch++)//保护采样波形数据
      {
        for(int samp=0;samp<WAVE_SAMPLE_COUNT;samp++)
          pUnit->SampleWaveValue[ch][samp] = ProtectionCalculater[unitno].ProtectionSample.ChannelValue[ch][samp];
      }

      switch(pUnit->UnitType)
      {
        case 1://中压线路保护
        case 3://配电变压器保护
        case 8://低压线路保护
          if(ProtectionCalculater[unitno].ProtectionActionBreakerTrip[0]==TRUE)
          {
            pUnit->VarGroup[1].Var[30].Value = 1;
            if(pUnit->VarGroup[1].Var[0].Value!=pUnit->VarGroup[1].Var[1].Value)
            {
              if(pUnit->VarLock[0]==FALSE)
              {
                pUnit->VarGroup[1].Var[0].Value = 0;
                pUnit->VarGroup[1].Var[1].Value = 1;
                pUnit->VarGroup[1].Var[23].Value = 0;
                ProtectionActionTrip = TRUE;
              }
              else
                pUnit->VarGroup[1].Var[23].Value = 1;
            }
          }
          ::ProductSOEReport(pUnit->UnitID,ProtectionCalculater[unitno].ProtectionActionType,CTime::GetCurrentTime(),(WORD)ProtectionCalculater[unitno].ProtectionActionTime,1,ActionValue);
          break;
        case 4://整流机组保护
          if(ProtectionCalculater[unitno].ProtectionActionBreakerTrip[0]==TRUE)
          {
            pUnit->VarGroup[1].Var[30].Value = 1;
            if(pUnit->VarGroup[1].Var[0].Value!=pUnit->VarGroup[1].Var[1].Value)
            {
              if(pUnit->VarLock[0]==FALSE)
              {
                pUnit->VarGroup[1].Var[0].Value = 0;
                pUnit->VarGroup[1].Var[1].Value = 1;
                pUnit->VarGroup[1].Var[23].Value = 0;
                ProtectionActionTrip = TRUE;
              }
              else
                pUnit->VarGroup[1].Var[23].Value = 1;
            }
            if(g_Unit[pUnit->UnitID+2].VarGroup[1].Var[0].Value!=g_Unit[pUnit->UnitID+2].VarGroup[1].Var[1].Value)
            {
              if(g_Unit[pUnit->UnitID+2].VarLock[0]==FALSE)
              {
                g_Unit[pUnit->UnitID+2].VarGroup[1].Var[0].Value = 0;
                g_Unit[pUnit->UnitID+2].VarGroup[1].Var[1].Value = 1;
                g_Unit[pUnit->UnitID+2].VarGroup[1].Var[23].Value = 0;
                ProtectionActionTrip = TRUE;
              }
              else
                g_Unit[pUnit->UnitID+3].VarGroup[1].Var[23].Value = 1;
            }
          }
          ::ProductSOEReport(pUnit->UnitID,ProtectionCalculater[unitno].ProtectionActionType,CTime::GetCurrentTime(),(WORD)ProtectionCalculater[unitno].ProtectionActionTime,1,ActionValue);
          break;
        case 5://直流进线保护
          if(ProtectionCalculater[unitno].ProtectionActionBreakerTrip[0]==TRUE)
          {
            pUnit->VarGroup[1].Var[30].Value = 1;
            if(pUnit->VarGroup[1].Var[0].Value!=pUnit->VarGroup[1].Var[1].Value)
            {
              if(pUnit->VarLock[0]==FALSE)
              {
                pUnit->VarGroup[1].Var[0].Value = 0;
                pUnit->VarGroup[1].Var[1].Value = 1;
                pUnit->VarGroup[1].Var[23].Value = 0;
                ProtectionActionTrip = TRUE;
              }
              else
                pUnit->VarGroup[1].Var[23].Value = 1;
            }
          }
          ::ProductSOEReport(pUnit->UnitID,ProtectionCalculater[unitno].ProtectionActionType,CTime::GetCurrentTime(),(WORD)ProtectionCalculater[unitno].ProtectionActionTime,1,ActionValue);
          break;
        case 6://直流馈线保护
          if(ProtectionCalculater[unitno].ProtectionActionBreakerReclose==TRUE)
          {
            if(pUnit->VarGroup[1].Var[0].Value!=pUnit->VarGroup[1].Var[1].Value)
            {
              pUnit->VarGroup[1].Var[0].Value = 1;
              pUnit->VarGroup[1].Var[1].Value = 0;
            }
            for(int no=0;no<COMPUTER_COUNT;no++)
            {
              if(g_ProtocolTH104[no].m_pCommThread!=NULL)
              {
                if(g_ProtocolTH104[no].m_StartSendDataFrame==TRUE)
                {
                  g_ProtocolTH104[no].SendVarFrame(unitno,&pUnit->VarGroup[1].Var[0]);
                  g_ProtocolTH104[no].SendVarFrame(unitno,&pUnit->VarGroup[1].Var[1]);
                }
              }
            }
          }
          if(ProtectionCalculater[unitno].ProtectionActionBreakerTrip[0]==TRUE)
          {
            pUnit->VarGroup[1].Var[30].Value = 1;
            if(pUnit->VarGroup[1].Var[0].Value!=pUnit->VarGroup[1].Var[1].Value)
            {
              if(pUnit->VarLock[0]==FALSE)
              {
                pUnit->VarGroup[1].Var[0].Value = 0;
                pUnit->VarGroup[1].Var[1].Value = 1;
                pUnit->VarGroup[1].Var[23].Value = 0;
                ProtectionActionTrip = TRUE;
              }
              else
                pUnit->VarGroup[1].Var[23].Value = 1;
            }

            for(int no=0;no<COMPUTER_COUNT;no++)
            {
              if(g_ProtocolTH104[no].m_pCommThread!=NULL)
              {
                if(g_ProtocolTH104[no].m_StartSendDataFrame==TRUE)
                {
                  g_ProtocolTH104[no].SendVarFrame(unitno,&pUnit->VarGroup[1].Var[0]);
                  g_ProtocolTH104[no].SendVarFrame(unitno,&pUnit->VarGroup[1].Var[1]);
                }
              }
            }
          }
          ::ProductSOEReport(pUnit->UnitID,ProtectionCalculater[unitno].ProtectionActionType,CTime::GetCurrentTime(),(WORD)ProtectionCalculater[unitno].ProtectionActionTime,1,ActionValue);
          break;
        case 7://直流框架保护
          if(ProtectionCalculater[unitno].ProtectionActionBreakerTrip[0]==TRUE)
          {
            pUnit->VarGroup[1].Var[30].Value = 1;
            for(int unitid=9;unitid<=16;unitid++)
            {
              if(g_Unit[unitid].VarGroup[1].Var[0].Value!=g_Unit[unitid].VarGroup[1].Var[1].Value)
              {
                if(g_Unit[unitid].VarLock[0]==FALSE)
                {
                  g_Unit[unitid].VarGroup[1].Var[0].Value = 0;
                  g_Unit[unitid].VarGroup[1].Var[1].Value = 1;
                  g_Unit[unitid].VarGroup[1].Var[23].Value = 0;
                  ProtectionActionTrip = TRUE;
                }
                else
                  g_Unit[unitid].VarGroup[1].Var[23].Value = 1;
              }
            }
          }
          ::ProductSOEReport(pUnit->UnitID,ProtectionCalculater[unitno].ProtectionActionType,CTime::GetCurrentTime(),(WORD)ProtectionCalculater[unitno].ProtectionActionTime,1,ActionValue);
          break;
      }

      ProtectionCalculater[unitno].ProtectionActionType = 0;
    }

    if(ProtectionActionBreakerTrip==FALSE)//应该有保护跳闸
      return;

    if(ProtectionActionTrip==TRUE)//应该有保护跳闸，并且有跳闸
      return;
  }
}

void CPowerThread::ProcessSampleWave()
{
  for(int unitno=0;unitno<UNITCOUNT;unitno++)
  {
    switch(g_Unit[unitno].UnitType)
    {
      case 1://中压线路保护
      case 3://配电变压器保护
      case 4://整流机组保护
      case 8://低压线路保护
      {
        float Uah = g_Unit[unitno].VarGroup[0].Var[3].Value;
        float Ubh = g_Unit[unitno].VarGroup[0].Var[4].Value;
        float Uch = g_Unit[unitno].VarGroup[0].Var[5].Value;
        float Iah = g_Unit[unitno].VarGroup[0].Var[7].Value;
        float Ibh = g_Unit[unitno].VarGroup[0].Var[8].Value;
        float Ich = g_Unit[unitno].VarGroup[0].Var[9].Value;
        float Ual = Uah / (float)110 * (float)35;
        float Ubl = Uch / (float)110 * (float)35;
        float Ucl = Uah / (float)110 * (float)35;
        float AngleValue = (float)::acos(g_Unit[unitno].VarGroup[0].Var[13].Value)*(float)180/(float)3.1415926;
        for(int samp=0;samp<320;samp++)
        {
          g_Unit[unitno].SampleWaveValue[0][samp] = (float)Uah * (float)1.414 * (float)::sin((float)(samp+(float)0/(float)360*(float)32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
          g_Unit[unitno].SampleWaveValue[1][samp] = (float)Ubh * (float)1.414 *(float)::sin((float)(samp+(float)120/(float)360*(float)32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
          g_Unit[unitno].SampleWaveValue[2][samp] = (float)Uch * (float)1.414 *(float)::sin((float)(samp+(float)240/(float)360*(float)32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
          g_Unit[unitno].SampleWaveValue[3][samp] = (float)Ual * (float)1.414 *(float)::sin((float)(samp+(float)0/(float)360*32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
          g_Unit[unitno].SampleWaveValue[4][samp] = (float)Ubl * (float)1.414 *(float)::sin((float)(samp+(float)60/(float)360*(float)32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
          g_Unit[unitno].SampleWaveValue[5][samp] = (float)Ucl * (float)1.414 *(float)::sin((float)(samp+(float)60/(float)360*(float)32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
          g_Unit[unitno].SampleWaveValue[6][samp] = (float)Iah * (float)1.414 *(float)::sin((float)(samp+(float)(0+AngleValue)/(float)360*32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
          g_Unit[unitno].SampleWaveValue[7][samp] = (float)Ibh * (float)1.414 *(float)::sin((float)(samp+(float)(120+AngleValue)/(float)360*(float)32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
          g_Unit[unitno].SampleWaveValue[8][samp] = (float)Ich * (float)1.414 *(float)::sin((float)(samp+(float)(240+AngleValue)/(float)360*(float)32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
        }
        break;
      }
      case 5://直流进线保护
      case 6://直流馈线保护
      {
        float U = g_Unit[unitno].VarGroup[0].Var[0].Value;
        float I = g_Unit[unitno].VarGroup[0].Var[1].Value;
        for(int samp=0;samp<320;samp++)
        {
          g_Unit[unitno].SampleWaveValue[0][samp] = U;
          g_Unit[unitno].SampleWaveValue[1][samp] = I;
        }
        break;
      }
      case 2://中压备自投
        break;
      case 9://低压备自投
        break;
      case 7://直流框架保护
      {
        for(int samp=0;samp<320;samp++)
        {
          g_Unit[unitno].SampleWaveValue[0][samp] = 0;
          g_Unit[unitno].SampleWaveValue[1][samp] = 0;
        }
        break;
      }
    }
  }
}

void CPowerThread::ProcessPowerState()
{
  WORD PowerState = 0x00; //负荷模式
  if(g_FreeTransmissionList.GetCount()>0)
    PowerState |= 0x01;
  if(g_FreeTrainList.GetCount()>0)
    PowerState |= 0x02;
  if(g_SimTrain.SimTrainState>0)
    PowerState |= 0x04;

  m_PowerState = PowerState;
}

int CPowerThread::Run() 
{
  Sleep(1000);
  while(m_Run)
  {
    Sleep(500);
    
    if(g_SimCommunicationMain==FALSE)
      continue;

    if(g_EnableSendPowerState==TRUE)
      g_DataExchangePowerStateSocket.ProcessSendPowerState();
    if(g_EnableReceiveTrainState==TRUE)
      g_DataExchangeTrainStateSocket.ProcessReceiveTrainState();
    if(g_EnableReceivePowerState==TRUE)
      g_DataExchangePowerStateSocketOther.ProcessReceivePowerState();

    CSingleLock sLock(&g_PowerMutex);
    sLock.Lock();

    int PresetID=0;
    while(TRUE)
    {
      TagPresetVarStruct *pPresetVar = ::ReadPresetVar(PresetID);
      if(pPresetVar==NULL)
        break;

      PresetID++;

      if(pPresetVar->PresetTime<=CTime::GetCurrentTime())
      {
        g_Unit[pPresetVar->UnitID].VarGroup[pPresetVar->VarGroupID].Var[pPresetVar->VarID].Value = pPresetVar->PresetValue;    
        ::RemovePresetVar(pPresetVar);
      }
    }

    ProcessPowerState();

    if(g_FaultTransmission.FaultTransmissionState==2)//故障传动模式
    {
      ProcessPowerFaultTransmission();
      continue;
    }
    if(g_SimTrain.SimTrainState==2)//仿真行车模式
    {
      ProcessPowerSimTrain();
      ProcessSampleWave();
      ProcessProtectionCalculater();
      continue;
    }

    if((g_FreeTransmissionList.GetCount()>0)||(g_FreeTrainList.GetCount()>0))//馈线传动及自由行车模式
    {
      ProcessPowerNoloadVotlage();
      ProcessPowerFeedTransmissionAndFreeTrain();
      ProcessPowerCurrent();
      ProcessSampleWave();
      ProcessProtectionCalculater();
      continue;
    }

    BOOL TrainState = FALSE;
    for(int no=0;no<256;no++)
    {
      if(g_DataExchangeList[no].TrainState==TRUE)
      {
        TrainState = TRUE;
        break;
      }
    }
    if(TrainState==TRUE)//数据交换行车模式
    {
      ProcessPowerNoloadVotlage();
      ProcessPowerFeedTransmissionAndFreeTrain();
      ProcessPowerCurrent();
      ProcessSampleWave();
      ProcessProtectionCalculater();
      continue;
    }

    if((g_FreeTransmissionList.GetCount()==0)&&(g_FreeTrainList.GetCount()==0)&&(g_SimTrain.SimTrainState==0))//空载模式
    {
      ProcessPowerNoloadVotlage();
      ProcessPowerNoloadCurrent();
      ProcessPowerCurrent();
      ProcessSampleWave();
      ProcessProtectionCalculater();
    }
    {
      if((g_FaultTransmission.FaultTransmissionState==1)&&(strlen(g_FaultTransmission.FaultTransmissionDataFileName)>0))//故障传动数据文件名称
      {
        CString FileName = g_FaultTransmission.FaultTransmissionDataFileName;

        ::ClearPower();

        //g_pDlgMessageBox->DoShow("正在读取故障传动数据文件 <"+FileName+">",3);

        CString FullFileName = g_ProjectFilePath+"FaultData\\"+FileName+".txt";
        CFile file;
        if(!file.Open(FullFileName, CFile::shareDenyRead, NULL))
          g_pDlgMessageBox->DoShow("故障传动数据文件读取失败 <"+FileName+">",3);
        else
        {
          CString DataList;

          CArchive ar(&file,CArchive::load);
          CString dataline;
	        while(ar.ReadString(dataline))
            DataList += dataline+"\r\n";

          file.Close();

          CString inifilename = g_ProjectFilePath + "PowerPath.ini";
          CNewString datans(DataList,"\t","\r\n");
          int DataChannelsCount = datans.GetWordCount(0);

          if(DataChannelsCount<=0)
            g_pDlgMessageBox->DoShow("故障传动数据文件格式错误 <"+FileName+">",3);
          else
          {
            for(int unitno=1;unitno<UNITCOUNT;unitno++)
            {
              for(int ch=0;ch<WAVE_CHANNEL_COUNT;ch++)
              {
                for(int samp=0;samp<WAVE_SAMPLE_COUNT;samp++)
                  g_Unit[unitno].FaultWaveValue[ch][samp] = 0;
              }

              CString UnitName = g_Unit[unitno].UnitName + "故障";
              for(ch=0;ch<12;ch++)
              {
                char buf[1024];
                CString key;
                key.Format("%02d",ch);
                ::GetPrivateProfileString(UnitName,key,"",buf,1024,inifilename);
    
                CNewString ns(buf,";","\r\n");
                CString ChannelsName = ns.GetWord(0,0);
                float   ChannelsValueK = ns.GetFloatValue(0,1);
                if(ChannelsValueK==0)
                  ChannelsValueK = 1;

                if(ChannelsName=="")
                  continue;

                int FindChannelID = -1;
                for(int fch=0;fch<DataChannelsCount;fch++)
                {
                  if(ChannelsName == datans.GetWord(0,fch))
                  {
                    FindChannelID = fch;
                    break;
                  }
                }
  
                if(FindChannelID>=0)
                {
                  for(int samp=0;samp<WAVE_SAMPLE_COUNT;samp++)
                  {
                    float Value = datans.GetFloatValue(samp+1,FindChannelID)*ChannelsValueK;
                    g_Unit[unitno].FaultWaveValue[ch][samp] = Value;
                  }
                }
              }
            }

            BOOL Result = TRUE;

            for(unitno=1;unitno<UNITCOUNT;unitno++)
            {
              if(Result==FALSE)
                break;

              switch(g_Unit[unitno].UnitType)
              {
                case 7://备自投
                  break;
                case 1://变压器差动保护
                case 2://变压器高后备保护
                case 3://变压器低后备保护
                case 4://线路保护
                case 5://电容器保护
                case 6://综合测控
                {
                  if(g_Unit[unitno].VarGroup[1].Var[0].Value==0)//断路器
                  {
                    int samp;
                    for(samp=0;samp<WAVE_SAMPLE_COUNT;samp++)
                      g_Unit[unitno].FaultWaveValue[7][samp] = 0;
                    for(samp=0;samp<WAVE_SAMPLE_COUNT;samp++)
                      g_Unit[unitno].FaultWaveValue[8][samp] = 0;
                    for(samp=0;samp<WAVE_SAMPLE_COUNT;samp++)
                      g_Unit[unitno].FaultWaveValue[9][samp] = 0;
                  }
                  break;
                }

              }
              switch(g_Unit[unitno].UnitType)//处理馈线电压波形数据，避免PT异常
              {
                case 4://线路保护
                case 5://电容器保护
                {
                  for(int varid=0;varid<6;varid++)
                  {
                    float ValueFirst = PowerCalculaterValue(g_Unit[unitno].FaultWaveValue[varid],0);
                    float ValueLast = PowerCalculaterValue(g_Unit[unitno].FaultWaveValue[varid],320-32);
                    if((ValueFirst>5)&&(ValueLast<5))
                    {
                      for(int samp=0;samp<320;samp++)
                      {
                        if(samp<=64)//前2个周波
                        {
                          g_Unit[unitno].FaultWaveValue[varid][samp] = (float)ValueFirst * (float)1.414 * (float)::sin((float)(samp+(float)0/(float)360*(float)32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
                        }
                        else
                          g_Unit[unitno].FaultWaveValue[varid][samp] = (float)ValueFirst*(float)0.2 * (float)1.414 * (float)::sin((float)(samp+(float)0/(float)360*(float)32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
                      }
                    }
                  }
                  break;
                }
              }
            }  
            if(Result)
            {
              g_FaultTransmission.FaultTransmissionState = 2;
              g_FaultTransmission.BeginTime = CTime::GetCurrentTime()+CTimeSpan(0,0,0,g_FaultTransmission.StartupDelaySecond);
            }
          }
        }
      }

      if((g_SimTrain.SimTrainState==1)&&(strlen(g_SimTrain.SimTrainDataFileName)>0))//行车仿真数据文件名称
      {
        CString FileName = g_SimTrain.SimTrainDataFileName;

        ::ClearPower();

        //g_pDlgMessageBox->DoShow("正在读取行车仿真数据文件 <"+FileName+">",3);

        CString FullFileName = g_ProjectFilePath+"TrainData\\"+FileName+".txt";
        CFile file;
        if(!file.Open(FullFileName, CFile::shareDenyRead, NULL))
          g_pDlgMessageBox->DoShow("行车仿真数据文件读取失败 <"+FileName+">",3);
        else
        {
          CString DataList;

          CArchive ar(&file,CArchive::load);
          CString dataline;
	        while(ar.ReadString(dataline))
            DataList += dataline+"\r\n";

          file.Close();

          CString inifilename = g_ProjectFilePath + "PowerPath.ini";
          CNewString datans(DataList,"\t","\r\n");
          int DataChannelsCount = datans.GetWordCount(0);
          int DataTrainCount = datans.GetLineCount() - 1;

          if((DataTrainCount<=0)||(DataChannelsCount<=0))
            g_pDlgMessageBox->DoShow("行车仿真数据文件格式错误 " + FileName,3);
          else
          {
            if(DataChannelsCount>100)
              DataChannelsCount = 100;
    
            float SimTrainData[UNITCOUNT][32][100];

            for(int unitno=1;unitno<UNITCOUNT;unitno++)
            {
              CString UnitName = g_Unit[unitno].UnitName + "行车";
              for(int varno=0;varno<32;varno++)
              {
                for(int datano=0;datano<100;datano++)
                  SimTrainData[unitno][varno][datano] = 0;

                char buf[1024];
                CString key;
                key.Format("%02d",varno);
                ::GetPrivateProfileString(UnitName,key,"",buf,1024,inifilename);
    
                CNewString ns(buf,";","\r\n");
                CString ChannelsName = ns.GetWord(0,0);
                float   ChannelsValueK = ns.GetFloatValue(0,1);
                if(ChannelsValueK==0)
                  ChannelsValueK = 1;

                if(ChannelsName=="")
                  continue;

                int FindChannelID = -1;
                for(int fch=0;fch<DataChannelsCount;fch++)
                {
                  if(ChannelsName == datans.GetWord(0,fch))
                  {
                    FindChannelID = fch;
                    break;
                  }
                }
  
                if(FindChannelID>=0)
                {
                  for(int datano=0;datano<DataTrainCount;datano++)
                  {
                    float Value = datans.GetFloatValue(datano+1,FindChannelID)*ChannelsValueK;
                    SimTrainData[unitno][varno][datano] = Value;
                  }
                }
              }
            }
  
            BOOL Result = TRUE;

            for(unitno=1;unitno<UNITCOUNT;unitno++)
            {
              if(Result==FALSE)
                break;

              switch(g_Unit[unitno].UnitType)
              {
                case 1://变压器差动保护
                  break;
                case 2://变压器高后备保护
                case 3://变压器低后备保护
                {
                  if(g_Unit[unitno].VarGroup[1].Var[0].Value==0)
                  {
                    for(int datano=0;datano<DataTrainCount;datano++)
                    {
                      for(int varno=7;varno<32;varno++)
                        SimTrainData[unitno][varno][datano] = 0;
                    }
                  }
                  break;
                }
                case 4://线路保护
                {
                  if(g_Unit[unitno].VarGroup[1].Var[17].Value==0)
                  {
                    for(int datano=0;datano<DataTrainCount;datano++)
                      SimTrainData[unitno][0][datano] = 0;
                  }

                  break;
                }
                case 5://电容器保护
                {
                  if(g_Unit[unitno].VarGroup[1].Var[0].Value==0)
                  {
                    for(int datano=0;datano<DataTrainCount;datano++)
                      SimTrainData[unitno][0][datano] = 0;
                  }

                  break;
                }
                case 6://综合测控
                case 7://备自投
                {
                  for(int varno=0;varno<32;varno++)
                  {
                    for(int datano=0;datano<100;datano++)
                      SimTrainData[unitno][varno][datano] = 0;
                  }
                  break;
                }
              }
            }  
  
            if(Result== TRUE)
            {
              for(int datano=0;datano<DataTrainCount;datano++)
              {
                for(unitno=1;unitno<UNITCOUNT;unitno++)
                {
                  for(int varno=0;varno<32;varno++)
                    g_SimTrainData[unitno][varno][datano] = SimTrainData[unitno][varno][datano];
                }
              }
              g_SimTrain.SimTrainState = 2;
	            g_SimTrain.SimTrainDataCurrentPos = 0;
	            g_SimTrain.SimTrainDataCount = DataTrainCount;
              g_SimTrain.BeginTime = CTime::GetCurrentTime()+CTimeSpan(0,0,0,g_SimTrain.StartupDelaySecond);
            }
          }
        }
      }
    }

   ::WriteUnitToShareMemory();
  }
	m_Running = FALSE;
  return CWinThread::Run();
}

float CPowerThread::PowerCalculaterValue(float SampleValueList[WAVE_SAMPLE_COUNT],WORD BeginPosition)	//计算从BeginPosition开始的32个采样点的基波值
{
  float Value = 0;
				
	float sintab[32],costab[32];
	int sampcount=32;
	int i,temp;

	for(i=0;i<sampcount;i++)
	{
		temp=i;
		sintab[i]=(float)sin(2*(double)3.14159265*(double)temp/(double)sampcount);		//sin(2*pi*k/32)
	}
	for(i=0;i<sampcount;i++)
	{
		temp=i;
		costab[i]=(float)cos(2*(double)3.14159265*(double)temp/(double)sampcount);
	}
	
	float Re=0,Im=0,Rms=0;
	int j=0;

	for(i=0;i<1;i++)
	{
		while(j/sampcount==i)
		{
			float *Value=&SampleValueList[j+BeginPosition];
			Im+=*Value*sintab[j];
			Re+=*Value*costab[j];
			j++;
		}
	}
	Rms=(float)sqrt(Re*Re+Im*Im)/(float)sampcount;
	Rms=(float)1.41421356*Rms;
	Value=Rms;

	return Value;
}

void CPowerThread::KillThread()//结束线程
{
	m_Run = FALSE;
  while(m_Running==TRUE)
    Sleep(100);
  TerminateThread(m_hThread,0);
}
