// DLgParameterInput.cpp : implementation file
//

#include "stdafx.h"
#include "DLgParameterInput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLgParameterInput dialog


CDLgParameterInput::CDLgParameterInput(CWnd* pParent /*=NULL*/)
	: CDialog(CDLgParameterInput::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDLgParameterInput)
	m_ParameterValue = 0.0f;
	//}}AFX_DATA_INIT
}


void CDLgParameterInput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLgParameterInput)
	DDX_Text(pDX, IDC_PARAMETER_VALUE, m_ParameterValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLgParameterInput, CDialog)
	//{{AFX_MSG_MAP(CDLgParameterInput)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLgParameterInput message handlers
