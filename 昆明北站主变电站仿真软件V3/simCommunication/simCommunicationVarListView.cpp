// simCommunicationListView.cpp : implementation file
//

#include "stdafx.h"
#include "simCommunication.h"
#include "MainFrm.h"
#include "simCommunicationSOEListView.h"
#include "simCommunicationVarListView.h"
#include "simCommunicationUnitListView.h"
#include "DlgPowerThumbnail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CsimCommunicationVarListView

IMPLEMENT_DYNCREATE(CsimCommunicationVarListView, CListView)

CsimCommunicationVarListView::CsimCommunicationVarListView()
{
}

CsimCommunicationVarListView::~CsimCommunicationVarListView()
{
}


BEGIN_MESSAGE_MAP(CsimCommunicationVarListView, CListView)
	//{{AFX_MSG_MAP(CsimCommunicationVarListView)
	ON_WM_CREATE()
	ON_COMMAND(ID_OPERATER_YX_ON, OnOperaterYxOn)
	ON_UPDATE_COMMAND_UI(ID_OPERATER_YX_ON, OnUpdateOperaterYxOn)
	ON_COMMAND(ID_OPERATER_YX_OFF, OnOperaterYxOff)
	ON_UPDATE_COMMAND_UI(ID_OPERATER_YX_OFF, OnUpdateOperaterYxOff)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_COMMAND(ID_OPERATER_YX_LOCK, OnOperaterYxLock)
	ON_COMMAND(ID_OPERATER_YX_UNLOCK, OnOperaterYxUnlock)
	ON_UPDATE_COMMAND_UI(ID_OPERATER_YX_LOCK, OnUpdateOperaterYxLock)
	ON_UPDATE_COMMAND_UI(ID_OPERATER_YX_UNLOCK, OnUpdateOperaterYxUnlock)
	ON_COMMAND(ID_OPERATER_YX_ABNORMAL, OnOperaterYxAbnormal)
	ON_UPDATE_COMMAND_UI(ID_OPERATER_YX_ABNORMAL, OnUpdateOperaterYxAbnormal)
	ON_COMMAND(ID_OPERATER_YX_NORMAL, OnOperaterYxNormal)
	ON_UPDATE_COMMAND_UI(ID_OPERATER_YX_NORMAL, OnUpdateOperaterYxNormal)
	ON_COMMAND(ID_OPERATER_YX_THUMBNAIL, OnOperaterYxThumbnail)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CsimCommunicationVarListView drawing

void CsimCommunicationVarListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CsimCommunicationVarListView diagnostics

#ifdef _DEBUG
void CsimCommunicationVarListView::AssertValid() const
{
	CListView::AssertValid();
}

void CsimCommunicationVarListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CsimCommunicationVarListView message handlers

int CsimCommunicationVarListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	lpCreateStruct->style |= LVS_SHOWSELALWAYS ;
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle(LVS_TYPEMASK, LVS_REPORT|LVS_SHOWSELALWAYS);

  CListCtrl &ctlList=GetListCtrl();
  ctlList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
  ::AddListTitle(ctlList,"序号",0);
  ::AddListTitle(ctlList,"遥侧",1);
  ::AddListTitle(ctlList,"数值",2);
  ctlList.SetColumnWidth(0,40);
  ctlList.SetColumnWidth(1,120);
  ctlList.SetColumnWidth(2,80);

  ::AddListTitle(ctlList,"序号",3);
  ::AddListTitle(ctlList,"遥信",4);
  ::AddListTitle(ctlList,"数值",5);
  ctlList.SetColumnWidth(3,40);
  ctlList.SetColumnWidth(4,168);
  ctlList.SetColumnWidth(5,80);

  ::AddListTitle(ctlList,"序号",6);
  ::AddListTitle(ctlList,"电度",7);
  ::AddListTitle(ctlList,"数值",8);
  ctlList.SetColumnWidth(6,40);
  ctlList.SetColumnWidth(7,120);
  ctlList.SetColumnWidth(8,80);

  MakeListView();

  return 0;
}

