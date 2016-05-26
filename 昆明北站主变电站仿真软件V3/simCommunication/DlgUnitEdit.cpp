// DlgUnitEdit.cpp : implementation file
//

#include "stdafx.h"
#include "DlgUnitEdit.h"
#include "DlgEditVar.h"
#include "DlgReplace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TagUnitStruct g_UnitCopy;

/////////////////////////////////////////////////////////////////////////////
// CDlgUnitEdit dialog


CDlgUnitEdit::CDlgUnitEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgUnitEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgUnitEdit)
	m_UnitName = _T("");
	m_UnitType = -1;
	m_UnitPT = 0.0f;
	m_UnitCT = 0.0f;
	//}}AFX_DATA_INIT
}


void CDlgUnitEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgUnitEdit)
	DDX_Control(pDX, IDC_UNIT_CT, m_ctlUnitCT);
	DDX_Control(pDX, IDC_UNIT_PT, m_ctlUnitPT);
	DDX_Control(pDX, IDC_UNIT_NAME, m_ctlUnitName);
	DDX_Control(pDX, IDC_UNIT_TYPE, m_ctlUnitType);
	DDX_Text(pDX, IDC_UNIT_NAME, m_UnitName);
	DDX_CBIndex(pDX, IDC_UNIT_TYPE, m_UnitType);
	DDX_Text(pDX, IDC_UNIT_PT, m_UnitPT);
	DDX_Text(pDX, IDC_UNIT_CT, m_UnitCT);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgUnitEdit, CDialog)
	//{{AFX_MSG_MAP(CDlgUnitEdit)
	ON_BN_CLICKED(IDC_EDIT_YC, OnEditYc)
	ON_BN_CLICKED(IDC_EDIT_YX, OnEditYx)
	ON_BN_CLICKED(IDC_EDIT_DD, OnEditDd)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	ON_BN_CLICKED(IDC_REPLASE, OnReplase)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgUnitEdit message handlers

BOOL CDlgUnitEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  ::AddStringToList(m_ctlUnitType,defUnitType);

	m_ctlUnitType.SetCurSel(m_UnitType);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgUnitEdit::OnOK() 
{
	m_UnitType = m_ctlUnitType.GetCurSel();
	
	CDialog::OnOK();
}

void CDlgUnitEdit::OnEditYc() 
{
  CDlgEditVar dlg;
  for(int varno=0;varno<VARCOUNT;varno++)
  {
    dlg.m_VarName[varno] = m_VarName[0][varno];
    dlg.m_TextFormat[varno] = m_TextFormat[0][varno];
  }
  if(dlg.DoModal()==IDOK)
  {
    for(int varno=0;varno<VARCOUNT;varno++)
    {
      m_VarName[0][varno] = dlg.m_VarName[varno];
      m_TextFormat[0][varno] = dlg.m_TextFormat[varno];
    }
  }
}

void CDlgUnitEdit::OnEditYx() 
{
  CDlgEditVar dlg;
  for(int varno=0;varno<VARCOUNT;varno++)
  {
    dlg.m_VarName[varno] = m_VarName[1][varno];
    dlg.m_TextFormat[varno] = m_TextFormat[1][varno];
  }
  if(dlg.DoModal()==IDOK)
  {
    for(int varno=0;varno<VARCOUNT;varno++)
    {
      m_VarName[1][varno] = dlg.m_VarName[varno];
      m_TextFormat[1][varno] = dlg.m_TextFormat[varno];
    }
  }
}

void CDlgUnitEdit::OnEditDd() 
{
  CDlgEditVar dlg;
  for(int varno=0;varno<VARCOUNT;varno++)
  {
    dlg.m_VarName[varno] = m_VarName[2][varno];
    dlg.m_TextFormat[varno] = m_TextFormat[2][varno];
  }
  if(dlg.DoModal()==IDOK)
  {
    for(int varno=0;varno<VARCOUNT;varno++)
    {
      m_VarName[2][varno] = dlg.m_VarName[varno];
      m_TextFormat[2][varno] = dlg.m_TextFormat[varno];
    }
  }
}

void CDlgUnitEdit::OnCopy() 
{
  CString UnitName;
  CString UnitPT;
  CString UnitCT;
  m_ctlUnitName.GetWindowText(UnitName);
  m_ctlUnitPT.GetWindowText(UnitPT);
  m_ctlUnitCT.GetWindowText(UnitCT);
  g_UnitCopy.UnitName = UnitName;
  g_UnitCopy.UnitPT = (float)atof(UnitPT);
  g_UnitCopy.UnitCT = (float)atof(UnitCT);
  g_UnitCopy.UnitType = m_ctlUnitType.GetCurSel();
  for(int vargroupno=0;vargroupno<VARGROUPCOUNT;vargroupno++)
  {
    for(int varno=0;varno<VARCOUNT;varno++)
    {
      g_UnitCopy.VarGroup[vargroupno].Var[varno].VarName = m_VarName[vargroupno][varno];
      g_UnitCopy.VarGroup[vargroupno].Var[varno].TextFormat = m_TextFormat[vargroupno][varno];
    }
  }	
}

void CDlgUnitEdit::OnPaste() 
{
  for(int vargroupno=0;vargroupno<VARGROUPCOUNT;vargroupno++)
  {
    for(int varno=0;varno<VARCOUNT;varno++)
    {
      m_VarName[vargroupno][varno] = g_UnitCopy.VarGroup[vargroupno].Var[varno].VarName;
      m_TextFormat[vargroupno][varno] = g_UnitCopy.VarGroup[vargroupno].Var[varno].TextFormat;
    }
  }	
  m_ctlUnitName.SetWindowText(g_UnitCopy.UnitName);
  m_ctlUnitPT.SetWindowText(::ftostr(g_UnitCopy.UnitPT));
  m_ctlUnitCT.SetWindowText(::ftostr(g_UnitCopy.UnitCT));
  m_ctlUnitType.SetCurSel(g_UnitCopy.UnitType);
}

void CDlgUnitEdit::OnReplase() 
{
  CString UnitName;
  m_ctlUnitName.GetWindowText(UnitName);
  CDlgReplace dlg;
  dlg.m_Old = UnitName;
	if(dlg.DoModal()==IDOK)
  {
    UnitName.Replace(dlg.m_Old,dlg.m_New);
    m_ctlUnitName.SetWindowText(UnitName);
    for(int vargroupno=0;vargroupno<VARGROUPCOUNT;vargroupno++)
    {
      for(int varno=0;varno<VARCOUNT;varno++)
      {
        m_VarName[vargroupno][varno].Replace(dlg.m_Old,dlg.m_New);
        m_TextFormat[vargroupno][varno].Replace(dlg.m_Old,dlg.m_New);
      }
    }	
  }
}
