// SocketSer.cpp : implementation file
//

#include "stdafx.h"
#include "DataExchangeSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDataExchangeSocket g_DataExchangePowerStateSocket;
CDataExchangeSocket g_DataExchangeTrainStateSocket;
CDataExchangeSocket g_DataExchangePowerStateSocketOther;
TagDataExchangeStruct g_DataExchangeList[256];     //本所数据交换列表
TagDataExchangeStruct g_DataExchangeListOther[256];//外所数据交换列表

WORD    g_EnableReceivePowerState = FALSE;
WORD    g_EnableSendPowerState = FALSE;
WORD    g_EnableReceiveTrainState = FALSE;
WORD    g_EnableSendTrainState = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CDataExchangeSocket

CDataExchangeSocket::CDataExchangeSocket()
{
  m_Open   = FALSE;
  m_FrameReceiveCount = 0;
  m_FrameSendCount = 0;

  m_LocalSocketPort = 3000;
  m_OtherSocketPort = 3000;

  memset(m_DataBuffer,0x00,1024);	
  m_DataLen = 0;	
}

CDataExchangeSocket::~CDataExchangeSocket()
{
}

BOOL CDataExchangeSocket::PhysicalReceive(BYTE *pReceive, int & nLen )
{
  BYTE receivebuf[0xffff];
	
	int receivelen = Receive(receivebuf, sizeof(receivebuf));

  if(receivelen == SOCKET_ERROR)
	{
  }
  else
  {
    m_FrameReceiveCount++;
    memcpy(pReceive,receivebuf,receivelen);
    nLen = receivelen;
    return TRUE;
  }
  return FALSE;
}

int CDataExchangeSocket::PhysicalSend(BYTE * buf, int nLen )
{
  if ( nLen <=0 )
		return -1;

  m_FrameSendCount++;
  int nSend = SendTo((BYTE*)buf, nLen,m_OtherSocketPort,m_OtherIPAddress); 

  return  (int)nSend;
}

BOOL CDataExchangeSocket::CreateSocket()
{
  m_FrameReceiveCount = 0;
  m_FrameSendCount = 0;

  CString LocalIPAddress;
  HOSTENT *pHost = ::gethostbyname(NULL);
  if(pHost)
  {
    struct in_addr in;
    in.s_addr = *(DWORD*)pHost->h_addr_list[0];
    LocalIPAddress = ::inet_ntoa(in);
  }

  m_LocalIPAddress = LocalIPAddress;
  m_OtherIPAddress = m_LocalIPAddress.Left(m_LocalIPAddress.ReverseFind('.'))+".255";

  CString msg;
  BOOL result = Create(m_LocalSocketPort, SOCK_DGRAM, FD_READ | FD_WRITE, m_LocalIPAddress);
  if(result == SOCKET_ERROR)
  {
    msg.Format("创建UDPSocket失败 IP地址=%s 端口地址=%d",m_LocalIPAddress,m_LocalSocketPort);
    m_Open = FALSE;
    ::AfxMessageBox(msg);
  }
  else
  {
    msg.Format("创建UDPSocket成功 IP地址=%s 端口地址=%d",m_LocalIPAddress,m_LocalSocketPort);
    m_Open = TRUE;
  }

  return TRUE;
}

