// DlgOperateParameter.cpp : implementation file
//

#include "stdafx.h"
#include "DlgOperateParameter.h"
#include "DLgParameterInput.h"
#include "DlgParameterOnOff.h"
#include "DlgMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static float g_ParameterGroup[PARAMETERCOUNT];

/////////////////////////////////////////////////////////////////////////////
// CDlgOperateParameter dialog


CDlgOperateParameter::CDlgOperateParameter(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOperateParameter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOperateParameter)
	//}}AFX_DATA_INIT
}


void CDlgOperateParameter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOperateParameter)
	DDX_Control(pDX, IDC_GROUP_ID, m_ctlGroupID);
	DDX_Control(pDX, IDC_LIST, m_ctlList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOperateParameter, CDialog)
	//{{AFX_MSG_MAP(CDlgOperateParameter)
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_BN_CLICKED(IDC_CALL, OnCall)
	ON_BN_CLICKED(IDC_SEND, OnSend)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_GROUP_0, OnGroup0)
	ON_BN_CLICKED(IDC_GROUP_1, OnGroup1)
	ON_BN_CLICKED(IDC_GROUP_2, OnGroup2)
	ON_BN_CLICKED(IDC_GROUP_3, OnGroup3)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOperateParameter message handlers

BOOL CDlgOperateParameter::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_ctlList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
  ::AddListTitle(m_ctlList,"序号",0);
  ::AddListTitle(m_ctlList,"名称",1);
  ::AddListTitle(m_ctlList,"数值",2);
  m_ctlList.SetColumnWidth(0,40);
  m_ctlList.SetColumnWidth(1,160);
  m_ctlList.SetColumnWidth(2,80);

  WORD GroupID = g_Unit[m_UnitID].CurrentParamaterGroupID;
  for(int varno=0;varno<PARAMETERCOUNT;varno++)
  {
    ::AddListItem(m_ctlList,varno,0,::GetFormatText("%d",varno));
    ::AddListItem(m_ctlList,varno,1,g_Unit[m_UnitID].ParameterGroup[GroupID].Var[varno].VarName);
    ::AddListItem(m_ctlList,varno,2,::GetFormatText(::GetDataFormat(g_Unit[m_UnitID].ParameterGroup[GroupID].Var[varno].TextFormat),g_Unit[m_UnitID].ParameterGroup[GroupID].Var[varno].Value));
  }
	
	m_ctlGroupID.SetWindowText(::GetFormatText("%d",GroupID));

  SetTimer(500,500,NULL);

  return TRUE;
}

void CDlgOperateParameter::OnTimer(UINT nIDEvent) 
{
  WORD GroupID = g_Unit[m_UnitID].CurrentParamaterGroupID;
  for(int varno=0;varno<PARAMETERCOUNT;varno++)
  {
    ::SetItemText(m_ctlList,varno,0,::GetFormatText("%d",varno));
    ::SetItemText(m_ctlList,varno,1,g_Unit[m_UnitID].ParameterGroup[GroupID].Var[varno].VarName);
    ::SetItemText(m_ctlList,varno,2,::GetFormatText(::GetDataFormat(g_Unit[m_UnitID].ParameterGroup[GroupID].Var[varno].TextFormat),g_Unit[m_UnitID].ParameterGroup[GroupID].Var[varno].Value));
  }
	
	m_ctlGroupID.SetWindowText(::GetFormatText("%d",GroupID));

	CDialog::OnTimer(nIDEvent);
}

void CDlgOperateParameter::OnSet() 
{
  if(m_ctlList.GetSelectedCount()!=1)
    return;

  POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
  int VarID = m_ctlList.GetNextSelectedItem(pos);
  WORD GroupID = g_Unit[m_UnitID].CurrentParamaterGroupID;

  if(VarID<32)//压板
  {
    CDlgParameterOnOff dlg;
    dlg.DoModal();
    g_Unit[m_UnitID].ParameterGroup[GroupID].Var[VarID].Value = dlg.m_ParameterValue;
  }
  else//定值
  {
    CDLgParameterInput dlg;
    dlg.m_ParameterValue = g_Unit[m_UnitID].ParameterGroup[GroupID].Var[VarID].Value;
    if(dlg.DoModal()==IDOK)
      g_Unit[m_UnitID].ParameterGroup[GroupID].Var[VarID].Value = dlg.m_ParameterValue;
  }

  ::WriteUnitToShareMemory();
}

void CDlgOperateParameter::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
  OnSet();
	
	*pResult = 0;
}

void CDlgOperateParameter::OnCall() 
{
}

void CDlgOperateParameter::OnSend() 
{
}

void CDlgOperateParameter::OnGroup0() 
{
  g_Unit[m_UnitID].CurrentParamaterGroupID = 0;

  ::WriteUnitToShareMemory();
}

void CDlgOperateParameter::OnGroup1() 
{
  g_Unit[m_UnitID].CurrentParamaterGroupID = 1;

  ::WriteUnitToShareMemory();
}

void CDlgOperateParameter::OnGroup2() 
{
  g_Unit[m_UnitID].CurrentParamaterGroupID = 2;

  ::WriteUnitToShareMemory();
}

void CDlgOperateParameter::OnGroup3() 
{
  g_Unit[m_UnitID].CurrentParamaterGroupID = 3;

  ::WriteUnitToShareMemory();
}

void CDlgOperateParameter::OnCopy() 
{
  WORD GroupID = g_Unit[m_UnitID].CurrentParamaterGroupID;
  for(int varno=0;varno<PARAMETERCOUNT;varno++)
    g_ParameterGroup[varno] = g_Unit[m_UnitID].ParameterGroup[GroupID].Var[varno].Value;
}

void CDlgOperateParameter::OnPaste() 
{
  WORD GroupID = g_Unit[m_UnitID].CurrentParamaterGroupID;
  for(int varno=0;varno<PARAMETERCOUNT;varno++)
    g_Unit[m_UnitID].ParameterGroup[GroupID].Var[varno].Value = g_ParameterGroup[varno];
}
