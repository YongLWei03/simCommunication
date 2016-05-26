// DlgOperateUnit.cpp : implementation file
//

#include "stdafx.h"
#include "DlgOperateUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOperateUnit dialog


CDlgOperateUnit::CDlgOperateUnit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOperateUnit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOperateUnit)
	//}}AFX_DATA_INIT
}


void CDlgOperateUnit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOperateUnit)
	DDX_Control(pDX, IDC_LIST, m_ctlList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOperateUnit, CDialog)
	//{{AFX_MSG_MAP(CDlgOperateUnit)
	ON_BN_CLICKED(IDC_ON, OnOn)
	ON_BN_CLICKED(IDC_OFF, OnOff)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOperateUnit message handlers

BOOL CDlgOperateUnit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_ctlList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
  ::AddListTitle(m_ctlList,"ÐòºÅ",0);
  ::AddListTitle(m_ctlList,"Ãû³Æ",1);
  ::AddListTitle(m_ctlList,"ÊýÖµ",2);
  m_ctlList.SetColumnWidth(0,40);
  m_ctlList.SetColumnWidth(1,120);
  m_ctlList.SetColumnWidth(2,80);

  for(int varno=0;varno<VARCOUNT;varno++)
  {
    ::AddListItem(m_ctlList,varno,0,::GetFormatText("%d",varno));
    ::AddListItem(m_ctlList,varno,1,m_VarList[varno].VarName);
    ::AddListItem(m_ctlList,varno,2,::GetFormatText(m_VarList[varno].TextFormat,m_VarList[varno].Value));
  }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOperateUnit::UpdateList()
{
  for(int varno=0;varno<VARCOUNT;varno++)
  {
    ::SetItemText(m_ctlList,varno,0,::GetFormatText("%d",varno));
    ::SetItemText(m_ctlList,varno,1,m_VarList[varno].VarName);
    ::SetItemText(m_ctlList,varno,2,::GetFormatText(m_VarList[varno].TextFormat,m_VarList[varno].Value));
  }
}

void CDlgOperateUnit::OnOn() 
{
  if(m_ctlList.GetSelectedCount()==0)
    return;

  POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
  int VarID = m_ctlList.GetNextSelectedItem(pos);
	m_VarList[VarID].Value = 1;
  UpdateList();
}

void CDlgOperateUnit::OnOff() 
{
  if(m_ctlList.GetSelectedCount()==0)
    return;

  POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
  int VarID = m_ctlList.GetNextSelectedItem(pos);
	m_VarList[VarID].Value = 0;
  UpdateList();
}

void CDlgOperateUnit::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
  if(m_ctlList.GetSelectedCount()==0)
    return;

  POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
  int VarID = m_ctlList.GetNextSelectedItem(pos);
	if(m_VarList[VarID].Value==0)
	  m_VarList[VarID].Value = 1;
  else
	  m_VarList[VarID].Value = 0;
  UpdateList();
}