BOOL CDataExchangeSocket::CloseSocket()
{
  m_FrameReceiveCount = 0;
  m_FrameSendCount = 0;

  Close();
  m_Open = FALSE;
  CString msg;
  msg.Format("关闭UDPSocket IP地址=%s 端口地址=%d",m_LocalIPAddress,m_LocalSocketPort);
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDataExchangeSocket member functions

void CDataExchangeSocket::ProcessReceivePowerState()
{
  while(PhysicalReceive(m_DataBuffer,m_DataLen)==TRUE)
  {
    while(m_DataLen>0)
    {
      if(m_DataBuffer[0]!=0xeb)
      {
        m_DataLen = 0;
        return;
      }
      if(m_DataBuffer[1]!=0x90)
      {
        m_DataLen = 0;
        return;
      }
      if(m_DataBuffer[2]!=0xeb)
      {
        m_DataLen = 0;
        return;
      }
      if(m_DataBuffer[3]!=0x90)
      {
        m_DataLen = 0;
        return;
      }
      if(m_DataBuffer[4]!=0xeb)
      {
        m_DataLen = 0;
        return;
      }
      if(m_DataBuffer[5]!=0x90)
      {
        m_DataLen = 0;
        return;
      }
    
      WORD DataLen  = m_DataBuffer[7]*256 + m_DataBuffer[6];
      if(DataLen<m_DataLen)
      {
        m_DataLen = 0;
        return;
      }

      WORD DataCount = (DataLen - 8) / 2;
      for(int no=0;no<DataCount;no++)
      {
        WORD Value = m_DataBuffer[9+no*2]*256+m_DataBuffer[8+no*2];
        WORD Item = Value&0xfff;
        WORD State = (Value&0x8000)/0x8000;
        if((Item/256)==5)//外所数据
          g_DataExchangeListOther[Item&0xff].PowerState = (BOOL)State;
      }
  
      BYTE tempbuf[1024];
      memcpy(tempbuf,m_DataBuffer+DataLen,m_DataLen);
      memcpy(m_DataBuffer,tempbuf,m_DataLen);
      m_DataLen -= DataLen;
    }
  }
}

void CDataExchangeSocket::ProcessSendPowerState()
{
  WORD Count = 0;
  for(int no=0;no<256;no++)
  {
    if(g_DataExchangeList[no].UnitID>0)
    {
      WORD UnitID = g_DataExchangeList[no].UnitID;
      WORD VarGroupID = g_DataExchangeList[no].VarGroupID;
      WORD VarID = g_DataExchangeList[no].VarID;
      if(VarGroupID==0)//遥测
      {
        if(g_Unit[UnitID].VarGroup[VarGroupID].Var[VarID].Value>=1)
          g_DataExchangeList[no].PowerState = TRUE;
        else
          g_DataExchangeList[no].PowerState = FALSE;
      }
      if(VarGroupID==1)//遥信
      {
        if(g_Unit[UnitID].VarGroup[VarGroupID].Var[VarID].Value!=0)
          g_DataExchangeList[no].PowerState = TRUE;
        else
          g_DataExchangeList[no].PowerState = FALSE;
      }
      Count++;
    }
    else
      break;
  }
  if(Count==0)
    return;

  WORD DataLen = Count*2+8;
  BYTE DataBuf[1024];
  DataBuf[0] = 0xeb;
  DataBuf[1] = 0x90;
  DataBuf[2] = 0xeb;
  DataBuf[3] = 0x90;
  DataBuf[4] = 0xeb;
  DataBuf[5] = 0x90;
  DataBuf[6] = LOBYTE(DataLen);
  DataBuf[7] = HIBYTE(DataLen);

  for(no=0;no<Count;no++)
  {
    WORD Item = no;
    WORD State = (WORD)g_DataExchangeList[no].PowerState;
    WORD Data = Item&0x0fff | g_PowerID*256;
    if(State)
      Data |= 0x8000;
    
    DataBuf[8+no*2] = LOBYTE(Data);
    DataBuf[9+no*2] = HIBYTE(Data);
  }

  if(PhysicalSend(DataBuf,DataLen)!=DataLen)
  {
    ;
  }    
}

void CDataExchangeSocket::ProcessReceiveTrainState()
{
  while(PhysicalReceive(m_DataBuffer,m_DataLen)==TRUE)
  {
    while(m_DataLen>0)
    {
      if(m_DataBuffer[0]!=0xeb)
      {
        m_DataLen = 0;
        return;
      }
      if(m_DataBuffer[1]!=0x90)
      {
        m_DataLen = 0;
        return;
      }
      if(m_DataBuffer[2]!=0xeb)
      {
        m_DataLen = 0;
        return;
      }
      if(m_DataBuffer[3]!=0x90)
      {
        m_DataLen = 0;
        return;
      }
      if(m_DataBuffer[4]!=0xeb)
      {
        m_DataLen = 0;
        return;
      }
      if(m_DataBuffer[5]!=0x90)
      {
        m_DataLen = 0;
        return;
      }
    
      WORD DataLen  = m_DataBuffer[7]*256 + m_DataBuffer[6];
      if(DataLen<m_DataLen)
      {
        m_DataLen = 0;
        return;
      }

      WORD DataCount = (DataLen - 8) / 2;
      for(int no=0;no<DataCount;no++)
      {
        WORD Value = m_DataBuffer[9+no*2]*256+m_DataBuffer[8+no*2];
        WORD Item = Value&0xfff;
        WORD State = (Value&0x8000)/0x8000;
        if((Item/256)==g_PowerID)
          g_DataExchangeList[Item&0xff].TrainState = (BOOL)State;
      }
  
      BYTE tempbuf[1024];
      memcpy(tempbuf,m_DataBuffer+DataLen,m_DataLen);
      memcpy(m_DataBuffer,tempbuf,m_DataLen);
      m_DataLen -= DataLen;
    }
  }
}

void CDataExchangeSocket::ProcessSendTrainState()
{
  WORD Count = 0;
  for(int no=0;no<256;no++)
  {
    if(g_DataExchangeList[no].UnitID>0)
      Count++;
    else
      break;
  }
  if(Count==0)
    return;

  WORD DataLen = Count*2+8;
  BYTE DataBuf[1024];
  DataBuf[0] = 0xeb;
  DataBuf[1] = 0x90;
  DataBuf[2] = 0xeb;
  DataBuf[3] = 0x90;
  DataBuf[4] = 0xeb;
  DataBuf[5] = 0x90;
  DataBuf[6] = LOBYTE(DataLen);
  DataBuf[7] = HIBYTE(DataLen);

  for(no=0;no<Count;no++)
  {
    WORD Item = no;
    WORD State = (WORD)g_DataExchangeList[no].TrainState;
    WORD Data = Item&0x0fff | g_PowerID*256;
    if(State)
      Data |= 0x8000;
    
    DataBuf[8+no*2] = LOBYTE(Data);
    DataBuf[9+no*2] = HIBYTE(Data);
  }

  if(PhysicalSend(DataBuf,DataLen)!=DataLen)
  {
    ;
  }    
}
