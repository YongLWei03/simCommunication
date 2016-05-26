// SocketSer.cpp : implementation file
//

#include "stdafx.h"
#include "simCommunication.h"
#include "NewSocket.h"
#include "DlgMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CNewSocket;
class CNewClientSocketThread;

CMutex  g_SendSOEReportListMutex;
CPtrList g_SendSOEReportList;
BOOL g_EnableStudentExitOperate = TRUE;

CNewSocket g_NetworkManageClientSocket;       //连接网管

CString g_ServerSocketName[] = {"<管理中心>","<监控仿真>","<通讯仿真>","<保护仿真>"};

BYTE  g_Frame = 0x00;

/////////////////////////////////////////////////////////////////////////////
// CNewClientSocketThread

IMPLEMENT_DYNCREATE(CNewClientSocketThread, CWinThread)

CNewClientSocketThread::CNewClientSocketThread()
{
  m_Run = TRUE;       //运行标志
  m_Running = TRUE;   //正在运行标志
}

CNewClientSocketThread::~CNewClientSocketThread()
{
}

BOOL CNewClientSocketThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CNewClientSocketThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CNewClientSocketThread, CWinThread)
	//{{AFX_MSG_MAP(CSocketThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewServerSocketThread message handlers

DWORD g_SendSendAllDataTickCount = 0;
BOOL  g_SendAllData = FALSE;

