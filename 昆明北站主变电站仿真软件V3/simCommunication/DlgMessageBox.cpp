// DlgMessageBox.cpp : implementation file
//

#include "stdafx.h"
#include "DlgMessageBox.h"

CDlgMessageBox *g_pDlgMessageBox;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMessageBox dialog


CDlgMessageBox::CDlgMessageBox(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMessageBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMessageBox)
	m_Title = _T("");
	//}}AFX_DATA_INIT
}


void CDlgMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMessageBox)
	DDX_Control(pDX, IDC_PROGRESS, m_ctlProgress);
	DDX_Control(pDX, IDC_TITLE, m_ctlTitle);
	DDX_Text(pDX, IDC_TITLE, m_Title);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMessageBox, CDialog)
	//{{AFX_MSG_MAP(CDlgMessageBox)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMessageBox message handlers

BOOL CDlgMessageBox::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetTimer(250,250,NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMessageBox::DoShow(CString Title,int DelaySecond)
{
  m_ctlTitle.SetWindowText(Title);
  m_ctlProgress.SetRange(0,DelaySecond*4);
  m_ctlProgress.SetPos(0);
  ShowWindow(SW_SHOW);
  SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
}

void CDlgMessageBox::OnCancel() 
{
  ShowWindow(SW_HIDE);
}

void CDlgMessageBox::OnTimer(UINT nIDEvent) 
{
  if(IsWindowVisible())
  {
    int low,up;
    m_ctlProgress.GetRange(low,up);
    int second = m_ctlProgress.GetPos();
    if(second>=up)
	    OnCancel();
    else
  	  m_ctlProgress.SetPos(second+1);
  }

	CDialog::OnTimer(nIDEvent);
}
