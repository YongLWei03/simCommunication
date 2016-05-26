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

CPtrList g_Path1L1T110;
CPtrList g_Path2L2T110;
CPtrList g_PathBus35;
CPtrList g_Path1B35;
CPtrList g_Path2B35;
CPtrList g_1B35FeedList;
CPtrList g_2B35FeedList;
CPtrList g_1LPTList;
CPtrList g_2LPTList;
CPtrList g_1L1TAutoModeOnList;
CPtrList g_1L1TAutoModeOffList;
CPtrList g_2L2TAutoModeOnList;
CPtrList g_2L2TAutoModeOffList;
CPtrList g_BusAutoModeOnList;
CPtrList g_BusAutoModeOffList;

WORD g_UnitID1THi = 0;
WORD g_UnitID2THi = 0;
WORD g_UnitID1TLo = 0;
WORD g_UnitID2TLo = 0;
WORD g_UnitIDBus35 = 0;

BOOL g_Power1T = TRUE;
BOOL g_Power2T = TRUE;
BOOL g_PowerBus = TRUE;

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
  g_Path1L1T110.RemoveAll();
  g_Path2L2T110.RemoveAll();
  g_PathBus35.RemoveAll();
  g_1LPTList.RemoveAll();
  g_2LPTList.RemoveAll();
  g_Path1B35.RemoveAll();
  g_Path2B35.RemoveAll();
  g_1B35FeedList.RemoveAll();
  g_2B35FeedList.RemoveAll();
  g_1L1TAutoModeOnList.RemoveAll();
  g_1L1TAutoModeOffList.RemoveAll();
  g_2L2TAutoModeOnList.RemoveAll();
  g_2L2TAutoModeOffList.RemoveAll();
  g_BusAutoModeOnList.RemoveAll();
  g_BusAutoModeOffList.RemoveAll();

  CString filename = g_ProjectFilePath + "PowerPath.ini";

  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("1回-1T-110",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);
      pPowerPath->Uab = 0;
      pPowerPath->Ubc = 0;
      pPowerPath->Uca = 0;
      pPowerPath->Ua = 0;
      pPowerPath->Ub = 0;
      pPowerPath->Uc = 0;

      g_Path1L1T110.AddTail(pPowerPath);

      if(pPowerPath->VarID==0)
        g_UnitID1THi = pPowerPath->UnitID;
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("2回-2T-110",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);
      pPowerPath->Uab = 0;
      pPowerPath->Ubc = 0;
      pPowerPath->Uca = 0;
      pPowerPath->Ua = 0;
      pPowerPath->Ub = 0;
      pPowerPath->Uc = 0;

      g_Path2L2T110.AddTail(pPowerPath);

      if(pPowerPath->VarID==0)
        g_UnitID2THi = pPowerPath->UnitID;
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("1回PT110",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString Point = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = atoi(Point.Left(2));
      pPowerPath->VarID = atoi(Point.Right(2));

      g_1LPTList.AddTail(pPowerPath);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("2回PT110",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString Point = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = atoi(Point.Left(2));
      pPowerPath->VarID = atoi(Point.Right(2));

      g_2LPTList.AddTail(pPowerPath);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("35kV母线I",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);
      pPowerPath->Uab = 0;
      pPowerPath->Ubc = 0;
      pPowerPath->Uca = 0;
      pPowerPath->Ua = 0;
      pPowerPath->Ub = 0;
      pPowerPath->Uc = 0;

      g_Path1B35.AddTail(pPowerPath);

      if(pPowerPath->VarID==0)
        g_UnitID1TLo = pPowerPath->UnitID;
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("35kV母线II",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);
      pPowerPath->Uab = 0;
      pPowerPath->Ubc = 0;
      pPowerPath->Uca = 0;
      pPowerPath->Ua = 0;
      pPowerPath->Ub = 0;
      pPowerPath->Uc = 0;

      g_Path2B35.AddTail(pPowerPath);

      if(pPowerPath->VarID==0)
        g_UnitID2TLo = pPowerPath->UnitID;
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("35kV母联",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString Point = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = atoi(Point.Left(2));
      pPowerPath->VarID = atoi(Point.Right(2));

      g_PathBus35.AddTail(pPowerPath);

      if(pPowerPath->VarID==0)
        g_UnitIDBus35 = pPowerPath->UnitID;
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("35kV母线I馈线",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      WORD UnitID = atoi(buf);
      g_1B35FeedList.AddTail(&g_Unit[UnitID]);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("35kV母线II馈线",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      WORD UnitID = atoi(buf);
      g_2B35FeedList.AddTail(&g_Unit[UnitID]);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("1回1T并列运行合位",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);
      
      g_1L1TAutoModeOnList.AddTail(pPowerPath);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("1回1T并列运行分位",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);

      g_1L1TAutoModeOffList.AddTail(pPowerPath);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("2回2T并列运行合位",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);

      g_2L2TAutoModeOnList.AddTail(pPowerPath);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("2回2T并列运行分位",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);

      g_2L2TAutoModeOffList.AddTail(pPowerPath);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("分列运行合位",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);
      
      g_BusAutoModeOnList.AddTail(pPowerPath);
    }
  }
  {
    char buf[1024];
    CString key;

    for(int no=0;no<256;no++)
    {
      key.Format("%02d",no);
      ::GetPrivateProfileString("分列运行分位",key,"",buf,1024,filename);
      if(strlen(buf)==0)
        break;
      
      CString VarInfo = buf;
      struct TagPowerPathStruct *pPowerPath = new TagPowerPathStruct;
      pPowerPath->UnitID = ::GetUnitIDFromVarInfo(VarInfo);
      pPowerPath->VarID = ::GetVarIDFromVarInfo(VarInfo);

      g_BusAutoModeOffList.AddTail(pPowerPath);
    }
  }

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
    Ua1In = (float)atof(buf);
    Ub1In = (float)atof(buf);
    Uc1In = (float)atof(buf);
    Ua2In = (float)atof(buf);
    Ub2In = (float)atof(buf);
    Uc2In = (float)atof(buf);

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

    g_Unit[g_UnitID1THi].VarGroup[0].Var[15].Value = (float)g_PTVoltage.T1Gear;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[15].Value = (float)g_PTVoltage.T2Gear;
    g_Unit[g_UnitID1THi].VarGroup[0].Var[16].Value = (float)g_PTVoltage.T1Temp1;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[16].Value = (float)g_PTVoltage.T2Temp1;
    g_Unit[g_UnitID1THi].VarGroup[0].Var[17].Value = (float)g_PTVoltage.T1Temp2;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[17].Value = (float)g_PTVoltage.T2Temp2;

    g_Current.NoloadCurrentChangeMaxValue = NoloadCurrentChangeMaxValue;
    g_Current.BeginTime = 0;
  }
  {
    CString filename = g_ProjectFilePath + "PowerPath.ini";

    char buf[1024];
    ::GetPrivateProfileString("A相电容器","补偿形式","0",buf,1024,filename);
    g_Compensation[0].CompensationType = atoi(buf);
    ::GetPrivateProfileString("A相电容器","保护装置","0",buf,1024,filename);
    g_Compensation[0].CompensationUnitID = atoi(buf);

    for(int branchno=0;branchno<4;branchno++)
    {
      ::GetPrivateProfileString("A相电容器",::GetFormatText("支路%d遥信路径",branchno+1),"",g_Compensation[0].CompensationBranchPowerPathVarList[branchno],128,filename);
      ::GetPrivateProfileString("A相电容器",::GetFormatText("支路%d电流指示",branchno+1),"",g_Compensation[0].CompensationBranchVarI[branchno],8,filename);
      ::GetPrivateProfileString("A相电容器",::GetFormatText("支路%d无功指示",branchno+1),"",g_Compensation[0].CompensationBranchVarQ[branchno],8,filename);

      ::GetPrivateProfileString("A相电容器",::GetFormatText("支路%d补偿阻抗",branchno+1),"1",buf,1024,filename);
      g_Compensation[0].CompensationBranchValueZ[branchno] = (float)atof(buf);
      ::GetPrivateProfileString("A相电容器",::GetFormatText("支路%d补偿系数",branchno+1),"1",buf,1024,filename);
      g_Compensation[0].CompensationBranchValueK[branchno] = (float)atof(buf);
    }
  }
  {
    CString filename = g_ProjectFilePath + "PowerPath.ini";

    char buf[1024];
    ::GetPrivateProfileString("B相电容器","补偿形式","0",buf,1024,filename);
    g_Compensation[1].CompensationType = atoi(buf);
    ::GetPrivateProfileString("B相电容器","保护装置","0",buf,1024,filename);
    g_Compensation[1].CompensationUnitID = atoi(buf);

    for(int branchno=0;branchno<4;branchno++)
    {
      ::GetPrivateProfileString("B相电容器",::GetFormatText("支路%d遥信路径",branchno+1),"",g_Compensation[1].CompensationBranchPowerPathVarList[branchno],128,filename);
      ::GetPrivateProfileString("B相电容器",::GetFormatText("支路%d电流指示",branchno+1),"",g_Compensation[1].CompensationBranchVarI[branchno],8,filename);
      ::GetPrivateProfileString("B相电容器",::GetFormatText("支路%d无功指示",branchno+1),"",g_Compensation[1].CompensationBranchVarQ[branchno],8,filename);

      ::GetPrivateProfileString("B相电容器",::GetFormatText("支路%d补偿阻抗",branchno+1),"1",buf,1024,filename);
      g_Compensation[1].CompensationBranchValueZ[branchno] = (float)atof(buf);
      ::GetPrivateProfileString("B相电容器",::GetFormatText("支路%d补偿系数",branchno+1),"1",buf,1024,filename);
      g_Compensation[1].CompensationBranchValueK[branchno] = (float)atof(buf);
    }
  }
  {
    CString filename = g_ProjectFilePath + "PowerPath.ini";

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
  {
    POSITION pos = g_1B35FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct*)g_1B35FeedList.GetNext(pos);
      if(pUnit->UnitName == FeederName)
        return pUnit->UnitID;
    }
  }
  {
    POSITION pos = g_2B35FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct*)g_2B35FeedList.GetNext(pos);
      if(pUnit->UnitName == FeederName)
        return pUnit->UnitID;
    }
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

  float Ua1T = 0;
  float Ub1T = 0;
  float Uc1T = 0;
  float Uab1T = 0;
  float Ubc1T = 0;
  float Uca1T = 0;
  float Uo1T = 0;

  float Ua2T = 0;
  float Ub2T = 0;
  float Uc2T = 0;
  float Uab2T = 0;
  float Ubc2T = 0;
  float Uca2T = 0;
  float Uo2T = 0;

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

  g_Power1T = TRUE;
  g_Power2T = TRUE;
  g_PowerBus = TRUE;

  BOOL Voltage1B = FALSE;
  BOOL Voltage2B = FALSE;

  {
    POSITION pos = g_Path1L1T110.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath = (TagPowerPathStruct *)g_Path1L1T110.GetNext(pos);
      WORD UnitID = pPowerPath->UnitID;
      WORD VarID = pPowerPath->VarID;
      if((g_Unit[UnitID].VarGroup[1].Var[VarID].Value==0))
      {
        g_Power1T = FALSE;
        break;
      }
    }
  }
  {
    POSITION pos = g_Path2L2T110.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath = (TagPowerPathStruct *)g_Path2L2T110.GetNext(pos);
      WORD UnitID = pPowerPath->UnitID;
      WORD VarID = pPowerPath->VarID;
      if((g_Unit[UnitID].VarGroup[1].Var[VarID].Value==0))
      {
        g_Power2T = FALSE;
        break;
      }
    }
  }
  {
    POSITION pos = g_PathBus35.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath = (TagPowerPathStruct *)g_PathBus35.GetNext(pos);
      WORD UnitID = pPowerPath->UnitID;
      WORD VarID = pPowerPath->VarID;
      if((g_Unit[UnitID].VarGroup[1].Var[VarID].Value==0))
      {
        g_PowerBus = FALSE;
        break;
      }
    }
  }
  
  {
    POSITION pos = g_1LPTList.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath = (TagPowerPathStruct *)g_1LPTList.GetNext(pos);
      if(g_Unit[pPowerPath->UnitID].VarGroup[1].Var[pPowerPath->VarID].Value==0)//判断PT刀闸
      {
        Ua1In = 0;
        Ub1In = 0;
        Uc1In = 0;
        Uab1In = 0;
        Ubc1In = 0;
        Uca1In = 0;
        Uo1In = 0;
        break;
      }
    }    
  }
  {
    POSITION pos = g_2LPTList.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath = (TagPowerPathStruct *)g_2LPTList.GetNext(pos);
      if(g_Unit[pPowerPath->UnitID].VarGroup[1].Var[pPowerPath->VarID].Value==0)//判断PT刀闸
      {
        Ua2In = 0;
        Ub2In = 0;
        Uc2In = 0;
        Uab2In = 0;
        Ubc2In = 0;
        Uca2In = 0;
        Uo2In = 0;
        break;
      }
    }    
  }