int CNewClientSocketThread::Run() 
{
  m_Time=CTime::GetCurrentTime();
  CTime ProcessTime = CTime::GetCurrentTime();
  while(m_Run)
  {
    m_Time=CTime::GetCurrentTime();

    Sleep(1);
    if((::GetTickCount()-g_SendSendAllDataTickCount)>=500)
    {
      g_SendSendAllDataTickCount = ::GetTickCount();

      if(g_SendSOEReportList.GetCount())
      {
        CSingleLock sLock(&g_SendSOEReportListMutex);
        sLock.Lock();

        TagSOEReportStruct *pSOEReportStruct = (TagSOEReportStruct *)g_SendSOEReportList.RemoveHead();
        if(g_SimCommunicationMain==TRUE)
          SendSOEData(pSOEReportStruct);
      }
      else
      {
        if(g_SimCommunicationMain==FALSE)
          g_SendAllData = FALSE;

        if(g_SendAllData==TRUE)
        {
          g_SendAllData = FALSE;
          SendAllData();
        }
        else
          SendTestLinkInfo();
      }

      for(int UnitID=0;UnitID<UNITCOUNT;UnitID++)
      {
        if(g_Unit[UnitID].UnitType==0)
          continue;

        POSITION pos = g_Unit[UnitID].SOEReportList.GetHeadPosition();
        while(pos)
        {
          TagSOEReportStruct *pSOEReport = (TagSOEReportStruct *)g_Unit[UnitID].SOEReportList.GetNext(pos);
          if(pSOEReport->SendState)
            continue;
          pSOEReport->SendState = TRUE;

          for(int no=0;no<COMPUTER_COUNT;no++)
          {
            if(g_ProtocolTH104[no].m_pCommThread!=NULL)
            {
              if(g_ProtocolTH104[no].m_StartSendDataFrame==TRUE)
              {
                CSingleLock sLock(&g_ProtocolTH104[no].m_SendVarFrameMutex);
                sLock.Lock();

                TagSOEReportStruct *pSendSOEReport = new TagSOEReportStruct;
                memcpy(pSendSOEReport,pSOEReport,sizeof(TagSOEReportStruct));
                g_ProtocolTH104[no].m_SendSOEReportList[UnitID].AddTail(pSendSOEReport);
              }
            }
          }
        }
      }

      if(m_pClientSocket->m_Connect==FALSE)
      {
        m_pClientSocket->m_hSocket = ::socket(AF_INET,SOCK_STREAM,0);
        m_pClientSocket->m_Create = TRUE;//创建成功
        if(g_NetworkManageClientSocket.Connect(g_ComputerList[0].m_ComputerIPAddress,1502)==TRUE)
          g_pDlgMessageBox->DoShow("网管连接成功",1);
        else
          continue;
      }
    }

    BYTE totalreceivebuf[0xffff];
    WORD totalreceivelen = 0;

    if(m_pClientSocket->PhysicalReceive(totalreceivebuf,totalreceivelen)==TRUE)
    {
      BYTE receivebuf[0xffff];
      WORD receivelen = 0;

      int pos = 0;
      while(pos<totalreceivelen)
      {
        WORD receivelen = totalreceivebuf[pos+3]+totalreceivebuf[pos+4]*256+totalreceivebuf[pos+5]*256*256+totalreceivebuf[pos+6]*256*256*256;
        memcpy(receivebuf,totalreceivebuf+pos,receivelen);
        pos += receivelen;

        if((receivebuf[1]!=0xEB)||(receivebuf[2]!=0x90))
          continue;//接收失败

        if((receivebuf[3]+receivebuf[4]*256+receivebuf[5]*256*256+receivebuf[6]*256*256*256)!=receivelen)
          continue;//接收失败

        WORD MseeageType = receivebuf[7]+receivebuf[8]*256;
        switch(MseeageType)
        {
          case PM_ADJUSTCLOCK  :   //时钟同步
          {
            SYSTEMTIME ct;
            memcpy(&ct,receivebuf+9,sizeof(SYSTEMTIME));
            SetLocalTime(&ct);
            g_pDlgMessageBox->DoShow("接收远程时钟同步",3);
            break;
          }
          case PM_SEND_TESTLINK:
          {
            g_EnableStudentExitOperate = receivebuf[9]+receivebuf[10]*256;
            WORD PowerID = receivebuf[11]+receivebuf[12]*256;
            WORD DataExchangeState = receivebuf[13]+receivebuf[14]*256;
            if(DataExchangeState&0x0001)
              g_EnableReceivePowerState = TRUE;
            else
              g_EnableReceivePowerState = FALSE;
            if(DataExchangeState&0x0002)
              g_EnableSendPowerState = TRUE;
            else
              g_EnableSendPowerState = FALSE;
            if(DataExchangeState&0x0004)
              g_EnableReceiveTrainState = TRUE;
            else
              g_EnableReceiveTrainState = FALSE;
            if(DataExchangeState&0x0008)
              g_EnableSendTrainState = TRUE;
            else
              g_EnableSendTrainState = FALSE;
            break;
          }
          case PM_APPEXIT:
          {
            m_Run = FALSE;
            break;
          }
          case PM_CALL_ALLDATA:
          {
            g_SendAllData = TRUE;
            break;
          }
          case PM_SEND_ALLDATA:
          {
            g_EnableStudentExitOperate = receivebuf[9]+receivebuf[10]*256;

            WORD AllDataCount = receivebuf[15]+receivebuf[16]*256;

            for(int datano=0;datano<AllDataCount;datano++)
            {
              WORD UnitID = receivebuf[17+datano*14]+receivebuf[18+datano*14]*256;
              WORD VarGroupID = receivebuf[19+datano*14]+receivebuf[20+datano*14]*256;
              WORD VarID = receivebuf[21+datano*14]+receivebuf[22+datano*14]*256;
              WORD VarLock = receivebuf[23+datano*14]+receivebuf[24+datano*14]*256;
              WORD VarAbnormal = receivebuf[25+datano*14]+receivebuf[26+datano*14]*256;
              float Value;
              memcpy(&Value,receivebuf+27+datano*14,sizeof(float));
              if(VarGroupID==1)//遥信
              {
                g_Unit[UnitID].VarLock[VarID] = VarLock;
                g_Unit[UnitID].VarAbnormal[VarID] = VarAbnormal;
              }
              g_Unit[UnitID].VarGroup[VarGroupID].Var[VarID].Value = Value;
              g_Unit[UnitID].VarGroup[VarGroupID].Var[VarID].LastValue = !Value;

              g_SendAllData = TRUE;
            }
            break;
          }
          case PM_SEND_SOEDATA:
          {
            break;
          }
          case PM_SEND_REMOTEREVERT:
          {
            WORD UnitID = receivebuf[9]+receivebuf[10]*256;
            g_Unit[UnitID].VarGroup[1].Var[30].Value = 0;//事故信号
            g_Unit[UnitID].VarGroup[1].Var[31].Value = 0;//预告信号
            break;
          }
          case PM_SEND_REMOTEONOFF:
          {
            WORD UnitID = receivebuf[9]+receivebuf[10]*256;
            WORD VarID = receivebuf[11]+receivebuf[12]*256;
            WORD Value = receivebuf[13]+receivebuf[14]*256;
          
            float ActionValue[VARCOUNT];
            memset(&ActionValue,0x00,sizeof(ActionValue));

            if(Value)//遥控合闸
            {
              switch(g_Unit[UnitID].UnitType)
              {
                case 1://变压器差动保护
                case 2://变压器高后备保护
                case 4://线路保护
                case 5://电容器保护
                  if(g_Unit[UnitID].VarGroup[1].Var[0].Value!=g_Unit[UnitID].VarGroup[1].Var[1].Value)
                  {
                    if(g_Unit[UnitID].VarLock[0]==FALSE)
                      g_Unit[UnitID].VarGroup[1].Var[0].Value = 1;
                    if(g_Unit[UnitID].VarLock[1]==FALSE)
                      g_Unit[UnitID].VarGroup[1].Var[1].Value = 0;
                  }
                  ::ProductActionValue(UnitID,ActionValue);
                  ::ProductSOEReport(UnitID,34,CTime::GetCurrentTime(),(WORD)::GetTickCount()%1000,1,ActionValue);
                  ::AddPresetVar(UnitID,1,6,0,CTime::GetCurrentTime());
                  ::AddPresetVar(UnitID,1,6,1,CTime::GetCurrentTime()+CTimeSpan(10));
                  break;
                case 3://变压器低后备保护
                  if(g_Unit[UnitID].VarGroup[1].Var[0].Value!=g_Unit[UnitID].VarGroup[1].Var[1].Value)
                  {
                    if(g_Unit[UnitID].VarLock[0]==FALSE)
                      g_Unit[UnitID].VarGroup[1].Var[0].Value = 1;
                    if(g_Unit[UnitID].VarLock[1]==FALSE)
                      g_Unit[UnitID].VarGroup[1].Var[1].Value = 0;
                  }
                  if(g_Unit[UnitID].VarGroup[1].Var[2].Value!=g_Unit[UnitID].VarGroup[1].Var[3].Value)
                  {
                    if(g_Unit[UnitID].VarLock[2]==FALSE)
                      g_Unit[UnitID].VarGroup[1].Var[2].Value = 1;
                    if(g_Unit[UnitID].VarLock[3]==FALSE)
                      g_Unit[UnitID].VarGroup[1].Var[3].Value = 0;
                  }
                  ::ProductActionValue(UnitID,ActionValue);
                  ::ProductSOEReport(UnitID,34,CTime::GetCurrentTime(),(WORD)::GetTickCount()%1000,1,ActionValue);
                  ::AddPresetVar(UnitID,1,12,0,CTime::GetCurrentTime());
                  ::AddPresetVar(UnitID,1,12,1,CTime::GetCurrentTime()+CTimeSpan(10));
                  ::AddPresetVar(UnitID,1,13,0,CTime::GetCurrentTime());
                  ::AddPresetVar(UnitID,1,13,1,CTime::GetCurrentTime()+CTimeSpan(10));
                  break;
              }
            }
            else//遥控分闸
            {
              switch(g_Unit[UnitID].UnitType)
              {
                case 1://变压器差动保护
                case 2://变压器高后备保护
                case 4://线路保护
                case 5://电容器保护
                  if(g_Unit[UnitID].VarGroup[1].Var[0].Value!=g_Unit[UnitID].VarGroup[1].Var[1].Value)
                  {
                    g_Unit[UnitID].VarGroup[1].Var[0].Value = 0;
                    g_Unit[UnitID].VarGroup[1].Var[1].Value = 1;
                  
                  }
                  ::ProductActionValue(UnitID,ActionValue);
                  ::ProductSOEReport(UnitID,35,CTime::GetCurrentTime(),(WORD)::GetTickCount()%1000,1,ActionValue);
                  break;
                case 3://变压器低后备保护
                  if(g_Unit[UnitID].VarGroup[1].Var[0].Value!=g_Unit[UnitID].VarGroup[1].Var[1].Value)
                  {
                    g_Unit[UnitID].VarGroup[1].Var[0].Value = 0;
                    g_Unit[UnitID].VarGroup[1].Var[1].Value = 1;
                  
                  }
                  if(g_Unit[UnitID].VarGroup[1].Var[2].Value!=g_Unit[UnitID].VarGroup[1].Var[3].Value)
                  {
                    g_Unit[UnitID].VarGroup[1].Var[2].Value = 0;
                    g_Unit[UnitID].VarGroup[1].Var[3].Value = 1;
                  }
                  ::ProductActionValue(UnitID,ActionValue);
                  ::ProductSOEReport(UnitID,35,CTime::GetCurrentTime(),(WORD)::GetTickCount()%1000,1,ActionValue);
                  break;
              }
            }
            break;
          }
          case PM_CALL_PARAMETER:
          {
            g_pDlgMessageBox->DoShow("保护定值召唤成功",3);

            WORD UnitID = receivebuf[9]+receivebuf[10]*256;

            g_NetworkManageClientSocket.SendParameter(UnitID);

            break;
          }
          case PM_SEND_PARAMETER:
          {
            g_pDlgMessageBox->DoShow("保护定值接收成功",3);

            WORD UnitID = receivebuf[9]+receivebuf[10]*256;
            g_Unit[UnitID].CurrentParamaterGroupID = receivebuf[11]+receivebuf[12]*256;

            float ParameterValue[PARAMETERGROUPCOUNT][PARAMETERCOUNT];
            memcpy(&ParameterValue,receivebuf+13,4*PARAMETERGROUPCOUNT*PARAMETERCOUNT);
        
            for(int vargroupno=0;vargroupno<PARAMETERGROUPCOUNT;vargroupno++)
            {
              for(int varno=0;varno<PARAMETERCOUNT;varno++)
                g_Unit[UnitID].ParameterGroup[vargroupno].Var[varno].Value = ParameterValue[vargroupno][varno];
            }
            break;
          }
          case PM_SEND_FEED_TRANSMISION_BEGIN:
          {
            TagFreeTransmissionStruct *pFreeTransmission = new TagFreeTransmissionStruct;
            memcpy(pFreeTransmission,receivebuf+9,sizeof(TagFreeTransmissionStruct));
            pFreeTransmission->BeginTime = CTime::GetCurrentTime() + CTimeSpan(0,0,0,pFreeTransmission->StartupDelaySecond);
            pFreeTransmission->FinishTime = pFreeTransmission->BeginTime + CTimeSpan(0,0,0,pFreeTransmission->ContinuedSeconds);
            ::AddFreeTransmission(pFreeTransmission);

            g_pDlgMessageBox->DoShow("准备启动馈线传动",3);

            break;
          }
          case PM_SEND_FREE_TRAIN_BEGIN:
          {
            TagFreeTrainStruct *pFreeTrain = new TagFreeTrainStruct;
            memcpy(pFreeTrain,receivebuf+9,sizeof(TagFreeTrainStruct));
            pFreeTrain->InBeginTime = CTime::GetCurrentTime() + CTimeSpan(0,0,0,pFreeTrain->StartupDelaySecond);;
            pFreeTrain->InFinishTime = pFreeTrain->InBeginTime+CTimeSpan(0,0,0,pFreeTrain->InContinuedSeconds);
            pFreeTrain->ChangeBeginTime = pFreeTrain->InFinishTime;
            pFreeTrain->ChangeFinishTime = pFreeTrain->ChangeBeginTime+CTimeSpan(0,0,0,pFreeTrain->ChangeContinuedSeconds);
            pFreeTrain->OutBeginTime = pFreeTrain->ChangeFinishTime;
            pFreeTrain->OutFinishTime = pFreeTrain->OutBeginTime+CTimeSpan(0,0,0,pFreeTrain->OutContinuedSeconds);
            ::AddFreeTrain(pFreeTrain);

            g_pDlgMessageBox->DoShow("准备启动自由行车",3);

            break;
          }
          case PM_SEND_SIM_TRAIN_BEGIN:
          {
            memcpy(&g_SimTrain,receivebuf+9,sizeof(TagSimTrainStruct));
            g_SimTrain.BeginTime = CTime::GetCurrentTime() + CTimeSpan(0,0,0,g_SimTrain.StartupDelaySecond);;
            g_SimTrain.SimTrainDataCount = 0;
            g_SimTrain.SimTrainDataCurrentPos = 0;
            g_SimTrain.SimTrainState = 1;//准备读文件

            g_pDlgMessageBox->DoShow("准备启动仿真行车",3);

            break;
          }
          case PM_SEND_FAULT_TRANSMISION_BEGIN:
          {
            CFileStatus FileStatus;
            memcpy(&g_FaultTransmission,receivebuf+9,sizeof(TagFaultTransmissionStruct));
            g_FaultTransmission.BeginTime = CTime::GetCurrentTime() + CTimeSpan(0,0,0,g_FaultTransmission.StartupDelaySecond);;
            g_FaultTransmission.FaultTransmissionState = 1;//准备读文件

            g_pDlgMessageBox->DoShow("准备启动故障传动",3);
            break;
          }
          case PM_SEND_POWER_CLEAR:
          {
            ::ClearPower();

            g_pDlgMessageBox->DoShow("准备启动清除负荷",3);
            break;
          }
          case PM_SEND_OPERATE_EANBLE_ID:
          {
            g_pDlgMessageBox->DoShow("操作允许计算机编号接收成功",3);

            WORD ComputerID = receivebuf[9]+receivebuf[10]*256;
            for(int no=0;no<COMPUTER_COUNT;no++)
            {
              if((no==ComputerID)||(no==0)||(no==255))
                g_ProtocolTH104[no].m_OperateEnable = TRUE;
              else
                g_ProtocolTH104[no].m_OperateEnable = FALSE;
            }            
            break;
          }
          case PM_SEND_INLINE_VOTLAGE:
          {
            g_pDlgMessageBox->DoShow("进线电压设置接收成功",3);

            memcpy(&g_InlineVotlage,receivebuf+9,sizeof(TagInlineVotlageStruct));
            break;
          }
          case PM_SEND_PT_VOTLAGE:
          {
            g_pDlgMessageBox->DoShow("压互补偿设置接收成功",3);

            memcpy(&g_PTVoltage,receivebuf+9,sizeof(TagPTVotlageStruct));
            g_PTVoltage.BeginTime = 0;
            break;
          }
          case PM_SEND_CURRENT:
          {
            g_pDlgMessageBox->DoShow("负荷电流设置接收成功",3);

            memcpy(&g_Current,receivebuf+9,sizeof(TagCurrentStruct));
            g_Current.BeginTime = 0;
            break;
          }
          case PM_SEND_COMPENSATION:
          {
            g_pDlgMessageBox->DoShow("电容补偿设置接收成功",3);

            memcpy(&g_Compensation[0],receivebuf+9,sizeof(TagCompensationStruct));
            memcpy(&g_Compensation[1],receivebuf+9+sizeof(TagCompensationStruct),sizeof(TagCompensationStruct));
            break;
          }
          case PM_SEND_STARTUP_SCENE_FILE:
          {
            break;
          }
          case PM_SEND_END_SCENE_FILE:
          {
            break;
          }
          case PM_SEND_END_EXAMINATION:
          {
            m_Run = FALSE;
            break;
          }
        }
      }
    }

    ProcessTime = CTime::GetCurrentTime();
  }
	m_Running = FALSE;
	return CWinThread::Run();
}

