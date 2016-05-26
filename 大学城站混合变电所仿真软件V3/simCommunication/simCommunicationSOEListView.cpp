// simCommunicationListView.cpp : implementation file
//

#include "stdafx.h"
#include "simCommunication.h"
#include "simCommunicationSOEListView.h"
#include "simCommunicationVarListView.h"
#include "simCommunicationUnitListView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CsimCommunicationSOEListView

IMPLEMENT_DYNCREATE(CsimCommunicationSOEListView, CListView)

CsimCommunicationSOEListView::CsimCommunicationSOEListView()
{
}

CsimCommunicationSOEListView::~CsimCommunicationSOEListView()
{
}


BEGIN_MESSAGE_MAP(CsimCommunicationSOEListView, CListView)
	//{{AFX_MSG_MAP(CsimCommunicationSOEListView)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CsimCommunicationSOEListView drawing

void CsimCommunicationSOEListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CsimCommunicationSOEListView diagnostics

#ifdef _DEBUG
void CsimCommunicationSOEListView::AssertValid() const
{
	CListView::AssertValid();
}

void CsimCommunicationSOEListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CsimCommunicationSOEListView message handlers

int CsimCommunicationSOEListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	lpCreateStruct->style |= LVS_SHOWSELALWAYS ;
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle(LVS_TYPEMASK, LVS_REPORT|LVS_SHOWSELALWAYS);

  CListCtrl &ctlList=GetListCtrl();
  ctlList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

  return 0;
}

void CsimCommunicationSOEListView::MakeListView()
{
  CListCtrl &ctlList=GetListCtrl();

  while(ctlList.DeleteColumn(0))
    ;
  ctlList.DeleteAllItems( );

  
  CListCtrl &ctlUnitList=m_pUnitListView->GetListCtrl();
  if(ctlUnitList.GetSelectedCount()!=1)
    return;
  
  POSITION pos = ctlUnitList.GetFirstSelectedItemPosition();
  WORD UnitID = ctlUnitList.GetNextSelectedItem(pos) + 1;

  ::AddListTitle(ctlList,"动作序号",0);
  ::AddListTitle(ctlList,"动作名称",1);
  ::AddListTitle(ctlList,"动作状态",2);
  ::AddListTitle(ctlList,"动作时间",3);
  ctlList.SetColumnWidth(1,160);
  ctlList.SetColumnWidth(3,160);

  for(int no=0;no<VARCOUNT;no++)
  {
    CString ActionValueDefName = ::LoadActionValueDefName(g_Unit[UnitID].UnitType,no);
    if(ActionValueDefName=="")
      break;

    ::AddListTitle(ctlList,ActionValueDefName,4+no);
    ctlList.SetColumnWidth(4+no,80);
  }

  for(int item=0;item<10;item++)
    ::AddListItem(ctlList,item,0,::GetFormatText("%d",item+1));
}

void CsimCommunicationSOEListView::UpdateListView()
{
  CListCtrl &ctlList=GetListCtrl();
  CListCtrl &ctlUnitList = m_pUnitListView->GetListCtrl();
  
  if(ctlUnitList.GetSelectedCount()==1)
  {
    POSITION pos = ctlUnitList.GetFirstSelectedItemPosition();
    int UnitID = ctlUnitList.GetNextSelectedItem(pos)+1;

    for(int no=0;no<10;no++)
    {
      POSITION pos = g_Unit[UnitID].SOEReportList.FindIndex(no);
      if(pos)
      {
        TagSOEReportStruct *pSOEReportStruct = (TagSOEReportStruct*)g_Unit[UnitID].SOEReportList.GetAt(pos);
        ::SetItemText(ctlList,no,1,::LoadSOEDefName(g_Unit[UnitID].UnitType,pSOEReportStruct->SOEID));
        if(pSOEReportStruct->SOEValue)
          ::SetItemText(ctlList,no,2,"动作");
        else
          ::SetItemText(ctlList,no,2,"复归");
        ::SetItemText(ctlList,no,3,::GetTimeText(pSOEReportStruct->SOETime,pSOEReportStruct->SOEMSEL));

        for(int item=0;item<32;item++)
          ::SetItemText(ctlList,no,4+item,::GetFormatText("%0.3f",pSOEReportStruct->ActionValue[item]));
      }
      else
      {
        for(int item=1;item<32;item++)
          ::SetItemText(ctlList,no,item,"");
      }
    }
  }
  else
  {
    for(int no=0;no<10;no++)
    {
      for(int item=1;item<32;item++)
        ::SetItemText(ctlList,no,item,"");
    }
  }
}