void CsimCommunicationVarListView::MakeListView()
{
  CListCtrl &ctlList = GetListCtrl();
  ctlList.DeleteAllItems();

  for(int varno=0;varno<VARCOUNT;varno++)
    ::AddListItem(ctlList,varno,0,"");
}

void CsimCommunicationVarListView::UpdateListView()
{
  CListCtrl &ctlList=GetListCtrl();
  CListCtrl &ctlUnitList = m_pUnitListView->GetListCtrl();
  
  if(ctlUnitList.GetSelectedCount()>0)
  {
    POSITION pos = ctlUnitList.GetFirstSelectedItemPosition();
    int UnitID = ctlUnitList.GetNextSelectedItem(pos)+1;

    {
      TagVarGroupStruct *pVarGroup = &g_Unit[UnitID].VarGroup[0];
      for(int varno=0;varno<VARCOUNT;varno++)
      {
        ::SetItemText(ctlList,varno,0,::GetFormatText("%d",varno));
        ::SetItemText(ctlList,varno,1,pVarGroup->Var[varno].VarName);
        ::SetItemText(ctlList,varno,2,::GetFormatText(::GetDataFormat(pVarGroup->Var[varno].TextFormat),pVarGroup->Var[varno].Value));
      }
    }
    {
      TagVarGroupStruct *pVarGroup = &g_Unit[UnitID].VarGroup[1];
      for(int varno=0;varno<VARCOUNT;varno++)
      {
        ::SetItemText(ctlList,varno,3,::GetFormatText("%d",varno));
        CString VarName = pVarGroup->Var[varno].VarName;
        if(g_Unit[pVarGroup->UnitID].VarLock[varno]==TRUE)
          VarName += "(拒动)";
        if(g_Unit[pVarGroup->UnitID].VarAbnormal[varno]==TRUE)
          VarName += "(异常)";
        ::SetItemText(ctlList,varno,4,VarName);
        ::SetItemText(ctlList,varno,5,::GetFormatText(::GetDataFormat(pVarGroup->Var[varno].TextFormat),pVarGroup->Var[varno].Value));
      }
    }
    {
      TagVarGroupStruct *pVarGroup = &g_Unit[UnitID].VarGroup[2];
      for(int varno=0;varno<VARCOUNT;varno++)
      {
        ::SetItemText(ctlList,varno,6,::GetFormatText("%d",varno));
        ::SetItemText(ctlList,varno,7,pVarGroup->Var[varno].VarName);
        ::SetItemText(ctlList,varno,8,::GetFormatText(::GetDataFormat(pVarGroup->Var[varno].TextFormat),pVarGroup->Var[varno].Value));
      }
    }
  }
  else
  {
    for(int varno=0;varno<VARCOUNT;varno++)
    {
      ::SetItemText(ctlList,varno,0,"");
      ::SetItemText(ctlList,varno,1,"");
      ::SetItemText(ctlList,varno,2,"");
      ::SetItemText(ctlList,varno,3,"");
      ::SetItemText(ctlList,varno,4,"");
      ::SetItemText(ctlList,varno,5,"");
      ::SetItemText(ctlList,varno,6,"");
      ::SetItemText(ctlList,varno,7,"");
      ::SetItemText(ctlList,varno,8,"");
      ::SetItemText(ctlList,varno,9,"");
    }
  }
}

void CsimCommunicationVarListView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
  CPoint point;
  ::GetCursorPos(&point);

  CMenu popmenu;
  popmenu.LoadMenu(MAKEINTRESOURCE(IDR_POPUP_OPERATER_YX));
  if(popmenu)
  {
    CMenu *psubmenu;
    psubmenu=popmenu.GetSubMenu(0);
    psubmenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,point.x,point.y,this);
  }
	
	*pResult = 0;
}

