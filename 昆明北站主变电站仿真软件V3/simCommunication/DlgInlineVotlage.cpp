// DlgInlineVotlage.cpp : implementation file
//

#include "stdafx.h"
#include "simcommunication.h"
#include "PowerThread.h"
#include "DlgInlineVotlage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgInlineVotlage dialog


CDlgInlineVotlage::CDlgInlineVotlage(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInlineVotlage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgInlineVotlage)
	//}}AFX_DATA_INIT
}


void CDlgInlineVotlage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInlineVotlage)
	DDX_Control(pDX, IDC_EDIT_2T_LO_VOLTAGE_CHANGE_RATE, m_ctl2TLoVoltageChangeRate);
	DDX_Control(pDX, IDC_EDIT_1T_LO_VOLTAGE_CHANGE_RATE, m_ctl1TLoVoltageChangeRate);
	DDX_Control(pDX, IDC_EDIT_UA1, m_ctlEditUa1);
	DDX_Control(pDX, IDC_EDIT_UB1, m_ctlEditUb1);
	DDX_Control(pDX, IDC_EDIT_UC1, m_ctlEditUc1);
	DDX_Control(pDX, IDC_EDIT_UA2, m_ctlEditUa2);
	DDX_Control(pDX, IDC_EDIT_UB2, m_ctlEditUb2);
	DDX_Control(pDX, IDC_EDIT_UC2, m_ctlEditUc2);
	DDX_Control(pDX, IDC_EDIT_1T_HI_VOLTAGE_CHANGE_RATE, m_ctl1THiVoltageChangeRate);
	DDX_Control(pDX, IDC_EDIT_2T_HI_VOLTAGE_CHANGE_RATE, m_ctl2THiVoltageChangeRate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgInlineVotlage, CDialog)
	//{{AFX_MSG_MAP(CDlgInlineVotlage)
	ON_BN_CLICKED(IDC_NORMAL_UA1, OnNormalUa1)
	ON_BN_CLICKED(IDC_NORMAL_UB1, OnNormalUb1)
	ON_BN_CLICKED(IDC_NORMAL_UC1, OnNormalUc1)
	ON_BN_CLICKED(IDC_LOST_UA1, OnLostUa1)
	ON_BN_CLICKED(IDC_LOST_UB1, OnLostUb1)
	ON_BN_CLICKED(IDC_LOST_UC1, OnLostUc1)
	ON_BN_CLICKED(IDC_LOW_UA1, OnLowUa1)
	ON_BN_CLICKED(IDC_LOW_UB1, OnLowUb1)
	ON_BN_CLICKED(IDC_LOW_UC1, OnLowUc1)
	ON_BN_CLICKED(IDC_NORMAL_UA2, OnNormalUa2)
	ON_BN_CLICKED(IDC_NORMAL_UB2, OnNormalUb2)
	ON_BN_CLICKED(IDC_NORMAL_UC2, OnNormalUc2)
	ON_BN_CLICKED(IDC_LOST_UA2, OnLostUa2)
	ON_BN_CLICKED(IDC_LOST_UB2, OnLostUb2)
	ON_BN_CLICKED(IDC_LOST_UC2, OnLostUc2)
	ON_BN_CLICKED(IDC_LOW_UA2, OnLowUa2)
	ON_BN_CLICKED(IDC_LOW_UB2, OnLowUb2)
	ON_BN_CLICKED(IDC_LOW_UC2, OnLowUc2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInlineVotlage message handlers

BOOL CDlgInlineVotlage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_ctlEditUa1.SetWindowText(::GetFormatText("%0.1f",m_pInlineVotlage->Ua1));
  m_ctlEditUb1.SetWindowText(::GetFormatText("%0.1f",m_pInlineVotlage->Ub1));
  m_ctlEditUc1.SetWindowText(::GetFormatText("%0.1f",m_pInlineVotlage->Uc1));
  m_ctlEditUa2.SetWindowText(::GetFormatText("%0.1f",m_pInlineVotlage->Ua2));
  m_ctlEditUb2.SetWindowText(::GetFormatText("%0.1f",m_pInlineVotlage->Ub2));
  m_ctlEditUc2.SetWindowText(::GetFormatText("%0.1f",m_pInlineVotlage->Uc2));
	
  m_ctl1THiVoltageChangeRate.SetWindowText(::GetFormatText("%0.1f",m_pInlineVotlage->T1HiVoltageChangeRate*100));
  m_ctl2THiVoltageChangeRate.SetWindowText(::GetFormatText("%0.1f",m_pInlineVotlage->T2HiVoltageChangeRate*100));
  m_ctl1TLoVoltageChangeRate.SetWindowText(::GetFormatText("%0.1f",m_pInlineVotlage->T1LoVoltageChangeRate*100));
  m_ctl2TLoVoltageChangeRate.SetWindowText(::GetFormatText("%0.1f",m_pInlineVotlage->T2LoVoltageChangeRate*100));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgInlineVotlage::OnNormalUa1() 
{
  CString filename = g_ProjectFilePath + "simPower.ini";
  char buf[1024];
  ::GetPrivateProfileString("默认值","进线正常电压值","66.4",buf,1024,filename);
  float UValue = (float)atof(buf);
  m_ctlEditUa1.SetWindowText(::GetFormatText("%0.1f",UValue));
}

void CDlgInlineVotlage::OnNormalUb1() 
{
  CString filename = g_ProjectFilePath + "simPower.ini";
  char buf[1024];
  ::GetPrivateProfileString("默认值","进线正常电压值","66.4",buf,1024,filename);
  float UValue = (float)atof(buf);
  m_ctlEditUb1.SetWindowText(::GetFormatText("%0.1f",UValue));
}

void CDlgInlineVotlage::OnNormalUc1() 
{
  CString filename = g_ProjectFilePath + "simPower.ini";
  char buf[1024];
  ::GetPrivateProfileString("默认值","进线正常电压值","66.4",buf,1024,filename);
  float UValue = (float)atof(buf);
  m_ctlEditUc1.SetWindowText(::GetFormatText("%0.1f",UValue));
}

void CDlgInlineVotlage::OnLostUa1() 
{
  CString filename = g_ProjectFilePath + "simPower.ini";
  char buf[1024];
  ::GetPrivateProfileString("默认值","进线失压电压值","00.0",buf,1024,filename);
  float UValue = (float)atof(buf);
  m_ctlEditUa1.SetWindowText(::GetFormatText("%0.1f",UValue));
}

void CDlgInlineVotlage::OnLostUb1() 
{
  CString filename = g_ProjectFilePath + "simPower.ini";
  char buf[1024];
  ::GetPrivateProfileString("默认值","进线失压电压值","00.0",buf,1024,filename);
  float UValue = (float)atof(buf);
  m_ctlEditUb1.SetWindowText(::GetFormatText("%0.1f",UValue));
}

void CDlgInlineVotlage::OnLostUc1() 
{
  CString filename = g_ProjectFilePath + "simPower.ini";
  char buf[1024];
  ::GetPrivateProfileString("默认值","进线失压电压值","00.0",buf,1024,filename);
  float UValue = (float)atof(buf);
  m_ctlEditUc1.SetWindowText(::GetFormatText("%0.1f",UValue));
}

void CDlgInlineVotlage::OnLowUa1() 
{
  CString filename = g_ProjectFilePath + "simPower.ini";
  char buf[1024];
  ::GetPrivateProfileString("默认值","进线欠压电压值","46.8",buf,1024,filename);
  float UValue = (float)atof(buf);
  m_ctlEditUa1.SetWindowText(::GetFormatText("%0.1f",UValue));
}

void CDlgInlineVotlage::OnLowUb1() 
{
  CString filename = g_ProjectFilePath + "simPower.ini";
  char buf[1024];
  ::GetPrivateProfileString("默认值","进线欠压电压值","46.8",buf,1024,filename);
  float UValue = (float)atof(buf);
  m_ctlEditUb1.SetWindowText(::GetFormatText("%0.1f",UValue));
}

void CDlgInlineVotlage::OnLowUc1() 
{
  CString filename = g_ProjectFilePath + "simPower.ini";
  char buf[1024];
  ::GetPrivateProfileString("默认值","进线欠压电压值","46.8",buf,1024,filename);
  float UValue = (float)atof(buf);
  m_ctlEditUc1.SetWindowText(::GetFormatText("%0.1f",UValue));
}

void CDlgInlineVotlage::OnNormalUa2() 
{
  CString filename = g_ProjectFilePath + "simPower.ini";
  char buf[1024];
  ::GetPrivateProfileString("默认值","进线正常电压值","66.4",buf,1024,filename);
  float UValue = (float)atof(buf);
  m_ctlEditUa2.SetWindowText(::GetFormatText("%0.1f",UValue));
}

void CDlgInlineVotlage::OnNormalUb2() 
{
  CString filename = g_ProjectFilePath + "simPower.ini";
  char buf[1024];
  ::GetPrivateProfileString("默认值","进线正常电压值","66.4",buf,1024,filename);
  float UValue = (float)atof(buf);
  m_ctlEditUb2.SetWindowText(::GetFormatText("%0.1f",UValue));
}

void CDlgInlineVotlage::OnNormalUc2() 
{
  CString filename = g_ProjectFilePath + "simPower.ini";
  char buf[1024];
  ::GetPrivateProfileString("默认值","进线正常电压值","66.4",buf,1024,filename);
  float UValue = (float)atof(buf);
  m_ctlEditUc2.SetWindowText(::GetFormatText("%0.1f",UValue));
}

void CDlgInlineVotlage::OnLostUa2() 
{
  CString filename = g_ProjectFilePath + "simPower.ini";
  char buf[1024];
  ::GetPrivateProfileString("默认值","进线失压电压值","00.0",buf,1024,filename);
  float UValue = (float)atof(buf);
  m_ctlEditUa2.SetWindowText(::GetFormatText("%0.1f",UValue));
}

void CDlgInlineVotlage::OnLostUb2() 
{
  CString filename = g_ProjectFilePath + "simPower.ini";
  char buf[1024];
  ::GetPrivateProfileString("默认值","进线失压电压值","00.0",buf,1024,filename);
  float UValue = (float)atof(buf);
  m_ctlEditUb2.SetWindowText(::GetFormatText("%0.1f",UValue));
}

void CDlgInlineVotlage::OnLostUc2() 
{
  CString filename = g_ProjectFilePath + "simPower.ini";
  char buf[1024];
  ::GetPrivateProfileString("默认值","进线失压电压值","00.0",buf,1024,filename);
  float UValue = (float)atof(buf);
  m_ctlEditUc2.SetWindowText(::GetFormatText("%0.1f",UValue));
}

void CDlgInlineVotlage::OnLowUa2() 
{
  CString filename = g_ProjectFilePath + "simPower.ini";
  char buf[1024];
  ::GetPrivateProfileString("默认值","进线欠压电压值","46.8",buf,1024,filename);
  float UValue = (float)atof(buf);
  m_ctlEditUa2.SetWindowText(::GetFormatText("%0.1f",UValue));
}

void CDlgInlineVotlage::OnLowUb2() 
{
  CString filename = g_ProjectFilePath + "simPower.ini";
  char buf[1024];
  ::GetPrivateProfileString("默认值","进线欠压电压值","46.8",buf,1024,filename);
  float UValue = (float)atof(buf);
  m_ctlEditUb2.SetWindowText(::GetFormatText("%0.1f",UValue));
}

void CDlgInlineVotlage::OnLowUc2() 
{
  CString filename = g_ProjectFilePath + "simPower.ini";
  char buf[1024];
  ::GetPrivateProfileString("默认值","进线欠压电压值","46.8",buf,1024,filename);
  float UValue = (float)atof(buf);
  m_ctlEditUc2.SetWindowText(::GetFormatText("%0.1f",UValue));
}

void CDlgInlineVotlage::OnOK() 
{
  CString Ua1;
  CString Ub1;
  CString Uc1;
  CString Ua2;
  CString Ub2;
  CString Uc2;
  m_ctlEditUa1.GetWindowText(Ua1);
  m_ctlEditUb1.GetWindowText(Ub1);
  m_ctlEditUc1.GetWindowText(Uc1);
  m_ctlEditUa2.GetWindowText(Ua2);
  m_ctlEditUb2.GetWindowText(Ub2);
  m_ctlEditUc2.GetWindowText(Uc2);
  m_pInlineVotlage->Ua1 = (float)atof(Ua1);
  m_pInlineVotlage->Ub1 = (float)atof(Ub1);
  m_pInlineVotlage->Uc1 = (float)atof(Uc1);
  m_pInlineVotlage->Ua2 = (float)atof(Ua2);
  m_pInlineVotlage->Ub2 = (float)atof(Ub2);
  m_pInlineVotlage->Uc2 = (float)atof(Uc2);

  CString T1HiVoltageChangeRate;
  CString T2HiVoltageChangeRate;
  CString T1LoVoltageChangeRate;
  CString T2LoVoltageChangeRate;

  m_ctl1THiVoltageChangeRate.GetWindowText(T1HiVoltageChangeRate);
  m_ctl2THiVoltageChangeRate.GetWindowText(T2HiVoltageChangeRate);
  m_ctl1TLoVoltageChangeRate.GetWindowText(T1LoVoltageChangeRate);
  m_ctl2TLoVoltageChangeRate.GetWindowText(T2LoVoltageChangeRate);

  m_pInlineVotlage->T1HiVoltageChangeRate = (float)atof(T1HiVoltageChangeRate)/(float)100;
  m_pInlineVotlage->T2HiVoltageChangeRate = (float)atof(T2HiVoltageChangeRate)/(float)100;
  m_pInlineVotlage->T1LoVoltageChangeRate = (float)atof(T1LoVoltageChangeRate)/(float)100;
  m_pInlineVotlage->T2LoVoltageChangeRate = (float)atof(T2LoVoltageChangeRate)/(float)100;
  m_pInlineVotlage->StartupDelaySecond = 0;
  m_pInlineVotlage->BeginTime = 0;

	CDialog::OnOK();
}
