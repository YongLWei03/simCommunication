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
	DDX_Control(pDX, IDC_TRAIN_LIST, m_ctlTrainList);
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
  m_ctlTrainList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
  m_ctlFeedList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	::AddListTitle(m_ctlTransmissionList,"����״̬",0);
	::AddListTitle(m_ctlTransmissionList,"������������",1);
	::AddListTitle(m_ctlTransmissionList,"��������ʱ��",2);
	::AddListTitle(m_ctlTransmissionList,"��������",3);
	::AddListTitle(m_ctlTransmissionList,"������λ",4);
	::AddListTitle(m_ctlTransmissionList,"����ʱ��",5);
  m_ctlTransmissionList.SetColumnWidth(0,120);
  m_ctlTransmissionList.SetColumnWidth(1,120);

	::AddListTitle(m_ctlTrainList,"�г�״̬",0);
	::AddListTitle(m_ctlTrainList,"�г���������",1);
	::AddListTitle(m_ctlTrainList,"�г�����ʱ��",2);
	::AddListTitle(m_ctlTrainList,"�г�����",3);
	::AddListTitle(m_ctlTrainList,"������λ",4);
	::AddListTitle(m_ctlTrainList,"����ʱ��",5);
  m_ctlTrainList.SetColumnWidth(0,120);
  m_ctlTrainList.SetColumnWidth(1,120);

  ::AddListTitle(m_ctlFeedList,"��������",0);
  ::AddListTitle(m_ctlFeedList,"���ߵ���",1);

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
      WORD FeedCount = 0;
      POSITION pos = FeedList.GetHeadPosition();
      while(pos)
      {
        struct TagUnitStruct *pUnit = (TagUnitStruct *)FeedList.GetNext(pos);
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
          ::SetItemText(m_ctlFeedList,item,0,pUnit->UnitName);
          switch(pUnit->UnitType)
          {
            case 1://��ѹ��·����
            case 8://��ѹ��·����
            {
              ::SetItemText(m_ctlFeedList,item,1,::GetFormatText("%0.1f",pUnit->VarGroup[0].Var[7].Value));
              break;
            }
            case 6://ֱ�����߱���
            {
              ::SetItemText(m_ctlFeedList,item,1,::GetFormatText("%0.1f",pUnit->VarGroup[0].Var[1].Value));
              break;
            }
          }
          item++;
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
          msg.Format("�����ȴ� %d(��)",(pTransmission->BeginTime-ct).GetTotalSeconds());
        else if(ct>pTransmission->FinishTime)
          msg.Format("������� %d(��)",(ct-pTransmission->FinishTime).GetTotalSeconds());
        else
          msg.Format("������.. %d(��)",(pTransmission->FinishTime-ct).GetTotalSeconds());
        ::SetItemText(m_ctlTransmissionList,item,0,msg);

        ::SetItemText(m_ctlTransmissionList,item,1,g_Unit[pTransmission->FeederUnitID].UnitName);
        ::SetItemText(m_ctlTransmissionList,item,2,::GetFormatText("%d",pTransmission->ContinuedSeconds));
        ::SetItemText(m_ctlTransmissionList,item,3,::GetFormatText("%0.0f",pTransmission->AverCurrent));
        ::SetItemText(m_ctlTransmissionList,item,4,::GetFormatText("%0.0f",pTransmission->AverAngle));
        ::SetItemText(m_ctlTransmissionList,item,5,::GetFormatText("%d",pTransmission->StartupDelaySecond));

        item++;
      }
    }
    {
      WORD PowerCount = g_FreeTrainList.GetCount();
      while(m_ctlTrainList.GetItemCount()<PowerCount)
      {
        ::AddListItem(m_ctlTrainList,0,0,"");
      }
      while(m_ctlTrainList.GetItemCount()>PowerCount)
      {
        m_ctlTrainList.DeleteItem(0);
      }

	    int item = 0;

      POSITION pos = g_FreeTrainList.GetHeadPosition();
      while(pos)
      {
        TagFreeTrainStruct *pFreeTrain = (TagFreeTrainStruct *)g_FreeTrainList.GetNext(pos);

        CString msg;
        CTime ct = CTime::GetCurrentTime();
        if(ct<pFreeTrain->InBeginTime)
          msg.Format("�г��ȴ� %d(��)",(pFreeTrain->InBeginTime-ct).GetTotalSeconds());
        else if(ct>pFreeTrain->OutFinishTime)
          msg.Format("�г���� %d(��)",(ct-pFreeTrain->OutFinishTime).GetTotalSeconds());
        else if(ct<pFreeTrain->ChangeBeginTime)
          msg.Format("ʻ����.. %d(��)",(pFreeTrain->InFinishTime-ct).GetTotalSeconds());
        else if(ct<pFreeTrain->ChangeFinishTime)
          msg.Format("������.. %d(��)",(pFreeTrain->ChangeFinishTime-ct).GetTotalSeconds());
        else
          msg.Format("ʻ����.. %d(��)",(pFreeTrain->OutFinishTime-ct).GetTotalSeconds());
        ::SetItemText(m_ctlTrainList,item,0,msg);

        ::SetItemText(m_ctlTrainList,item,1,g_Unit[pFreeTrain->InFeederUnitID].UnitName+"->"+g_Unit[pFreeTrain->OutFeederUnitID].UnitName);
        ::SetItemText(m_ctlTrainList,item,2,::GetFormatText("%d",pFreeTrain->InContinuedSeconds)+"-"+::GetFormatText("%d",pFreeTrain->ChangeContinuedSeconds)+"-"+::GetFormatText("%d",pFreeTrain->OutContinuedSeconds));
        ::SetItemText(m_ctlTrainList,item,3,::GetFormatText("%0.0f",pFreeTrain->AverCurrent));
        ::SetItemText(m_ctlTrainList,item,4,::GetFormatText("%0.0f",pFreeTrain->AverAngle));
        ::SetItemText(m_ctlTrainList,item,5,::GetFormatText("%d",pFreeTrain->StartupDelaySecond));

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
