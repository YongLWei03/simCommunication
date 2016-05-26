// ProtocolTH104.cpp : implementation file
//
 
#include "stdafx.h"
#include "simCommunication.h"
#include "ProtocolTH104.h"
#include "DlgMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CNewSocketTH104;
class CProtocolTH104;

CNewSocketTH104 g_NewSocketTH104;
CNewServerSocketTH104Thread *g_pServerSocketTH104Thread = NULL;

CProtocolTH104  g_ProtocolTH104[COMPUTER_COUNT];

BOOL g_DistributionOperateMode = TRUE;//0:集中操作模式 1:分散操作模式
static DWORD g_LastRemoteOperateTickCount = 0;

BOOL GetRemoteLockState(WORD UnitID,WORD VarID,float Value)
{
  CString filename = g_ProjectFilePath + "PowerPath.ini";
  for(int no=0;no<256;no++)
  {
    char buf[1024];
    CString key;

    key.Format("%02d",no);
    ::GetPrivateProfileString("遥控闭锁",key,"",buf,1024,filename);
    if(strlen(buf)==0)
      break;

    CNewString ns(buf,".",";");
    CString VarInfo = ns.GetLine(0);
    WORD LockUnitID = atoi(VarInfo.Mid(0,2));
    WORD LockVarID = atoi(VarInfo.Mid(3,2));
    WORD LockValue = atoi(VarInfo.Mid(6,2));
    if((UnitID==LockUnitID)&&(VarID==LockVarID)&&(Value==LockValue))
    {
      for(int varno=1;varno<ns.GetLineCount();varno++)
      {
        VarInfo = ns.GetLine(varno);
        LockUnitID = atoi(VarInfo.Mid(0,2));
        LockVarID = atoi(VarInfo.Mid(3,2));
        LockValue = atoi(VarInfo.Mid(6,2));
        if(g_Unit[LockUnitID].VarGroup[1].Var[LockVarID].Value!=(float)LockValue)
          return FALSE;
      }
      return TRUE;
    }
  }

  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CProtocolTH104

CProtocolTH104::CProtocolTH104()
{
  m_ComputerID = 0;
  m_OperateEnable = FALSE;
  m_pSocketTH104 = NULL;
  m_pCommThread = NULL;

  m_TotalLen = 0;
	m_ReceiveLen = 0;
	memset(m_TotalBuf,0,4096);
	memset(m_ReceiveBuf,0,4096);

  m_FloatSendTypeID = 13;
  m_BitSendTypeID = 1;
  m_SOESendTypeID = 2;

  m_YCAddress=0x4001;
  m_YXAddress=0x0001;
  m_DDAddress=0x6401;
  m_SOEAddress=0x0001;
  m_YKAddress=0x6001;

  m_ChangeFloatDValue=0.5;
  m_ChangeFloatHValue=100.0;
  m_ChangeFloatLValue=0.0;

  m_InterCountVk = 12;//最大发送编号
  m_InterCountVw = 8; //最大确认编号

  m_InterCountVR = 0; //内部接受计数器V(R)
  m_InterCountVS = 0; //内部发送计数器V(S)

  m_InterCountConfirmVR = 0;//内部接受确认计数器V(R)
  m_InterCountConfirmVS = 0;//内部发送确认计数器V(R)

	m_StartSendDataFrame = FALSE;

  m_SendAllFloatDataTime = 0;
  m_SendAllBitDataTime = 0;
  m_SendAllLongDataTime = 0;
}

CProtocolTH104::~CProtocolTH104()
{
}

CString CProtocolTH104::GetTextHexFromBuffer(BYTE * Buf,int nLen)
{
  CString total="";
  for(int pos=0;pos<nLen;pos++)
  {
    CString str;
    str.Format("%02X ",Buf[pos]);
    total+=str;
  }
  return total;
}

BOOL CProtocolTH104::PhysicalReceive( BYTE *pReceive, int & nLen )
{
  if(m_pSocketTH104->PhysicalReceive(pReceive, nLen))
  {
    CString str;
    str.Format("接收成功 (L=%d) %s",nLen,GetTextHexFromBuffer(pReceive,nLen));
    m_pSocketTH104->m_ReceiveText = ::GetTimeText(CTime::GetCurrentTime())+"->"+str;
    m_pSocketTH104->m_ReceiveFrameCount++;
    return TRUE;
  }
  return FALSE;
}

BOOL CProtocolTH104::PhysicalSend( BYTE * buf, int nLen )
{
  if ( nLen <=0 )
    return  FALSE;

  if(m_pSocketTH104==NULL)
    return  FALSE;


  CString str;
  if(m_pSocketTH104->PhysicalSend(buf, nLen)>0)
    str.Format("发送成功 (L=%d) %s",nLen,GetTextHexFromBuffer(buf,nLen));
  else
  {
    str.Format("发送失败 (L=%d) %s",nLen,GetTextHexFromBuffer(buf,nLen));
    m_StartSendDataFrame = FALSE;
    m_pSocketTH104 = NULL;
    return  FALSE;
  }

  m_pSocketTH104->m_SendText = ::GetTimeText(CTime::GetCurrentTime())+"->"+str;
  m_pSocketTH104->m_SendFrameCount++;

  return  TRUE;
}

BOOL CProtocolTH104::SendAllFloatData(WORD UnitID)
{
  if(g_Unit[UnitID].VarGroup[1].Var[63].Value==1)//通讯异常
    return TRUE;

  BYTE SendBuf[1024];
  int SendLen=0;
  memset(SendBuf,0,1024);

  int YCCount = 24;
  float ValueK[24]={(float)0.01,(float)0.01,(float)0.01,(float)0.01,(float)0.1,(float)0.1,(float)0.1,(float)0.1,(float)0.1,(float)0.1,(float)0.1,(float)1,(float)1,(float)0.001,(float)0.001,(float)1,(float)1,(float)1,(float)1,(float)1,(float)1,(float)1,(float)1};

  DWORD HexFloatValue[256];
  WORD  HexValue[256];
  WORD  HexAddress[256];
  BYTE  HexQDS[256];

  int DataCount=0;
  for(int YCVarID=0;YCVarID<YCCount;YCVarID++)
  {
    if(g_Unit[UnitID].VarGroup[0].Var[YCVarID].VarName=="")
      continue;

    g_Unit[UnitID].VarGroup[0].Var[YCVarID].LastValue = g_Unit[UnitID].VarGroup[0].Var[YCVarID].Value;

    HexValue[DataCount] = (WORD)(g_Unit[UnitID].VarGroup[0].Var[YCVarID].Value);
    float Value = (float)(g_Unit[UnitID].VarGroup[0].Var[YCVarID].Value);
    memcpy(&HexFloatValue[DataCount],&Value,4);
    HexAddress[DataCount] = (WORD)(m_YCAddress+YCVarID);
		HexQDS[DataCount] = 0x00;
      
    int MaxDataCount = 0;
    switch(m_FloatSendTypeID)
    {
      case 9:
      case 11:
        MaxDataCount=40;
        break;
      case 13:
        MaxDataCount=30;
        break;
      case 21:
        MaxDataCount=48;
        break;
    }

    DataCount++;

    if(DataCount>=MaxDataCount)
    {
      if(m_FloatSendTypeID==21)
      {
        SendLen=12+DataCount*5;
        SendBuf[0] = 0x68;
	      SendBuf[1] = SendLen-2;
	      SendBuf[2] = 0x00;
	      SendBuf[3] = 0x00;
        SendBuf[4] = 0x00;
	      SendBuf[5] = 0x00;
        SendBuf[6]=21;//不带品质遥测
        SendBuf[7]=DataCount;
        SendBuf[8]=5;
	      SendBuf[9]=0;
	      SendBuf[10]=(BYTE)UnitID;
	      SendBuf[11]=0;
        for(int no=0;no<DataCount;no++)
        {
          SendBuf[12+no*5]=LOBYTE(HexAddress[no]);
          SendBuf[13+no*5]=HIBYTE(HexAddress[no]);
          SendBuf[14+no*5]=0x00;
          SendBuf[15+no*5]=LOBYTE(HexValue[no]);
          SendBuf[16+no*5]=HIBYTE(HexValue[no]);
        }
        PhysicalSend(SendBuf,SendLen);
      }
      if((m_FloatSendTypeID==9)||(m_FloatSendTypeID==11))
      {
        SendLen=12+DataCount*6;
        SendBuf[0] = 0x68;
	      SendBuf[1] = SendLen-2;
	      SendBuf[2] = 0x00;
	      SendBuf[3] = 0x00;
        SendBuf[4] = 0x00;
	      SendBuf[5] = 0x00;
        SendBuf[6]=m_FloatSendTypeID;//带品质遥测
        SendBuf[7]=DataCount;
        SendBuf[8]=5;
	      SendBuf[9]=0;
	      SendBuf[10]=(BYTE)UnitID;
	      SendBuf[11]=0;
        for(int no=0;no<DataCount;no++)
        {
          SendBuf[12+no*6]=LOBYTE(HexAddress[no]);
          SendBuf[13+no*6]=HIBYTE(HexAddress[no]);
          SendBuf[14+no*6]=0x00;
          SendBuf[15+no*6]=LOBYTE(HexValue[no]);
          SendBuf[16+no*6]=HIBYTE(HexValue[no]);
          SendBuf[17+no*6]=HexQDS[no];
        }
        PhysicalSend(SendBuf,SendLen);
      }
      if((m_FloatSendTypeID==13))
      {
        SendLen=12+DataCount*8;
        SendBuf[0] = 0x68;
	      SendBuf[1] = SendLen-2;
	      SendBuf[2] = 0x00;
	      SendBuf[3] = 0x00;
        SendBuf[4] = 0x00;
	      SendBuf[5] = 0x00;
        SendBuf[6]=m_FloatSendTypeID;//带品质遥测
        SendBuf[7]=DataCount;
        SendBuf[8]=5;
	      SendBuf[9]=0;
	      SendBuf[10]=(BYTE)UnitID;
	      SendBuf[11]=0;
        for(int no=0;no<DataCount;no++)
        {
          SendBuf[12+no*8]=LOBYTE(HexAddress[no]);
          SendBuf[13+no*8]=HIBYTE(HexAddress[no]);
          SendBuf[14+no*8]=0x00;
          memcpy(SendBuf+15+no*8,&HexFloatValue[no],4);
        //SendBuf[16+no*8];
        //SendBuf[17+no*8];
        //SendBuf[18+no*8];
          SendBuf[19+no*8]=HexQDS[no];
        }
        PhysicalSend(SendBuf,SendLen);
      }
      DataCount = 0;
    }
  }
  
  if(DataCount>0)
  {
    if(m_FloatSendTypeID==21)
    {
      SendLen=12+DataCount*5;
      SendBuf[0] = 0x68;
	    SendBuf[1] = SendLen-2;
	    SendBuf[2] = 0x00;
	    SendBuf[3] = 0x00;
      SendBuf[4] = 0x00;
	    SendBuf[5] = 0x00;
      SendBuf[6]=21;//不带品质遥测
      SendBuf[7]=DataCount;
      SendBuf[8]=5;
	    SendBuf[9]=0;
	    SendBuf[10]=(BYTE)UnitID;
	    SendBuf[11]=0;
      for(int no=0;no<DataCount;no++)
      {
        SendBuf[12+no*5]=LOBYTE(HexAddress[no]);
        SendBuf[13+no*5]=HIBYTE(HexAddress[no]);
        SendBuf[14+no*5]=0x00;
        SendBuf[15+no*5]=LOBYTE(HexValue[no]);
        SendBuf[16+no*5]=HIBYTE(HexValue[no]);
      }
      PhysicalSend(SendBuf,SendLen);
    }
    if((m_FloatSendTypeID==9)||(m_FloatSendTypeID==11))
    {
      SendLen=12+DataCount*6;
      SendBuf[0] = 0x68;
	    SendBuf[1] = SendLen-2;
	    SendBuf[2] = 0x00;
	    SendBuf[3] = 0x00;
      SendBuf[4] = 0x00;
	    SendBuf[5] = 0x00;
      SendBuf[6]=m_FloatSendTypeID;//带品质遥测
      SendBuf[7]=DataCount;
      SendBuf[8]=5;
	    SendBuf[9]=0;
	    SendBuf[10]=(BYTE)UnitID;
	    SendBuf[11]=0;
      for(int no=0;no<DataCount;no++)
      {
        SendBuf[12+no*6]=LOBYTE(HexAddress[no]);
        SendBuf[13+no*6]=HIBYTE(HexAddress[no]);
        SendBuf[14+no*6]=0x00;
        SendBuf[15+no*6]=LOBYTE(HexValue[no]);
        SendBuf[16+no*6]=HIBYTE(HexValue[no]);
        SendBuf[17+no*6]=HexQDS[no];
      }
      PhysicalSend(SendBuf,SendLen);
    }
    if((m_FloatSendTypeID==13))
    {
      SendLen=12+DataCount*8;
      SendBuf[0] = 0x68;
	    SendBuf[1] = SendLen-2;
	    SendBuf[2] = 0x00;
	    SendBuf[3] = 0x00;
      SendBuf[4] = 0x00;
	    SendBuf[5] = 0x00;
      SendBuf[6]=m_FloatSendTypeID;//带品质遥测
      SendBuf[7]=DataCount;
      SendBuf[8]=5;
	    SendBuf[9]=0;
	    SendBuf[10]=(BYTE)UnitID;
	    SendBuf[11]=0;
      for(int no=0;no<DataCount;no++)
      {
        SendBuf[12+no*8]=LOBYTE(HexAddress[no]);
        SendBuf[13+no*8]=HIBYTE(HexAddress[no]);
        SendBuf[14+no*8]=0x00;
        memcpy(SendBuf+15+no*8,&HexFloatValue[no],4);
      //SendBuf[16+no*8];
      //SendBuf[17+no*8];
      //SendBuf[18+no*8];
        SendBuf[19+no*8]=HexQDS[no];
      }
      PhysicalSend(SendBuf,SendLen);
    }
  }

  return TRUE;
}

BOOL CProtocolTH104::SendAllBitData(WORD UnitID)
{
  BYTE SendBuf[1024];
  int SendLen=0;
  memset(SendBuf,0,1024);

  int YXCount = 64;

  BYTE HexValueSP[256];
  BYTE HexValueDP[256];
  WORD HexAddress[256];

  int DataCount=0;
  for(int YXVarID=0;YXVarID<YXCount;YXVarID++)
  {
    if(g_Unit[UnitID].VarGroup[1].Var[YXVarID].VarName=="")
      continue;

    if((g_Unit[UnitID].VarGroup[1].Var[63].Value==1)&&(YXVarID!=63))//通讯异常
      continue;

    g_Unit[UnitID].VarGroup[1].Var[YXVarID].LastValue = g_Unit[UnitID].VarGroup[1].Var[YXVarID].Value;

    float Value = g_Unit[UnitID].VarGroup[1].Var[YXVarID].Value;
    if(g_Unit[UnitID].VarAbnormal[YXVarID]==TRUE)
    {
      if(Value==0)
        Value = 1;
      else
        Value = 0;
    }
    if(Value==0)
    {
      HexValueSP[DataCount] = (BYTE)0x00;
      HexValueDP[DataCount] = (BYTE)0x01;
    }
    else
    {
      HexValueSP[DataCount] = (BYTE)0x01;
      HexValueDP[DataCount] = (BYTE)0x02;
    }
    HexAddress[DataCount] = (WORD)(m_YXAddress+YXVarID);
      
    DataCount++;

    if(DataCount>=32)
    {
      SendLen=12+DataCount*4;
      SendBuf[0] = 0x68;
	    SendBuf[1] = SendLen-2;
	    SendBuf[2] = 0x00;
	    SendBuf[3] = 0x00;
      SendBuf[4] = 0x00;
	    SendBuf[5] = 0x00;
      if(m_BitSendTypeID==1)
        SendBuf[6]=1;//单点遥信
      if(m_BitSendTypeID==3)
        SendBuf[6]=3;//双点遥信
      SendBuf[7]=DataCount;
      SendBuf[8]=5;
      SendBuf[9]=0;
      SendBuf[10]=(BYTE)UnitID;
      SendBuf[11]=0;
      for(int no=0;no<DataCount;no++)
      {
        SendBuf[12+no*4]=LOBYTE(HexAddress[no]);
        SendBuf[13+no*4]=HIBYTE(HexAddress[no]);
        SendBuf[14+no*4]=0;
        if(m_BitSendTypeID==1)
          SendBuf[15+no*4]=HexValueSP[no];
        if(m_BitSendTypeID==3)
          SendBuf[15+no*4]=HexValueDP[no];
      }
      PhysicalSend(SendBuf,SendLen);
      DataCount=0;
    }
  }

  if(DataCount>0)
  {
    SendLen=12+DataCount*4;
    SendBuf[0] = 0x68;
	  SendBuf[1] = SendLen-2;
	  SendBuf[2] = 0x00;
	  SendBuf[3] = 0x00;
    SendBuf[4] = 0x00;
	  SendBuf[5] = 0x00;
    if(m_BitSendTypeID==1)
      SendBuf[6]=1;//单点遥信
    if(m_BitSendTypeID==3)
      SendBuf[6]=3;//双点遥信
    SendBuf[7]=DataCount;
    SendBuf[8]=5;
    SendBuf[9]=0;
    SendBuf[10]=(BYTE)UnitID;
    SendBuf[11]=0;
    for(int no=0;no<DataCount;no++)
    {
      SendBuf[12+no*4]=LOBYTE(HexAddress[no]);
      SendBuf[13+no*4]=HIBYTE(HexAddress[no]);
      SendBuf[14+no*4]=0;
      if(m_BitSendTypeID==1)
        SendBuf[15+no*4]=HexValueSP[no];
      if(m_BitSendTypeID==3)
        SendBuf[15+no*4]=HexValueDP[no];
    }
    PhysicalSend(SendBuf,SendLen);
    DataCount=0;
  }

  return TRUE;
}

BOOL CProtocolTH104::SendChangeFloatData(WORD UnitID)
{
  BYTE SendBuf[1024];
  int SendLen=0;
  memset(SendBuf,0,1024);

  int YCCount = 24;
  float ValueK[24]={(float)0.01,(float)0.01,(float)0.01,(float)0.01,(float)0.1,(float)0.1,(float)0.1,(float)0.1,(float)0.1,(float)0.1,(float)0.1,(float)1,(float)1,(float)0.001,(float)0.001,(float)1,(float)1,(float)1,(float)1,(float)1,(float)1,(float)1,(float)1};

  DWORD HexFloatValue[256];
  WORD  HexValue[256];
  WORD  HexAddress[256];
  BYTE  HexQDS[256];

  int DataCount=0;
  for(int YCVarID=0;YCVarID<YCCount;YCVarID++)
  {
    if(g_Unit[UnitID].VarGroup[0].Var[YCVarID].VarName=="")
      continue;

    if(g_Unit[UnitID].VarGroup[0].Var[YCVarID].LastValue==g_Unit[UnitID].VarGroup[0].Var[YCVarID].Value)
      continue;

    g_Unit[UnitID].VarGroup[0].Var[YCVarID].LastValue = g_Unit[UnitID].VarGroup[0].Var[YCVarID].Value;

    HexValue[DataCount] = (WORD)(g_Unit[UnitID].VarGroup[0].Var[YCVarID].Value);
    float Value = (float)(g_Unit[UnitID].VarGroup[0].Var[YCVarID].Value);
    memcpy(&HexFloatValue[DataCount],&Value,4);
    HexAddress[DataCount] = (WORD)(m_YCAddress+YCVarID);
		HexQDS[DataCount] = 0x00;
      
    int MaxDataCount = 0;
    switch(m_FloatSendTypeID)
    {
      case 9:
      case 11:
        MaxDataCount=40;
        break;
      case 13:
        MaxDataCount=30;
        break;
      case 21:
        MaxDataCount=48;
        break;
    }

    DataCount++;

    if(DataCount>=MaxDataCount)
    {
      if(m_FloatSendTypeID==21)
      {
        SendLen=12+DataCount*5;
        SendBuf[0] = 0x68;
	      SendBuf[1] = SendLen-2;
	      SendBuf[2] = 0x00;
	      SendBuf[3] = 0x00;
        SendBuf[4] = 0x00;
	      SendBuf[5] = 0x00;
        SendBuf[6]=21;//不带品质遥测
        SendBuf[7]=DataCount;
        SendBuf[8]=5;
	      SendBuf[9]=0;
	      SendBuf[10]=(BYTE)UnitID;
	      SendBuf[11]=0;
        for(int no=0;no<DataCount;no++)
        {
          SendBuf[12+no*5]=LOBYTE(HexAddress[no]);
          SendBuf[13+no*5]=HIBYTE(HexAddress[no]);
          SendBuf[14+no*5]=0x00;
          SendBuf[15+no*5]=LOBYTE(HexValue[no]);
          SendBuf[16+no*5]=HIBYTE(HexValue[no]);
        }
        PhysicalSend(SendBuf,SendLen);
      }
      if((m_FloatSendTypeID==9)||(m_FloatSendTypeID==11))
      {
        SendLen=12+DataCount*6;
        SendBuf[0] = 0x68;
	      SendBuf[1] = SendLen-2;
	      SendBuf[2] = 0x00;
	      SendBuf[3] = 0x00;
        SendBuf[4] = 0x00;
	      SendBuf[5] = 0x00;
        SendBuf[6]=m_FloatSendTypeID;//带品质遥测
        SendBuf[7]=DataCount;
        SendBuf[8]=5;
	      SendBuf[9]=0;
	      SendBuf[10]=(BYTE)UnitID;
	      SendBuf[11]=0;
        for(int no=0;no<DataCount;no++)
        {
          SendBuf[12+no*6]=LOBYTE(HexAddress[no]);
          SendBuf[13+no*6]=HIBYTE(HexAddress[no]);
          SendBuf[14+no*6]=0x00;
          SendBuf[15+no*6]=LOBYTE(HexValue[no]);
          SendBuf[16+no*6]=HIBYTE(HexValue[no]);
          SendBuf[17+no*6]=HexQDS[no];
        }
        PhysicalSend(SendBuf,SendLen);
      }
      if((m_FloatSendTypeID==13))
      {
        SendLen=12+DataCount*8;
        SendBuf[0] = 0x68;
	      SendBuf[1] = SendLen-2;
	      SendBuf[2] = 0x00;
	      SendBuf[3] = 0x00;
        SendBuf[4] = 0x00;
	      SendBuf[5] = 0x00;
        SendBuf[6]=m_FloatSendTypeID;//带品质遥测
        SendBuf[7]=DataCount;
        SendBuf[8]=5;
	      SendBuf[9]=0;
	      SendBuf[10]=(BYTE)UnitID;
	      SendBuf[11]=0;
        for(int no=0;no<DataCount;no++)
        {
          SendBuf[12+no*8]=LOBYTE(HexAddress[no]);
          SendBuf[13+no*8]=HIBYTE(HexAddress[no]);
          SendBuf[14+no*8]=0x00;
          memcpy(SendBuf+15+no*8,&HexFloatValue[no],4);
        //SendBuf[16+no*8];
        //SendBuf[17+no*8];
        //SendBuf[18+no*8];
          SendBuf[19+no*8]=HexQDS[no];
        }
        PhysicalSend(SendBuf,SendLen);
      }
      DataCount = 0;
    }
  }
  
  if(DataCount>0)
  {
    if(m_FloatSendTypeID==21)
    {
      SendLen=12+DataCount*5;
      SendBuf[0] = 0x68;
	    SendBuf[1] = SendLen-2;
	    SendBuf[2] = 0x00;
	    SendBuf[3] = 0x00;
      SendBuf[4] = 0x00;
	    SendBuf[5] = 0x00;
      SendBuf[6]=21;//不带品质遥测
      SendBuf[7]=DataCount;
      SendBuf[8]=5;
	    SendBuf[9]=0;
	    SendBuf[10]=(BYTE)UnitID;
	    SendBuf[11]=0;
      for(int no=0;no<DataCount;no++)
      {
        SendBuf[12+no*5]=LOBYTE(HexAddress[no]);
        SendBuf[13+no*5]=HIBYTE(HexAddress[no]);
        SendBuf[14+no*5]=0x00;
        SendBuf[15+no*5]=LOBYTE(HexValue[no]);
        SendBuf[16+no*5]=HIBYTE(HexValue[no]);
      }
      PhysicalSend(SendBuf,SendLen);
    }
    if((m_FloatSendTypeID==9)||(m_FloatSendTypeID==11))
    {
      SendLen=12+DataCount*6;
      SendBuf[0] = 0x68;
	    SendBuf[1] = SendLen-2;
	    SendBuf[2] = 0x00;
	    SendBuf[3] = 0x00;
      SendBuf[4] = 0x00;
	    SendBuf[5] = 0x00;
      SendBuf[6]=m_FloatSendTypeID;//带品质遥测
      SendBuf[7]=DataCount;
      SendBuf[8]=5;
	    SendBuf[9]=0;
	    SendBuf[10]=(BYTE)UnitID;
	    SendBuf[11]=0;
      for(int no=0;no<DataCount;no++)
      {
        SendBuf[12+no*6]=LOBYTE(HexAddress[no]);
        SendBuf[13+no*6]=HIBYTE(HexAddress[no]);
        SendBuf[14+no*6]=0x00;
        SendBuf[15+no*6]=LOBYTE(HexValue[no]);
        SendBuf[16+no*6]=HIBYTE(HexValue[no]);
        SendBuf[17+no*6]=HexQDS[no];
      }
      PhysicalSend(SendBuf,SendLen);
    }
    if((m_FloatSendTypeID==13))
    {
      SendLen=12+DataCount*8;
      SendBuf[0] = 0x68;
	    SendBuf[1] = SendLen-2;
	    SendBuf[2] = 0x00;
	    SendBuf[3] = 0x00;
      SendBuf[4] = 0x00;
	    SendBuf[5] = 0x00;
      SendBuf[6]=m_FloatSendTypeID;//带品质遥测
      SendBuf[7]=DataCount;
      SendBuf[8]=5;
	    SendBuf[9]=0;
	    SendBuf[10]=(BYTE)UnitID;
	    SendBuf[11]=0;
      for(int no=0;no<DataCount;no++)
      {
        SendBuf[12+no*8]=LOBYTE(HexAddress[no]);
        SendBuf[13+no*8]=HIBYTE(HexAddress[no]);
        SendBuf[14+no*8]=0x00;
        memcpy(SendBuf+15+no*8,&HexFloatValue[no],4);
      //SendBuf[16+no*8];
      //SendBuf[17+no*8];
      //SendBuf[18+no*8];
        SendBuf[19+no*8]=HexQDS[no];
      }
      PhysicalSend(SendBuf,SendLen);
    }
  }

  return TRUE;
}

BOOL CProtocolTH104::SendChangeBitData(WORD UnitID)
{
  BYTE SendBuf[1024];
  int SendLen=0;
  memset(SendBuf,0,1024);

  int YXCount = 64;

  BYTE HexValueSP[256];
  BYTE HexValueDP[256];
  WORD HexAddress[256];

  int DataCount=0;
  for(int YXVarID=0;YXVarID<YXCount;YXVarID++)
  {
    if(g_Unit[UnitID].VarGroup[1].Var[YXVarID].VarName=="")
      continue;

    if((g_Unit[UnitID].VarGroup[1].Var[63].Value==1)&&(YXVarID!=63))//通讯异常
      continue;

    if(g_Unit[UnitID].VarGroup[1].Var[YXVarID].LastValue==g_Unit[UnitID].VarGroup[1].Var[YXVarID].Value)
      continue;

    g_Unit[UnitID].VarGroup[1].Var[YXVarID].LastValue = g_Unit[UnitID].VarGroup[1].Var[YXVarID].Value;

    float Value = g_Unit[UnitID].VarGroup[1].Var[YXVarID].Value;
    if(g_Unit[UnitID].VarAbnormal[YXVarID]==TRUE)
    {
      if(Value==0)
        Value = 1;
      else
        Value = 0;
    }
    if(Value==0)
    {
      HexValueSP[DataCount] = (BYTE)0x00;
      HexValueDP[DataCount] = (BYTE)0x01;
    }
    else
    {
      HexValueSP[DataCount] = (BYTE)0x01;
      HexValueDP[DataCount] = (BYTE)0x02;
    }
    HexAddress[DataCount] = (WORD)(m_YXAddress+YXVarID);
      
    DataCount++;

    if(DataCount>=32)
    {
      SendLen=12+DataCount*4;
      SendBuf[0] = 0x68;
	    SendBuf[1] = SendLen-2;
	    SendBuf[2] = 0x00;
	    SendBuf[3] = 0x00;
      SendBuf[4] = 0x00;
	    SendBuf[5] = 0x00;
      if(m_BitSendTypeID==1)
        SendBuf[6]=1;//单点遥信
      if(m_BitSendTypeID==3)
        SendBuf[6]=3;//双点遥信
      SendBuf[7]=DataCount;
      SendBuf[8]=5;
      SendBuf[9]=0;
      SendBuf[10]=(BYTE)UnitID;
      SendBuf[11]=0;
      for(int no=0;no<DataCount;no++)
      {
        SendBuf[12+no*4]=LOBYTE(HexAddress[no]);
        SendBuf[13+no*4]=HIBYTE(HexAddress[no]);
        SendBuf[14+no*4]=0;
        if(m_BitSendTypeID==1)
          SendBuf[15+no*4]=HexValueSP[no];
        if(m_BitSendTypeID==3)
          SendBuf[15+no*4]=HexValueDP[no];
      }
      PhysicalSend(SendBuf,SendLen);
      DataCount=0;
    }
  }

  if(DataCount>0)
  {
    SendLen=12+DataCount*4;
    SendBuf[0] = 0x68;
	  SendBuf[1] = SendLen-2;
	  SendBuf[2] = 0x00;
	  SendBuf[3] = 0x00;
    SendBuf[4] = 0x00;
	  SendBuf[5] = 0x00;
    if(m_BitSendTypeID==1)
      SendBuf[6]=1;//单点遥信
    if(m_BitSendTypeID==3)
      SendBuf[6]=3;//双点遥信
    SendBuf[7]=DataCount;
    SendBuf[8]=5;
    SendBuf[9]=0;
    SendBuf[10]=(BYTE)UnitID;
    SendBuf[11]=0;
    for(int no=0;no<DataCount;no++)
    {
      SendBuf[12+no*4]=LOBYTE(HexAddress[no]);
      SendBuf[13+no*4]=HIBYTE(HexAddress[no]);
      SendBuf[14+no*4]=0;
      if(m_BitSendTypeID==1)
        SendBuf[15+no*4]=HexValueSP[no];
      if(m_BitSendTypeID==3)
        SendBuf[15+no*4]=HexValueDP[no];
    }
    PhysicalSend(SendBuf,SendLen);
    DataCount=0;
  }

  return TRUE;
}

BOOL CProtocolTH104::SendChangeSOEData(WORD UnitID)
{
  if(g_Unit[UnitID].VarGroup[1].Var[63].Value==1)//通讯异常
    return TRUE;

  CSingleLock sLock(&m_SendVarFrameMutex);
  sLock.Lock();

  if(m_SendSOEReportList[UnitID].GetCount()==0)
    return FALSE;

  TagSOEReportStruct *pSendSOEReport = (TagSOEReportStruct *)m_SendSOEReportList[UnitID].RemoveHead();

  if(::LoadSOEDefActionValue(g_Unit[pSendSOEReport->UnitID].UnitType,pSendSOEReport->SOEID)==TRUE)
    SendActionValue(UnitID,pSendSOEReport);

  BYTE SendBuf[1024];
  int SendLen=0;
  memset(SendBuf,0,1024);

  BYTE HexValueSP[256];
  BYTE HexValueDP[256];
  BYTE HexYear[256];
  BYTE HexMon[256];
  BYTE HexDay[256];
  BYTE HexHour[256];
  BYTE HexMin[256];
  WORD HexMSEL[256];
  WORD HexAddress[256];

  int DataCount=0;

  if(pSendSOEReport->ActionValue==0)
  {
    HexValueSP[DataCount] = (BYTE)0x00;
    HexValueDP[DataCount] = (BYTE)0x01;
  }
  else
  {
    HexValueSP[DataCount] = (BYTE)0x01;
    HexValueDP[DataCount] = (BYTE)0x02;
  }

  HexYear[DataCount] = (BYTE)(pSendSOEReport->SOETime.GetYear()-2000);
  HexMon[DataCount] = (BYTE)pSendSOEReport->SOETime.GetMonth();
  HexDay[DataCount] = (BYTE)pSendSOEReport->SOETime.GetDay();
  HexHour[DataCount] = (BYTE)pSendSOEReport->SOETime.GetHour();
  HexMin[DataCount] = (BYTE)pSendSOEReport->SOETime.GetMinute();
  HexMSEL[DataCount] = (WORD)(pSendSOEReport->SOEMSEL+pSendSOEReport->SOETime.GetSecond()*1000);
  HexAddress[DataCount] = (WORD)(m_SOEAddress+pSendSOEReport->SOEID);

  delete pSendSOEReport;

  DataCount++;

  if(m_SOESendTypeID==2)
  {
    SendLen=12+DataCount*7;
    SendBuf[0] = 0x68;
	  SendBuf[1] = SendLen-2;
	  SendBuf[2] = 0x00;
	  SendBuf[3] = 0x00;
    SendBuf[4] = 0x00;
	  SendBuf[5] = 0x00;
    SendBuf[6]=2;//单点SOE短时标
    SendBuf[7]=DataCount;
    SendBuf[8]=5;
    SendBuf[9]=0;
    SendBuf[10]=(BYTE)UnitID;
    SendBuf[11]=0;
    for(int no=0;no<DataCount;no++)
    {
      SendBuf[12+no*7]=LOBYTE(HexAddress[no]);
      SendBuf[13+no*7]=HIBYTE(HexAddress[no]);
      SendBuf[14+no*7]=0;
      SendBuf[15+no*7]=HexValueSP[no];
      SendBuf[16+no*7]=LOBYTE(HexMSEL[no]);
      SendBuf[17+no*7]=HIBYTE(HexMSEL[no]);
      SendBuf[18+no*7]=HexMin[no];
    }
    PhysicalSend(SendBuf,SendLen);
  }
  if(m_SOESendTypeID==30)
  {
    SendLen=12+DataCount*11;
    SendBuf[0] = 0x68;
	  SendBuf[1] = SendLen-2;
	  SendBuf[2] = 0x00;
	  SendBuf[3] = 0x00;
    SendBuf[4] = 0x00;
	  SendBuf[5] = 0x00;
    SendBuf[6]=30;//单点SOE长时标
    SendBuf[7]=DataCount;
    SendBuf[8]=5;
    SendBuf[9]=0;
    SendBuf[10]=(BYTE)UnitID;
    SendBuf[11]=0;
    for(int no=0;no<DataCount;no++)
    {
      SendBuf[12+no*11]=LOBYTE(HexAddress[no]);
      SendBuf[13+no*11]=HIBYTE(HexAddress[no]);
      SendBuf[14+no*11]=0;
      SendBuf[15+no*11]=HexValueSP[no];
      SendBuf[16+no*11]=LOBYTE(HexMSEL[no]);
      SendBuf[17+no*11]=HIBYTE(HexMSEL[no]);
      SendBuf[18+no*11]=HexMin[no];
      SendBuf[19+no*11]=HexHour[no];
      SendBuf[20+no*11]=HexDay[no];
      SendBuf[21+no*11]=HexMon[no];
      SendBuf[22+no*11]=HexYear[no];
    }
    PhysicalSend(SendBuf,SendLen);
  }
  if(m_SOESendTypeID==4)
  {
    SendLen=12+DataCount*7;
    SendBuf[0] = 0x68;
	  SendBuf[1] = SendLen-2;
	  SendBuf[2] = 0x00;
	  SendBuf[3] = 0x00;
    SendBuf[4] = 0x00;
	  SendBuf[5] = 0x00;
    SendBuf[6]=4;//双点SOE短时标
    SendBuf[7]=DataCount;
    SendBuf[8]=5;
    SendBuf[9]=0;
    SendBuf[10]=(BYTE)UnitID;
    SendBuf[11]=0;
    for(int no=0;no<DataCount;no++)
    {
      SendBuf[12+no*7]=LOBYTE(HexAddress[no]);
      SendBuf[13+no*7]=HIBYTE(HexAddress[no]);
      SendBuf[14+no*7]=0;
      SendBuf[15+no*7]=HexValueDP[no];
      SendBuf[16+no*7]=LOBYTE(HexMSEL[no]);
      SendBuf[17+no*7]=HIBYTE(HexMSEL[no]);
      SendBuf[18+no*7]=HexMin[no];
    }
    PhysicalSend(SendBuf,SendLen);
  }
  if(m_SOESendTypeID==31)
  {
    SendLen=12+DataCount*11;
    SendBuf[0] = 0x68;
	  SendBuf[1] = SendLen-2;
	  SendBuf[2] = 0x00;
	  SendBuf[3] = 0x00;
    SendBuf[4] = 0x00;
	  SendBuf[5] = 0x00;
    SendBuf[6]=31;//双点SOE长时标
    SendBuf[7]=DataCount;
    SendBuf[8]=5;
    SendBuf[9]=0;
    SendBuf[10]=(BYTE)UnitID;
    SendBuf[11]=0;
    for(int no=0;no<DataCount;no++)
    {
      SendBuf[12+no*11]=LOBYTE(HexAddress[no]);
      SendBuf[13+no*11]=HIBYTE(HexAddress[no]);
      SendBuf[14+no*11]=0;
      SendBuf[15+no*11]=HexValueDP[no];
      SendBuf[16+no*11]=LOBYTE(HexMSEL[no]);
      SendBuf[17+no*11]=HIBYTE(HexMSEL[no]);
      SendBuf[18+no*11]=HexMin[no];
      SendBuf[19+no*11]=HexHour[no];
      SendBuf[20+no*11]=HexDay[no];
      SendBuf[21+no*11]=HexMon[no];
      SendBuf[22+no*11]=HexYear[no];
    }
    PhysicalSend(SendBuf,SendLen);
  }
  DataCount = 0;

  return TRUE;
}

BOOL CProtocolTH104::SendVarFrame(WORD UnitID,TagVarStruct *pVar)//变量发送
{
  BYTE SendBuf[1024];
  int SendLen=0;
  memset(SendBuf,0,1024);

  int YXCount = 64;

  BYTE HexValueSP;
  BYTE HexValueDP;
  WORD HexAddress;

  if(g_Unit[UnitID].VarGroup[1].Var[pVar->VarID].Value==0)
  {
    HexValueSP = (BYTE)0x00;
    HexValueDP = (BYTE)0x01;
  }
  else
  {
    HexValueSP = (BYTE)0x01;
    HexValueDP = (BYTE)0x02;
  }
  HexAddress = (WORD)(m_YXAddress+pVar->VarID);

  SendLen=12+4;
  SendBuf[0] = 0x68;
	SendBuf[1] = SendLen-2;
	SendBuf[2] = 0x00;
	SendBuf[3] = 0x00;
  SendBuf[4] = 0x00;
	SendBuf[5] = 0x00;
  if(m_BitSendTypeID==1)
    SendBuf[6]=1;//单点遥信
  if(m_BitSendTypeID==3)
    SendBuf[6]=3;//双点遥信
  SendBuf[7]=1;
  SendBuf[8]=5;
  SendBuf[9]=0;
  SendBuf[10]=(BYTE)UnitID;
  SendBuf[11]=0;
  SendBuf[12]=LOBYTE(HexAddress);
  SendBuf[13]=HIBYTE(HexAddress);
  SendBuf[14]=0;
  if(m_BitSendTypeID==1)
    SendBuf[15]=HexValueSP;
  if(m_BitSendTypeID==3)
    SendBuf[15]=HexValueDP;

  PhysicalSend(SendBuf,SendLen);

  return TRUE;
}

BOOL CProtocolTH104::SendAllLongData(WORD UnitID)
{
  return TRUE;

  BYTE SendBuf[1024];
  int SendLen=0;
  memset(SendBuf,0,1024);

  int DDCount = 8;

  WORD HexHValue[256];
  WORD HexLValue[256];
  WORD  HexAddress[256];

  int DataCount=0;
  for(int DDVarID=0;DDVarID<DDCount;DDVarID++)
  {
    DWORD HexValue = (DWORD)(g_Unit[UnitID].VarGroup[2].Var[DDVarID].Value*100);
    HexHValue[DataCount] = HIWORD(HexValue);
    HexLValue[DataCount] = LOWORD(HexValue);
    HexAddress[DataCount] = (WORD)(m_DDAddress+DDVarID);

    DataCount++;
    if((DataCount>=30)||(DDVarID>=(DDCount-1)))
    {
      SendLen=12+DataCount*8;
      SendBuf[0] = 0x68;
	    SendBuf[1] = SendLen-2;
	    SendBuf[2] = 0x00;
	    SendBuf[3] = 0x00;
      SendBuf[4] = 0x00;
	    SendBuf[5] = 0x00;
      SendBuf[6]=15;//电度
      SendBuf[7]=DataCount;
      SendBuf[8]=37;
	    SendBuf[9] = 0;
      SendBuf[10]=(BYTE)UnitID;
	    SendBuf[11] = 0;
      for(int no=0;no<DataCount;no++)
      {
        SendBuf[12+no*8]=LOBYTE(HexAddress[no]);
        SendBuf[13+no*8]=HIBYTE(HexAddress[no]);
        SendBuf[14+no*8]=0;
        SendBuf[15+no*8]=LOBYTE(HexLValue[no]);
        SendBuf[16+no*8]=HIBYTE(HexLValue[no]);
        SendBuf[17+no*8]=LOBYTE(HexHValue[no]);
        SendBuf[18+no*8]=HIBYTE(HexHValue[no]);
        SendBuf[19+no*8]=0x00;
      }
      PhysicalSend(SendBuf,SendLen);
      DataCount = 0;
    }
  }
  return TRUE;
}

void CProtocolTH104::ProcessCallAllDataBegin(BYTE CallType)   //召唤全数据
{
  BYTE SendBuf[1024];
  int SendLen=0;
  memset(SendBuf,0,1024);

  SendLen=16;
  SendBuf[0] = 0x68;
	SendBuf[1] = SendLen-2;
	SendBuf[2] = 0x00;
	SendBuf[3] = 0x00;
  SendBuf[4] = 0x00;
	SendBuf[5] = 0x00;
	SendBuf[6] = 100;
  SendBuf[7] = 0x01;
  SendBuf[8] = 7;//传输确认
	SendBuf[9] = 0x00;
	SendBuf[10] = (BYTE)0x00;
	SendBuf[11] = 0x00;
	SendBuf[12] = 0x00;
	SendBuf[13] = 0x00;
	SendBuf[14] = 0x00;
	SendBuf[15] = CallType;
  PhysicalSend(SendBuf,SendLen);

  for(int unitid=0;unitid<UNITCOUNT;unitid++)
  {
    if(g_Unit[unitid].UnitType==0)
      continue;
    SendAllFloatData(unitid);
    SendAllBitData(unitid);
  }
}

BOOL CProtocolTH104::SendParameterData(WORD UnitID,WORD GroupID)
{
  if(GroupID>3)
    GroupID = g_Unit[UnitID].CurrentParamaterGroupID;
  
  WORD InfoAddress = 0x0000;
  switch(GroupID)
  {
    case 0:
      InfoAddress = 0x5001;
      break;
    case 1:
      InfoAddress = 0x5101;
      break;
    case 2:
      InfoAddress = 0x5201;
      break;
    case 3:
      InfoAddress = 0x5301;
      break;
  }

  BYTE SendBuf[1024];
  int SendLen=0;
  memset(SendBuf,0,1024);

  BYTE SEQUDCS = 0x00;
  int ParameterCount = 32;

  {
    Sleep(250);
    SendLen=16+32*4;
    SendBuf[0] = 0x68;
	  SendBuf[1] = SendLen-2;
	  SendBuf[2] = 0x00;
	  SendBuf[3] = 0x00;
    SendBuf[4] = 0x00;
	  SendBuf[5] = 0x00;
    SendBuf[6]=50;//定值
    SendBuf[7]=ParameterCount+0x80;
    SendBuf[8]=7;
	  SendBuf[9]=0;
	  SendBuf[10]=(BYTE)UnitID;
	  SendBuf[11]=0;
    SendBuf[12]=LOBYTE(InfoAddress);
    SendBuf[13]=HIBYTE(InfoAddress);
    SendBuf[14]=0x00;
    SendBuf[15]=SEQUDCS;

    for(int VarID=0;VarID<32;VarID++)
    {
      float Value = g_Unit[UnitID].ParameterGroup[GroupID].Var[VarID].Value;
      memcpy(SendBuf+16+VarID*4,&Value,4);
    }
    PhysicalSend(SendBuf,SendLen);
  }
  {
    Sleep(250);
    SendLen=16+32*4;
    SendBuf[0] = 0x68;
	  SendBuf[1] = SendLen-2;
	  SendBuf[2] = 0x00;
	  SendBuf[3] = 0x00;
    SendBuf[4] = 0x00;
	  SendBuf[5] = 0x00;
    SendBuf[6]=50;//定值
    SendBuf[7]=ParameterCount+0x80;
    SendBuf[8]=7;
	  SendBuf[9]=0;
	  SendBuf[10]=(BYTE)UnitID;
	  SendBuf[11]=0;
    SendBuf[12]=LOBYTE(InfoAddress+32);
    SendBuf[13]=HIBYTE(InfoAddress+32);
    SendBuf[14]=0x00;
    SendBuf[15]=SEQUDCS;

    for(int VarID=0;VarID<32;VarID++)
    {
      float Value = g_Unit[UnitID].ParameterGroup[GroupID].Var[VarID+32].Value;
      memcpy(SendBuf+16+VarID*4,&Value,4);
    }
    PhysicalSend(SendBuf,SendLen);
  }
  {
    Sleep(250);
    SendLen=16+32*4;
    SendBuf[0] = 0x68;
	  SendBuf[1] = SendLen-2;
	  SendBuf[2] = 0x00;
	  SendBuf[3] = 0x00;
    SendBuf[4] = 0x00;
	  SendBuf[5] = 0x00;
    SendBuf[6]=50;//定值
    SendBuf[7]=ParameterCount+0x80;
    SendBuf[8]=7;
	  SendBuf[9]=0;
	  SendBuf[10]=(BYTE)UnitID;
	  SendBuf[11]=0;
    SendBuf[12]=LOBYTE(InfoAddress+64);
    SendBuf[13]=HIBYTE(InfoAddress+64);
    SendBuf[14]=0x00;
    SendBuf[15]=SEQUDCS;

    for(int VarID=0;VarID<32;VarID++)
    {
      float Value = g_Unit[UnitID].ParameterGroup[GroupID].Var[VarID+64].Value;
      memcpy(SendBuf+16+VarID*4,&Value,4);
    }
    PhysicalSend(SendBuf,SendLen);
  }
  {
    Sleep(250);
    SendLen=16+32*4;
    SendBuf[0] = 0x68;
	  SendBuf[1] = SendLen-2;
	  SendBuf[2] = 0x00;
	  SendBuf[3] = 0x00;
    SendBuf[4] = 0x00;
	  SendBuf[5] = 0x00;
    SendBuf[6]=50;//定值
    SendBuf[7]=ParameterCount+0x80;
    SendBuf[8]=7;
	  SendBuf[9]=0;
	  SendBuf[10]=(BYTE)UnitID;
	  SendBuf[11]=0;
    SendBuf[12]=LOBYTE(InfoAddress+96);
    SendBuf[13]=HIBYTE(InfoAddress+96);
    SendBuf[14]=0x00;
    SendBuf[15]=SEQUDCS;

    for(int VarID=0;VarID<32;VarID++)
    {
      float Value = g_Unit[UnitID].ParameterGroup[GroupID].Var[VarID+96].Value;
      memcpy(SendBuf+16+VarID*4,&Value,4);
    }
    PhysicalSend(SendBuf,SendLen);
  }
  {
    Sleep(250);
    SendLen=16;
    SendBuf[0] = 0x68;
	  SendBuf[1] = SendLen-2;
	  SendBuf[2] = 0x00;
	  SendBuf[3] = 0x00;
    SendBuf[4] = 0x00;
	  SendBuf[5] = 0x00;
    SendBuf[6]=50;//定值
    SendBuf[7]=0x80;//发送完成
    SendBuf[8]=7;
	  SendBuf[9]=0;
	  SendBuf[10]=(BYTE)UnitID;
	  SendBuf[11]=0;
    SendBuf[12]=LOBYTE(InfoAddress);
    SendBuf[13]=HIBYTE(InfoAddress);
    SendBuf[14]=0x00;
    SendBuf[15]=SEQUDCS;

    PhysicalSend(SendBuf,SendLen);
  }

  return TRUE;
}

BOOL CProtocolTH104::SendParameterGroupID(WORD UnitID)
{
  Sleep(1000);

  WORD InfoAddress = 0x0000;
  switch(g_Unit[UnitID].CurrentParamaterGroupID)
  {
    case 0:
      InfoAddress = 0x5001;
      break;
    case 1:
      InfoAddress = 0x5101;
      break;
    case 2:
      InfoAddress = 0x5201;
      break;
    case 3:
      InfoAddress = 0x5301;
      break;
    case 4:
      InfoAddress = 0x5401;
      break;
  }

  BYTE SendBuf[1024];
  int SendLen=0;
  memset(SendBuf,0,1024);

  BYTE SEQUDCS = 0x00;

  SendLen=16;
  SendBuf[0] = 0x68;
	SendBuf[1] = SendLen-2;
	SendBuf[2] = 0x00;
	SendBuf[3] = 0x00;
  SendBuf[4] = 0x00;
	SendBuf[5] = 0x00;
  SendBuf[6]=51;//定值组号
  SendBuf[7]=0x80;
  SendBuf[8]=7;
	SendBuf[9]=0;
	SendBuf[10]=(BYTE)UnitID;
	SendBuf[11]=0;
  SendBuf[12]=LOBYTE(InfoAddress);
  SendBuf[13]=HIBYTE(InfoAddress);
  SendBuf[14]=0x00;
  SendBuf[15]=SEQUDCS;

  PhysicalSend(SendBuf,SendLen);

  return TRUE;
}

BOOL CProtocolTH104::SendActionValue(WORD UnitID,TagSOEReportStruct *pSOE)
{
  WORD InfoAddress = 0x7001;
  BYTE HexYear = (BYTE)(pSOE->SOETime.GetYear()-2000);
  BYTE HexMon = (BYTE)pSOE->SOETime.GetMonth();
  BYTE HexDay = (BYTE)pSOE->SOETime.GetDay();
  BYTE HexHour = (BYTE)pSOE->SOETime.GetHour();
  BYTE HexMin = (BYTE)pSOE->SOETime.GetMinute();
  WORD HexMSEL = (WORD)(pSOE->SOEMSEL+pSOE->SOETime.GetSecond()*1000);

  BYTE SendBuf[1024];
  int SendLen=0;
  memset(SendBuf,0,1024);

  BYTE SEQUDCS = 0x00;
  int ValueCount = 32;

  {
    SendLen=16+9+32*4;
    SendBuf[0] = 0x68;
	  SendBuf[1] = SendLen-2;
	  SendBuf[2] = 0x00;
	  SendBuf[3] = 0x00;
    SendBuf[4] = 0x00;
	  SendBuf[5] = 0x00;
    SendBuf[6]=52;//动作值
    SendBuf[7]=ValueCount+0x80;
    SendBuf[8]=7;
	  SendBuf[9]=0;
	  SendBuf[10]=(BYTE)UnitID;
	  SendBuf[11]=0;
    SendBuf[12]=LOBYTE(InfoAddress);
    SendBuf[13]=HIBYTE(InfoAddress);
    SendBuf[14]=0x00;
    SendBuf[15]=SEQUDCS;
    SendBuf[16]=LOBYTE(pSOE->SOEID);
    SendBuf[17]=HIBYTE(pSOE->SOEID);
    SendBuf[18]=LOBYTE(HexMSEL);
    SendBuf[19]=HIBYTE(HexMSEL);
    SendBuf[20]=HexMin;
    SendBuf[21]=HexHour;
    SendBuf[22]=HexDay;
    SendBuf[23]=HexMon;
    SendBuf[24]=HexYear;

    for(int VarID=0;VarID<ValueCount;VarID++)
    {
      float Value = pSOE->ActionValue[VarID];
      memcpy(SendBuf+25+VarID*4,&Value,4);
    }
    PhysicalSend(SendBuf,SendLen);
  }
  {
    SendLen=16+9+32*4;
    SendBuf[0] = 0x68;
	  SendBuf[1] = SendLen-2;
	  SendBuf[2] = 0x00;
	  SendBuf[3] = 0x00;
    SendBuf[4] = 0x00;
	  SendBuf[5] = 0x00;
    SendBuf[6]=52;//动作值
    SendBuf[7]=ValueCount+0x80;
    SendBuf[8]=7;
	  SendBuf[9]=0;
	  SendBuf[10]=(BYTE)UnitID;
	  SendBuf[11]=0;
    SendBuf[12]=LOBYTE(InfoAddress+32);
    SendBuf[13]=HIBYTE(InfoAddress+32);
    SendBuf[14]=0x00;
    SendBuf[15]=SEQUDCS;
    SendBuf[16]=LOBYTE(pSOE->SOEID);
    SendBuf[17]=HIBYTE(pSOE->SOEID);
    SendBuf[18]=LOBYTE(HexMSEL);
    SendBuf[19]=HIBYTE(HexMSEL);
    SendBuf[20]=HexMin;
    SendBuf[21]=HexHour;
    SendBuf[22]=HexDay;
    SendBuf[23]=HexMon;
    SendBuf[24]=HexYear;

    for(int VarID=0;VarID<ValueCount;VarID++)
    {
      float Value = pSOE->ActionValue[VarID+32];
      memcpy(SendBuf+25+VarID*4,&Value,4);
    }
    PhysicalSend(SendBuf,SendLen);
  }
  {
    SendLen=16+9;
    SendBuf[0] = 0x68;
	  SendBuf[1] = SendLen-2;
	  SendBuf[2] = 0x00;
	  SendBuf[3] = 0x00;
    SendBuf[4] = 0x00;
	  SendBuf[5] = 0x00;
    SendBuf[6]=52;//动作值
    SendBuf[7]=0x80;//发送完成
    SendBuf[8]=7;
	  SendBuf[9]=0;
	  SendBuf[10]=(BYTE)UnitID;
	  SendBuf[11]=0;
    SendBuf[12]=LOBYTE(InfoAddress);
    SendBuf[13]=HIBYTE(InfoAddress);
    SendBuf[14]=0x00;
    SendBuf[15]=SEQUDCS;
    SendBuf[16]=LOBYTE(pSOE->SOEID);
    SendBuf[17]=HIBYTE(pSOE->SOEID);
    SendBuf[18]=LOBYTE(HexMSEL);
    SendBuf[19]=HIBYTE(HexMSEL);
    SendBuf[20]=HexMin;
    SendBuf[21]=HexHour;
    SendBuf[22]=HexDay;
    SendBuf[23]=HexMon;
    SendBuf[24]=HexYear;

    PhysicalSend(SendBuf,SendLen);
  }

  return TRUE;
}

BOOL CProtocolTH104::SendRecordWaveInfo(WORD UnitID,WORD GroupID)
{
  TagSOEReportStruct *pSendSOE = NULL;

  if(GroupID>=10)
    GroupID = g_Unit[UnitID].SOEReportList.GetCount()-1;

  if(g_Unit[UnitID].SOEReportList.GetCount()==0)
    GroupID = 0;

  POSITION pos = g_Unit[UnitID].SOEReportList.FindIndex(GroupID);
  if(pos)
    pSendSOE = (TagSOEReportStruct *)g_Unit[UnitID].SOEReportList.GetAt(pos);

  WORD InfoAddress = 0x7001 + GroupID;
  BYTE SEQUDCS = 0x00;

  if(pSendSOE==NULL)
  {
    BYTE SendBuf[1024];
    int SendLen=0;
    memset(SendBuf,0,1024);

    SendLen=16;
    SendBuf[0] = 0x68;
	  SendBuf[1] = SendLen-2;
	  SendBuf[2] = 0x00;
	  SendBuf[3] = 0x00;
    SendBuf[4] = 0x00;
	  SendBuf[5] = 0x00;
    SendBuf[6]=53;//录波信息
    SendBuf[7]=0x80;
    SendBuf[8]=0x47;
	  SendBuf[9]=0;
	  SendBuf[10]=(BYTE)UnitID;
	  SendBuf[11]=0;
    SendBuf[12]=LOBYTE(InfoAddress);
    SendBuf[13]=HIBYTE(InfoAddress);
    SendBuf[14]=0x00;
    SendBuf[15]=SEQUDCS;

    return PhysicalSend(SendBuf,SendLen);
  }
  else
  {
    BYTE SendBuf[1024];
    int SendLen=0;
    memset(SendBuf,0,1024);

    BYTE HexYear = (BYTE)(pSendSOE->SOETime.GetYear()-2000);
    BYTE HexMon = (BYTE)pSendSOE->SOETime.GetMonth();
    BYTE HexDay = (BYTE)pSendSOE->SOETime.GetDay();
    BYTE HexHour = (BYTE)pSendSOE->SOETime.GetHour();
    BYTE HexMin = (BYTE)pSendSOE->SOETime.GetMinute();
    WORD HexMSEL = (WORD)(pSendSOE->SOEMSEL+pSendSOE->SOETime.GetSecond()*1000);

    SendLen=25;
    SendBuf[0] = 0x68;
	  SendBuf[1] = SendLen-2;
	  SendBuf[2] = 0x00;
	  SendBuf[3] = 0x00;
    SendBuf[4] = 0x00;
	  SendBuf[5] = 0x00;
    SendBuf[6]=53;//录波信息
    SendBuf[7]=0x01;
    SendBuf[8]=7;
    SendBuf[9]=0;
    SendBuf[10]=(BYTE)UnitID;
    SendBuf[11]=0;
    SendBuf[12]=LOBYTE(InfoAddress);
    SendBuf[13]=HIBYTE(InfoAddress);
    SendBuf[14]=0;
    SendBuf[15]=SEQUDCS;
    SendBuf[16]=LOBYTE(pSendSOE->SOEID);
    SendBuf[17]=HIBYTE(pSendSOE->SOEID);
    SendBuf[18]=LOBYTE(HexMSEL);
    SendBuf[19]=HIBYTE(HexMSEL);
    SendBuf[20]=HexMin;
    SendBuf[21]=HexHour;
    SendBuf[22]=HexDay;
    SendBuf[23]=HexMon;
    SendBuf[24]=HexYear;

    PhysicalSend(SendBuf,SendLen);
  }
  return TRUE;
}

BOOL CProtocolTH104::SendRecordWaveData(WORD UnitID,WORD GroupID)
{
  TagSOEReportStruct *pSendSOE = NULL;

  if(GroupID>=10)
    GroupID = g_Unit[UnitID].SOEReportList.GetCount()-1;

  if(g_Unit[UnitID].SOEReportList.GetCount()==0)
    GroupID = 0;

  POSITION pos = g_Unit[UnitID].SOEReportList.FindIndex(GroupID);
  if(pos)
    pSendSOE = (TagSOEReportStruct *)g_Unit[UnitID].SOEReportList.GetAt(pos);

  if(pSendSOE==NULL)
    return TRUE;

  WORD InfoAddress = 0x7001 + GroupID;
  BYTE SEQUDCS = 0x00;
  int ValueCount = 10;

  {
    for(int samp=0;samp<320;samp++) 
    {
      BYTE SendBuf[1024];
      int SendLen=0;
      memset(SendBuf,0,1024);

      SendLen=16+2+10*4;
      SendBuf[0] = 0x68;
	    SendBuf[1] = SendLen-2;
	    SendBuf[2] = 0x00;
	    SendBuf[3] = 0x00;
      SendBuf[4] = 0x00;
	    SendBuf[5] = 0x00;
      SendBuf[6]=54;//录波数据
      SendBuf[7]=ValueCount+0x80;
      SendBuf[8]=7;
      SendBuf[9]=0;
      SendBuf[10]=(BYTE)UnitID;
      SendBuf[11]=0;
      SendBuf[12]=LOBYTE(InfoAddress);
      SendBuf[13]=HIBYTE(InfoAddress);
      SendBuf[14]=0x00;
      SendBuf[15]=SEQUDCS;

      SendBuf[16]=LOBYTE(samp);
      SendBuf[17]=HIBYTE(samp);
      for(int ch=0;ch<9;ch++)
      { 
        float Value = pSendSOE->RecordWaveValue[ch][samp];
        memcpy(SendBuf+18+ch*4,&Value,4);
      }

      PhysicalSend(SendBuf,SendLen);
      
      if((samp%10)==0)
        Sleep(100);
    }
  }
  {
    BYTE SendBuf[1024];
    int SendLen=0;
    memset(SendBuf,0,1024);

    SendLen=16;
    SendBuf[0] = 0x68;
	  SendBuf[1] = SendLen-2;
	  SendBuf[2] = 0x00;
	  SendBuf[3] = 0x00;
    SendBuf[4] = 0x00;
	  SendBuf[5] = 0x00;
    SendBuf[6]=54;//录波数据
    SendBuf[7]=0x80;//发送完成
    SendBuf[8]=7;
	  SendBuf[9]=0;
	  SendBuf[10]=(BYTE)UnitID;
	  SendBuf[11]=0;
    SendBuf[12]=LOBYTE(InfoAddress);
    SendBuf[13]=HIBYTE(InfoAddress);
    SendBuf[14]=0x00;
    SendBuf[15]=SEQUDCS;

    PhysicalSend(SendBuf,SendLen);
  }

  return TRUE;
}

void CProtocolTH104::ProcessCallAllDataStop(BYTE CallType)  //召唤数据停止
{
  BYTE SendBuf[1024];
  int SendLen=0;
  memset(SendBuf,0,1024);

  SendLen=16;
  SendBuf[0] = 0x68;
	SendBuf[1] = SendLen-2;
	SendBuf[2] = 0x00;
	SendBuf[3] = 0x00;
  SendBuf[4] = 0x00;
	SendBuf[5] = 0x00;
	SendBuf[6] = 100;
  SendBuf[7] = 0x01;
  SendBuf[8] = 9;//停止传输
	SendBuf[9] = 0x00;
	SendBuf[10] = (BYTE)0x00;
	SendBuf[11] = 0x00;
	SendBuf[12] = 0x00;
	SendBuf[13] = 0x00;
	SendBuf[14] = 0x00;
	SendBuf[15] = CallType;
  PhysicalSend(SendBuf,SendLen);
}

void CProtocolTH104::ProcessCallLongDataBegin(BYTE CallType)  //召唤电度数据
{
  BYTE SendBuf[1024];
  int SendLen=0;
  memset(SendBuf,0,1024);

  SendLen=16;
  SendBuf[0] = 0x68;
	SendBuf[1] = SendLen-2;
	SendBuf[2] = 0x00;
	SendBuf[3] = 0x00;
  SendBuf[4] = 0x00;
	SendBuf[5] = 0x00;
	SendBuf[6] = 101;
  SendBuf[7] = 0x01;
  SendBuf[8] = 7;//传输确认
	SendBuf[9] = 0x00;
	SendBuf[10] = (BYTE)0x00;
	SendBuf[11] = 0x00;
	SendBuf[12] = 0x00;
	SendBuf[13] = 0x00;
	SendBuf[14] = 0x00;
	SendBuf[15] = CallType;
  PhysicalSend(SendBuf,SendLen);

  for(int unitid=0;unitid<UNITCOUNT;unitid++)
  {
    if(g_Unit[unitid].UnitType==0)
      continue;
    SendAllLongData(unitid);
  }
}

void CProtocolTH104::ProcessCallLongDataStop(BYTE CallType)  //召唤电度数据停止
{
  BYTE SendBuf[1024];
  int SendLen=0;
  memset(SendBuf,0,1024);

  SendLen=16;
  SendBuf[0] = 0x68;
	SendBuf[1] = SendLen-2;
	SendBuf[2] = 0x00;
	SendBuf[3] = 0x00;
  SendBuf[4] = 0x00;
	SendBuf[5] = 0x00;
	SendBuf[6] = 101;
  SendBuf[7] = 0x01;
  SendBuf[8] = 9;//停止传输
	SendBuf[9] = (BYTE)0x00;
	SendBuf[9] = 0x00;
	SendBuf[10] = (BYTE)0x00;
	SendBuf[11] = 0x00;
	SendBuf[12] = 0x00;
	SendBuf[13] = 0x00;
	SendBuf[14] = 0x00;
	SendBuf[15] = CallType;
  PhysicalSend(SendBuf,SendLen);
}

BOOL CProtocolTH104::UpdateComm()
{
  CSingleLock sLock(&g_PowerMutex);
  sLock.Lock();

  if(m_pSocketTH104!=NULL)
  {
    BYTE receivebuf[0xffff];
	  int receivelen = 0;
    if(PhysicalReceive(receivebuf,receivelen))
    {
      int pos = 0;
      while(pos<receivelen)
      {
        if(receivebuf[pos]!=0x68)
          break;
        int framelen = (WORD)((BYTE)(receivebuf[pos+1])+2);
        ProcessDataFrame(receivebuf+pos);
        pos += framelen;
      }
    }

    if(m_StartSendDataFrame==NULL)
      return TRUE;
/*
    {
      for(int unitid=0;unitid<UNITCOUNT;unitid++)
      {
        if(g_Unit[unitid].UnitType==0)
          continue;
        for(int VarID=0;VarID<VARCOUNT;VarID++)
        {
          if(g_Unit[unitid].VarLock[VarID]==TRUE)
            g_Unit[unitid].VarGroup[1].Var[VarID].Value = g_Unit[unitid].VarGroup[1].Var[VarID].LastValue;
        }
      }
    }
*/
    if((CTime::GetCurrentTime()-m_SendAllFloatDataTime).GetTotalSeconds()>0)
    {
      m_SendAllFloatDataTime = CTime::GetCurrentTime();

      for(int unitid=0;unitid<UNITCOUNT;unitid++)
      {
        if(g_Unit[unitid].UnitType==0)
          continue;

        SendAllFloatData(unitid);
        SendChangeBitData(unitid);
        SendChangeSOEData(unitid);
      }
      Sleep(500);
    }
    if((CTime::GetCurrentTime()-m_SendAllBitDataTime).GetTotalSeconds()>60)
    {
      m_SendAllBitDataTime = CTime::GetCurrentTime();

      for(int unitid=0;unitid<UNITCOUNT;unitid++)
      {
        if(g_Unit[unitid].UnitType==0)
          continue;

        SendAllBitData(unitid);
      }
    }
    if((CTime::GetCurrentTime()-m_SendAllLongDataTime).GetTotalSeconds()>60)
    {
      m_SendAllLongDataTime = CTime::GetCurrentTime();

      for(int unitid=0;unitid<UNITCOUNT;unitid++)
      {
        if(g_Unit[unitid].UnitType==0)
          continue;

        SendAllLongData(unitid);
      }
    }
  }

	return TRUE;
}

BOOL CProtocolTH104::SendTestFrame()//发送测试启动帧
{
	BYTE  DataBuf[128];
	int   nSendLen=6;

  DataBuf[0]=0x68;
  DataBuf[1]=nSendLen-2;
  DataBuf[2]=0x43;
  DataBuf[3]=0x00;
  DataBuf[4]=0x00;
  DataBuf[5]=0x00;

	return PhysicalSend(DataBuf, nSendLen);
}

BOOL CProtocolTH104::ProcessDataFrame(BYTE *DataBuf)
{
  BYTE SendBuf[1024];
  int SendLen=0;
  memset(SendBuf,0,1024);

	if(DataBuf[2]==0x01)//S格式 接收确认
  {
    m_InterCountConfirmVS = 0;
	  return TRUE;
  }

	if((DataBuf[2]&0x03)==0x03)//U格式
  {
    if(DataBuf[2]==0x07)
    {
      m_SendAllFloatDataTime = 0;
      m_SendAllBitDataTime = 0;
      m_SendAllLongDataTime = 0;

	    BYTE  SendDataBuf[128];
	    int   nSendLen=6;

      SendDataBuf[0]=0x68;
      SendDataBuf[1]=nSendLen-2;
      SendDataBuf[2]=0x0B;
      SendDataBuf[3]=0x00;
      SendDataBuf[4]=0x00;
      SendDataBuf[5]=0x00;

	    m_InterCountVS = 0;
      m_StartSendDataFrame = TRUE;
      return PhysicalSend(SendDataBuf, nSendLen);
    }
    if(DataBuf[2]==0x13)
    {
	    BYTE  SendDataBuf[128];
	    int   nSendLen=6;

      SendDataBuf[0]=0x68;
      SendDataBuf[1]=nSendLen-2;
      SendDataBuf[2]=0x23;
      SendDataBuf[3]=0x00;
      SendDataBuf[4]=0x00;
      SendDataBuf[5]=0x00;
      m_InterCountVS = 0;

	    m_StartSendDataFrame = FALSE;
      PhysicalSend(SendDataBuf, nSendLen);
      return TRUE;
    }
    if(DataBuf[2]==0x43)
    {
      m_InterCountConfirmVS = 0;

	    BYTE  SendDataBuf[128];
	    int   nSendLen=6;

      SendDataBuf[0]=0x68;
      SendDataBuf[1]=nSendLen-2;
      SendDataBuf[2]=0x83;
      SendDataBuf[3]=0x00;
      SendDataBuf[4]=0x00;
      SendDataBuf[5]=0x00;

	    return PhysicalSend(SendDataBuf, nSendLen);
    }

    return TRUE;
  }

	//I格式 接收确认
  {
    m_InterCountConfirmVS = 0;
  }
/*
  m_InterCountConfirmVR++;
  m_InterCountVR = MAKEWORD(DataBuf[2],DataBuf[3])/2+1;
  if(m_InterCountConfirmVR>=m_InterCountVw)//S格式 发送确认
  {
	  BYTE  SendDataBuf[128];
	  int   nSendLen=6;

    SendDataBuf[0]=0x68;
    SendDataBuf[1]=nSendLen-2;
    SendDataBuf[2]=0x01;
    SendDataBuf[3]=0x00;
    SendDataBuf[4]=(LOBYTE(m_InterCountVR*2));
    SendDataBuf[5]=(HIBYTE(m_InterCountVR*2));
	  PhysicalSend(SendDataBuf, nSendLen);
    m_InterCountConfirmVR = 0;
  }
*/

  BYTE UnitID = DataBuf[10];
  switch(DataBuf[6])
  {
		case   1:     //单点YX
    {
      if((g_DistributionOperateMode==FALSE)&&(m_OperateEnable==FALSE))//集中操作模式且无控制权限
        return TRUE;

      int DataNum = DataBuf[7]&0x7f;
	    BOOL SQ = DataBuf[7]&0x80;  //全数据标志
      if(SQ)
			{
				int nStartVarID = (DataBuf[12] + DataBuf[13]*256 + DataBuf[14]*256*256) - 1;
				for(int i = 0; i < DataNum; i++)
				{
					WORD VarID = nStartVarID + i;

					switch(DataBuf[14+i])
          {
            case 0: //赋值
            case 3: //置数
					    if(DataBuf[15+i]&0x01)
                g_Unit[UnitID].VarGroup[1].Var[VarID].Value = 1;
					    else
                g_Unit[UnitID].VarGroup[1].Var[VarID].Value = 0;
              break;
            case 5: //解锁
              g_Unit[UnitID].VarLock[VarID] = 0;
              break;
            case 6: //闭锁
              g_Unit[UnitID].VarLock[VarID] = 1;
              break;
            case 7: //正常
              g_Unit[UnitID].VarAbnormal[VarID] = 0;
              break;
            case 8: //异常
              g_Unit[UnitID].VarAbnormal[VarID] = 1;
              break;
          }
          g_Unit[UnitID].VarGroup[1].Var[VarID].LastValue = !g_Unit[UnitID].VarGroup[1].Var[VarID].Value;
				}
			}
			else
			{
				for(int i = 0; i < DataNum; i++)
				{
					int nStartVarID = (DataBuf[12+i*4] + DataBuf[13+i*4]*256 + DataBuf[14+i*4]*256*256) - 1;
					WORD VarID = nStartVarID;

					switch(DataBuf[14+i*4])
          {
            case 0: //赋值
            case 3: //置数
					    if(DataBuf[15+i*4]&0x01)
                g_Unit[UnitID].VarGroup[1].Var[VarID].Value = 1;
					    else
                g_Unit[UnitID].VarGroup[1].Var[VarID].Value = 0;
              break;
            case 5: //解锁
              g_Unit[UnitID].VarLock[VarID] = 0;
              break;
            case 6: //闭锁
              g_Unit[UnitID].VarLock[VarID] = 1;
              break;
            case 7: //正常
              g_Unit[UnitID].VarAbnormal[VarID] = 0;
              break;
            case 8: //异常
              g_Unit[UnitID].VarAbnormal[VarID] = 1;
          }
          g_Unit[UnitID].VarGroup[1].Var[VarID].LastValue = !g_Unit[UnitID].VarGroup[1].Var[VarID].Value;
				}
			}
			break;
    }
    case 100://召唤命令
    {
      return TRUE;
      if(DataBuf[8]==0x06)//召唤数据
        ProcessCallAllDataBegin(DataBuf[15]);
      if(DataBuf[8]==0x08)//停止召唤
        ProcessCallAllDataStop(DataBuf[15]);
      return TRUE;
      break;
    }
    case 101://电能召唤命令
    {
      return TRUE;
      if(DataBuf[8]==0x06)//召唤数据
        ProcessCallLongDataBegin(DataBuf[15]);
      if(DataBuf[8]==0x08)//停止召唤
        ProcessCallLongDataStop(DataBuf[15]);
      return TRUE;
      break;
    }
    case 103://校时命令
    {
      SYSTEMTIME ct;
	    ct.wMilliseconds = MAKEWORD(DataBuf[15],DataBuf[16]);
      ct.wSecond = ct.wMilliseconds/1000;
	    ct.wMilliseconds %= 1000;
	    ct.wMinute = DataBuf[17]&0x3f;
	    ct.wHour = DataBuf[18]&0x1f;
	    ct.wDay = DataBuf[19]&0x1f;
	    ct.wMonth = DataBuf[20]&0x0f;
	    ct.wYear = DataBuf[21] + 2000 ;
      SetLocalTime(&ct);

      GetLocalTime(&ct);
      SendLen=22;
      SendBuf[0] = 0x68;
	    SendBuf[1] = SendLen-2;
	    SendBuf[2] = 0x00;
	    SendBuf[3] = 0x00;
      SendBuf[4] = 0x00;
	    SendBuf[5] = 0x00;
	    SendBuf[6] = 103;
      SendBuf[7] = 0x01;
	    SendBuf[8] = 0x07;
	    SendBuf[9] = 0x00;
	    SendBuf[10] = (BYTE)UnitID;
	    SendBuf[11] = 0x00;
	    SendBuf[12] = 0x00;
	    SendBuf[13] = 0x00;
	    SendBuf[14] = 0x00;
	    SendBuf[15] = BYTE(ct.wMilliseconds);
	    SendBuf[16] = BYTE(ct.wMilliseconds>>8);
	    SendBuf[17] = BYTE(ct.wMinute);
	    SendBuf[18] = BYTE(ct.wHour);
	    SendBuf[19] = BYTE(ct.wDay)|BYTE(ct.wDayOfWeek<<5);
	    SendBuf[20] = BYTE(ct.wMonth);
	    SendBuf[21] = BYTE(ct.wYear-2000);
      PhysicalSend(SendBuf,SendLen);
      
      Sleep(1000);

      return TRUE;
      break;
    }
    case 45://复归命令
    {
      WORD UnitID = DataBuf[10];
      WORD VarID = MAKEWORD(DataBuf[12],DataBuf[13]) - m_YKAddress;

      for(UnitID = 1;UnitID<UNITCOUNT;UnitID++)
      {
        g_Unit[UnitID].VarGroup[1].Var[30].Value = 0;
        g_Unit[UnitID].VarGroup[1].Var[31].Value = 0;
        g_Unit[UnitID].UnitState = 0;
        ::WriteUnitToShareMemory();
      }

      break;
    }
    case 46://双点遥控命令
    {
      BOOL Result=FALSE;

      WORD UnitID = DataBuf[10];
      WORD VarID = MAKEWORD(DataBuf[12],DataBuf[13]) - m_YKAddress;

      if(UnitID>=32)
        break;
      if(VarID>=64)
        break;

      BYTE CtrlResult = DataBuf[8];//传送原因
      BYTE CtrlType = DataBuf[15]&0x80;  //0x80=遥控选择;0x00=遥控执行
      BYTE CtrlValue = DataBuf[15]&0x03; //0x01=遥控分;  0x02=遥控合

      if((CtrlValue==1)||(CtrlValue==2))
      {
        if(CtrlResult==6)
        {
          if(CtrlType==0x80)
            Result = TRUE;
          else
            Result = TRUE;
        }
        if(CtrlResult==8)
        {
          Result = TRUE;
        }
      }

      if((g_DistributionOperateMode==FALSE)&&(m_OperateEnable==FALSE))//集中操作模式且无控制权限
        Result = FALSE;

      if((CtrlResult==6)&&(CtrlType==0x80))//遥控选择
      {
        if(Result)
        {
          if(VarID==0)//遥控复归
          {
            return TRUE;
            break;
          }
          else if(VarID==63)//遥控升降档
          {
            if((::GetTickCount()-g_LastRemoteOperateTickCount)>3000)
            {
              g_LastRemoteOperateTickCount = ::GetTickCount();
              if(CtrlValue==1)//遥降
              {
                if(g_Unit[UnitID].VarGroup[0].Var[15].Value==0)//0档位置
                  Result = FALSE;
              }
              if(CtrlValue==2)//遥升
              {
                if(g_Unit[UnitID].VarGroup[0].Var[15].Value==21)//21档位置
                  Result = FALSE;
              }
            }
          }
          else if(VarID>0)//遥控合分闸
          {
            switch(g_Unit[UnitID].UnitType)
            {
              case 1://差动
                break;
              case 2://高后备
              case 3://低后备
              case 4://线路
              case 5://电容器
              case 6://综合测控 
              {
                if(VarID==1)//主断路器
                {
                  if(g_Unit[UnitID].VarGroup[1].Var[2].Value==1)//就地位置
                    Result = FALSE;

                  if(CtrlValue==1)//遥分
                  {
                    if(g_Unit[UnitID].VarGroup[1].Var[0].Value==0)//已经是分位置
                      Result = FALSE;
                  }
                  if(CtrlValue==2)//遥合
                  {
                    if(g_Unit[UnitID].VarGroup[1].Var[0].Value==1)//已经是合位置
                      Result = FALSE;
                  }
                }
                else//电动刀闸
                {
                  if(g_Unit[UnitID].VarGroup[1].Var[VarID+1].Value==1)//就地位置
                    Result = FALSE;

                  if(CtrlValue==1)//遥分
                  {
                    if(g_Unit[UnitID].VarGroup[1].Var[VarID].Value==0)//已经是分位置
                      Result = FALSE;
                  }
                  if(CtrlValue==2)//遥合
                  {
                    if(g_Unit[UnitID].VarGroup[1].Var[VarID].Value==1)//已经是合位置
                      Result = FALSE;
                  }
                }
                g_Unit[UnitID].VarGroup[1].Var[23].Value = 0;//断路器拒动清除
                break;
              }
              case 7://备自投
              {
                if(g_Unit[UnitID].VarGroup[1].Var[0].Value==1)//就地位置
                  Result = FALSE;

                if(CtrlValue==1)//遥分
                {
                  if(g_Unit[UnitID].VarGroup[1].Var[VarID].Value==0)//已经是分位置
                    Result = FALSE;
                }
                if(CtrlValue==2)//遥合
                {
                  if(g_Unit[UnitID].VarGroup[1].Var[VarID].Value==1)//已经是合位置
                    Result = FALSE;
                }
              }
            }
          }
        }
        else
        {
          CtrlValue = 0;
        }
      }

      if((CtrlResult==6)&&(CtrlType==0x00))//遥控执行
      {
        if(Result)
        {
          if(VarID==0)//遥控复归
          {
            return TRUE;
            break;
          }
          else if(VarID>0)//遥控合分闸
          {
            if((g_DistributionOperateMode==FALSE)&&(m_OperateEnable==FALSE))//集中操作模式且无控制权限
              return TRUE;

            float ActionValue[VARCOUNT];
            memset(&ActionValue,0x00,sizeof(ActionValue));

            switch(g_Unit[UnitID].UnitType)
            {
              case 1://差动
                break;
              case 2://高后备
              case 3://低后备
              case 4://线路
              case 5://电容器
              case 6://综合测控 
              {
                if(VarID==1)//主断路器
                {
                  if(g_Unit[UnitID].VarGroup[1].Var[2].Value==1)//就地位置
                  {
                    Result = FALSE;
                    break;
                  }

                  BOOL RemoteResult = FALSE;//拒动标志
                  if(CtrlValue==1)//遥分
                  {
                    if(g_Unit[UnitID].VarGroup[1].Var[0].Value==0)//已经是分位置
                    {
                      Result = FALSE;
                      break;
                    }

                    if((g_Unit[UnitID].VarLock[0]==FALSE)&&(g_Unit[UnitID].VarGroup[1].Var[0].Value!=g_Unit[UnitID].VarGroup[1].Var[1].Value)&&(g_Unit[UnitID].VarLock[0]==FALSE))//无控断和无拒动
                      RemoteResult = TRUE;

                    if(::GetRemoteLockState(UnitID,0,0)==TRUE)//判断主断路器分闸闭锁条件
                      RemoteResult = FALSE;

                    if(RemoteResult==TRUE)
                    {
                      g_Unit[UnitID].VarGroup[1].Var[0].Value = 0;
                      g_Unit[UnitID].VarGroup[1].Var[1].Value = 1;
                      ::WriteUnitToShareMemory();
                    }
                    {
                      ::ProductActionValue(UnitID,ActionValue);
                      ::ProductSOEReport(UnitID,35,CTime::GetCurrentTime(),(WORD)::GetTickCount()%1000,1,ActionValue);
                    }
                  }
                  if(CtrlValue==2)//遥合
                  {
                    if(g_Unit[UnitID].VarGroup[1].Var[0].Value==1)//已经是合位置
                    {
                      Result = FALSE;
                      break;
                    }

                    if((g_Unit[UnitID].VarLock[0]==FALSE)&&(g_Unit[UnitID].VarGroup[1].Var[0].Value!=g_Unit[UnitID].VarGroup[1].Var[1].Value)&&(g_Unit[UnitID].VarLock[0]==FALSE))//无控断和无拒动
                      RemoteResult = TRUE;

                    if((UnitID==2)||(UnitID==5)||(UnitID==7))//1T、2T、母联
                    {
                      BOOL Power1T = TRUE;
                      BOOL Power2T = TRUE;
                      BOOL PowerBus = TRUE;

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
                            Power1T = FALSE;
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
                            Power2T = FALSE;
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
                            PowerBus = FALSE;
                            break;
                          }
                        }
                      }

                      if((UnitID==2)&&(Power2T==TRUE)&&(PowerBus==TRUE))//1T合、2T、母联投
                        RemoteResult = FALSE;
                      if((UnitID==5)&&(Power1T==TRUE)&&(PowerBus==TRUE))//2T合、1T、母联投
                        RemoteResult = FALSE;
                      if((UnitID==7)&&(Power1T==TRUE)&&(Power2T==TRUE))//母联合、1T、2T投
                        RemoteResult = FALSE;
                    }

                    if(::GetRemoteLockState(UnitID,0,1)==TRUE)//判断主断路器合闸闭锁条件
                      RemoteResult = FALSE;

                    if(RemoteResult==TRUE)
                    {
                      g_Unit[UnitID].VarGroup[1].Var[0].Value = 1;
                      g_Unit[UnitID].VarGroup[1].Var[1].Value = 0;
                      ::WriteUnitToShareMemory();
                      {
                        ::AddPresetVar(UnitID,1,3,0,CTime::GetCurrentTime());               //储能分
                        ::AddPresetVar(UnitID,1,3,1,CTime::GetCurrentTime()+CTimeSpan(10)); //储能合
                      }
                    }
                    {
                      ::ProductActionValue(UnitID,ActionValue);
                      ::ProductSOEReport(UnitID,34,CTime::GetCurrentTime(),(WORD)::GetTickCount()%1000,1,ActionValue);
                    }
                  }
                  if(RemoteResult==TRUE)
                    g_Unit[UnitID].VarGroup[1].Var[23].Value = 0;
                  else
                    g_Unit[UnitID].VarGroup[1].Var[23].Value = 1;
                  break;
                }
                else if(VarID==63)//遥控升降档
                {
                  if((::GetTickCount()-g_LastRemoteOperateTickCount)>3000)
                  {
                    g_LastRemoteOperateTickCount = ::GetTickCount();
                    if(CtrlValue==1)//遥降
                    {
                      if(g_Unit[UnitID].VarGroup[0].Var[15].Value>0)//0档位置
                      {
                        g_Unit[UnitID].VarGroup[0].Var[15].Value--;
                        break;
                      }
                    }
                    if(CtrlValue==2)//遥升
                    {
                      if(g_Unit[UnitID].VarGroup[0].Var[15].Value<21)//21档位置
                      {
                        g_Unit[UnitID].VarGroup[0].Var[15].Value++;
                        break;
                      }
                    }
                  }
                  break;
                }
                else//电动刀闸
                {
                  if(g_Unit[UnitID].VarGroup[1].Var[VarID+1].Value==1)//就地位置
                  {
                    Result = FALSE;
                    break;
                  }

                  BOOL RemoteResult = FALSE;//拒动标志
                  if(CtrlValue==1)//遥分
                  {
                    if(g_Unit[UnitID].VarGroup[1].Var[VarID].Value==0)//已经是分位置
                    {
                      Result = FALSE;
                      break;
                    }

                    if(g_Unit[UnitID].VarLock[VarID]==FALSE)
                      RemoteResult = TRUE;

                    if(::GetRemoteLockState(UnitID,VarID,0)==TRUE)//判断电动刀闸分闸闭锁条件
                      RemoteResult = FALSE;

                    if(RemoteResult==TRUE)
                    {
                      g_Unit[UnitID].VarGroup[1].Var[VarID].Value = 0;
                      ::WriteUnitToShareMemory();
                    }
                  }
                  if(CtrlValue==2)//遥合
                  {
                    if(g_Unit[UnitID].VarGroup[1].Var[VarID].Value==1)//已经是合位置
                    {
                      Result = FALSE;
                      break;
                    }

                    if(g_Unit[UnitID].VarLock[VarID]==FALSE)
                      RemoteResult = TRUE;

                    if(::GetRemoteLockState(UnitID,VarID,1)==TRUE)//判断电动刀闸合闸闭锁条件
                      RemoteResult = FALSE;

                    if(RemoteResult==TRUE)
                    {
                      g_Unit[UnitID].VarGroup[1].Var[VarID].Value = 1;
                      ::WriteUnitToShareMemory();
                    }
                  }
                  if(RemoteResult==TRUE)
                    g_Unit[UnitID].VarGroup[1].Var[23].Value = 0;
                  else
                    g_Unit[UnitID].VarGroup[1].Var[23].Value = 1;
                  break;
                }
                break;
              }
              case 7://备自投
              {
                if(g_Unit[UnitID].VarGroup[1].Var[0].Value==1)//就地位置
                {
                  Result = FALSE;
                  break;
                }

                if(CtrlValue==1)
                  g_Unit[UnitID].VarGroup[1].Var[VarID].Value = 0;
                if(CtrlValue==2)
                  g_Unit[UnitID].VarGroup[1].Var[VarID].Value = 1;

                ::WriteUnitToShareMemory();

                break;
              }
              default:
              {
                if(CtrlValue==1)
                  g_Unit[UnitID].VarGroup[1].Var[VarID].Value = 0;
                if(CtrlValue==2)
                  g_Unit[UnitID].VarGroup[1].Var[VarID].Value = 1;
              
                ::WriteUnitToShareMemory();
                
                break;
              }
            }
          }
        }
        else
          CtrlValue = 0;
      }

      if(CtrlResult==8)
      {
        if(Result)
          ;
        else
          ;
      }

      SendLen=16;
      SendBuf[0] = 0x68;
	    SendBuf[1] = SendLen-2;
	    SendBuf[2] = 0x00;
	    SendBuf[3] = 0x00;
      SendBuf[4] = 0x00;
	    SendBuf[5] = 0x00;
	    SendBuf[6] = 46;
      SendBuf[7] = 0x01;
      if(Result)
	      SendBuf[8] = CtrlResult+1;
	    else
	      SendBuf[8] = CtrlResult+1|0x40;
      SendBuf[9] = 0x00;
	    SendBuf[10] = (BYTE)UnitID;
	    SendBuf[11] = 0x00;
	    SendBuf[12] = LOBYTE(VarID+m_YKAddress);
	    SendBuf[13] = HIBYTE(VarID+m_YKAddress);
	    SendBuf[14] = 0x00;
      SendBuf[15] = CtrlType|CtrlValue;
      PhysicalSend(SendBuf,SendLen);
      
      return TRUE;
      break;
    }
    case 50://定值
    {
      int DataNum = DataBuf[7]&0x7f;
      WORD UnitID = DataBuf[10];
      WORD InfoAddress = MAKEWORD(DataBuf[12],DataBuf[13]);
      BYTE SEQUDCS = DataBuf[15];
      
      WORD GroupID = 0;
      if((InfoAddress>=0x5001)&&(InfoAddress<0x5101))
        GroupID = 0;
      if((InfoAddress>=0x5101)&&(InfoAddress<0x5201))
        GroupID = 1;
      if((InfoAddress>=0x5201)&&(InfoAddress<0x5301))
        GroupID = 2;
      if((InfoAddress>=0x5301)&&(InfoAddress<0x5401))
        GroupID = 3;
      if((InfoAddress>=0x5401)&&(InfoAddress<0x5501))
        GroupID = 4;

      if(GroupID==4)
      {
        GroupID = g_Unit[UnitID].CurrentParamaterGroupID;
        switch(GroupID)
        {
          case 0:
            InfoAddress=0x5001+(InfoAddress-0x5401);
            break;
          case 1:
            InfoAddress=0x5101+(InfoAddress-0x5401);
            break;
          case 2:
            InfoAddress=0x5201+(InfoAddress-0x5401);
            break;
          case 3:
            InfoAddress=0x5301+(InfoAddress-0x5401);
            break;
        }
      }

      if(SEQUDCS==0x00)//定值召唤
      {
        SendParameterData(UnitID,GroupID);
        g_Unit[UnitID].VarGroup[1].Var[24].Value = 1;
        g_Unit[UnitID].VarGroup[1].Var[25].Value = 0;
        g_Unit[UnitID].VarGroup[1].Var[26].Value = 0;
        g_Unit[UnitID].VarGroup[1].Var[27].Value = 0;
        g_Unit[UnitID].VarGroup[1].Var[28].Value = 0;
        g_Unit[UnitID].VarGroup[1].Var[29].Value = 0;
        return TRUE;
      }
      if(SEQUDCS==0x01)//定值下装
      {
        if((g_DistributionOperateMode==FALSE)&&(m_OperateEnable==FALSE))//集中操作模式且无控制权限
          return TRUE;

				for(int no = 0; no < DataNum; no++)
				{
          BYTE VarID = InfoAddress%0x0100-1+no;
					float Value;
					memcpy(&Value,DataBuf+16+no*4,4);
          g_Unit[UnitID].ParameterGroup[GroupID].Var[VarID].Value = Value;
        }
        
        ::WriteUnitToShareMemory();

        Sleep(250);
        SendLen=16;
        SendBuf[0] = 0x68;
	      SendBuf[1] = SendLen-2;
	      SendBuf[2] = 0x00;
	      SendBuf[3] = 0x00;
        SendBuf[4] = 0x00;
	      SendBuf[5] = 0x00;
	      SendBuf[6] = 50;
        SendBuf[7] = 0x01;
        SendBuf[8] = 0x7;
        SendBuf[9] = 0x00;
	      SendBuf[10] = (BYTE)UnitID;
	      SendBuf[11] = 0x00;
	      SendBuf[12] = LOBYTE(InfoAddress);
	      SendBuf[13] = HIBYTE(InfoAddress);
	      SendBuf[14] = 0x00;
        SendBuf[15] = SEQUDCS;
        
        PhysicalSend(SendBuf,SendLen);

        return TRUE;
      }
    }
    case 51://定值组号
    {
      WORD UnitID = DataBuf[10];
      WORD InfoAddress = MAKEWORD(DataBuf[12],DataBuf[13]);
      BYTE SEQUDCS = DataBuf[15];
      
      WORD GroupID = 0;
      switch(InfoAddress)
      {
        case 0x5001:
          GroupID = 0;
          break;
        case 0x5101:
          GroupID = 1;
          break;
        case 0x5201:
          GroupID = 2;
          break;
        case 0x5301:
          GroupID = 3;
          break;
        case 0x5401:
          GroupID = g_Unit[UnitID].CurrentParamaterGroupID;
          break;
      }

      if(SEQUDCS==0x00)//定值组号召唤
      {
        SendParameterGroupID(UnitID);
        g_Unit[UnitID].VarGroup[1].Var[24].Value = 1;
        g_Unit[UnitID].VarGroup[1].Var[25].Value = 0;
        g_Unit[UnitID].VarGroup[1].Var[26].Value = 0;
        g_Unit[UnitID].VarGroup[1].Var[27].Value = 0;
        g_Unit[UnitID].VarGroup[1].Var[28].Value = 0;
        g_Unit[UnitID].VarGroup[1].Var[29].Value = 0;
        g_Unit[UnitID].VarGroup[1].Var[26+g_Unit[UnitID].CurrentParamaterGroupID].Value = 1;
        return TRUE;
      }
      if(SEQUDCS==0x01)//定值组号下装
      {
        if((g_DistributionOperateMode==FALSE)&&(m_OperateEnable==FALSE))//集中操作模式且无控制权限
          return TRUE;

        Sleep(1000);

        g_Unit[UnitID].CurrentParamaterGroupID = GroupID;
        
        g_Unit[UnitID].VarGroup[1].Var[24].Value = 0;
        g_Unit[UnitID].VarGroup[1].Var[25].Value = 1;
        g_Unit[UnitID].VarGroup[1].Var[26].Value = 0;
        g_Unit[UnitID].VarGroup[1].Var[27].Value = 0;
        g_Unit[UnitID].VarGroup[1].Var[28].Value = 0;
        g_Unit[UnitID].VarGroup[1].Var[29].Value = 0;
        g_Unit[UnitID].VarGroup[1].Var[26+GroupID].Value = 1;

        ::WriteUnitToShareMemory();

        SendLen=16;
        SendBuf[0] = 0x68;
	      SendBuf[1] = SendLen-2;
	      SendBuf[2] = 0x00;
	      SendBuf[3] = 0x00;
        SendBuf[4] = 0x00;
	      SendBuf[5] = 0x00;
	      SendBuf[6] = 51;
        SendBuf[7] = 0x01;
        SendBuf[8] = 0x7;
        SendBuf[9] = 0x00;
	      SendBuf[10] = (BYTE)UnitID;
	      SendBuf[11] = 0x00;
	      SendBuf[12] = LOBYTE(InfoAddress);
	      SendBuf[13] = HIBYTE(InfoAddress);
	      SendBuf[14] = 0x00;
        SendBuf[15] = SEQUDCS;
        PhysicalSend(SendBuf,SendLen);

        Sleep(1000);

        return TRUE;
      }
    }
    case 53://录波召唤
    {
      WORD UnitID = DataBuf[10];
      WORD InfoAddress = MAKEWORD(DataBuf[12],DataBuf[13]);
      BYTE GroupID = InfoAddress - 0x7001;
      
      SendRecordWaveInfo(UnitID,GroupID);
      SendRecordWaveData(UnitID,GroupID);
      return TRUE;
    }
    case 0xAA://启动场景
    {
      if((g_DistributionOperateMode==FALSE)&&(m_OperateEnable==FALSE))//集中操作模式且无控制权限
        return TRUE;

      char filename[256];
      memset(filename,0x00,256);
      memcpy(filename,DataBuf+15,240);

      g_SceneOperateFileName = filename;

      return TRUE;
    }
    case 0xBB://终止场景
    {
      ::ClearSceneOperate(&g_SceneOperate);

      for(int unitno=0;unitno<UNITCOUNT;unitno++)
      {
        g_Unit[unitno].CurrentParamaterGroupID = 0;
        g_Unit[unitno].TransmissionState = FALSE;
        g_Unit[unitno].TrainState = FALSE;

        for(int varno=0;varno<VARCOUNT;varno++)
        {
          g_Unit[unitno].VarGroup[1].Var[63].Value = 0;
          g_Unit[unitno].VarLock[varno] = FALSE;
          if(g_Unit[unitno].VarAbnormal[varno]==TRUE)
          {
            g_Unit[unitno].VarAbnormal[varno] = FALSE;
            g_Unit[unitno].VarGroup[1].Var[varno].LastValue = !g_Unit[unitno].VarGroup[1].Var[varno].Value;
          }
        }
      }
      
      ::ClearPower();

      //g_pDlgMessageBox->DoShow(g_SceneOperate.SceneName + " 场景终止",1);

      return TRUE;
    }
    case 0xCC://场景操作
    {
      g_SceneOperateID = MAKEWORD(DataBuf[15],DataBuf[16]);

      //g_pDlgMessageBox->DoShow(g_SceneOperate.SceneName + " 场景终止",1);

      return TRUE;
    }
  }

  return FALSE;
}