//-----------1回-1T-110----------//
  {
    BOOL NoVoltage = FALSE;

    POSITION pos = g_Path1L1T110.GetHeadPosition();
    while(pos)
    {
      struct TagPowerPathStruct *pPowerPath = (TagPowerPathStruct *)g_Path1L1T110.GetNext(pos);
      if(pPowerPath->UnitID>0)
      {
        struct TagUnitStruct *pUnit = NULL;
        pUnit = &g_Unit[pPowerPath->UnitID];
        if(pUnit->VarGroup[1].Var[pPowerPath->VarID].Value==0)//分位
        {
          NoVoltage = TRUE;
          break;
        }
      }
    }

    if(NoVoltage==FALSE)
    {
      Ua1T = Ua1In;
      Ub1T = Ub1In;
      Uc1T = Uc1In;
      Uab1T = Uab1In;
      Ubc1T = Ubc1In;
      Uca1T = Uca1In;
      Uo1T = Uo1In;
    }

    struct TagUnitStruct *pUnit = &g_Unit[g_UnitID1THi];
    pUnit->VarGroup[0].Var[0].Value = Uab1In;
    pUnit->VarGroup[0].Var[1].Value = Ubc1In;
    pUnit->VarGroup[0].Var[2].Value = Uca1In;
    pUnit->VarGroup[0].Var[3].Value = Ua1In;
    pUnit->VarGroup[0].Var[4].Value = Ub1In;
    pUnit->VarGroup[0].Var[5].Value = Uc1In;
    pUnit->VarGroup[0].Var[6].Value = Uo1In;
  }
//-----------2回-2T-110----------//
  {
    BOOL NoVoltage = FALSE;

    POSITION pos = g_Path2L2T110.GetHeadPosition();
    while(pos)
    {
      struct TagPowerPathStruct *pPowerPath = (TagPowerPathStruct *)g_Path2L2T110.GetNext(pos);
      if(pPowerPath->UnitID>0)
      {
        struct TagUnitStruct *pUnit = NULL;
        pUnit = &g_Unit[pPowerPath->UnitID];
        if(pUnit->VarGroup[1].Var[pPowerPath->VarID].Value==0)//分位
        {
          NoVoltage = TRUE;
          break;
        }
      }
    }

    if(NoVoltage==FALSE)
    {
      Ua2T = Ua2In;
      Ub2T = Ub2In;
      Uc2T = Uc2In;
      Uab2T = Uab2In;
      Ubc2T = Ubc2In;
      Uca2T = Uca2In;
      Uo2T = Uo2In;
    }

    struct TagUnitStruct *pUnit = &g_Unit[g_UnitID2THi];
    pUnit->VarGroup[0].Var[0].Value = Uab2In;
    pUnit->VarGroup[0].Var[1].Value = Ubc2In;
    pUnit->VarGroup[0].Var[2].Value = Uca2In;
    pUnit->VarGroup[0].Var[3].Value = Ua2In;
    pUnit->VarGroup[0].Var[4].Value = Ub2In;
    pUnit->VarGroup[0].Var[5].Value = Uc2In;
    pUnit->VarGroup[0].Var[6].Value = Uo2In;
  }
//-----------1T-I段----------//
  {
    BOOL NoVoltage = FALSE;

    POSITION pos = g_Path1B35.GetHeadPosition();
    while(pos)
    {
      struct TagPowerPathStruct *pPowerPath = (TagPowerPathStruct *)g_Path1B35.GetNext(pos);
      if(pPowerPath->UnitID>0)
      {
        struct TagUnitStruct *pUnit = NULL;
        pUnit = &g_Unit[pPowerPath->UnitID];
        if(pUnit->VarGroup[1].Var[pPowerPath->VarID].Value==0)//分位
        {
          NoVoltage = TRUE;
          break;
        }
      }
    }

    if(NoVoltage==FALSE)
    {
      Ua1B = Ua1T / (float)110 * (float)35;
      Ub1B = Ub1T / (float)110 * (float)35;
      Uc1B = Uc1T / (float)110 * (float)35;
      Uab1B = Uab1T / (float)110 * (float)35;
      Ubc1B = Ubc1T / (float)110 * (float)35;
      Uca1B = Uca1T / (float)110 * (float)35;
      Uo1B = Uo1T / (float)110 * (float)35;
    }

    Ua1B = Ua1B+(Ua1B*(float)(rand()%100)/(float)100*g_InlineVotlage.T1LoVoltageChangeRate);
    Ub1B = Ub1B+(Ub1B*(float)(rand()%100)/(float)100*g_InlineVotlage.T1LoVoltageChangeRate);
    Uc1B = Uc1B+(Uc1B*(float)(rand()%100)/(float)100*g_InlineVotlage.T1LoVoltageChangeRate);
    Uab1B = Uab1B+(Uab1B*(float)(rand()%100)/(float)100*g_InlineVotlage.T1LoVoltageChangeRate);
    Ubc1B = Ubc1B+(Ubc1B*(float)(rand()%100)/(float)100*g_InlineVotlage.T1LoVoltageChangeRate);
    Uca1B = Uca1B+(Uca1B*(float)(rand()%100)/(float)100*g_InlineVotlage.T1LoVoltageChangeRate);

    if((Ua1B>0)||(Ub1B>0)||(Uc1B>0))
      Voltage1B = TRUE;
  }
//-----------2T-II段----------//
  {
    BOOL NoVoltage = FALSE;

    POSITION pos = g_Path2B35.GetHeadPosition();
    while(pos)
    {
      struct TagPowerPathStruct *pPowerPath = (TagPowerPathStruct *)g_Path2B35.GetNext(pos);
      if(pPowerPath->UnitID>0)
      {
        struct TagUnitStruct *pUnit = NULL;
        pUnit = &g_Unit[pPowerPath->UnitID];
        if(pUnit->VarGroup[1].Var[pPowerPath->VarID].Value==0)//分位
        {
          NoVoltage = TRUE;
          break;
        }
      }
    }

    if(NoVoltage==FALSE)
    {
      Ua2B = Ua2T / (float)110 * (float)35;
      Ub2B = Ub2T / (float)110 * (float)35;
      Uc2B = Uc2T / (float)110 * (float)35;
      Uab2B = Uab2T / (float)110 * (float)35;
      Ubc2B = Ubc2T / (float)110 * (float)35;
      Uca2B = Uca2T / (float)110 * (float)35;
      Uo2B = Uo2T / (float)110 * (float)35;
    }

    Ua2B = Ua2B+(Ua2B*(float)(rand()%100)/(float)100*g_InlineVotlage.T2LoVoltageChangeRate);
    Ub2B = Ub2B+(Ub2B*(float)(rand()%100)/(float)100*g_InlineVotlage.T2LoVoltageChangeRate);
    Uc2B = Uc2B+(Uc2B*(float)(rand()%100)/(float)100*g_InlineVotlage.T2LoVoltageChangeRate);
    Uab2B = Uab2B+(Uab2B*(float)(rand()%100)/(float)100*g_InlineVotlage.T2LoVoltageChangeRate);
    Ubc2B = Ubc2B+(Ubc2B*(float)(rand()%100)/(float)100*g_InlineVotlage.T2LoVoltageChangeRate);
    Uca2B = Uca2B+(Uca2B*(float)(rand()%100)/(float)100*g_InlineVotlage.T2LoVoltageChangeRate);

    if((Ua2B>0)||(Ub2B>0)||(Uc2B>0))
      Voltage2B = TRUE;
  }

  if((g_Power1T==TRUE)&&(g_Power2T==TRUE)&&(g_PowerBus==FALSE))//1T、2T运行分列
  {
    ;
  }
  if((g_Power1T==TRUE)&&(g_Power2T==FALSE)&&(g_PowerBus==TRUE))//1T并列运行
  {
    Ua2B = Ua1B;
    Ub2B = Ub1B;
    Uc2B = Uc1B;
    Uab2B = Uab1B;
    Ubc2B = Ubc1B;
    Uca2B = Uca1B;
    Uo2B = Uo1B;
  }
  if((g_Power1T==FALSE)&&(g_Power2T==TRUE)&&(g_PowerBus==TRUE))//2T并列运行
  {
    Ua1B = Ua2B;
    Ub1B = Ub2B;
    Uc1B = Uc2B;
    Uab1B = Uab2B;
    Ubc1B = Ubc2B;
    Uca1B = Uca2B;
    Uo1B = Uo2B;
  }

  TagUnitStruct *pUnit1 = &g_Unit[g_UnitID1TLo];
  pUnit1->VarGroup[0].Var[0].Value = Uab1B;
  pUnit1->VarGroup[0].Var[1].Value = Ubc1B;
  pUnit1->VarGroup[0].Var[2].Value = Uca1B;
  pUnit1->VarGroup[0].Var[3].Value = Ua1B;
  pUnit1->VarGroup[0].Var[4].Value = Ub1B;
  pUnit1->VarGroup[0].Var[5].Value = Uc1B;
  pUnit1->VarGroup[0].Var[6].Value = Uo1B;

  TagUnitStruct *pUnit2 = &g_Unit[g_UnitID2TLo];
  pUnit2->VarGroup[0].Var[0].Value = Uab2B;
  pUnit2->VarGroup[0].Var[1].Value = Ubc2B;
  pUnit2->VarGroup[0].Var[2].Value = Uca2B;
  pUnit2->VarGroup[0].Var[3].Value = Ua2B;
  pUnit2->VarGroup[0].Var[4].Value = Ub2B;
  pUnit2->VarGroup[0].Var[5].Value = Uc2B;
  pUnit2->VarGroup[0].Var[6].Value = Uo2B;