void CsimCommunicationVarListView::OnOperaterYxOn() 
{
  CListCtrl &ctlUnitList = m_pUnitListView->GetListCtrl();
  CListCtrl &ctlVarList = GetListCtrl();

  POSITION pos = ctlUnitList.GetFirstSelectedItemPosition();
  WORD UnitID = ctlUnitList.GetNextSelectedItem(pos)+1;

  pos = ctlVarList.GetFirstSelectedItemPosition();
  while(pos)
  {
    WORD VarID = ctlVarList.GetNextSelectedItem(pos);

//    if(g_Unit[UnitID].VarLock[VarID]==TRUE)
//      continue;

    g_Unit[UnitID].VarGroup[1].Var[VarID].Value = 1;
  }	

  if(g_SimCommunicationMain==FALSE)
    ::WriteUnitToShareMemory();
}

void CsimCommunicationVarListView::OnUpdateOperaterYxOn(CCmdUI* pCmdUI) 
{
  CListCtrl &ctlUnitList=m_pUnitListView->GetListCtrl();
  CListCtrl &ctlVarList = GetListCtrl();

  if(ctlUnitList.GetSelectedCount()!=1)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

  if(ctlVarList.GetSelectedCount()==0)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

  pCmdUI->Enable(TRUE);
}

void CsimCommunicationVarListView::OnOperaterYxOff() 
{
  CListCtrl &ctlUnitList = m_pUnitListView->GetListCtrl();
  CListCtrl &ctlVarList = GetListCtrl();

  POSITION pos = ctlUnitList.GetFirstSelectedItemPosition();
  WORD UnitID = ctlUnitList.GetNextSelectedItem(pos)+1;

  pos = ctlVarList.GetFirstSelectedItemPosition();
  while(pos)
  {
    WORD VarID = ctlVarList.GetNextSelectedItem(pos);

//    if(g_Unit[UnitID].VarLock[VarID]==TRUE)
 //     continue;

    g_Unit[UnitID].VarGroup[1].Var[VarID].Value = 0;
  }	

  if(g_SimCommunicationMain==FALSE)
    ::WriteUnitToShareMemory();
}

void CsimCommunicationVarListView::OnUpdateOperaterYxOff(CCmdUI* pCmdUI) 
{
  CListCtrl &ctlUnitList=m_pUnitListView->GetListCtrl();
  CListCtrl &ctlVarList = GetListCtrl();

  if(ctlUnitList.GetSelectedCount()!=1)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

  if(ctlVarList.GetSelectedCount()==0)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

  pCmdUI->Enable(TRUE);
}


void CsimCommunicationVarListView::OnOperaterYxLock() 
{
  CListCtrl &ctlUnitList = m_pUnitListView->GetListCtrl();
  CListCtrl &ctlVarList = GetListCtrl();

  POSITION pos = ctlUnitList.GetFirstSelectedItemPosition();
  WORD UnitID = ctlUnitList.GetNextSelectedItem(pos)+1;

  pos = ctlVarList.GetFirstSelectedItemPosition();
  while(pos)
  {
    WORD VarID = ctlVarList.GetNextSelectedItem(pos);
    g_Unit[UnitID].VarLock[VarID] = TRUE;
  }	
}

void CsimCommunicationVarListView::OnOperaterYxUnlock() 
{
  CListCtrl &ctlUnitList = m_pUnitListView->GetListCtrl();
  CListCtrl &ctlVarList = GetListCtrl();

  POSITION pos = ctlUnitList.GetFirstSelectedItemPosition();
  WORD UnitID = ctlUnitList.GetNextSelectedItem(pos)+1;

  pos = ctlVarList.GetFirstSelectedItemPosition();
  while(pos)
  {
    WORD VarID = ctlVarList.GetNextSelectedItem(pos);
    g_Unit[UnitID].VarLock[VarID] = FALSE;
  }	
}

void CsimCommunicationVarListView::OnUpdateOperaterYxLock(CCmdUI* pCmdUI) 
{
  CListCtrl &ctlUnitList=m_pUnitListView->GetListCtrl();
  CListCtrl &ctlVarList = GetListCtrl();

  if(ctlUnitList.GetSelectedCount()!=1)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

  if(ctlVarList.GetSelectedCount()==0)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

  pCmdUI->Enable(TRUE);
}

