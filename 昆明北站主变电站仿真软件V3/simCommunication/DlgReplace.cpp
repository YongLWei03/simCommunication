// DlgReplace.cpp : implementation file
//

#include "stdafx.h"
#include "simcommunication.h"
#include "DlgReplace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgReplace dialog


CDlgReplace::CDlgReplace(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgReplace::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgReplace)
	m_Old = _T("");
	m_New = _T("");
	//}}AFX_DATA_INIT
}


void CDlgReplace::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgReplace)
	DDX_CBString(pDX, IDC_EDIT_OLD, m_Old);
	DDX_CBString(pDX, IDC_EDIT_NEW, m_New);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgReplace, CDialog)
	//{{AFX_MSG_MAP(CDlgReplace)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgReplace message handlers