//-----------I段馈线----------//
  {
    struct TagUnitStruct *pUnit1B = (TagUnitStruct *)&g_Unit[g_UnitID1TLo];

    POSITION pos = g_1B35FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_1B35FeedList.GetNext(pos);
      if(pUnit->UnitID>0)
      {
        pUnit->VotlageValue = 0;
        
        pUnit->VarGroup[0].Var[0].Value = pUnit1B->VarGroup[0].Var[0].Value;
        pUnit->VarGroup[0].Var[1].Value = pUnit1B->VarGroup[0].Var[1].Value;
        pUnit->VarGroup[0].Var[2].Value = pUnit1B->VarGroup[0].Var[2].Value;
        pUnit->VarGroup[0].Var[3].Value = pUnit1B->VarGroup[0].Var[3].Value;
        pUnit->VarGroup[0].Var[4].Value = pUnit1B->VarGroup[0].Var[4].Value;
        pUnit->VarGroup[0].Var[5].Value = pUnit1B->VarGroup[0].Var[5].Value;
        pUnit->VarGroup[0].Var[6].Value = pUnit1B->VarGroup[0].Var[6].Value;

        if(pUnit->UnitType==4)//线路保护
        {
          pUnit->VarGroup[1].Var[17].Value = 0;
          if((pUnit->VarGroup[0].Var[0].Value>0)&&(pUnit->VarGroup[0].Var[1].Value>0)&&(pUnit->VarGroup[0].Var[2].Value>0))
          {
            if((pUnit->VarGroup[1].Var[0].Value!=0)&&(pUnit->VarGroup[1].Var[5].Value!=0))
              pUnit->VarGroup[1].Var[17].Value = 1;//判断有电
          }
        }
      }
    }
  }
//-----------II段馈线----------//
  {
    struct TagUnitStruct *pUnit2B = (TagUnitStruct *)&g_Unit[g_UnitID2TLo];

    POSITION pos = g_2B35FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_2B35FeedList.GetNext(pos);
      if(pUnit->UnitID>0)
      {
        pUnit->VotlageValue = 0;
        
        pUnit->VarGroup[0].Var[0].Value = pUnit2B->VarGroup[0].Var[0].Value;
        pUnit->VarGroup[0].Var[1].Value = pUnit2B->VarGroup[0].Var[1].Value;
        pUnit->VarGroup[0].Var[2].Value = pUnit2B->VarGroup[0].Var[2].Value;
        pUnit->VarGroup[0].Var[3].Value = pUnit2B->VarGroup[0].Var[3].Value;
        pUnit->VarGroup[0].Var[4].Value = pUnit2B->VarGroup[0].Var[4].Value;
        pUnit->VarGroup[0].Var[5].Value = pUnit2B->VarGroup[0].Var[5].Value;
        pUnit->VarGroup[0].Var[6].Value = pUnit2B->VarGroup[0].Var[6].Value;

        if(pUnit->UnitType==4)//线路保护
        {
          pUnit->VarGroup[1].Var[17].Value = 0;
          if((pUnit->VarGroup[0].Var[0].Value>0)&&(pUnit->VarGroup[0].Var[1].Value>0)&&(pUnit->VarGroup[0].Var[2].Value>0))
          {
            if((pUnit->VarGroup[1].Var[0].Value!=0)&&(pUnit->VarGroup[1].Var[5].Value!=0))
              pUnit->VarGroup[1].Var[17].Value = 1;//判断有电
          }
        }
      }
    }
  }
}

void CPowerThread::ProcessPowerPTVotlage()
{
  if(CTime::GetCurrentTime()<g_PTVoltage.BeginTime)
    return;

  float VoltageValueK[4];
  float CurrentMaxValue[4];
  struct TagUnitStruct *pUnit[4];

  VoltageValueK[0] = g_PTVoltage.T1HiVoltageValueK;
  VoltageValueK[1] = g_PTVoltage.T2HiVoltageValueK;
  VoltageValueK[2] = g_PTVoltage.T1LoVoltageValueK;
  VoltageValueK[3] = g_PTVoltage.T2LoVoltageValueK;

  CurrentMaxValue[0] = g_PTVoltage.T1HiCurrentMaxValue;
  CurrentMaxValue[1] = g_PTVoltage.T2HiCurrentMaxValue;
  CurrentMaxValue[2] = g_PTVoltage.T1LoCurrentMaxValue;
  CurrentMaxValue[3] = g_PTVoltage.T2LoCurrentMaxValue;

  pUnit[0] = &g_Unit[g_UnitID1THi];
  pUnit[1] = &g_Unit[g_UnitID2THi];
  pUnit[2] = &g_Unit[g_UnitID1TLo];
  pUnit[3] = &g_Unit[g_UnitID2TLo];

  for(int no=0;no<4;no++)
  {
    pUnit[no]->VarGroup[0].Var[0].Value *= VoltageValueK[no];
    pUnit[no]->VarGroup[0].Var[1].Value *= VoltageValueK[no];
    pUnit[no]->VarGroup[0].Var[2].Value *= VoltageValueK[no];
    pUnit[no]->VarGroup[0].Var[3].Value *= VoltageValueK[no];
    pUnit[no]->VarGroup[0].Var[4].Value *= VoltageValueK[no];
    pUnit[no]->VarGroup[0].Var[5].Value *= VoltageValueK[no];
    pUnit[no]->VarGroup[0].Var[6].Value *= VoltageValueK[no];

    if(CurrentMaxValue[no]>0)
    {
      float Ua = pUnit[no]->VarGroup[0].Var[3].Value;
      float Ub = pUnit[no]->VarGroup[0].Var[4].Value;
      float Uc = pUnit[no]->VarGroup[0].Var[5].Value;

      float Ia = pUnit[no]->VarGroup[0].Var[7].Value;
      float Ib = pUnit[no]->VarGroup[0].Var[8].Value;
      float Ic = pUnit[no]->VarGroup[0].Var[9].Value;

      float Ias = Ia / CurrentMaxValue[no];
      float Ibs = Ib / CurrentMaxValue[no];
      float Ics = Ic / CurrentMaxValue[no];
    
      if(Ias<0)
        Ias = 0;
      if(Ias>1)
        Ias = 1;
      if(Ibs<0)
        Ibs = 0;
      if(Ics>1)
        Ics = 1;
      Ias = 1 - Ias;
      Ibs = 1 - Ibs;
      Ics = 1 - Ics;
    
      Ua = Ua * Ias;
      Ub = Ub * Ibs;
      Uc = Uc * Ics;

      float Uab = (float)::sqrt(Ua*Ua + Ub*Ub + Ua*Ub);
      float Ubc = (float)::sqrt(Ub*Ub + Uc*Uc + Ub*Uc);
      float Uca = (float)::sqrt(Uc*Uc + Ua*Ua + Uc*Ua);

      pUnit[no]->VarGroup[0].Var[0].Value = Uab;
      pUnit[no]->VarGroup[0].Var[1].Value = Ubc;
      pUnit[no]->VarGroup[0].Var[2].Value = Uca;
      pUnit[no]->VarGroup[0].Var[3].Value = Ua;
      pUnit[no]->VarGroup[0].Var[4].Value = Ub;
      pUnit[no]->VarGroup[0].Var[5].Value = Uc;
    }
  }
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
    POSITION pos = FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)FeedList.GetNext(pos);
      if(pUnit->UnitID>0)
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
      }
    }
  }
}

void CPowerThread::ProcessPowerCurrent()
{
  if(CTime::GetCurrentTime()<g_Current.BeginTime)
    return;

  g_Unit[1].VarGroup[0].Var[0].Value = 0;
  g_Unit[1].VarGroup[0].Var[1].Value = 0;
  g_Unit[1].VarGroup[0].Var[2].Value = 0;
  g_Unit[1].VarGroup[0].Var[3].Value = 0;
  g_Unit[1].VarGroup[0].Var[4].Value = 0;
  g_Unit[4].VarGroup[0].Var[0].Value = 0;
  g_Unit[4].VarGroup[0].Var[1].Value = 0;
  g_Unit[4].VarGroup[0].Var[2].Value = 0;
  g_Unit[4].VarGroup[0].Var[3].Value = 0;
  g_Unit[4].VarGroup[0].Var[4].Value = 0;

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

  Uab1B = g_Unit[g_UnitID1TLo].VarGroup[0].Var[0].Value;
  Ubc1B = g_Unit[g_UnitID1TLo].VarGroup[0].Var[1].Value;
  Uca1B = g_Unit[g_UnitID1TLo].VarGroup[0].Var[2].Value;
  Ua1B = g_Unit[g_UnitID1TLo].VarGroup[0].Var[3].Value;
  Ub1B = g_Unit[g_UnitID1TLo].VarGroup[0].Var[4].Value;
  Uc1B = g_Unit[g_UnitID1TLo].VarGroup[0].Var[5].Value;
  Uo1B = g_Unit[g_UnitID1TLo].VarGroup[0].Var[6].Value;

  Uab2B = g_Unit[g_UnitID2TLo].VarGroup[0].Var[0].Value;
  Ubc2B = g_Unit[g_UnitID2TLo].VarGroup[0].Var[1].Value;
  Uca2B = g_Unit[g_UnitID2TLo].VarGroup[0].Var[2].Value;
  Ua2B = g_Unit[g_UnitID2TLo].VarGroup[0].Var[3].Value;
  Ub2B = g_Unit[g_UnitID2TLo].VarGroup[0].Var[4].Value;
  Uc2B = g_Unit[g_UnitID2TLo].VarGroup[0].Var[5].Value;
  Uo2B = g_Unit[g_UnitID2TLo].VarGroup[0].Var[6].Value;
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

//-----------1T低后备电流计算----------//
  {
    float TotalIa_r = 0;
    float TotalIb_r = 0;
    float TotalIc_r = 0;
    float TotalIa_x = 0;
    float TotalIb_x = 0;
    float TotalIc_x = 0;
    POSITION pos = g_1B35FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_1B35FeedList.GetNext(pos);
      if((pUnit->UnitID>0)&&(pUnit->UnitType==4))
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

    g_Unit[g_UnitID1TLo].VarGroup[0].Var[7].Value = TotalIa1B;
    g_Unit[g_UnitID1TLo].VarGroup[0].Var[8].Value = TotalIb1B;
    g_Unit[g_UnitID1TLo].VarGroup[0].Var[9].Value = TotalIc1B;
    g_Unit[g_UnitID1TLo].VarGroup[0].Var[11].Value = TotalP1B;
    g_Unit[g_UnitID1TLo].VarGroup[0].Var[12].Value = TotalQ1B;
    g_Unit[g_UnitID1TLo].VarGroup[0].Var[13].Value = Totalcos1B;
    g_Unit[g_UnitID1TLo].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);
  }