void CNewClientSocketThread::KillThread()//结束线程
{
	m_Run = FALSE;
  while(m_Running==TRUE)
    Sleep(100);
  TerminateThread(m_hThread,0);
}

BOOL CNewClientSocketThread::SendTestLinkInfo()//发送链路测试
{
  BYTE sendbuf[0xffff];
  WORD sendlen = 9;

  sendbuf[0]=g_Frame++;
  sendbuf[1]=0xEB;
  sendbuf[2]=0x90;
  sendbuf[3]=LOBYTE(LOWORD(sendlen));
  sendbuf[4]=HIBYTE(LOWORD(sendlen));
  sendbuf[5]=LOBYTE(HIWORD(sendlen));
  sendbuf[6]=HIBYTE(HIWORD(sendlen));
  sendbuf[7]=LOBYTE(PM_SEND_TESTLINK);
  sendbuf[8]=HIBYTE(PM_SEND_TESTLINK);

  return m_pClientSocket->PhysicalSend(sendbuf,sendlen);
}

BOOL CNewClientSocketThread::SendAllData()//发送全数据
{
  CPtrList DataList;

  for(int unitno=0;unitno<UNITCOUNT;unitno++)
  {
    if(g_Unit[unitno].UnitType==0)
      continue;
    for(int vargroupno=0;vargroupno<VARGROUPCOUNT;vargroupno++)
    {
      for(int varno=0;varno<VARCOUNT;varno++)
      {
        if(g_Unit[unitno].VarGroup[vargroupno].Var[varno].VarName=="")
          continue;

        DataList.AddTail(&g_Unit[unitno].VarGroup[vargroupno].Var[varno]);
      }
    }
  }

  WORD AllDataCount = DataList.GetCount();

  BYTE sendbuf[0xffff];
  WORD sendlen = 9 + 2 + 3*2 + AllDataCount*14;

  sendbuf[0]=g_Frame++;
  sendbuf[1]=0xEB;
  sendbuf[2]=0x90;
  sendbuf[3]=LOBYTE(LOWORD(sendlen));
  sendbuf[4]=HIBYTE(LOWORD(sendlen));
  sendbuf[5]=LOBYTE(HIWORD(sendlen));
  sendbuf[6]=HIBYTE(HIWORD(sendlen));
  sendbuf[7]=LOBYTE(PM_SEND_ALLDATA);
  sendbuf[8]=HIBYTE(PM_SEND_ALLDATA);
  sendbuf[9]=LOBYTE((WORD)g_pPowerThread->m_PowerState);  //当前电网模式
  sendbuf[10]=HIBYTE((WORD)g_pPowerThread->m_PowerState);  //当前电网模式
  sendbuf[11]=0x00;
  sendbuf[12]=0x00;
  sendbuf[13]=0x00;
  sendbuf[14]=0x00;
  sendbuf[15]=LOBYTE(AllDataCount);
  sendbuf[16]=HIBYTE(AllDataCount);
 
  for(int datano=0;datano<AllDataCount;datano++)
  {
    POSITION pos = DataList.FindIndex(datano);
    TagVarStruct *pVarStruct = (TagVarStruct*)DataList.GetAt(pos);

    sendbuf[17+datano*14]=LOBYTE(pVarStruct->UnitID);
    sendbuf[18+datano*14]=HIBYTE(pVarStruct->UnitID);
    sendbuf[19+datano*14]=LOBYTE(pVarStruct->VarGroupID);
    sendbuf[20+datano*14]=HIBYTE(pVarStruct->VarGroupID);
    sendbuf[21+datano*14]=LOBYTE(pVarStruct->VarID);
    sendbuf[22+datano*14]=HIBYTE(pVarStruct->VarID);
    if(pVarStruct->VarGroupID==1)//遥信
    {
      TagUnitStruct *pUnit = &g_Unit[pVarStruct->UnitID];
      sendbuf[23+datano*14]=LOBYTE(pUnit->VarLock[pVarStruct->VarID]);
      sendbuf[24+datano*14]=HIBYTE(pUnit->VarLock[pVarStruct->VarID]);
      sendbuf[25+datano*14]=LOBYTE(pUnit->VarAbnormal[pVarStruct->VarID]);
      sendbuf[26+datano*14]=HIBYTE(pUnit->VarAbnormal[pVarStruct->VarID]);
    }
    else
    {
      sendbuf[23+datano*14]=0x00;
      sendbuf[24+datano*14]=0x00;
      sendbuf[25+datano*14]=0x00;
      sendbuf[26+datano*14]=0x00;
    }
    memcpy(sendbuf+27+datano*14,&pVarStruct->Value,sizeof(float));
  }

  return m_pClientSocket->PhysicalSend(sendbuf,sendlen);
}

