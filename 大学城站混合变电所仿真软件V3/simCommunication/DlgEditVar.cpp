// DlgEditVar.cpp : implementation file
//

#include "stdafx.h"
#include "DlgEditVar.h"
#include "DlgReplace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgEditVar dialog


CDlgEditVar::CDlgEditVar(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditVar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEditVar)
	//}}AFX_DATA_INIT
}


void CDlgEditVar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEditVar)
	DDX_Control(pDX, IDC_VAR_NAME, m_ctlVarName);
	DDX_Control(pDX, IDC_TEXT_FORMAT, m_ctlTextFormat);
	DDX_Control(pDX, IDC_LIST, m_ctlList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEditVar, CDialog)
	//{{AFX_MSG_MAP(CDlgEditVar)
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	ON_BN_CLICKED(IDC_REPLASE, OnReplace)
	ON_EN_CHANGE(IDC_VAR_NAME, OnChangeVarName)
	ON_EN_CHANGE(IDC_TEXT_FORMAT, OnChangeTextFormat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEditVar message handlers

BOOL CDlgEditVar::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_ctlList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
  ::AddListTitle(m_ctlList,"ÐòºÅ",0);
  ::AddListTitle(m_ctlList,"Ãû³Æ",1);
  ::AddListTitle(m_ctlList,"¸ñÊ½",2);
  m_ctlList.SetColumnWidth(0,40);
  m_ctlList.SetColumnWidth(1,120);
  m_ctlList.SetColumnWidth(2,80);

  for(int varno=0;varno<VARCOUNT;varno++)
  {
    ::AddListItem(m_ctlList,varno,0,::GetFormatText("%d",varno));
    ::AddListItem(m_ctlList,varno,1,m_VarName[varno]);
    ::AddListItem(m_ctlList,varno,2,m_TextFormat[varno]);
  }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditVar::OnSet() 
{
  if(m_ctlList.GetSelectedCount()>0)
  {
    POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
    int varno = m_ctlList.GetNextSelectedItem(pos);
	  
    CString VarName,TextFormat;
    m_ctlVarName.GetWindowText(VarName);
    m_ctlTextFormat.GetWindowText(TextFormat);

    m_ctlList.SetItemText(varno,1,VarName);
    m_ctlList.SetItemText(varno,2,TextFormat);
  }
}

void CDlgEditVar::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
  if(m_ctlList.GetSelectedCount()>0)
  {
    POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
    int varno = m_ctlList.GetNextSelectedItem(pos);
	  
    m_ctlVarName.SetWindowText(m_ctlList.GetItemText(varno,1));
    m_ctlTextFormat.SetWindowText(m_ctlList.GetItemText(varno,2));
  }

	*pResult = 0;
}

void CDlgEditVar::OnOK() 
{
  for(int varno=0;varno<VARCOUNT;varno++)
  {
    m_VarName[varno] = m_ctlList.GetItemText(varno,1);
    m_TextFormat[varno] = m_ctlList.GetItemText(varno,2);
  }
	
	CDialog::OnOK();
}

void CDlgEditVar::OnCopy() 
{
	LV_COLUMN lvc;
  int Col=0;
  CString Str;
  lvc.mask = LVCF_TEXT|LVCF_WIDTH;
  char buf[80];
	lvc.pszText = (LPTSTR) buf;
  lvc.cchTextMax=80;

  CString list;
  if(m_ctlList.GetSelectedCount()==0)
  {
    for(int no=0;no<m_ctlList.GetItemCount();no++)
    {
      int col=1;
      while(m_ctlList.GetColumn(col,&lvc))
      {
        if(lvc.cx)
        {
          CString text = m_ctlList.GetItemText(no,col);
          list += text+"\t";
        }
        col++;
      }
      list += "\r\n";
    }
  }
  else
  {
    POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
    while(pos)
    {
      int no = m_ctlList.GetNextSelectedItem(pos);
      int col=1;
      while(m_ctlList.GetColumn(col,&lvc))
      {
        if(lvc.cx)
        {
          CString text = m_ctlList.GetItemText(no,col);
          list += text+"\t";
        }
        col++;
      }
      list += "\r\n";
    }
  }
  if(::OpenClipboard(NULL))
  {
    ::EmptyClipboard();
    ::CloseClipboard();
  }
  if(m_ctlList.OpenClipboard())
  {
    HANDLE hmem = ::GlobalAlloc(GMEM_FIXED,list.GetLength()+1);
    char *buf = (char*)::GlobalLock(hmem);
    strcpy(buf,list);
    ::SetClipboardData(CF_TEXT,hmem);
    ::GlobalUnlock(hmem);
    ::CloseClipboard();
  }
}

void CDlgEditVar::OnPaste() 
{
  CWaitCursor WaitCursor; 

  CString ClipboardText;

  if(::OpenClipboard(NULL))
  {
    HANDLE hmem = ::GetClipboardData(CF_TEXT);
    char *buf = (char*)::GlobalLock(hmem);
    ClipboardText = buf;
    ::GlobalUnlock(hmem);
    CloseClipboard();
  }	

  CNewString ns(ClipboardText,"\t","\r\n");

  if(m_ctlList.GetSelectedCount()==0)
  {
    for(int varno=0;varno<m_ctlList.GetItemCount();varno++)
    {
      m_VarName[varno] = ns.GetWord(varno,0);
      m_TextFormat[varno] = ns.GetWord(varno,1);
      m_ctlList.SetItemText(varno,1,m_VarName[varno]);
      m_ctlList.SetItemText(varno,2,m_TextFormat[varno]);
    }
  }
  else
  {
    int no = 0;
    POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
    while(pos)
    {
      int item = m_ctlList.GetNextSelectedItem(pos);
    
      m_VarName[item] = ns.GetWord(no,0);
      m_TextFormat[item] = ns.GetWord(no,1);
      m_ctlList.SetItemText(item,1,m_VarName[item]);
      m_ctlList.SetItemText(item,2,m_TextFormat[item]);
      no++;
    }
  }
}

void CDlgEditVar::OnReplace() 
{
  CDlgReplace dlg;
	if(dlg.DoModal()==IDOK)
  {
    POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
    while(pos)
    {
      int item = m_ctlList.GetNextSelectedItem(pos);
    
      m_VarName[item].Replace(dlg.m_Old,dlg.m_New);
      m_TextFormat[item].Replace(dlg.m_Old,dlg.m_New);
      m_ctlList.SetItemText(item,1,m_VarName[item]);
      m_ctlList.SetItemText(item,2,m_TextFormat[item]);
    }
  }
}

void CDlgEditVar::OnChangeVarName() 
{
  if((GetFocus()==&m_ctlVarName)&&(m_ctlList.GetSelectedCount()>0))
  {
    CString VarName;
    m_ctlVarName.GetWindowText(VarName);
    POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
    while(pos)
    {
      int varno = m_ctlList.GetNextSelectedItem(pos);
      m_ctlList.SetItemText(varno,1,VarName);
    }
  }
}

void CDlgEditVar::OnChangeTextFormat() 
{
  if((GetFocus()==&m_ctlTextFormat)&&(m_ctlList.GetSelectedCount()>0))
  {
    CString TextFormat;
    m_ctlTextFormat.GetWindowText(TextFormat);
    POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
    while(pos)
    {
      int varno = m_ctlList.GetNextSelectedItem(pos);
      m_ctlList.SetItemText(varno,2,TextFormat);
    }
  }
}