//-----------1T低后备电流计算----------//
//-----------2T低后备电流计算----------//
  {
    float TotalIa_r = 0;
    float TotalIb_r = 0;
    float TotalIc_r = 0;
    float TotalIa_x = 0;
    float TotalIb_x = 0;
    float TotalIc_x = 0;
    POSITION pos = g_2B35FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_2B35FeedList.GetNext(pos);
      if((pUnit->UnitID>0)&&(pUnit->UnitType==4))
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

    g_Unit[g_UnitID2TLo].VarGroup[0].Var[7].Value = TotalIa2B;
    g_Unit[g_UnitID2TLo].VarGroup[0].Var[8].Value = TotalIb2B;
    g_Unit[g_UnitID2TLo].VarGroup[0].Var[9].Value = TotalIc2B;
    g_Unit[g_UnitID2TLo].VarGroup[0].Var[11].Value = TotalP2B;
    g_Unit[g_UnitID2TLo].VarGroup[0].Var[12].Value = TotalQ2B;
    g_Unit[g_UnitID2TLo].VarGroup[0].Var[13].Value = Totalcos2B;
    g_Unit[g_UnitID2TLo].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);
  }
//-----------2T低后备电流计算----------//

//-----------高后备电流计算----------//
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

  if((g_Power1T==TRUE)&&(g_Power2T==TRUE)&&(g_PowerBus==FALSE))//分列运行
  {
    g_Unit[g_UnitID1THi].VarGroup[0].Var[7].Value = TotalIa1B / (float)110 * (float)35;
    g_Unit[g_UnitID1THi].VarGroup[0].Var[8].Value = TotalIb1B / (float)110 * (float)35;
    g_Unit[g_UnitID1THi].VarGroup[0].Var[9].Value = TotalIc1B / (float)110 * (float)35;
    g_Unit[g_UnitID1THi].VarGroup[0].Var[11].Value = TotalP1B;
    g_Unit[g_UnitID1THi].VarGroup[0].Var[12].Value = TotalQ1B;
    g_Unit[g_UnitID1THi].VarGroup[0].Var[13].Value = Totalcos1B;
    g_Unit[g_UnitID1THi].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);

    g_Unit[g_UnitID2THi].VarGroup[0].Var[7].Value = TotalIa2B / (float)110 * (float)35;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[8].Value = TotalIb2B / (float)110 * (float)35;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[9].Value = TotalIc2B / (float)110 * (float)35;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[11].Value = TotalP2B;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[12].Value = TotalQ2B;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[13].Value = Totalcos2B;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);
    
    g_Unit[g_UnitIDBus35].VarGroup[0].Var[7].Value = 0;
    g_Unit[g_UnitIDBus35].VarGroup[0].Var[8].Value = 0;
    g_Unit[g_UnitIDBus35].VarGroup[0].Var[9].Value = 0;
  }
  if((g_Power1T==TRUE)&&(g_Power2T==FALSE)&&(g_PowerBus==TRUE))//1T运行
  {
    g_Unit[g_UnitID1THi].VarGroup[0].Var[7].Value = TotalIa / (float)110 * (float)35;
    g_Unit[g_UnitID1THi].VarGroup[0].Var[8].Value = TotalIb / (float)110 * (float)35;
    g_Unit[g_UnitID1THi].VarGroup[0].Var[9].Value = TotalIc / (float)110 * (float)35;
    g_Unit[g_UnitID1THi].VarGroup[0].Var[11].Value = TotalP;
    g_Unit[g_UnitID1THi].VarGroup[0].Var[12].Value = TotalQ;
    g_Unit[g_UnitID1THi].VarGroup[0].Var[13].Value = Totalcos;
    g_Unit[g_UnitID1THi].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);

    g_Unit[g_UnitIDBus35].VarGroup[0].Var[7].Value = g_TotalIa2B;
    g_Unit[g_UnitIDBus35].VarGroup[0].Var[8].Value = g_TotalIb2B;
    g_Unit[g_UnitIDBus35].VarGroup[0].Var[9].Value = g_TotalIc2B;

    g_Unit[g_UnitID2THi].VarGroup[0].Var[7].Value = 0;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[8].Value = 0;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[9].Value = 0;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[11].Value = 0;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[12].Value = 0;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[13].Value = 0;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);
  }
  if((g_Power1T==FALSE)&&(g_Power2T==TRUE)&&(g_PowerBus==TRUE))//2T运行
  {
    g_Unit[g_UnitID1THi].VarGroup[0].Var[7].Value = 0;
    g_Unit[g_UnitID1THi].VarGroup[0].Var[8].Value = 0;
    g_Unit[g_UnitID1THi].VarGroup[0].Var[9].Value = 0;
    g_Unit[g_UnitID1THi].VarGroup[0].Var[11].Value = 0;
    g_Unit[g_UnitID1THi].VarGroup[0].Var[12].Value = 0;
    g_Unit[g_UnitID1THi].VarGroup[0].Var[13].Value = 0;
    g_Unit[g_UnitID1THi].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);

    g_Unit[g_UnitIDBus35].VarGroup[0].Var[7].Value = g_TotalIa1B;
    g_Unit[g_UnitIDBus35].VarGroup[0].Var[8].Value = g_TotalIb1B;
    g_Unit[g_UnitIDBus35].VarGroup[0].Var[9].Value = g_TotalIc1B;

    g_Unit[g_UnitID2THi].VarGroup[0].Var[7].Value = TotalIa / (float)110 * (float)35;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[8].Value = TotalIb / (float)110 * (float)35;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[9].Value = TotalIc / (float)110 * (float)35;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[11].Value = TotalP;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[12].Value = TotalQ;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[13].Value = Totalcos;
    g_Unit[g_UnitID2THi].VarGroup[0].Var[14].Value = 50+((float)(rand()%100)/(float)100000);

  }
//-----------高后备电流计算----------//

