// DlgPowerMonitor.cpp : implementation file
//

#include "stdafx.h"
#include "simcommunication.h"
#include "DlgPowerMonitor.h"

CDlgPowerMonitor *g_pDlgPowerMonitor = NULL;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPowerMonitor dialog


CDlgPowerMonitor::CDlgPowerMonitor(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPowerMonitor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPowerMonitor)
	//}}AFX_DATA_INIT
}


void CDlgPowerMonitor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPowerMonitor)
	DDX_Control(pDX, IDC_TRANSMISSION_LIST, m_ctlTransmissionList);
	DDX_Control(pDX, IDC_FEED_LIST, m_ctlFeedList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPowerMonitor, CDialog)
	//{{AFX_MSG_MAP(CDlgPowerMonitor)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPowerMonitor message handlers

BOOL CDlgPowerMonitor::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_ctlTransmissionList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
  m_ctlFeedList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	::AddListTitle(m_ctlTransmissionList,"传动状态",0);
	::AddListTitle(m_ctlTransmissionList,"传动馈线名称",1);
	::AddListTitle(m_ctlTransmissionList,"传动持续时间",2);
	::AddListTitle(m_ctlTransmissionList,"传动电流",3);
	::AddListTitle(m_ctlTransmissionList,"电流相位",4);
	::AddListTitle(m_ctlTransmissionList,"启动时间",5);
  m_ctlTransmissionList.SetColumnWidth(0,120);
  m_ctlTransmissionList.SetColumnWidth(1,120);

  ::AddListTitle(m_ctlFeedList,"馈线名称",0);
  ::AddListTitle(m_ctlFeedList,"馈线电流",1);

  SetTimer(500,500,NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPowerMonitor::OnTimer(UINT nIDEvent) 
{
  if(IsWindowVisible())
  {
    CSingleLock sLock(&g_PowerMutex);
    sLock.Lock();

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
      WORD FeedCount = 0;
      POSITION pos = FeedList.GetHeadPosition();
      while(pos)
      {
        struct TagUnitStruct *pUnit = (TagUnitStruct *)FeedList.GetNext(pos);
        if(pUnit->UnitType==4)//线路保护
          FeedCount++;
      }

      while(m_ctlFeedList.GetItemCount()<FeedCount)
      {
        ::AddListItem(m_ctlFeedList,0,0,"");
      }
      while(m_ctlFeedList.GetItemCount()>FeedCount)
      {
        m_ctlFeedList.DeleteItem(0);
      }

      {
        int item=0;
        POSITION pos = FeedList.GetHeadPosition();
        while(pos)
        {
          struct TagUnitStruct *pUnit = (TagUnitStruct *)FeedList.GetNext(pos);
          if(pUnit->UnitType==4)//线路保护
          {
            ::SetItemText(m_ctlFeedList,item,0,pUnit->UnitName);
            ::SetItemText(m_ctlFeedList,item,1,::GetFormatText("%0.1f",pUnit->VarGroup[0].Var[8].Value));
            item++;
          }
        }
      }
    }
    {
      WORD PowerCount = g_FreeTransmissionList.GetCount();
      while(m_ctlTransmissionList.GetItemCount()<PowerCount)
      {
        ::AddListItem(m_ctlTransmissionList,0,0,"");
      }
      while(m_ctlTransmissionList.GetItemCount()>PowerCount)
      {
        m_ctlTransmissionList.DeleteItem(0);
      }

	    int item = 0;

      POSITION pos = g_FreeTransmissionList.GetHeadPosition();
      while(pos)
      {
        TagFreeTransmissionStruct *pTransmission = (TagFreeTransmissionStruct *)g_FreeTransmissionList.GetNext(pos);
   
        CString msg;
        CTime ct = CTime::GetCurrentTime();
        if(ct<pTransmission->BeginTime)
          msg.Format("传动等待 %d(秒)",(pTransmission->BeginTime-ct).GetTotalSeconds());
        else if(ct>pTransmission->FinishTime)
          msg.Format("传动完成 %d(秒)",(ct-pTransmission->FinishTime).GetTotalSeconds());
        else
          msg.Format("传动中.. %d(秒)",(pTransmission->FinishTime-ct).GetTotalSeconds());
        ::SetItemText(m_ctlTransmissionList,item,0,msg);

        ::SetItemText(m_ctlTransmissionList,item,1,g_Unit[pTransmission->FeederUnitID].UnitName);
        ::SetItemText(m_ctlTransmissionList,item,2,::GetFormatText("%d",pTransmission->ContinuedSeconds));
        ::SetItemText(m_ctlTransmissionList,item,3,::GetFormatText("%0.0f",pTransmission->AverCurrent));
        ::SetItemText(m_ctlTransmissionList,item,4,::GetFormatText("%0.0f",pTransmission->AverAngle));
        ::SetItemText(m_ctlTransmissionList,item,5,::GetFormatText("%d",pTransmission->StartupDelaySecond));

        item++;
      }
    }
  }

  CDialog::OnTimer(nIDEvent);
}

void CDlgPowerMonitor::OnClear() 
{
  ::ClearPower();
}