void CsimCommunicationVarListView::OnUpdateOperaterYxUnlock(CCmdUI* pCmdUI) 
{
  CListCtrl &ctlUnitList=m_pUnitListView->GetListCtrl();
  CListCtrl &ctlVarList = GetListCtrl();

  if(ctlUnitList.GetSelectedCount()!=1)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

  if(ctlVarList.GetSelectedCount()==0)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

  pCmdUI->Enable(TRUE);
}

void CsimCommunicationVarListView::OnOperaterYxAbnormal() 
{
  CListCtrl &ctlUnitList = m_pUnitListView->GetListCtrl();
  CListCtrl &ctlVarList = GetListCtrl();

  POSITION pos = ctlUnitList.GetFirstSelectedItemPosition();
  WORD UnitID = ctlUnitList.GetNextSelectedItem(pos)+1;

  pos = ctlVarList.GetFirstSelectedItemPosition();
  while(pos)
  {
    WORD VarID = ctlVarList.GetNextSelectedItem(pos);
    g_Unit[UnitID].VarAbnormal[VarID] = TRUE;
    g_Unit[UnitID].VarGroup[1].Var[VarID].LastValue = !g_Unit[UnitID].VarGroup[1].Var[VarID].Value;
  }	
}

void CsimCommunicationVarListView::OnOperaterYxNormal() 
{
  CListCtrl &ctlUnitList = m_pUnitListView->GetListCtrl();
  CListCtrl &ctlVarList = GetListCtrl();

  POSITION pos = ctlUnitList.GetFirstSelectedItemPosition();
  WORD UnitID = ctlUnitList.GetNextSelectedItem(pos)+1;

  pos = ctlVarList.GetFirstSelectedItemPosition();
  while(pos)
  {
    WORD VarID = ctlVarList.GetNextSelectedItem(pos);
    g_Unit[UnitID].VarAbnormal[VarID] = FALSE;
    g_Unit[UnitID].VarGroup[1].Var[VarID].LastValue = !g_Unit[UnitID].VarGroup[1].Var[VarID].Value;
  }	
}

void CsimCommunicationVarListView::OnUpdateOperaterYxAbnormal(CCmdUI* pCmdUI) 
{
  CListCtrl &ctlUnitList=m_pUnitListView->GetListCtrl();
  CListCtrl &ctlVarList = GetListCtrl();

  if(ctlUnitList.GetSelectedCount()!=1)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

  if(ctlVarList.GetSelectedCount()==0)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

  pCmdUI->Enable(TRUE);
}

void CsimCommunicationVarListView::OnUpdateOperaterYxNormal(CCmdUI* pCmdUI) 
{
  CListCtrl &ctlUnitList=m_pUnitListView->GetListCtrl();
  CListCtrl &ctlVarList = GetListCtrl();

  if(ctlUnitList.GetSelectedCount()!=1)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

  if(ctlVarList.GetSelectedCount()==0)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

  pCmdUI->Enable(TRUE);
}

void CsimCommunicationVarListView::OnOperaterYxThumbnail() 
{
  CDlgPowerThumbnail dlg(this);
  dlg.m_PowerState = TRUE;
  for(int unitno=0;unitno<UNITCOUNT;unitno++)
  {
    for(int varno=0;varno<VARCOUNT;varno++)
      dlg.m_PowerStateList[unitno][varno] = g_Unit[unitno].VarGroup[1].Var[varno].Value;
  }
  if(dlg.DoModal()==IDOK)
  {
    for(int unitno=0;unitno<UNITCOUNT;unitno++)
    {
      for(int varno=0;varno<VARCOUNT;varno++)
        g_Unit[unitno].VarGroup[1].Var[varno].Value = dlg.m_PowerStateList[unitno][varno];
    }

    if(g_SimCommunicationMain==FALSE)
      ::WriteUnitToShareMemory();
  }
}