//-----------电容器电流计算----------//
  float TotalCompensationIa1B = 0;
  float TotalCompensationIb1B = 0;
  float TotalCompensationIc1B = 0;
  float TotalCompensationQ1B = 0;

  float TotalCompensationIa2B = 0;
  float TotalCompensationIb2B = 0;
  float TotalCompensationIc2B = 0;
  float TotalCompensationQ2B = 0;

  for(int sectionno=0;sectionno<2;sectionno++)
  {
    float Ua = 0;
    float Ub = 0;
    float Uc = 0;
    float Iax = 0;
    float Ibx = 0;
    float Icx = 0;
    float TotalIa = 0;
    float TotalIb = 0;
    float TotalIc = 0;
    float TotalQ = 0;

    if(sectionno==0)//I段
    {
      Ua = Ua1B;
      Ub = Ub1B;
      Uc = Uc1B;
      Iax = TotalIa1B*(float)::sqrt(1-Totalcos1B*Totalcos1B);
      Ibx = TotalIb1B*(float)::sqrt(1-Totalcos1B*Totalcos1B);
      Icx = TotalIc1B*(float)::sqrt(1-Totalcos1B*Totalcos1B);
    }
    if(sectionno==1)//II段
    {
      Ua = Ua2B;
      Ub = Ub2B;
      Uc = Uc2B;
      Iax = TotalIa2B*(float)::sqrt(1-Totalcos2B*Totalcos2B);
      Ibx = TotalIb2B*(float)::sqrt(1-Totalcos2B*Totalcos2B);
      Icx = TotalIc2B*(float)::sqrt(1-Totalcos2B*Totalcos2B);
    }

    switch(g_Compensation[sectionno].CompensationType)
    {
      case 0://常规
      {
        CPtrList PowerPathVarList[4];
        for(int branchno=0;branchno<4;branchno++)
          ::GetVarListFromVarListInfo(PowerPathVarList[branchno],g_Compensation[sectionno].CompensationBranchPowerPathVarList[branchno],1);

        for(branchno=0;branchno<4;branchno++)
        {
          if(PowerPathVarList[branchno].GetCount()==0)
            continue;

          float CompensationUa = Ua;
          float CompensationUb = Ub;
          float CompensationUc = Uc;
          float CompensationIa = 0;
          float CompensationIb = 0;
          float CompensationIc = 0;
          float CompensationQ = 0;
          POSITION pos = PowerPathVarList[branchno].GetHeadPosition();
          while(pos)
          {
            TagVarStruct *pVar = (TagVarStruct*)PowerPathVarList[branchno].GetNext(pos);
            if(pVar->Value==0)
            {
              CompensationUa = 0;
              CompensationUb = 0;
              CompensationUc = 0;
            }
          }
          CompensationIa = CompensationUa * 1000 / g_Compensation[sectionno].CompensationBranchValueZ[branchno] * g_Compensation[sectionno].CompensationBranchValueK[branchno];
          CompensationIb = CompensationUb * 1000 / g_Compensation[sectionno].CompensationBranchValueZ[branchno] * g_Compensation[sectionno].CompensationBranchValueK[branchno];
          CompensationIc = CompensationUc * 1000 / g_Compensation[sectionno].CompensationBranchValueZ[branchno] * g_Compensation[sectionno].CompensationBranchValueK[branchno];
          CompensationQ = (CompensationUa * CompensationIa) + (CompensationUb * CompensationIb)+ (CompensationUc * CompensationIc);

          TagVarStruct *pVarI = ::GetVarFromVarInfo(g_Compensation[sectionno].CompensationBranchVarI[branchno],1);
          if(pVarI)//给支路电流赋值
          {
            WORD UnitID = pVarI->UnitID;
            g_Unit[UnitID].VarGroup[0].Var[7].Value = CompensationIa;
            g_Unit[UnitID].VarGroup[0].Var[8].Value = CompensationIb;
            g_Unit[UnitID].VarGroup[0].Var[9].Value = CompensationIc;
          }

          TagVarStruct *pVarQ = ::GetVarFromVarInfo(g_Compensation[sectionno].CompensationBranchVarQ[branchno],1);
          if(pVarQ)//给支路无功赋值
            pVarQ->Value = CompensationQ;

          TotalIa += CompensationIa;
          TotalIb += CompensationIb;
          TotalIc += CompensationIc;
          TotalQ += CompensationQ;
        }
        if(g_Compensation[sectionno].CompensationUnitID>0)//给保护装置赋值
        {
          g_Unit[g_Compensation[sectionno].CompensationUnitID].VarGroup[0].Var[7].Value = (float)fabs(TotalIa);
          g_Unit[g_Compensation[sectionno].CompensationUnitID].VarGroup[0].Var[8].Value = (float)fabs(TotalIb);
          g_Unit[g_Compensation[sectionno].CompensationUnitID].VarGroup[0].Var[9].Value = (float)fabs(TotalIc);
          g_Unit[g_Compensation[sectionno].CompensationUnitID].VarGroup[0].Var[12].Value = (float)fabs(TotalQ);
          g_Unit[g_Compensation[sectionno].CompensationUnitID].VarGroup[0].Var[13].Value = (float)1.0;
        }
        break;
      }
      case 1://SVC
      {
        float SVCCompensationIa[4];//各支路补偿电流
        float SVCCompensationIb[4];//各支路补偿电流
        float SVCCompensationIc[4];//各支路补偿电流
        float SVCCompensationQ[4];//各支路补偿无功
        CPtrList PowerPathVarList[4];
        for(int branchno=0;branchno<4;branchno++)
        {
          SVCCompensationIa[branchno] = 0;
          SVCCompensationIb[branchno] = 0;
          SVCCompensationIc[branchno] = 0;
          SVCCompensationQ[branchno] = 0;
          ::GetVarListFromVarListInfo(PowerPathVarList[branchno],g_Compensation[sectionno].CompensationBranchPowerPathVarList[branchno],1);
        }

        for(branchno=1;branchno<4;branchno++)
        {
          if(PowerPathVarList[branchno].GetCount()==0)
            continue;

          float CompensationUa = Ua;
          float CompensationUb = Ub;
          float CompensationUc = Uc;
          float CompensationIa = 0;
          float CompensationIb = 0;
          float CompensationIc = 0;
          float CompensationQ = 0;
          POSITION pos = PowerPathVarList[branchno].GetHeadPosition();
          while(pos)
          {
            TagVarStruct *pVar = (TagVarStruct*)PowerPathVarList[branchno].GetNext(pos);
            if(pVar->Value==0)
            {
              CompensationUa = 0;
              CompensationUb = 0;
              CompensationUc = 0;
            }
          }
          CompensationIa = CompensationUa * 1000 / g_Compensation[sectionno].CompensationBranchValueZ[branchno] * g_Compensation[sectionno].CompensationBranchValueK[branchno];
          CompensationIb = CompensationUb * 1000 / g_Compensation[sectionno].CompensationBranchValueZ[branchno] * g_Compensation[sectionno].CompensationBranchValueK[branchno];
          CompensationIc = CompensationUc * 1000 / g_Compensation[sectionno].CompensationBranchValueZ[branchno] * g_Compensation[sectionno].CompensationBranchValueK[branchno];
          CompensationQ = (CompensationUa * CompensationIa) + (CompensationUb * CompensationIb)+ (CompensationUc * CompensationIc);

          TotalIa += CompensationIa;
          TotalIb += CompensationIb;
          TotalIc += CompensationIc;
          TotalQ += CompensationQ;

          SVCCompensationIa[branchno] = CompensationIa;
          SVCCompensationIb[branchno] = CompensationIb;
          SVCCompensationIc[branchno] = CompensationIc;
          SVCCompensationQ[branchno] = CompensationQ;
        }

        if(PowerPathVarList[sectionno].GetCount()>0)//TCR
        {
          float CompensationUa = Ua;
          float CompensationUb = Ub;
          float CompensationUc = Uc;
          float CompensationIa = 0;
          float CompensationIb = 0;
          float CompensationIc = 0;
          float CompensationQ = 0;
          POSITION pos = PowerPathVarList[0].GetHeadPosition();
          while(pos)
          {
            TagVarStruct *pVar = (TagVarStruct*)PowerPathVarList[0].GetNext(pos);
            if(pVar->Value==0)
            {
              CompensationUa = 0;
              CompensationUb = 0;
              CompensationUc = 0;
            }
          }
          if(Iax>TotalIa)//负荷电流大于补偿电流
            Iax = TotalIa;
          if(Ibx>TotalIb)//负荷电流大于补偿电流
            Ibx = TotalIb;
          if(Icx>TotalIc)//负荷电流大于补偿电流
            Icx = TotalIc;

          CompensationIa = Iax * g_Compensation[sectionno].CompensationBranchValueK[0] - TotalIa;
          CompensationIb = Ibx * g_Compensation[sectionno].CompensationBranchValueK[0] - TotalIb;
          CompensationIc = Icx * g_Compensation[sectionno].CompensationBranchValueK[0] - TotalIc;
          CompensationIa += CompensationIa*(float)(rand()%100)/(float)100*(float)0.01;
          CompensationIb += CompensationIb*(float)(rand()%100)/(float)100*(float)0.01;
          CompensationIc += CompensationIc*(float)(rand()%100)/(float)100*(float)0.01;
          CompensationQ = (CompensationUa * CompensationIa) + (CompensationUb * CompensationIb)+ (CompensationUc * CompensationIc);

          TotalIa += CompensationIa;
          TotalIb += CompensationIb;
          TotalIc += CompensationIc;
          TotalQ += CompensationQ;

          SVCCompensationIa[0] = CompensationIa;
          SVCCompensationIb[0] = CompensationIb;
          SVCCompensationIc[0] = CompensationIc;
          SVCCompensationQ[0] = CompensationQ;
        }

        for(branchno=0;branchno<4;branchno++)
        {
          TagVarStruct *pVarI = ::GetVarFromVarInfo(g_Compensation[sectionno].CompensationBranchVarI[branchno],0);
          if(pVarI)//清除支路电流
          {
            WORD UnitID = pVarI->UnitID;
            g_Unit[UnitID].VarGroup[0].Var[7].Value = 0;
            g_Unit[UnitID].VarGroup[0].Var[8].Value = 0;
            g_Unit[UnitID].VarGroup[0].Var[9].Value = 0;
          }

          TagVarStruct *pVarQ = ::GetVarFromVarInfo(g_Compensation[sectionno].CompensationBranchVarQ[branchno],0);
          if(pVarQ)//清除支路无功
            pVarQ->Value = 0;
        }
        for(branchno=0;branchno<4;branchno++)
        {
          TagVarStruct *pVarI = ::GetVarFromVarInfo(g_Compensation[sectionno].CompensationBranchVarI[branchno],0);
          if(pVarI)//给支路电流赋值
          {
            WORD UnitID = pVarI->UnitID;
            g_Unit[UnitID].VarGroup[0].Var[7].Value += SVCCompensationIa[branchno];
            g_Unit[UnitID].VarGroup[0].Var[8].Value += SVCCompensationIb[branchno];
            g_Unit[UnitID].VarGroup[0].Var[9].Value += SVCCompensationIc[branchno];
          }

          TagVarStruct *pVarQ = ::GetVarFromVarInfo(g_Compensation[sectionno].CompensationBranchVarQ[branchno],0);
          if(pVarQ)//给支路无功赋值
            pVarQ->Value += SVCCompensationQ[branchno];
        }
        for(branchno=0;branchno<4;branchno++)
        {
          TagVarStruct *pVarI = ::GetVarFromVarInfo(g_Compensation[sectionno].CompensationBranchVarI[branchno],0);
          if(pVarI)//给支路电流去符号
          {
            WORD UnitID = pVarI->UnitID;
            g_Unit[UnitID].VarGroup[0].Var[7].Value = (float)fabs(g_Unit[UnitID].VarGroup[0].Var[7].Value);
            g_Unit[UnitID].VarGroup[0].Var[8].Value = (float)fabs(g_Unit[UnitID].VarGroup[0].Var[8].Value);
            g_Unit[UnitID].VarGroup[0].Var[9].Value = (float)fabs(g_Unit[UnitID].VarGroup[0].Var[9].Value);
          }

          TagVarStruct *pVarQ = ::GetVarFromVarInfo(g_Compensation[sectionno].CompensationBranchVarQ[branchno],0);
          if(pVarQ)//给支路无功去符号
            pVarQ->Value += (float)fabs(pVarQ->Value);
        }

        if(g_Compensation[sectionno].CompensationUnitID>0)//给保护装置赋值
        {
          g_Unit[g_Compensation[sectionno].CompensationUnitID].VarGroup[0].Var[7].Value = (float)fabs(TotalIa);
          g_Unit[g_Compensation[sectionno].CompensationUnitID].VarGroup[0].Var[8].Value = (float)fabs(TotalIb);
          g_Unit[g_Compensation[sectionno].CompensationUnitID].VarGroup[0].Var[9].Value = (float)fabs(TotalIc);
          g_Unit[g_Compensation[sectionno].CompensationUnitID].VarGroup[0].Var[12].Value = (float)fabs(TotalQ);
          g_Unit[g_Compensation[sectionno].CompensationUnitID].VarGroup[0].Var[13].Value = (float)1.0;
        }
        break;
      }
      case 2://SVG
        break;
    }

    if(sectionno==0)//I段
    {
      TotalCompensationIa1B += TotalIa;
      TotalCompensationIb1B += TotalIb;
      TotalCompensationIc1B += TotalIc;
      TotalCompensationQ1B += TotalQ;
    }
    if(sectionno==1)//II段
    {
      TotalCompensationIa2B += TotalIa;
      TotalCompensationIb2B += TotalIb;
      TotalCompensationIc2B += TotalIc;
      TotalCompensationQ2B += TotalQ;
    }
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
        if(no>0)
        {
          if(g_DataExchangeList[no].UnitID==g_DataExchangeList[no-1].UnitID)
            continue;
        }

        TagUnitStruct *pUnit = (TagUnitStruct *)&g_Unit[g_DataExchangeList[no].UnitID];

        pUnit->CurrentValue = g_DataExchangeList[no].AverCurrent;
        pUnit->AngleValue = g_DataExchangeList[no].AverAngle;

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

        float Ia = pUnit->CurrentValue*CurrentChangeRate[TimeID]+(pUnit->CurrentValue*CurrentChangeRate[TimeID]*(float)(rand()%100)/(float)100*pTransmission->CurrentChangeRate);
        float Ib = pUnit->CurrentValue*CurrentChangeRate[TimeID]+(pUnit->CurrentValue*CurrentChangeRate[TimeID]*(float)(rand()%100)/(float)100*pTransmission->CurrentChangeRate);
        float Ic = pUnit->CurrentValue*CurrentChangeRate[TimeID]+(pUnit->CurrentValue*CurrentChangeRate[TimeID]*(float)(rand()%100)/(float)100*pTransmission->CurrentChangeRate);
        float Pa = pUnit->VarGroup[0].Var[3].Value*Ia*(float)cos((float)3.1415926*pUnit->AngleValue/(float)180)*3;
        float Pb = pUnit->VarGroup[0].Var[4].Value*Ib*(float)cos((float)3.1415926*pUnit->AngleValue/(float)180)*3;
        float Pc = pUnit->VarGroup[0].Var[5].Value*Ic*(float)cos((float)3.1415926*pUnit->AngleValue/(float)180)*3;
        float Qa = pUnit->VarGroup[0].Var[3].Value*Ia*(float)sin((float)3.1415926*pUnit->AngleValue/(float)180)*3;
        float Qb = pUnit->VarGroup[0].Var[4].Value*Ib*(float)sin((float)3.1415926*pUnit->AngleValue/(float)180)*3;
        float Qc = pUnit->VarGroup[0].Var[5].Value*Ic*(float)sin((float)3.1415926*pUnit->AngleValue/(float)180)*3;
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
      }
      else
      {
        FeedValue[pUnit->UnitID][7] = g_Current.NoloadCurrentChangeMaxValue*(float)(float)(rand()%100)/(float)100;
        FeedValue[pUnit->UnitID][8] = g_Current.NoloadCurrentChangeMaxValue*(float)(float)(rand()%100)/(float)100;
        FeedValue[pUnit->UnitID][9] = g_Current.NoloadCurrentChangeMaxValue*(float)(float)(rand()%100)/(float)100;
        FeedValue[pUnit->UnitID][11] = 0;
        FeedValue[pUnit->UnitID][12] = 0;
        FeedValue[pUnit->UnitID][13] = 1;
        FeedValue[pUnit->UnitID][14] = 50+((float)(rand()%100)/(float)100000);

        pos = g_FreeTransmissionList.Find(pTransmission);
        delete pTransmission;
        g_FreeTransmissionList.RemoveAt(pos);
        pos = g_FreeTransmissionList.GetHeadPosition();
        continue;
      }
    }
  }
