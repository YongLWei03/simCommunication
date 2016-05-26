// DlgListSelect.cpp : implementation file
//

#include "stdafx.h"
#include "DlgListSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int m_LastSelected = 0;
/////////////////////////////////////////////////////////////////////////////
// CDlgListSelect dialog


CDlgListSelect::CDlgListSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgListSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgListSelect)
	//}}AFX_DATA_INIT
}


void CDlgListSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgListSelect)
	DDX_Control(pDX, IDC_LIST, m_ctlList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgListSelect, CDialog)
	//{{AFX_MSG_MAP(CDlgListSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgListSelect message handlers

BOOL CDlgListSelect::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  if(m_TextTitle!="")
    SetWindowText(m_TextTitle);

  m_DataWaveType = 0;
  if(m_TextTitle.Find("¹ÊÕÏ")>=0)
    m_DataWaveType = 1;
  if(m_TextTitle.Find("ÐÐ³µ")>=0)
    m_DataWaveType = 2;

  m_ctlList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_ctlImage16.Create(16,20,ILC_COLOR4,10,1);
	m_ctlImage16.Add( AfxGetApp() -> LoadIcon(IDI_FILE));
	m_ctlImage16.Add( AfxGetApp() -> LoadIcon(IDI_FAULT_DATA));
	m_ctlImage16.Add( AfxGetApp() -> LoadIcon(IDI_TRAIN_DATA));
  m_ctlList.SetImageList( &m_ctlImage16,LVSIL_SMALL);

	::AddListTitle(m_ctlList,"ÐòºÅ",0,0,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_CENTER);
	::AddListTitle(m_ctlList,"Ãû³Æ",1,1,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_LEFT);
  m_ctlList.SetColumnWidth(1,1024);

  int item = 0;
  POSITION pos = m_TextList.GetHeadPosition();
  while(pos)
  {
    CString text = m_TextList.GetNext(pos);
    ::AddListItem(m_ctlList,item,0,::GetFormatText("%d",item+1));
    ::AddListItem(m_ctlList,item,1,text);
    ::SetItemImage(m_ctlList,item,m_DataWaveType);
    item++;
  }

  ::SelectListItem(m_ctlList,m_LastSelected);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgListSelect::OnOK() 
{
  if(m_ctlList.GetSelectedCount()!=1)
  {
    ::AfxMessageBox("Ñ¡Ôñ´íÎó");
    return;
  }
  
  POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
  int item = m_ctlList.GetNextSelectedItem(pos);
  m_TextSelected = m_ctlList.GetItemText(item,1);
  m_LastSelected = item;
	
	CDialog::OnOK();
}

