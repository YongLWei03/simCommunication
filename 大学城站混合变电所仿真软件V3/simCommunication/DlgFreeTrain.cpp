// DlgFreeTrain.cpp : implementation file
//

#include "stdafx.h"
#include "simcommunication.h"
#include "DlgFreeTrain.h"
#include "DlgCurrentDistribution.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFreeTrain dialog


CDlgFreeTrain::CDlgFreeTrain(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFreeTrain::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFreeTrain)
	//}}AFX_DATA_INIT
}


void CDlgFreeTrain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFreeTrain)
	DDX_Control(pDX, IDC_EDIT_TRAIN_DIRECT, m_ctlTrainDirect);
	DDX_Control(pDX, IDC_OUT_LIST, m_ctlOutList);
	DDX_Control(pDX, IDC_IN_LIST, m_ctlInList);
	DDX_Control(pDX, IDC_EDIT_FREE_TRAIN_CURRENT_CHANGE_RATE, m_ctlCurrentChangeRate);
	DDX_Control(pDX, IDC_EDIT_FREE_TRAIN_STARTUP_DELAY_SECOND, m_ctlStartupDelaySecond);
	DDX_Control(pDX, IDC_EDIT_AVER_CURRENT, m_ctlAverCurrent);
	DDX_Control(pDX, IDC_EDIT_OUT_CONTINUED_SECONDS, m_ctlOutContinuedSeconds);
	DDX_Control(pDX, IDC_EDIT_IN_CONTINUED_SECONDS, m_ctlInContinuedSeconds);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFreeTrain, CDialog)
	//{{AFX_MSG_MAP(CDlgFreeTrain)
	ON_BN_CLICKED(IDC_IN_SET, OnInSet)
	ON_BN_CLICKED(IDC_OUT_SET, OnOutSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFreeTrain message handlers

BOOL CDlgFreeTrain::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_ctlInContinuedSeconds.SetWindowText(::GetFormatText("%d",m_pFreeTrain->InContinuedSeconds));
  m_ctlOutContinuedSeconds.SetWindowText(::GetFormatText("%d",m_pFreeTrain->OutContinuedSeconds));
  m_ctlAverCurrent.SetWindowText(::GetFormatText("%d",(int)m_pFreeTrain->AverCurrent));

  m_ctlInList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
  AddListTitle(m_ctlInList,"序号",0);
  AddListTitle(m_ctlInList,"数值",1);
  m_ctlOutList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
  AddListTitle(m_ctlOutList,"序号",0);
  AddListTitle(m_ctlOutList,"数值",1);

  m_ctlTrainDirect.AddString("上行(212-214))");
  m_ctlTrainDirect.AddString("下行(213-211))");
  if((m_pFreeTrain->InFeederUnitID==14)&&(m_pFreeTrain->OutFeederUnitID==16))//上行(212-214)
    m_ctlTrainDirect.SetCurSel(0);
  if((m_pFreeTrain->InFeederUnitID==15)&&(m_pFreeTrain->OutFeederUnitID==13))//下行(213-211)
    m_ctlTrainDirect.SetCurSel(1);
  if(m_ctlTrainDirect.GetCurSel()<0)
    m_ctlTrainDirect.SetCurSel(0);

  {
    CNewString ns(m_pFreeTrain->InCurrentChangeRateList,";");
    for(int no=0;no<100;no++)
    {
      float CurrentChangeRate = ns.GetFloatValue(0,no);
      if(ns.GetWord(0,no)=="")
        CurrentChangeRate = (float)1;

      ::AddListItem(m_ctlInList,no,0,::GetFormatText("%02d",no));
      ::AddListItem(m_ctlInList,no,1,::GetFormatText("%02d",(WORD)((float)CurrentChangeRate*(float)100)));
    }
  }
  {
    CNewString ns(m_pFreeTrain->OutCurrentChangeRateList,";");
    for(int no=0;no<100;no++)
    {
      float CurrentChangeRate = ns.GetFloatValue(0,no);
      if(ns.GetWord(0,no)=="")
        CurrentChangeRate = (float)1;

      ::AddListItem(m_ctlOutList,no,0,::GetFormatText("%02d",no));
      ::AddListItem(m_ctlOutList,no,1,::GetFormatText("%02d",(WORD)((float)CurrentChangeRate*(float)100)));
    }
  }

  m_ctlCurrentChangeRate.SetWindowText(::GetFormatText("%0.1f",m_pFreeTrain->CurrentChangeRate*(float)100));
  m_ctlStartupDelaySecond.SetWindowText(::GetFormatText("%0d",m_pFreeTrain->StartupDelaySecond));


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFreeTrain::OnOK() 
{
  CString InContinuedSeconds;
  CString OutContinuedSeconds;
  CString AverCurrent;

  m_ctlInContinuedSeconds.GetWindowText(InContinuedSeconds);
  m_ctlOutContinuedSeconds.GetWindowText(OutContinuedSeconds);
  m_ctlAverCurrent.GetWindowText(AverCurrent);
	
  m_pFreeTrain->InContinuedSeconds = atoi(InContinuedSeconds);
  m_pFreeTrain->OutContinuedSeconds = atoi(OutContinuedSeconds);
  m_pFreeTrain->ChangeContinuedSeconds = 1;
  m_pFreeTrain->AverCurrent = (float)atof(AverCurrent);
  m_pFreeTrain->AverAngle = (float)0;

  CString CurrentChangeRate;
  CString StartupDelaySecond;

  m_ctlCurrentChangeRate.GetWindowText(CurrentChangeRate);
  m_ctlStartupDelaySecond.GetWindowText(StartupDelaySecond);

  m_pFreeTrain->CurrentChangeRate = (float)atof(CurrentChangeRate)/(float)100;
  m_pFreeTrain->StartupDelaySecond = (WORD)atoi(StartupDelaySecond);;

  if(m_ctlTrainDirect.GetCurSel()==0)//上行(212-214)
  {
    m_pFreeTrain->InFeederUnitID = 14;
    m_pFreeTrain->OutFeederUnitID = 16;
  }
  if(m_ctlTrainDirect.GetCurSel()==1)//下行(213-211)
  {
    m_pFreeTrain->InFeederUnitID = 15;
    m_pFreeTrain->OutFeederUnitID = 13;
  }

	CDialog::OnOK();
}

void CDlgFreeTrain::OnInSet() 
{
  CDlgCurrentDistribution dlg;
  dlg.m_ChangeRageList = m_pFreeTrain->InCurrentChangeRateList;
  if(dlg.DoModal()==IDOK)
  {
    strcpy(m_pFreeTrain->InCurrentChangeRateList,dlg.m_ChangeRageList.GetBuffer(0));

    CNewString ns(m_pFreeTrain->InCurrentChangeRateList,";","\r\n");
    for(int no=0;no<100;no++)
    {
      float CurrentChangeRate = ns.GetFloatValue(0,no);
      ::SetItemText(m_ctlInList,no,0,::GetFormatText("%02d",no));
      ::SetItemText(m_ctlInList,no,1,::GetFormatText("%02d",(WORD)((float)CurrentChangeRate*(float)100)));
    }
  }
}

void CDlgFreeTrain::OnOutSet() 
{
  CDlgCurrentDistribution dlg;
  dlg.m_ChangeRageList = m_pFreeTrain->OutCurrentChangeRateList;
  if(dlg.DoModal()==IDOK)
  {
    strcpy(m_pFreeTrain->OutCurrentChangeRateList,dlg.m_ChangeRageList.GetBuffer(0));

    CNewString ns(m_pFreeTrain->OutCurrentChangeRateList,";","\r\n");
    for(int no=0;no<100;no++)
    {
      float CurrentChangeRate = ns.GetFloatValue(0,no);
      ::SetItemText(m_ctlOutList,no,0,::GetFormatText("%02d",no));
      ::SetItemText(m_ctlOutList,no,1,::GetFormatText("%02d",(WORD)((float)CurrentChangeRate*(float)100)));
    }
  }
}