/*
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

        if(g_Unit[UnitID].VarGroup[1].Var[15].Value>0)//馈线有电压，可以驶入
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
        if(g_Unit[UnitID].VarGroup[1].Var[15].Value>0)//馈线有电压，可以驶出
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
*/
  CPtrList TotalFeedList;
//-----------A相馈线----------//
  {
    POSITION pos = g_1B35FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_1B35FeedList.GetNext(pos);
      TotalFeedList.AddTail(pUnit);
    }
  }
//-----------B相馈线----------//
  {
    POSITION pos = g_2B35FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)g_2B35FeedList.GetNext(pos);
      TotalFeedList.AddTail(pUnit);
    }
  }
//-----------全部馈线----------//
  {
    POSITION pos = TotalFeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)TotalFeedList.GetNext(pos);
      pUnit->VarGroup[0].Var[7].Value = FeedValue[pUnit->UnitID][7];
      pUnit->VarGroup[0].Var[8].Value = FeedValue[pUnit->UnitID][8];
      pUnit->VarGroup[0].Var[9].Value = FeedValue[pUnit->UnitID][9];
      pUnit->VarGroup[0].Var[11].Value = FeedValue[pUnit->UnitID][11];
      pUnit->VarGroup[0].Var[12].Value = FeedValue[pUnit->UnitID][12];
      pUnit->VarGroup[0].Var[13].Value = FeedValue[pUnit->UnitID][13];
      pUnit->VarGroup[0].Var[14].Value = FeedValue[pUnit->UnitID][14];
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
        float ValueIa = g_Unit[unitno].VarGroup[0].Var[7].Value;
        float ValueIb = g_Unit[unitno].VarGroup[0].Var[8].Value;
        float ValueIc = g_Unit[unitno].VarGroup[0].Var[9].Value;

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

