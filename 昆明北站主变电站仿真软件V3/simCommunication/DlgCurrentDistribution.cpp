// DlgCurrentDistribution.cpp : implementation file
//

#include "stdafx.h"
#include "simcommunication.h"
#include "DlgCurrentDistribution.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCurrentDistribution dialog


int g_BoxWidth = 8;
int g_AreaPx = 10;
int g_AreaPy = 10;
int g_AreaWidth = 800;
int g_AreaHeight = 300;

/////////////////////////////////////////////////////////////////////////////
// CDlgCurrentDistribution dialog

CDlgCurrentDistribution::CDlgCurrentDistribution(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCurrentDistribution::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCurrentDistribution)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
}

void CDlgCurrentDistribution::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCurrentDistribution)
	DDX_Control(pDX, IDC_EDIT_VALUE, m_ctlValue);
	DDX_Control(pDX, IDC_LIST, m_ctlList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgCurrentDistribution, CDialog)
	//{{AFX_MSG_MAP(CDlgCurrentDistribution)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_SET_VALUE, OnSetValue)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedList)
	ON_EN_CHANGE(IDC_EDIT_VALUE, OnChangeEditValue)
	ON_BN_CLICKED(IDC_SET_VALUE1, OnSetValue1)
	ON_BN_CLICKED(IDC_SET_VALUE2, OnSetValue2)
	ON_BN_CLICKED(IDC_SET_VALUE3, OnSetValue3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCurrentDistribution message handlers

BOOL CDlgCurrentDistribution::OnInitDialog()
{
	CDialog::OnInitDialog();

  m_ctlList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
  AddListTitle(m_ctlList,"ÐòºÅ",0);
  AddListTitle(m_ctlList,"ÊýÖµ",1);

  CString ChangeRageList = "";
  CNewString ns(m_ChangeRageList,";","\r\n");
  for(int no=0;no<100;no++)
  {
    float ChangeRage = ns.GetFloatValue(0,no) * 100;
    if(ns.GetWord(0,no)=="")
      ChangeRage = 100;
    ChangeRageList += ::GetFormatText("%0.0f",ChangeRage) + ";";

    ::AddListItem(m_ctlList,no,0,::GetFormatText("%02d",no));
  }
  m_ChangeRageList = ChangeRageList;

  UpdateList();
  SelectListFirstItem(m_ctlList);
	
  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDlgCurrentDistribution::OnPaint() 
{
  DrawList();

  CDialog::OnPaint();
}

void CDlgCurrentDistribution::UpdateList()
{
  CString ChangeRageList;
  CNewString ns(m_ChangeRageList,";","\r\n");
  for(int no=0;no<100;no++)
  {
    CString sChangeRate = ns.GetWord(0,no);
    if(sChangeRate=="")
      sChangeRate = "100";

    float fChangeRate = (float)atof(sChangeRate);
    if(fChangeRate<0)
      fChangeRate = 0;
    if(fChangeRate>100)
      fChangeRate = 100;
    sChangeRate = ::GetFormatText("%0.0f",fChangeRate);
    ChangeRageList += sChangeRate+";";
    SetItemText(m_ctlList,no,1,sChangeRate);
  }
  m_ChangeRageList = ChangeRageList;
}

void CDlgCurrentDistribution::DrawList()
{
  CClientDC dc(this);

  CNewString ns(m_ChangeRageList,";","\r\n");

  int BoxPos = 0;
  if(m_ctlList.GetSelectedCount()>0)
  {
    POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
    BoxPos = m_ctlList.GetNextSelectedItem(pos);
  }
  else
    SelectListFirstItem(m_ctlList);

  for(int no=0;no<100;no++)
  {
    float fChangeRate = ns.GetFloatValue(0,no);

    COLORREF color = 0x000000ff;
    if(BoxPos==no)
      color = 0x00ff0000;

    dc.FillSolidRect(g_AreaPx+g_BoxWidth*no,g_AreaPy,g_BoxWidth,g_AreaHeight-(WORD)(fChangeRate/(float)100*(float)g_AreaHeight),0x00000000);
    dc.FillSolidRect(g_AreaPx+g_BoxWidth*no,g_AreaPy+g_AreaHeight-(WORD)(fChangeRate/(float)100*(float)g_AreaHeight),g_BoxWidth,(WORD)(fChangeRate/(float)100*(float)g_AreaHeight),color);
  }
}

void CDlgCurrentDistribution::OnLButtonDown(UINT nFlags, CPoint point) 
{
  CRect BoxList[100];
  
  ::GetCursorPos(&point);
  ScreenToClient(&point);

  int BoxPos = -1;
  for(int no=0;no<100;no++)
  {
    BoxList[no].left = g_AreaPx+g_BoxWidth*no;
    BoxList[no].right = BoxList[no].left+g_BoxWidth;
    BoxList[no].top = g_AreaPy-1;
    BoxList[no].bottom = g_AreaPy+g_AreaHeight+1;
    if(BoxList[no].PtInRect(point))
    {
      BoxPos = no;
      CRect rect(g_AreaPx,g_AreaPy-1,g_AreaPx+g_AreaWidth,g_AreaPy+g_AreaHeight+1);
      ClientToScreen(&rect);
      ::ClipCursor(rect);
      
      break;
    }
  }
	
  if((BoxPos>=0)&&(BoxPos<100))
  {
    float ChangeRate = (float)(g_AreaHeight - (point.y - g_AreaPy)) / (float)g_AreaHeight * (float)100;
    CNewString ns(m_ChangeRageList,";","\r\n");
    ns.SetWord(0,BoxPos,::GetFormatText("%0.0f",ChangeRate));
    m_ChangeRageList = ns.GetPage();

    if(!(::GetKeyState(VK_CONTROL)&0xff00))
      UnSelectListAllItem(m_ctlList);

    SelectListItem(m_ctlList,BoxPos);
    UpdateList();
    DrawList();
  }

  CDialog::OnLButtonDown(nFlags, point);
}

void CDlgCurrentDistribution::OnMouseMove(UINT nFlags, CPoint point) 
{
  if(::GetKeyState(VK_LBUTTON)&0xff00)
  {
    CRect BoxList[100];
  
    ::GetCursorPos(&point);
    ScreenToClient(&point);

    int BoxPos = -1;
    for(int no=0;no<100;no++)
    {
      BoxList[no].left = g_AreaPx+g_BoxWidth*no;
      BoxList[no].right = BoxList[no].left+g_BoxWidth;
      BoxList[no].top = g_AreaPy-1;
      BoxList[no].bottom = g_AreaPy+g_AreaHeight+1;
      if(BoxList[no].PtInRect(point))
      {
        BoxPos = no;
        break;
      }
    }
	  
    if((BoxPos>=0)&&(BoxPos<100))
    {
      float ChangeRate = (float)(g_AreaHeight-(point.y-g_AreaPy))/(float)g_AreaHeight*(float)100;
      CNewString ns(m_ChangeRageList,";","\r\n");
      ns.SetWord(0,BoxPos,::GetFormatText("%0.0f",ChangeRate));
      m_ChangeRageList = ns.GetPage();

      if(!(::GetKeyState(VK_CONTROL)&0xff00))
        UnSelectListAllItem(m_ctlList);

      SelectListItem(m_ctlList,BoxPos);
      UpdateList();
      DrawList();
    }
  }
	
	CDialog::OnMouseMove(nFlags, point);
}

void CDlgCurrentDistribution::OnLButtonUp(UINT nFlags, CPoint point) 
{
  ::ClipCursor(NULL);
	
  UpdateList();
  DrawList();

	CDialog::OnLButtonUp(nFlags, point);
}

void CDlgCurrentDistribution::OnSetValue() 
{
  CString ChangeRate;
  m_ctlValue.GetWindowText(ChangeRate);
  if(atoi(ChangeRate)>100)
  {
    ChangeRate = "100";
    m_ctlValue.SetWindowText(ChangeRate);
  }

  CNewString ns(m_ChangeRageList,";","\r\n");
  POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
  while(pos)
  {
    int item = m_ctlList.GetNextSelectedItem(pos);
    ns.SetWord(0,item,ChangeRate);
  }
  m_ChangeRageList = ns.GetPage();

  UpdateList();
  DrawList();
}

void CDlgCurrentDistribution::OnSetValue1() 
{
  if(m_ctlList.GetSelectedCount()>2)
  {
    int FirstPos = m_ctlList.GetItemCount();
    int EndPos = 0;
    POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
    while(pos)
    {
      int item = m_ctlList.GetNextSelectedItem(pos);
      if(FirstPos>item)
        FirstPos = item;
      if(EndPos<item)
        EndPos = item;
    }
  
    CNewString ns(m_ChangeRageList,";","\r\n");
    float FirstChangeRate = ns.GetFloatValue(0,FirstPos);
    float EndChangeRate = ns.GetFloatValue(0,EndPos);
    float dChangeRate = (EndChangeRate - FirstChangeRate) / (float)(m_ctlList.GetSelectedCount()-1);
    
    int no = 0;
    pos = m_ctlList.GetFirstSelectedItemPosition();
    while(pos)
    {
      int item = m_ctlList.GetNextSelectedItem(pos);
      float ChangeRate = FirstChangeRate + dChangeRate * (float)no;
      ns.SetWord(0,item,::GetFormatText("%0.0f",ChangeRate));
      no++;
    }
    m_ChangeRageList = ns.GetPage();
    UpdateList();
    DrawList();
  }
}

void CDlgCurrentDistribution::OnSetValue2() 
{
  float dChangeRate = 1;
  if(::GetKeyState(VK_CONTROL)&0xff00)
    dChangeRate = 10;

  if(m_ctlList.GetSelectedCount()>0)
  {
    CNewString ns(m_ChangeRageList,";","\r\n");
    POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
    while(pos)
    {
      int item = m_ctlList.GetNextSelectedItem(pos);
      float ChangeRate = ns.GetFloatValue(0,item) + dChangeRate;
      if(ChangeRate>100)
        ChangeRate = 100;
      ns.SetWord(0,item,::GetFormatText("%0.0f",ChangeRate));
    }
    m_ChangeRageList = ns.GetPage();
    UpdateList();
    DrawList();
  }
}

void CDlgCurrentDistribution::OnSetValue3() 
{
  float dChangeRate = 1;
  if(::GetKeyState(VK_CONTROL)&0xff00)
    dChangeRate = 10;

  if(m_ctlList.GetSelectedCount()>0)
  {
    CNewString ns(m_ChangeRageList,";","\r\n");
    POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
    while(pos)
    {
      int item = m_ctlList.GetNextSelectedItem(pos);
      float ChangeRate = ns.GetFloatValue(0,item) - dChangeRate;
      if(ChangeRate<0)
        ChangeRate = 0;
      ns.SetWord(0,item,::GetFormatText("%0.0f",ChangeRate));
    }
    m_ChangeRageList = ns.GetPage();
    UpdateList();
    DrawList();
  }
}

void CDlgCurrentDistribution::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
  POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
  if(pos)
  {
    int item = m_ctlList.GetNextSelectedItem(pos);
    CString ChangeRate = m_ctlList.GetItemText(item,1);
    m_ctlValue.SetWindowText(ChangeRate);

    UpdateList();
    DrawList();
  }

	*pResult = 0;
}

void CDlgCurrentDistribution::OnChangeEditValue() 
{
  if(GetFocus()==&m_ctlValue)
  {
    OnSetValue(); 
  }
}

/////////////////////////////////////////////////////////////////////////////
// CDlgCurrentDistribution message handlers

void CDlgCurrentDistribution::OnOK() 
{
  CString ChangeRageList = "";
  CNewString ns(m_ChangeRageList,";","\r\n");
  for(int no=0;no<100;no++)
  {
    float ChangeRage = ns.GetFloatValue(0,no) / (float)100;
    ChangeRageList += ::GetFormatText("%0.2f",ChangeRage) + ";";
  }
  m_ChangeRageList = ChangeRageList;
	
	CDialog::OnOK();
}