CNewSocketTH104::CNewSocketTH104()
{
  m_Open   = FALSE;

  m_LocalIPAddress="";
  m_SocketPort = 2404;
}

CNewSocketTH104::~CNewSocketTH104()
{
}

BOOL CNewSocketTH104::PhysicalReceive(BYTE *pReceiveBuf,int &ReceiveLen)
{
  WORD TotalReceiveLen = 0;

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
          TotalReceiveLen = receivebuf[1] + 2;

        memcpy(pReceiveBuf+ReceiveLen,receivebuf,receivelen);
        ReceiveLen += receivelen;

        if(ReceiveLen>=TotalReceiveLen)
          return TRUE;
      }
    }

    if(TotalReceiveLen==0)
      return FALSE;

    if((::GetTickCount()-TickCount)>1000)//接收超时
      return FALSE;
  }

  return FALSE;
}

BOOL CNewSocketTH104::PhysicalSend(BYTE *pSendBuf,int &SendLen)
{
  CSingleLock sLock(&m_SocketMutex);
  sLock.Lock();

  WORD PacketLen = SendLen;
  WORD SendPos = 0;
  DWORD BeginTickCount = ::GetTickCount();
  while(TRUE)
  {
    if((SendPos+PacketLen)>SendLen)
      PacketLen = (WORD)(SendLen - SendPos);

	  int Result = send(m_hSocket,(LPSTR)pSendBuf+SendPos,PacketLen,0);
    if(Result!=PacketLen)
    {
      closesocket(m_hSocket);
      m_Open = FALSE;
      m_hSocket = INVALID_SOCKET;

      return FALSE;
    }

    if((SendPos+PacketLen)>=SendLen)
      break;

    SendPos += PacketLen;
  }

  return TRUE;
}

