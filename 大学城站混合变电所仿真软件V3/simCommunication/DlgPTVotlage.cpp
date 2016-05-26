// DlgPTVotlage.cpp : implementation file
//

#include "stdafx.h"
#include "simcommunication.h"
#include "PowerThread.h"
#include "DlgPTVotlage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPTVotlage dialog


CDlgPTVotlage::CDlgPTVotlage(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPTVotlage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPTVotlage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgPTVotlage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPTVotlage)
	DDX_Control(pDX, IDC_EDIT_2T_TEMP_2, m_ctl2TTemp2);
	DDX_Control(pDX, IDC_EDIT_2T_TEMP_1, m_ctl2TTemp1);
	DDX_Control(pDX, IDC_EDIT_1T_TEMP_2, m_ctl1TTemp2);
	DDX_Control(pDX, IDC_EDIT_1T_TEMP_1, m_ctl1TTemp1);
	DDX_Control(pDX, IDC_EDIT_2T_GEAR, m_ctl2TGear);
	DDX_Control(pDX, IDC_EDIT_1T_GEAR, m_ctl1TGear);
	DDX_Control(pDX, IDC_EDIT_1T_HI_CURRENT_MAX_VALUE, m_ctl1THiCurrentMaxValue);
	DDX_Control(pDX, IDC_EDIT_1T_HI_VOLTAGE_VALUEK, m_ctl1THiVoltageValueK);
	DDX_Control(pDX, IDC_EDIT_2T_HI_CURRENT_MAX_VALUE, m_ctl2THiCurrentMaxValue);
	DDX_Control(pDX, IDC_EDIT_2T_HI_VOLTAGE_VALUEK, m_ctl2THiVoltageValueK);
	DDX_Control(pDX, IDC_EDIT_1T_LO_CURRENT_MAX_VALUE, m_ctl1TLoCurrentMaxValue);
	DDX_Control(pDX, IDC_EDIT_1T_LO_VOLTAGE_VALUEK, m_ctl1TLoVoltageValueK);
	DDX_Control(pDX, IDC_EDIT_2T_LO_CURRENT_MAX_VALUE, m_ctl2TLoCurrentMaxValue);
	DDX_Control(pDX, IDC_EDIT_2T_LO_VOLTAGE_VALUEK, m_ctl2TLoVoltageValueK);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPTVotlage, CDialog)
	//{{AFX_MSG_MAP(CDlgPTVotlage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPTVotlage message handlers

BOOL CDlgPTVotlage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_ctl1THiVoltageValueK.SetWindowText(::GetFormatText("%0.2f",m_pPTVotlage->T1HiVoltageValueK));
  m_ctl1THiCurrentMaxValue.SetWindowText(::GetFormatText("%0.1f",m_pPTVotlage->T1HiCurrentMaxValue));
  m_ctl2THiVoltageValueK.SetWindowText(::GetFormatText("%0.2f",m_pPTVotlage->T2HiVoltageValueK));
  m_ctl2THiCurrentMaxValue.SetWindowText(::GetFormatText("%0.1f",m_pPTVotlage->T2HiCurrentMaxValue));
  m_ctl1TLoVoltageValueK.SetWindowText(::GetFormatText("%0.2f",m_pPTVotlage->T1LoVoltageValueK));
  m_ctl1TLoCurrentMaxValue.SetWindowText(::GetFormatText("%0.1f",m_pPTVotlage->T1LoCurrentMaxValue));
  m_ctl2TLoVoltageValueK.SetWindowText(::GetFormatText("%0.2f",m_pPTVotlage->T2LoVoltageValueK));
  m_ctl2TLoCurrentMaxValue.SetWindowText(::GetFormatText("%0.1f",m_pPTVotlage->T2LoCurrentMaxValue));
  m_ctl1TGear.SetWindowText(::GetFormatText("%d",m_pPTVotlage->T1Gear));
  m_ctl2TGear.SetWindowText(::GetFormatText("%d",m_pPTVotlage->T2Gear));
  m_ctl1TTemp1.SetWindowText(::GetFormatText("%d",m_pPTVotlage->T1Temp1));
  m_ctl1TTemp2.SetWindowText(::GetFormatText("%d",m_pPTVotlage->T1Temp2));
  m_ctl2TTemp1.SetWindowText(::GetFormatText("%d",m_pPTVotlage->T2Temp1));
  m_ctl2TTemp2.SetWindowText(::GetFormatText("%d",m_pPTVotlage->T2Temp2));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPTVotlage::OnOK() 
{
  CString T1HiVoltageValueK;
  CString T1HiCurrentMaxValue;
  CString T2HiVoltageValueK;
  CString T2HiCurrentMaxValue;
  CString T1LoVoltageValueK;
  CString T1LoCurrentMaxValue;
  CString T2LoVoltageValueK;
  CString T2LoCurrentMaxValue;
  CString T1Gear;
  CString T2Gear;
  CString T1Temp1;
  CString T1Temp2;
  CString T2Temp1;
  CString T2Temp2;

  m_ctl1THiVoltageValueK.GetWindowText(T1HiVoltageValueK);
  m_ctl1THiCurrentMaxValue.GetWindowText(T1HiCurrentMaxValue);
  m_ctl2THiVoltageValueK.GetWindowText(T2HiVoltageValueK);
  m_ctl2THiCurrentMaxValue.GetWindowText(T2HiCurrentMaxValue);
  m_ctl1TLoVoltageValueK.GetWindowText(T1LoVoltageValueK);
  m_ctl1TLoCurrentMaxValue.GetWindowText(T1LoCurrentMaxValue);
  m_ctl2TLoVoltageValueK.GetWindowText(T2LoVoltageValueK);
  m_ctl2TLoCurrentMaxValue.GetWindowText(T2LoCurrentMaxValue);
  m_ctl1TGear.GetWindowText(T1Gear);
  m_ctl2TGear.GetWindowText(T2Gear);
  m_ctl1TTemp1.GetWindowText(T1Temp1);
  m_ctl1TTemp2.GetWindowText(T1Temp2);
  m_ctl2TTemp1.GetWindowText(T2Temp1);
  m_ctl2TTemp2.GetWindowText(T2Temp2);

  m_pPTVotlage->T1HiVoltageValueK = (float)atof(T1HiVoltageValueK);
  m_pPTVotlage->T1HiCurrentMaxValue = (float)atof(T1HiCurrentMaxValue);
  m_pPTVotlage->T2HiVoltageValueK = (float)atof(T2HiVoltageValueK);
  m_pPTVotlage->T2HiCurrentMaxValue = (float)atof(T2HiCurrentMaxValue);
  m_pPTVotlage->T1LoVoltageValueK = (float)atof(T1LoVoltageValueK);
  m_pPTVotlage->T1LoCurrentMaxValue = (float)atof(T1LoCurrentMaxValue);
  m_pPTVotlage->T2LoVoltageValueK = (float)atof(T2LoVoltageValueK);
  m_pPTVotlage->T2LoCurrentMaxValue = (float)atof(T2LoCurrentMaxValue);
  m_pPTVotlage->T1Gear = atoi(T1Gear);
  m_pPTVotlage->T2Gear = atoi(T2Gear);
  m_pPTVotlage->T1Temp1 = atoi(T1Temp1);
  m_pPTVotlage->T1Temp2 = atoi(T1Temp2);
  m_pPTVotlage->T2Temp1 = atoi(T2Temp1);
  m_pPTVotlage->T2Temp2 = atoi(T2Temp2);
  m_pPTVotlage->StartupDelaySecond = 0;
  m_pPTVotlage->BeginTime = 0;

	CDialog::OnOK();
}
