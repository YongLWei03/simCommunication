// simCommunicationListView.cpp : implementation file
//

#include "stdafx.h"
#include "simCommunication.h"
#include "MainFrm.h"
#include "simCommunicationSOEListView.h"
#include "simCommunicationVarListView.h"
#include "simCommunicationUnitListView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CsimCommunicationUnitListView

IMPLEMENT_DYNCREATE(CsimCommunicationUnitListView, CListView)

CsimCommunicationUnitListView::CsimCommunicationUnitListView()
{
}

CsimCommunicationUnitListView::~CsimCommunicationUnitListView()
{
}


BEGIN_MESSAGE_MAP(CsimCommunicationUnitListView, CListView)
	//{{AFX_MSG_MAP(CsimCommunicationUnitListView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_COMMAND(ID_UNIT_CLEAR, OnUnitClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CsimCommunicationUnitListView drawing

void CsimCommunicationUnitListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CsimCommunicationUnitListView diagnostics

#ifdef _DEBUG
void CsimCommunicationUnitListView::AssertValid() const
{
	CListView::AssertValid();
}

void CsimCommunicationUnitListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CsimCommunicationUnitListView message handlers

int CsimCommunicationUnitListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	lpCreateStruct->style |= LVS_SHOWSELALWAYS ;
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle(LVS_TYPEMASK, LVS_REPORT|LVS_SHOWSELALWAYS);

  CListCtrl &ctlList=GetListCtrl();
  ctlList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

  ::AddListTitle(ctlList,"序号",0);
  ::AddListTitle(ctlList,"装置类型",1);
  ::AddListTitle(ctlList,"装置名称",2);
  ::AddListTitle(ctlList,"CT",3);
  ::AddListTitle(ctlList,"PT",4);
  ctlList.SetColumnWidth(1,120);
  ctlList.SetColumnWidth(2,120);
  ctlList.SetColumnWidth(3,48);
  ctlList.SetColumnWidth(4,48);

	m_ctlImage16.Create(16,16,ILC_COLOR4,10,1);
	m_ctlImage16.Add( AfxGetApp() -> LoadIcon(IDI_UNITDISABLE));
	m_ctlImage16.Add( AfxGetApp() -> LoadIcon(IDI_UNIT));
	m_ctlImage16.Add( AfxGetApp() -> LoadIcon(IDI_UNITFAULT));
  ctlList.SetImageList( &m_ctlImage16,LVSIL_SMALL);

  return 0;
}

void CsimCommunicationUnitListView::MakeListView()
{
  CListCtrl &ctlList=GetListCtrl();
  ctlList.DeleteAllItems();

  for(int unitno=1;unitno<UNITCOUNT;unitno++)
  {
    ::AddListItem(ctlList,unitno-1,0,::GetFormatText("%d",unitno));
    ::AddListItem(ctlList,unitno-1,1,defUnitType[g_Unit[unitno].UnitType]);
    ::AddListItem(ctlList,unitno-1,2,g_Unit[unitno].UnitName);
  }
  ::SelectListFirstItem(ctlList);

  m_pSOEListView->MakeListView();
}

void CsimCommunicationUnitListView::UpdateListView()
{
  CListCtrl &ctlList=GetListCtrl();
  
  for(int unitno=1;unitno<UNITCOUNT;unitno++)
  {
    WORD ImageState;
    if(g_Unit[unitno].UnitType == 0)
      ImageState = 0;
    else if(g_Unit[unitno].UnitState == FALSE)
      ImageState = 1;
    else
      ImageState = 2;

    ::SetItemImage(ctlList,unitno-1,ImageState);
    ::SetItemText(ctlList,unitno-1,0,::GetFormatText("%d",unitno));
    ::SetItemText(ctlList,unitno-1,1,defUnitType[g_Unit[unitno].UnitType]);
    ::SetItemText(ctlList,unitno-1,2,g_Unit[unitno].UnitName);
    ::SetItemText(ctlList,unitno-1,3,::ftostr(g_Unit[unitno].UnitCT));
    ::SetItemText(ctlList,unitno-1,4,::ftostr(g_Unit[unitno].UnitPT));
  }
}


void CsimCommunicationUnitListView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
  g_pMainFrame->SendMessage(WM_COMMAND,ID_EDIT_UNIT,NULL);
	
	*pResult = 0;
}

void CsimCommunicationUnitListView::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
  m_pSOEListView->MakeListView();
	
	*pResult = 0;
}

void CsimCommunicationUnitListView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
  CPoint point;
  ::GetCursorPos(&point);

  CMenu popmenu;
  popmenu.LoadMenu(MAKEINTRESOURCE(IDR_POPUP_MAINFRAME));
  if(popmenu)
  {
    CMenu *psubmenu;
    psubmenu=popmenu.GetSubMenu(0);
    psubmenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,point.x,point.y,GetParentFrame());
  }
	
	*pResult = 0;
}

void CsimCommunicationUnitListView::OnUnitClear() 
{
  CListCtrl &ctlList=GetListCtrl();

  POSITION pos = ctlList.GetFirstSelectedItemPosition();
  while(pos)
  {
    WORD UnitID = ctlList.GetNextSelectedItem(pos)+1;
    while(g_Unit[UnitID].SOEReportList.GetCount())
      delete (TagSOEReportStruct *)g_Unit[UnitID].SOEReportList.RemoveHead();
  }
}