void CPowerThread::ProcessAutoOnOff(WORD UnitID)
{
  static BOOL  LostVotlageStartupState1;     //1#失压启动状态
	static DWORD LostVotlageStartupTickCount1; //1#失压启动计数
  static BOOL  HasVotlageStartupState1;      //1#有压启动状态
	static DWORD HasVotlageStartupTickCount1;  //1#有压启动计数
  static BOOL  LostVotlageStartupState2;     //2#失压启动状态
	static DWORD LostVotlageStartupTickCount2; //2#失压启动计数
  static BOOL  HasVotlageStartupState2;      //2#有压启动状态
	static DWORD HasVotlageStartupTickCount2;  //2#有压启动计数

  BOOL ChargeState1L1T = TRUE;             //1L1T充电
  BOOL ChargeState2L2T = TRUE;             //2L2T充电
  BOOL ChargeStateBus  = TRUE;             //Bus充电

  static BOOL  ChargeStartupState1L1T;     //1L1T充电启动状态
	static DWORD ChargeStartupTickCount1L1T; //1L1T充电启动计数
  static BOOL  ChargeStartupState2L2T;     //2L2T充电启动状态
	static DWORD ChargeStartupTickCount2L2T; //2L2T充电启动计数
  static BOOL  ChargeStartupStateBus;      //Bus充电启动状态
	static DWORD ChargeStartupTickCountBus;  //Bus充电启动计数

  WORD CurrentParamaterGroupID = g_Unit[UnitID].CurrentParamaterGroupID;

  if(g_Unit[1].VarGroup[1].Var[30].Value!=0)//1T差动故障
	  g_Unit[UnitID].VarGroup[1].Var[15].Value = 1;
  else
	  g_Unit[UnitID].VarGroup[1].Var[15].Value = 0;

  if(g_Unit[4].VarGroup[1].Var[30].Value!=0)
	  g_Unit[UnitID].VarGroup[1].Var[16].Value = 1;//2T差动故障
  else
	  g_Unit[UnitID].VarGroup[1].Var[16].Value = 0;

  BOOL To1T1L = FALSE;
  BOOL To2T2L = FALSE;
  BOOL ToBus = FALSE;

  if((g_Unit[UnitID].VarGroup[1].Var[1].Value!=0)&&(g_Unit[UnitID].VarGroup[1].Var[2].Value!=0))//判断自投压板
  {
//-------------------1回失压-----------------//
    if(g_Unit[UnitID].VarGroup[1].Var[18].Value!=0)//2回2T已充电
    {
      if(g_Unit[UnitID].VarGroup[1].Var[22].Value!=0)//分列运行方式
      {
        if((g_Unit[UnitID].VarGroup[1].Var[13].Value!=0)&&(g_Unit[2].VarGroup[1].Var[30].Value!=0))//1#进线失压且保护动作
          To2T2L = TRUE;
      }
    }
//-------------------2回失压-----------------//
    if(g_Unit[UnitID].VarGroup[1].Var[17].Value!=0)//1回1T已充电
    {
      if(g_Unit[UnitID].VarGroup[1].Var[22].Value!=0)//分列运行方式
      {
        if((g_Unit[UnitID].VarGroup[1].Var[14].Value!=0)&&(g_Unit[5].VarGroup[1].Var[30].Value!=0))//2#进线失压且保护动作
          To1T1L = TRUE;
      }
    }
//-------------------1T故障-----------------//
    if(g_Unit[UnitID].VarGroup[1].Var[18].Value!=0)//2回2T已充电
    {
      if(g_Unit[UnitID].VarGroup[1].Var[22].Value!=0)//分列运行方式
      {
        if(g_Unit[UnitID].VarGroup[1].Var[15].Value!=0)//1#主变故障
          To2T2L = TRUE;
      }
    }
//-------------------2T故障-----------------//
    if(g_Unit[UnitID].VarGroup[1].Var[17].Value!=0)//1回1T已充电
    {
      if(g_Unit[UnitID].VarGroup[1].Var[22].Value!=0)//分列运行方式
      {
        if(g_Unit[UnitID].VarGroup[1].Var[16].Value!=0)//2#主变故障
          To1T1L = TRUE;
      }
    }
  }

  if(To1T1L==TRUE)//自投到1L1T
  {
    float ActionValue[PROTECTION_ACTION_VALUE_COUNT];
    for(int varno=0;varno<6;varno++)
      ActionValue[varno] = g_Unit[UnitID].VarGroup[0].Var[varno].Value;

    BOOL VarLock = FALSE;

    if((VarLock==FALSE)&&(g_Unit[5].VarLock[0]==FALSE))
    {
      g_Unit[5].VarGroup[1].Var[0].Value = 0; //2T高压侧跳闸 合位
      g_Unit[5].VarGroup[1].Var[1].Value = 1; //2T高压侧跳闸 分位
    }
    else
      VarLock = TRUE;

    if((VarLock==FALSE)&&(g_Unit[7].VarLock[0]==FALSE))
    {
      ::AddPresetVar(7,1,0,1,CTime::GetCurrentTime()+CTimeSpan(5)); //母联合闸 合位
      ::AddPresetVar(7,1,1,0,CTime::GetCurrentTime()+CTimeSpan(5)); //母联合闸 分位
      ::AddPresetVar(7,1,3,0,CTime::GetCurrentTime()+CTimeSpan(5)); //母联储能
      ::AddPresetVar(7,1,3,1,CTime::GetCurrentTime()+CTimeSpan(15));
    }
    else if(VarLock==FALSE)
    {
      ::ProductSOEReport(UnitID,6,CTime::GetCurrentTime() + CTimeSpan(0,0,0,5),(WORD)::GetTickCount()%1000,1,ActionValue);
      VarLock = TRUE;
    }

    g_Unit[UnitID].VarGroup[1].Var[17].Value = 0; //1回1T自投放电
    g_Unit[UnitID].VarGroup[1].Var[18].Value = 0; //2回2T自投放电
    g_Unit[UnitID].VarGroup[1].Var[19].Value = 0; //Bus自投放电
    ChargeStartupState1L1T = FALSE;
    ChargeStartupTickCount1L1T = 0;
    ChargeStartupState2L2T = FALSE;
    ChargeStartupTickCount2L2T = 0;
    ChargeStartupStateBus = FALSE;
    ChargeStartupTickCountBus = 0;
    
    if(VarLock==FALSE)
      ::ProductSOEReport(UnitID,2,CTime::GetCurrentTime(),(WORD)::GetTickCount()%1000,1,ActionValue);
  }
  if(To2T2L==TRUE)//自投到2L2T
  {
    float ActionValue[PROTECTION_ACTION_VALUE_COUNT];
    for(int varno=0;varno<6;varno++)
      ActionValue[varno] = g_Unit[UnitID].VarGroup[0].Var[varno].Value;

    BOOL VarLock = FALSE;

    if((VarLock==FALSE)&&(g_Unit[2].VarLock[0]==FALSE))
    {
      g_Unit[2].VarGroup[1].Var[0].Value = 0; //1T高压侧跳闸 合位
      g_Unit[2].VarGroup[1].Var[1].Value = 1; //1T高压侧跳闸 分位
    }
    else
      VarLock = TRUE;

    if((VarLock==FALSE)&&(g_Unit[7].VarLock[0]==FALSE))
    {
      ::AddPresetVar(7,1,0,1,CTime::GetCurrentTime()+CTimeSpan(5)); //母联合闸 合位
      ::AddPresetVar(7,1,1,0,CTime::GetCurrentTime()+CTimeSpan(5)); //母联合闸 分位
      ::AddPresetVar(7,1,3,0,CTime::GetCurrentTime()+CTimeSpan(5)); //母联储能
      ::AddPresetVar(7,1,3,1,CTime::GetCurrentTime()+CTimeSpan(15));
    }
    else if(VarLock==FALSE)
    {
      ::ProductSOEReport(UnitID,5,CTime::GetCurrentTime() + CTimeSpan(0,0,0,5),(WORD)::GetTickCount()%1000,1,ActionValue);
      VarLock = TRUE;
    }

    g_Unit[UnitID].VarGroup[1].Var[17].Value = 0; //1回1T自投放电
    g_Unit[UnitID].VarGroup[1].Var[18].Value = 0; //2回2T自投放电
    g_Unit[UnitID].VarGroup[1].Var[19].Value = 0; //Bus自投放电
    ChargeStartupState1L1T = FALSE;
    ChargeStartupTickCount1L1T = 0;
    ChargeStartupState2L2T = FALSE;
    ChargeStartupTickCount2L2T = 0;
    ChargeStartupStateBus = FALSE;
    ChargeStartupTickCountBus = 0;

    if(VarLock==FALSE)
      ::ProductSOEReport(UnitID,1,CTime::GetCurrentTime(),(WORD)::GetTickCount()%1000,1,ActionValue);
  }

	g_Unit[UnitID].VarGroup[0].Var[0].Value = g_Unit[3].VarGroup[0].Var[3].Value;
	g_Unit[UnitID].VarGroup[0].Var[1].Value = g_Unit[3].VarGroup[0].Var[4].Value;
	g_Unit[UnitID].VarGroup[0].Var[2].Value = g_Unit[3].VarGroup[0].Var[5].Value;
	g_Unit[UnitID].VarGroup[0].Var[3].Value = g_Unit[6].VarGroup[0].Var[3].Value;
	g_Unit[UnitID].VarGroup[0].Var[4].Value = g_Unit[6].VarGroup[0].Var[4].Value;
	g_Unit[UnitID].VarGroup[0].Var[5].Value = g_Unit[6].VarGroup[0].Var[5].Value;

	BOOL b1L1TMode = TRUE;
	BOOL b2L2TMode = TRUE;
	BOOL bBusMode = TRUE;

  {
    POSITION pos = g_1L1TAutoModeOnList.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath  = (TagPowerPathStruct *)g_1L1TAutoModeOnList.GetNext(pos);
      if(g_Unit[pPowerPath->UnitID].VarGroup[1].Var[pPowerPath->VarID].Value==0)//合列表中遥信非合
      {
        b1L1TMode = FALSE;
        break;
      }
    }
  }
  {
    POSITION pos = g_1L1TAutoModeOffList.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath  = (TagPowerPathStruct *)g_1L1TAutoModeOffList.GetNext(pos);
      if(g_Unit[pPowerPath->UnitID].VarGroup[1].Var[pPowerPath->VarID].Value!=0)//分列表中遥信非分
      {
        b1L1TMode = FALSE;
        break;
      }
    }
  }
  {
    POSITION pos = g_2L2TAutoModeOnList.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath  = (TagPowerPathStruct *)g_2L2TAutoModeOnList.GetNext(pos);
      if(g_Unit[pPowerPath->UnitID].VarGroup[1].Var[pPowerPath->VarID].Value==0)//合列表中遥信非合
      {
        b2L2TMode = FALSE;
        break;
      }
    }
  }
  {
    POSITION pos = g_2L2TAutoModeOffList.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath  = (TagPowerPathStruct *)g_2L2TAutoModeOffList.GetNext(pos);
      if(g_Unit[pPowerPath->UnitID].VarGroup[1].Var[pPowerPath->VarID].Value!=0)//分列表中遥信非分
      {
        b2L2TMode = FALSE;
        break;
      }
    }
  }
  {
    POSITION pos = g_BusAutoModeOnList.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath  = (TagPowerPathStruct *)g_BusAutoModeOnList.GetNext(pos);
      if(g_Unit[pPowerPath->UnitID].VarGroup[1].Var[pPowerPath->VarID].Value==0)//合列表中遥信非合
      {
        bBusMode = FALSE;
        break;
      }
    }
  }
  {
    POSITION pos = g_BusAutoModeOffList.GetHeadPosition();
    while(pos)
    {
      TagPowerPathStruct *pPowerPath  = (TagPowerPathStruct *)g_BusAutoModeOffList.GetNext(pos);
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
    if(g_Unit[UnitID].VarGroup[1].Var[13].Value==0)//未失压
    {
      if(LostVotlageStartupState1==FALSE)
      {
        LostVotlageStartupState1 = TRUE;
        LostVotlageStartupTickCount1 = ::GetTickCount();
      }
      else
      {
        if((::GetTickCount()-LostVotlageStartupTickCount1)>=LostVotlageDelay)
        {
          LostVotlageStartupState1 = FALSE;
          LostVotlageStartupTickCount1 = 0;
          g_Unit[UnitID].VarGroup[1].Var[13].Value = 1;//设置失压
        }
      }
    }
  }
  if(U1>=HasVotlageValue)//判断1#进线有压
  {
    if(g_Unit[UnitID].VarGroup[1].Var[13].Value!=0)//已失压
    {
      if(HasVotlageStartupState1==FALSE)
      {
        HasVotlageStartupState1 = TRUE;
        HasVotlageStartupTickCount1 = ::GetTickCount();
      }
      else
      {
        if((::GetTickCount()-HasVotlageStartupTickCount1)>=HasVotlageDelay)
        {
          HasVotlageStartupState1 = FALSE;
          HasVotlageStartupTickCount1 = 0;
          g_Unit[UnitID].VarGroup[1].Var[13].Value = 0;//设置有压
        }
      }
    }
  }
  if(U2<=LostVotlageValue)//判断2#进线失压
  {
    if(g_Unit[UnitID].VarGroup[1].Var[14].Value==0)//未失压
    {
      if(LostVotlageStartupState2==FALSE)
      {
        LostVotlageStartupState2 = TRUE;
        LostVotlageStartupTickCount2 = ::GetTickCount();
      }
      else
      {
        if((::GetTickCount()-LostVotlageStartupTickCount2)>=LostVotlageDelay)
        {
          LostVotlageStartupState2 = FALSE;
          LostVotlageStartupTickCount2 = 0;
          g_Unit[UnitID].VarGroup[1].Var[14].Value = 1;//设置失压
        }
      }
    }
  }
  if(U2>=HasVotlageValue)//判断2#进线有压
  {
    if(g_Unit[UnitID].VarGroup[1].Var[14].Value!=0)//已失压
    {
      if(HasVotlageStartupState2==FALSE)
      {
        HasVotlageStartupState2 = TRUE;
        HasVotlageStartupTickCount2 = ::GetTickCount();
      }
      else
      {
        if((::GetTickCount()-HasVotlageStartupTickCount2)>=HasVotlageDelay)
        {
          HasVotlageStartupState2 = FALSE;
          HasVotlageStartupTickCount2 = 0;
          g_Unit[UnitID].VarGroup[1].Var[14].Value = 0;//设置有压
        }
      }
    }
  }

//-----------------主变故障，清除充电--------------//
  if((g_Unit[UnitID].VarGroup[1].Var[15].Value!=0)||(g_Unit[UnitID].VarGroup[1].Var[16].Value!=0))
  {
    ChargeState1L1T = FALSE;
    ChargeState2L2T = FALSE;
    ChargeStateBus = FALSE;
  }

//-----------------进线失压，清除充电--------------//
  if((g_Unit[UnitID].VarGroup[1].Var[13].Value!=0)&&(g_Unit[UnitID].VarGroup[1].Var[14].Value!=0))
  {
    ChargeState1L1T = FALSE;
    ChargeState2L2T = FALSE;
    ChargeStateBus = FALSE;
  }

//-----------------无运行方式，清除充电--------------//
  if((g_Unit[UnitID].VarGroup[1].Var[20].Value==0)&&(g_Unit[UnitID].VarGroup[1].Var[21].Value==0)&&(g_Unit[UnitID].VarGroup[1].Var[22].Value==0))
  {
    ChargeState1L1T = FALSE;
    ChargeState2L2T = FALSE;
    ChargeStateBus = FALSE;
  }

  if(g_Unit[UnitID].VarGroup[1].Var[20].Value!=0)
    ChargeState1L1T = FALSE;
  if(g_Unit[UnitID].VarGroup[1].Var[21].Value!=0)
    ChargeState2L2T = FALSE;
  if(g_Unit[UnitID].VarGroup[1].Var[22].Value!=0)
    ChargeStateBus = FALSE;

  if(ChargeState1L1T==FALSE)
  {
    g_Unit[UnitID].VarGroup[1].Var[17].Value = 0;
    ChargeStartupState1L1T = FALSE;
    ChargeStartupTickCount1L1T = 0;
  }
  if(ChargeState2L2T==FALSE)
  {
    g_Unit[UnitID].VarGroup[1].Var[18].Value = 0;
    ChargeStartupState2L2T = FALSE;
    ChargeStartupTickCount1L1T = 0;
  }
  if(ChargeStateBus==FALSE)
  {
    g_Unit[UnitID].VarGroup[1].Var[19].Value = 0;
    ChargeStartupStateBus = FALSE;
    ChargeStartupTickCountBus = 0;
  }

  if((ChargeState1L1T==TRUE)&&(g_Unit[UnitID].VarGroup[1].Var[17].Value==0))
  {
    if(ChargeStartupState1L1T==FALSE)
    {
      ChargeStartupState1L1T = TRUE;
      ChargeStartupTickCount1L1T = ::GetTickCount();
    }
    else
    {
      if((::GetTickCount()-ChargeStartupTickCount1L1T)>=10000)
      {
        ChargeStartupState1L1T = FALSE;
        ChargeStartupTickCount1L1T = 0;
        g_Unit[UnitID].VarGroup[1].Var[17].Value = 1;//1T1L设置充电
      }
    }
  }
  if((ChargeState2L2T==TRUE)&&(g_Unit[UnitID].VarGroup[1].Var[18].Value==0))
  {
    if(ChargeStartupState2L2T==FALSE)
    {
      ChargeStartupState2L2T = TRUE;
      ChargeStartupTickCount2L2T = ::GetTickCount();
    }
    else
    {
      if((::GetTickCount()-ChargeStartupTickCount2L2T)>=10000)
      {
        ChargeStartupState2L2T = FALSE;
        ChargeStartupTickCount2L2T = 0;
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

    if(pUnit->UnitType==7)//备自投
    {
      ProcessAutoOnOff(unitno);
      continue;
    }

    switch(pUnit->UnitType)
    {
      case 1://变压器差动保护
        break;
      case 2://变压器高后备保护
        if((pUnit->VarGroup[0].Var[0].Value==0)&&(pUnit->VarGroup[0].Var[1].Value==0)&&(pUnit->VarGroup[0].Var[2].Value==0)||
           (pUnit->VarGroup[1].Var[0].Value==0))
        {
          for(int samp=0;samp<WAVE_SAMPLE_COUNT;samp++)
          {
            pUnit->SampleWaveValue[6][samp] = 0;
            pUnit->SampleWaveValue[7][samp] = 0;
            pUnit->SampleWaveValue[8][samp] = 0;
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
      case 3://变压器低后备保护
      case 4://线路保护
      case 5://电容器保护
      case 6://所用变保护
      case 8://综合测控
        if((pUnit->VarGroup[0].Var[0].Value==0)&&(pUnit->VarGroup[0].Var[1].Value==0)&&(pUnit->VarGroup[0].Var[2].Value==0)||
           (pUnit->VarGroup[1].Var[0].Value==0))
        {
          for(int samp=0;samp<WAVE_SAMPLE_COUNT;samp++)
          {
            pUnit->SampleWaveValue[3][samp] = 0;
            pUnit->SampleWaveValue[4][samp] = 0;
            pUnit->SampleWaveValue[5][samp] = 0;
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
      case 7://备自投
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
        case 1://变压器差动保护
          if((g_Unit[unitno+1].VarGroup[1].Var[0].Value!=0)||(g_Unit[unitno+2].VarGroup[1].Var[0].Value!=0)||(g_Unit[unitno+2].VarGroup[1].Var[2].Value!=0))
          {
            if(ProtectionCalculater[unitno].ProtectionActionBreakerTrip[0]==TRUE)
            {
              pUnit->VarGroup[1].Var[30].Value = 1;

              g_Unit[unitno+1].VarGroup[1].Var[0].Value = 0;
              g_Unit[unitno+1].VarGroup[1].Var[1].Value = 1;
              ProtectionActionTrip = TRUE;
            }
            if(ProtectionCalculater[unitno].ProtectionActionBreakerTrip[1]==TRUE)
            {
              pUnit->VarGroup[1].Var[30].Value = 1;
              g_Unit[unitno+2].VarGroup[1].Var[0].Value = 0;
              g_Unit[unitno+2].VarGroup[1].Var[1].Value = 1;
              ProtectionActionTrip = TRUE;
            }
            ::ProductSOEReport(pUnit->UnitID,ProtectionCalculater[unitno].ProtectionActionType,CTime::GetCurrentTime(),(WORD)ProtectionCalculater[unitno].ProtectionActionTime,1,ActionValue);
          }
          break;
        case 2://变压器高后备保护
          if(ProtectionCalculater[unitno].ProtectionActionBreakerTrip[0]==TRUE)   //高压侧跳两侧
          {
            g_Unit[unitno].VarGroup[1].Var[30].Value = 1;
            if(g_Unit[unitno].VarGroup[1].Var[0].Value!=g_Unit[unitno].VarGroup[1].Var[1].Value)
            {
              if(g_Unit[unitno].VarLock[0]==FALSE)
              {
                g_Unit[unitno].VarGroup[1].Var[0].Value = 0;
                g_Unit[unitno].VarGroup[1].Var[1].Value = 1;
                g_Unit[unitno].VarGroup[1].Var[23].Value = 0;
                ProtectionActionTrip = TRUE;
              }
              else
                g_Unit[pUnit->UnitID].VarGroup[1].Var[23].Value = 1;
            }

            if(g_Unit[unitno+1].VarGroup[1].Var[0].Value!=g_Unit[unitno+1].VarGroup[1].Var[1].Value)
            {
              if(g_Unit[unitno+1].VarLock[0]==FALSE)
              {
                g_Unit[unitno+1].VarGroup[1].Var[0].Value = 0;
                g_Unit[unitno+1].VarGroup[1].Var[1].Value = 1;
                g_Unit[unitno+1].VarGroup[1].Var[23].Value = 0;
                ProtectionActionTrip = TRUE;
              }
              else
                g_Unit[unitno+1].VarGroup[1].Var[23].Value = 1;
            }
          }

          ::ProductSOEReport(pUnit->UnitID,ProtectionCalculater[unitno].ProtectionActionType,CTime::GetCurrentTime(),(WORD)ProtectionCalculater[unitno].ProtectionActionTime,1,ActionValue);
          break;
        case 3://变压器低后备保护
        case 5://电容器保护
        case 6://综合测控
          if(ProtectionCalculater[unitno].ProtectionActionBreakerTrip[0]==TRUE)   //跳闸
          {
            g_Unit[unitno].VarGroup[1].Var[30].Value = 1;
            if(g_Unit[unitno].VarGroup[1].Var[0].Value!=g_Unit[unitno].VarGroup[1].Var[1].Value)
            {
              if(g_Unit[unitno].VarLock[0]==FALSE)
              {
                g_Unit[unitno].VarGroup[1].Var[0].Value = 0;
                g_Unit[unitno].VarGroup[1].Var[1].Value = 1;
                g_Unit[unitno].VarGroup[1].Var[23].Value = 0;
                ProtectionActionTrip = TRUE;
              }
              else
                g_Unit[unitno+1].VarGroup[1].Var[23].Value = 1;
            }
          }

          ::ProductSOEReport(pUnit->UnitID,ProtectionCalculater[unitno].ProtectionActionType,CTime::GetCurrentTime(),(WORD)ProtectionCalculater[unitno].ProtectionActionTime,1,ActionValue);
          break;
        case 4://线路保护
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
        case 7://备自投
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
      case 1://变压器差动保护
      {
        float Iah = 0;
        float Ibh = 0;
        float Ich = 0;
        float Ial = 0;
        float Ibl = 0;
        float Icl = 0;
        for(int samp=0;samp<320;samp++)
        {
          g_Unit[unitno].SampleWaveValue[0][samp] = (float)Iah *(float)::sin((float)(samp+(float)0/(float)360*(float)32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
          g_Unit[unitno].SampleWaveValue[1][samp] = (float)Ibh *(float)::sin((float)(samp+(float)120/(float)360*(float)32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
          g_Unit[unitno].SampleWaveValue[2][samp] = (float)Ich *(float)::sin((float)(samp+(float)240/(float)360*(float)32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
          g_Unit[unitno].SampleWaveValue[3][samp] = (float)Ial *(float)::sin((float)(samp+(float)0/(float)360*(float)32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
          g_Unit[unitno].SampleWaveValue[4][samp] = (float)Ibl *(float)::sin((float)(samp+(float)120/(float)360*(float)32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
          g_Unit[unitno].SampleWaveValue[5][samp] = (float)Ibl *(float)::sin((float)(samp+(float)120/(float)360*(float)32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
        }
        break;
      }
      case 7://备自投
        break;
      case 2://变压器高后备保护
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
      case 3://变压器低后备保护
      case 4://线路保护
      case 5://电容器保护
      case 6://所用变保护
      case 8://综合测控
      {
        float Uab = g_Unit[unitno].VarGroup[0].Var[0].Value;
        float Ubc = g_Unit[unitno].VarGroup[0].Var[1].Value;
        float Uca = g_Unit[unitno].VarGroup[0].Var[2].Value;
        float Ua = g_Unit[unitno].VarGroup[0].Var[3].Value;
        float Ub = g_Unit[unitno].VarGroup[0].Var[4].Value;
        float Uc = g_Unit[unitno].VarGroup[0].Var[5].Value;
        float Ia = g_Unit[unitno].VarGroup[0].Var[7].Value;
        float Ib = g_Unit[unitno].VarGroup[0].Var[8].Value;
        float Ic = g_Unit[unitno].VarGroup[0].Var[9].Value;
        float AngleValue = (float)::acos(g_Unit[unitno].VarGroup[0].Var[13].Value)*(float)180/(float)3.1415926;
        for(int samp=0;samp<320;samp++)
        {
          g_Unit[unitno].SampleWaveValue[0][samp] = (float)Ua * (float)1.414 * (float)::sin((float)(samp+(float)0/(float)360*(float)32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
          g_Unit[unitno].SampleWaveValue[1][samp] = (float)Ub * (float)1.414 *(float)::sin((float)(samp+(float)120/(float)360*(float)32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
          g_Unit[unitno].SampleWaveValue[2][samp] = (float)Uc * (float)1.414 *(float)::sin((float)(samp+(float)240/(float)360*(float)32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
          g_Unit[unitno].SampleWaveValue[3][samp] = (float)Ia * (float)1.414 *(float)::sin((float)(samp+(float)(0+AngleValue)/(float)360*32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
          g_Unit[unitno].SampleWaveValue[4][samp] = (float)Ib * (float)1.414 *(float)::sin((float)(samp+(float)(120+AngleValue)/(float)360*(float)32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
          g_Unit[unitno].SampleWaveValue[5][samp] = (float)Ic * (float)1.414 *(float)::sin((float)(samp+(float)(240+AngleValue)/(float)360*(float)32)*((float)360/(float)(32)*(float)3.1415926/(float)180));
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

    g_Unit[g_UnitID1THi].VarGroup[0].Var[16].Value = (float)g_PTVoltage.T1Temp1+((float)g_PTVoltage.T1Temp1*(float)(rand()%100)/(float)100*(float)(0.05));
    g_Unit[g_UnitID2THi].VarGroup[0].Var[16].Value = (float)g_PTVoltage.T2Temp1+((float)g_PTVoltage.T2Temp1*(float)(rand()%100)/(float)100*(float)(0.05));
    g_Unit[g_UnitID1THi].VarGroup[0].Var[17].Value = (float)g_PTVoltage.T1Temp2+((float)g_PTVoltage.T1Temp2*(float)(rand()%100)/(float)100*(float)(0.05));
    g_Unit[g_UnitID2THi].VarGroup[0].Var[17].Value = (float)g_PTVoltage.T2Temp2+((float)g_PTVoltage.T2Temp2*(float)(rand()%100)/(float)100*(float)(0.05));

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
      ProcessPowerPTVotlage();
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
      ProcessPowerPTVotlage();
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
                case 1://变压器差动保护
                case 7://备自投
                  break;
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