BOOL CNewClientSocketThread::SendSOEData(TagSOEReportStruct *pSOEReportStruct)//发送SOE
{
  BYTE sendbuf[0xffff];
  WORD sendlen = 9 + sizeof(TagSOEReportStruct);

  sendbuf[0]=g_Frame++;
  sendbuf[1]=0xEB;
  sendbuf[2]=0x90;
  sendbuf[3]=LOBYTE(LOWORD(sendlen));
  sendbuf[4]=HIBYTE(LOWORD(sendlen));
  sendbuf[5]=LOBYTE(HIWORD(sendlen));
  sendbuf[6]=HIBYTE(HIWORD(sendlen));
  sendbuf[7]=LOBYTE(PM_SEND_SOEDATA);
  sendbuf[8]=HIBYTE(PM_SEND_SOEDATA);
  memcpy(sendbuf+9,pSOEReportStruct,sizeof(TagSOEReportStruct));

  return m_pClientSocket->PhysicalSend(sendbuf,sendlen);
}

/////////////////////////////////////////////////////////////////////////////
// CNewSocket

CNewSocket::CNewSocket()
{
  m_hSocket = INVALID_SOCKET;
  memset(&m_sockAddr,0x00,sizeof(SOCKADDR_IN));
  m_Create = FALSE;
  m_Connect = FALSE;
  m_SocketPort = 0;
  m_SocketID = 0;

  for(int no=0;no<4;no++)
  {
    m_AcceptTime[no] = NULL;
    m_AccessTime[no] = NULL;
    m_pClientSocket[no] = NULL;
  }
}

