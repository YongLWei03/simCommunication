// DlgParameterOnOff.cpp : implementation file
//

#include "stdafx.h"
#include "DlgParameterOnOff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgParameterOnOff dialog


CDlgParameterOnOff::CDlgParameterOnOff(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgParameterOnOff::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgParameterOnOff)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgParameterOnOff::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgParameterOnOff)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgParameterOnOff, CDialog)
	//{{AFX_MSG_MAP(CDlgParameterOnOff)
	ON_BN_CLICKED(IDC_PARAMETER_ON, OnParameterOn)
	ON_BN_CLICKED(IDC_PARAMETER_OFF, OnParameterOff)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgParameterOnOff message handlers

void CDlgParameterOnOff::OnParameterOn() 
{
  m_ParameterValue = 1;

  CDialog::OnOK();
}

void CDlgParameterOnOff::OnParameterOff() 
{
  m_ParameterValue = 0;

  CDialog::OnOK();
}
