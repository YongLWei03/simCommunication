// DlgFaultTransmission.cpp : implementation file
//

#include "stdafx.h"
#include "simcommunication.h"
#include "DlgFaultTransmission.h"
#include "DlgListSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFaultTransmission dialog

CDlgFaultTransmission::CDlgFaultTransmission(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFaultTransmission::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFaultTransmission)
	m_FileName = _T("");
	m_StartupDelaySecond = 0;
	//}}AFX_DATA_INIT
}


void CDlgFaultTransmission::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFaultTransmission)
	DDX_Control(pDX, IDC_FILENAME, m_ctlFileName);
	DDX_Text(pDX, IDC_FILENAME, m_FileName);
	DDX_Text(pDX, IDC_EDIT_STARTUP_DELAY_SECOND, m_StartupDelaySecond);
	DDV_MinMaxUInt(pDX, m_StartupDelaySecond, 0, 1000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFaultTransmission, CDialog)
	//{{AFX_MSG_MAP(CDlgFaultTransmission)
	ON_BN_CLICKED(IDC_FILE_SELECT, OnFileSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFaultTransmission message handlers

BOOL CDlgFaultTransmission::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFaultTransmission::OnFileSelect() 
{
  CString FilePath = g_ProjectFilePath+"FaultData\\";
  CString FileName = FilePath+"*.txt";

  CDlgListSelect dlg;
  dlg.m_TextTitle = "故障仿真数据文件选择";

  ::GetFileList(dlg.m_TextList,FileName);
  ::StringListSortAsc(dlg.m_TextList);

  if(dlg.m_TextList.GetCount()==0)
  {
    ::AfxMessageBox("没有找到任何故障仿真数据文件");
    return ;
  }

  if(dlg.DoModal()==IDCANCEL)
    return;

  m_ctlFileName.SetWindowText(dlg.m_TextSelected);
}

void CDlgFaultTransmission::OnOK() 
{
  m_ctlFileName.GetWindowText(m_FileName);
	
  if(m_FileName=="")
    return;
	
  CDialog::OnOK();
}