CNewSocket::CNewSocket(WORD SocketPort)
{
  m_hSocket = INVALID_SOCKET;
  memset(&m_sockAddr,0x00,sizeof(SOCKADDR_IN));
  m_Create = FALSE;
  m_Connect = FALSE;
  m_SocketPort = SocketPort;
  m_SocketID = SocketPort - 1500;

  for(int no=0;no<4;no++)
  {
    m_AcceptTime[no] = NULL;
    m_AccessTime[no] = NULL;
    m_pClientSocket[no] = NULL;
  }
}

CNewSocket::~CNewSocket()
{
  closesocket(m_hSocket);
  m_Create = FALSE;
  m_Connect = FALSE;
  m_hSocket = INVALID_SOCKET;
}

/////////////////////////////////////////////////////////////////////////////
// CNewSocket member functions

/////////////////////////////////////////////////////////////////////////////
// CNewSocket member functions

void CNewSocket::CreateClientSocket()
{
  m_hSocket = ::socket(AF_INET,SOCK_STREAM,0);
  
  m_Create = TRUE;//创建成功

  if(m_pClientSocketThread==NULL)
  {
    m_pClientSocketThread = (CNewClientSocketThread*)AfxBeginThread(RUNTIME_CLASS(CNewClientSocketThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
    m_pClientSocketThread->m_pClientSocket = this;
    m_pClientSocketThread->ResumeThread();
  }
}

void CNewSocket::CloseClientSocket()
{
  closesocket(m_hSocket);
  m_Create = FALSE;
  m_Connect = FALSE;
  m_hSocket = INVALID_SOCKET;

  if(m_pClientSocketThread)
  {
    m_pClientSocketThread->SuspendThread();
    m_pClientSocketThread->KillThread();
    delete m_pClientSocketThread;
    m_pClientSocketThread=NULL;
  }
}

BOOL CNewSocket::AcceptSocket()
{
  timeval time;
  time.tv_sec = 0;
  time.tv_usec = 0;

  fd_set fd;
  fd.fd_count = 1;
  fd.fd_array[0] = m_hSocket;

  int result = select(0,&fd,0,0,&time);
  if(result>0)
  {
    SOCKADDR_IN sockAddr;
    int sockAddrLen = sizeof(sockAddr);
    sockAddr.sin_family = AF_INET;
    SOCKET hsock = ::accept(m_hSocket,(SOCKADDR*)&sockAddr,&sockAddrLen);
    if(hsock>0)
    {
      CNewSocket* pSocket = new CNewSocket;
      pSocket->m_hSocket = hsock;
      pSocket->m_AccessTime[m_SocketID] = CTime::GetCurrentTime();

      CString OtherIP = inet_ntoa(sockAddr.sin_addr);
      for(int no=0;no<COMPUTER_COUNT;no++)
      {
        if(g_ComputerList[no].m_ComputerEnable==FALSE)
          continue;

        if(g_ComputerList[no].m_ComputerIPAddress == OtherIP)
        {
          CString msg = g_ComputerList[no].m_ComputerName + g_ServerSocketName[m_SocketID] + "接收连接";
          g_ComputerList[no].m_pComputerSocket[m_SocketID] = pSocket;
          g_pDlgMessageBox->DoShow(msg,3);
          ::ShowPaneTextMessage(msg);
        }
      }
    }
  }

  return TRUE;
}

BOOL CNewSocket::Connect(CString ServerIPAddress,WORD SocketPort)
{
	memset(&m_sockAddr,0,sizeof(m_sockAddr));
	LPSTR lpszAscii = (LPTSTR)ServerIPAddress.GetBuffer(0);
	m_sockAddr.sin_family = AF_INET;
	m_sockAddr.sin_addr.s_addr = inet_addr(lpszAscii);
	m_sockAddr.sin_port = htons((u_short)SocketPort);
  m_SocketPort = SocketPort;
  m_Connect=FALSE;
  if(::connect(m_hSocket,(SOCKADDR*)&m_sockAddr, sizeof(m_sockAddr)))
    return FALSE;

  m_Connect=TRUE;
  m_SocketID = SocketPort - 1500;
  return TRUE;
}

BOOL CNewSocket::PhysicalReceive(BYTE *pReceiveBuf,WORD &ReceiveLen)
{
  DWORD TotalReceiveLen = 0;

  fd_set fd;
  fd.fd_count = 1;
  fd.fd_array[0] = m_hSocket;
  timeval time;
  time.tv_sec = 0;
  time.tv_usec = 0;

  if(select(0,&fd,0,0,&time)==0)
    return FALSE;

  DWORD TickCount = ::GetTickCount();
  while(TRUE)
  {
    if(select(0,&fd,0,0,&time)>0)
    {
      BYTE receivebuf[0xffff];
      int receivelen = recv(m_hSocket, (char*)receivebuf, sizeof(receivebuf), 0);
      if(receivelen>0)
      {
        if(ReceiveLen==0)
        {
          if((receivebuf[1]!=0xEB)||(receivebuf[2]!=0x90))
            return FALSE;//接收失败

          TotalReceiveLen = receivebuf[3]+receivebuf[4]*256+receivebuf[5]*256*256+receivebuf[6]*256*256*256;
        }
        memcpy(pReceiveBuf+ReceiveLen,receivebuf,receivelen);
        ReceiveLen += receivelen;

        if(ReceiveLen>=TotalReceiveLen)
          return TRUE;
      }
    }

    if(TotalReceiveLen==0)
      return FALSE;

    if((::GetTickCount()-TickCount)>5000)//接收超时
      return FALSE;
  }

  return FALSE;
}

BOOL CNewSocket::PhysicalSend(BYTE *pSendBuf,WORD &SendLen)
{
  CSingleLock sLock(&m_SocketMutex);
  sLock.Lock();

  WORD PacketLen = SendLen;
  DWORD SendPos = 0;
  while(TRUE)
  {
    if((SendPos+PacketLen)>SendLen)
      PacketLen = (WORD)(SendLen - SendPos);

	  int Result = send(m_hSocket,(LPSTR)pSendBuf+SendPos,PacketLen,0);
    if(Result!=PacketLen)
    {
      closesocket(m_hSocket);
      m_Create = FALSE;
      m_Connect = FALSE;
      m_hSocket = INVALID_SOCKET;

      return FALSE;
    }

    if((SendPos+PacketLen)>=SendLen)
      break;

    SendPos += PacketLen;
  }

  Sleep(100);

  return TRUE;
}

BOOL CNewSocket::SendRemoteOnOff(WORD UnitID,WORD VarID,WORD Value)//发送遥控分合闸
{
  BYTE sendbuf[0xffff];
  WORD sendlen = 9 + 2 + 2 + 2;

  sendbuf[0]=g_Frame++;
  sendbuf[1]=0xEB;
  sendbuf[2]=0x90;
  sendbuf[3]=LOBYTE(LOWORD(sendlen));
  sendbuf[4]=HIBYTE(LOWORD(sendlen));
  sendbuf[5]=LOBYTE(HIWORD(sendlen));
  sendbuf[6]=HIBYTE(HIWORD(sendlen));
  sendbuf[7]=LOBYTE(PM_SEND_REMOTEONOFF);
  sendbuf[8]=HIBYTE(PM_SEND_REMOTEONOFF);
  sendbuf[9]=LOBYTE(UnitID);
  sendbuf[10]=HIBYTE(UnitID);
  sendbuf[11]=LOBYTE(VarID);
  sendbuf[12]=HIBYTE(VarID);
  sendbuf[13]=LOBYTE(Value);
  sendbuf[14]=HIBYTE(Value);

  return PhysicalSend(sendbuf,sendlen);
}

BOOL CNewSocket::SendRemoteRevert(WORD UnitID)//发送遥控复归
{
  BYTE sendbuf[0xffff];
  WORD sendlen = 9 + 2;

  sendbuf[0]=g_Frame++;
  sendbuf[1]=0xEB;
  sendbuf[2]=0x90;
  sendbuf[3]=LOBYTE(LOWORD(sendlen));
  sendbuf[4]=HIBYTE(LOWORD(sendlen));
  sendbuf[5]=LOBYTE(HIWORD(sendlen));
  sendbuf[6]=HIBYTE(HIWORD(sendlen));
  sendbuf[7]=LOBYTE(PM_SEND_REMOTEREVERT);
  sendbuf[8]=HIBYTE(PM_SEND_REMOTEREVERT);
  sendbuf[9]=LOBYTE(UnitID);
  sendbuf[10]=HIBYTE(UnitID);

  return PhysicalSend(sendbuf,sendlen);
}

BOOL CNewSocket::CallParameter(WORD UnitID)//召唤定值
{
  BYTE sendbuf[0xffff];
  WORD sendlen = 9 + 2;

  sendbuf[0]=g_Frame++;
  sendbuf[1]=0xEB;
  sendbuf[2]=0x90;
  sendbuf[3]=LOBYTE(LOWORD(sendlen));
  sendbuf[4]=HIBYTE(LOWORD(sendlen));
  sendbuf[5]=LOBYTE(HIWORD(sendlen));
  sendbuf[6]=HIBYTE(HIWORD(sendlen));
  sendbuf[7]=LOBYTE(PM_CALL_PARAMETER);
  sendbuf[8]=HIBYTE(PM_CALL_PARAMETER);
  sendbuf[9]=LOBYTE(UnitID);
  sendbuf[10]=HIBYTE(UnitID);

  return PhysicalSend(sendbuf,sendlen);
}

BOOL CNewSocket::SendParameter(WORD UnitID)//发送定值
{
  float ParameterValue[PARAMETERGROUPCOUNT][PARAMETERCOUNT];
  for(int vargroupno=0;vargroupno<PARAMETERGROUPCOUNT;vargroupno++)
  {
    for(int varno=0;varno<PARAMETERCOUNT;varno++)
      ParameterValue[vargroupno][varno] = g_Unit[UnitID].ParameterGroup[vargroupno].Var[varno].Value;
  }

  BYTE sendbuf[0xffff];
  WORD sendlen = 9 + 2 + 2 + 4*PARAMETERGROUPCOUNT*PARAMETERCOUNT;

  sendbuf[0]=g_Frame++;
  sendbuf[1]=0xEB;
  sendbuf[2]=0x90;
  sendbuf[3]=LOBYTE(LOWORD(sendlen));
  sendbuf[4]=HIBYTE(LOWORD(sendlen));
  sendbuf[5]=LOBYTE(HIWORD(sendlen));
  sendbuf[6]=HIBYTE(HIWORD(sendlen));
  sendbuf[7]=LOBYTE(PM_SEND_PARAMETER);
  sendbuf[8]=HIBYTE(PM_SEND_PARAMETER);
  sendbuf[9]=LOBYTE(UnitID);
  sendbuf[10]=HIBYTE(UnitID);
  sendbuf[11]=LOBYTE(g_Unit[UnitID].CurrentParamaterGroupID);
  sendbuf[12]=HIBYTE(g_Unit[UnitID].CurrentParamaterGroupID);
  memcpy(sendbuf+13,&ParameterValue,4*PARAMETERGROUPCOUNT*PARAMETERCOUNT);
  
  return PhysicalSend(sendbuf,sendlen);
}