BOOL CNewSocketTH104::CreateSocket()
{
  m_hSocket = ::socket(AF_INET,SOCK_STREAM,0);
  
  m_sockAddr.sin_family = AF_INET;
	m_sockAddr.sin_addr.s_addr = INADDR_ANY;
  m_sockAddr.sin_port = htons((u_short)m_SocketPort);

  ::bind(m_hSocket,(SOCKADDR*)&m_sockAddr,sizeof(m_sockAddr));

  if(::listen(m_hSocket,1)==0)//监听
  {
    CString msg;
    msg.Format("TH104 建立监听成功 端口地址=%d",m_SocketPort);
    ShowPaneTextMessage(msg);
    m_Open = TRUE;
  }
  else
  {
    CString msg;
    msg.Format("TH104 建立监听失败 端口地址=%d",m_SocketPort);
    ShowPaneTextMessage(msg);
    m_Open = FALSE;
  }

  return TRUE;
}

BOOL CNewSocketTH104::CloseSocket()
{
  closesocket(m_hSocket);
  m_hSocket = INVALID_SOCKET;
  m_Open = FALSE;
  return TRUE;
}

BOOL CNewSocketTH104::AcceptSocket()
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
      CNewSocketTH104* pSocket = new CNewSocketTH104;
      pSocket->m_hSocket = hsock;
      pSocket->m_LocalIPAddress = inet_ntoa(sockAddr.sin_addr);

      for(int no=0;no<COMPUTER_COUNT;no++)
      {
        if(g_ComputerList[no].m_ComputerIPAddress==pSocket->m_LocalIPAddress)
        {
          g_ProtocolTH104[no].m_ComputerID = no;
          g_ProtocolTH104[no].m_pSocketTH104 = pSocket;

          if(g_ProtocolTH104[no].m_pCommThread==NULL)
          {
            g_ProtocolTH104[no].m_pCommThread = (CCommThread*)AfxBeginThread(RUNTIME_CLASS(CCommThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
            g_ProtocolTH104[no].m_pCommThread->m_ComputerID = no;
            g_ProtocolTH104[no].m_pCommThread->ResumeThread();
          }

          CString msg;
          msg.Format("TH104 接收连接 IP地址=%s 端口地址=%d 主机编号=%d",pSocket->m_LocalIPAddress,m_SocketPort,no);
          ShowPaneTextMessage(msg);
      
          g_DistributionOperateMode = FALSE;//集中操作模式

          return TRUE;
        }
      }

      {
        g_ProtocolTH104[255].m_ComputerID = 255;
        g_ProtocolTH104[255].m_pSocketTH104 = pSocket;

        if(g_ProtocolTH104[255].m_pCommThread==NULL)
        {
          g_ProtocolTH104[255].m_pCommThread = (CCommThread*)AfxBeginThread(RUNTIME_CLASS(CCommThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
          g_ProtocolTH104[255].m_pCommThread->m_ComputerID = 255;
          g_ProtocolTH104[255].m_pCommThread->ResumeThread();
        }

        CString msg;
        msg.Format("TH104 接收连接 IP地址=%s 端口地址=%d 主机编号=%d",pSocket->m_LocalIPAddress,m_SocketPort,255);
        ShowPaneTextMessage(msg);

        g_DistributionOperateMode = TRUE;//分散操作模式

        return TRUE;
      }
    }
  
  }

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CNewServerSocketTH104Thread

IMPLEMENT_DYNCREATE(CNewServerSocketTH104Thread, CWinThread)

CNewServerSocketTH104Thread::CNewServerSocketTH104Thread()
{
  m_Run = TRUE;       //运行标志
  m_Running = TRUE;   //正在运行标志
}

CNewServerSocketTH104Thread::~CNewServerSocketTH104Thread()
{
}

BOOL CNewServerSocketTH104Thread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CNewServerSocketTH104Thread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CNewServerSocketTH104Thread, CWinThread)
	//{{AFX_MSG_MAP(CSocketThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewServerSocketTH104Thread message handlers

int CNewServerSocketTH104Thread::Run() 
{
  m_Time=CTime::GetCurrentTime();
  CTime ProcessTime = CTime::GetCurrentTime();
  while(m_Run)
  {
    m_Time=CTime::GetCurrentTime();

    Sleep(1);

    g_NewSocketTH104.AcceptSocket();//接收连接

    ProcessTime = CTime::GetCurrentTime();
  }
	m_Running = FALSE;
	return CWinThread::Run();
}

void CNewServerSocketTH104Thread::KillThread()//结束线程
{
	m_Run = FALSE;
  while(m_Running==TRUE)
    Sleep(100);
  TerminateThread(m_hThread,0);
}
