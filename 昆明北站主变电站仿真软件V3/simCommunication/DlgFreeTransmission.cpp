// DlgFreeTransmission.cpp : implementation file
//

#include "stdafx.h"
#include "simcommunication.h"
#include "DlgFreeTransmission.h"
#include "DlgCurrentDistribution.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFreeTransmission dialog


CDlgFreeTransmission::CDlgFreeTransmission(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFreeTransmission::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFreeTransmission)
	//}}AFX_DATA_INIT
}


void CDlgFreeTransmission::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFreeTransmission)
	DDX_Control(pDX, IDC_LIST, m_ctlList);
	DDX_Control(pDX, IDC_EDIT_FEED_NAME, m_ctlFeedUnitName);
	DDX_Control(pDX, IDC_EDIT_FREE_TRANSMISSION_CONTINUED_SECONDS, m_ctlContinuedSeconds);
	DDX_Control(pDX, IDC_EDIT_FREE_TRANSMISSION_CURRENT_CHANGE_RATE, m_ctlCurrentChangeRate);
	DDX_Control(pDX, IDC_EDIT_FREE_TRANSMISSION_STARTUP_DELAY_SECOND, m_ctlStartupDelaySecond);
	DDX_Control(pDX, IDC_EDIT_AVER_CURRENT, m_ctlAverCurrent);
	DDX_Control(pDX, IDC_EDIT_AVER_ANGLE, m_ctlAverAngle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFreeTransmission, CDialog)
	//{{AFX_MSG_MAP(CDlgFreeTransmission)
	ON_BN_CLICKED(IDC_SET, OnSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFreeTransmission message handlers

BOOL CDlgFreeTransmission::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
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

  m_ctlFeedUnitName.ResetContent();
  {
    POSITION pos = FeedList.GetHeadPosition();
    while(pos)
    {
      struct TagUnitStruct *pUnit = (TagUnitStruct *)FeedList.GetNext(pos);
      if(pUnit->UnitType==4)//线路保护
        m_ctlFeedUnitName.AddString(pUnit->UnitName);
    }
  }

  int Select = m_ctlFeedUnitName.FindString(0,g_Unit[m_pFreeTransmission->FeederUnitID].UnitName);
  if(Select==-1)
    m_ctlFeedUnitName.SetCurSel(0);
  else
    m_ctlFeedUnitName.SetCurSel(Select);

  m_ctlAverCurrent.SetWindowText(::GetFormatText("%d",(int)m_pFreeTransmission->AverCurrent));
  m_ctlAverAngle.SetWindowText(::GetFormatText("%d",(int)m_pFreeTransmission->AverAngle));

  m_ctlContinuedSeconds.SetWindowText(::GetFormatText("%d",m_pFreeTransmission->ContinuedSeconds));
  m_ctlCurrentChangeRate.SetWindowText(::GetFormatText("%0.1f",m_pFreeTransmission->CurrentChangeRate*(float)100));
  m_ctlStartupDelaySecond.SetWindowText(::GetFormatText("%d",m_pFreeTransmission->StartupDelaySecond));

  m_ctlList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
  AddListTitle(m_ctlList,"序号",0);
  AddListTitle(m_ctlList,"数值",1);

  CNewString ns(m_pFreeTransmission->CurrentChangeRateList,";","\r\n");
  for(int no=0;no<100;no++)
  {
    float CurrentChangeRate = ns.GetFloatValue(0,no);
    if(ns.GetWord(0,no)=="")
      CurrentChangeRate = (float)1;

    ::AddListItem(m_ctlList,no,0,::GetFormatText("%02d",no));
    ::AddListItem(m_ctlList,no,1,::GetFormatText("%02d",(WORD)((float)CurrentChangeRate*(float)100)));
  }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFreeTransmission::OnOK() 
{
  CString FeedName;
  m_ctlFeedUnitName.GetLBText(m_ctlFeedUnitName.GetCurSel(),FeedName);
  m_pFreeTransmission->FeederUnitID = ::GetFeederIDFromName(FeedName);

  CString AverCurrent;
  CString AverAngle;

  m_ctlAverCurrent.GetWindowText(AverCurrent);
  m_ctlAverAngle.GetWindowText(AverAngle);
	
  m_pFreeTransmission->AverCurrent= (float)atof(AverCurrent);
  m_pFreeTransmission->AverAngle= (float)atof(AverAngle);
  
  CString CurrentChangeRate;
  CString StartupDelaySecond;
  CString ContinuedSeconds;
  m_ctlContinuedSeconds.GetWindowText(ContinuedSeconds);
  m_ctlCurrentChangeRate.GetWindowText(CurrentChangeRate);
  m_ctlStartupDelaySecond.GetWindowText(StartupDelaySecond);
	m_pFreeTransmission->ContinuedSeconds = (WORD)atoi(ContinuedSeconds);
  m_pFreeTransmission->CurrentChangeRate = (float)atof(CurrentChangeRate)/(float)100;
	m_pFreeTransmission->StartupDelaySecond = (WORD)atoi(StartupDelaySecond);
	
	CDialog::OnOK();
}

void CDlgFreeTransmission::OnSet() 
{
  CDlgCurrentDistribution dlg;
  dlg.m_ChangeRageList = m_pFreeTransmission->CurrentChangeRateList;
  if(dlg.DoModal()==IDOK)
  {
    strcpy(m_pFreeTransmission->CurrentChangeRateList,dlg.m_ChangeRageList.GetBuffer(0));

    CNewString ns(m_pFreeTransmission->CurrentChangeRateList,";","\r\n");
    for(int no=0;no<100;no++)
    {
      float CurrentChangeRate = ns.GetFloatValue(0,no);
      ::SetItemText(m_ctlList,no,0,::GetFormatText("%02d",no));
      ::SetItemText(m_ctlList,no,1,::GetFormatText("%02d",(WORD)((float)CurrentChangeRate*(float)100)));
    }
  }
}
