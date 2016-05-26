// DlgSceneFileSelect.cpp : implementation file
//

#include "stdafx.h"
#include "simcommunication.h"
#include "DlgSceneFileSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int m_LastSelected = 0;

/////////////////////////////////////////////////////////////////////////////
// CDlgSceneFileSelect dialog


CDlgSceneFileSelect::CDlgSceneFileSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSceneFileSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSceneFileSelect)
	//}}AFX_DATA_INIT
}


void CDlgSceneFileSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSceneFileSelect)
	DDX_Control(pDX, IDC_LIST, m_ctlList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSceneFileSelect, CDialog)
	//{{AFX_MSG_MAP(CDlgSceneFileSelect)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SATRTUP, OnSatrtup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSceneFileSelect message handlers

BOOL CDlgSceneFileSelect::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_ctlList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_ctlImage16.Create(16,20,ILC_COLOR4,10,1);
	m_ctlImage16.Add( AfxGetApp() -> LoadIcon(IDI_FILE));
  m_ctlList.SetImageList( &m_ctlImage16,LVSIL_SMALL);

	::AddListTitle(m_ctlList,"ÐòºÅ",0,0,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_CENTER);
	::AddListTitle(m_ctlList,"Ãû³Æ",1,1,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_LEFT);
  m_ctlList.SetColumnWidth(1,1024);

  CString FilePath = g_ProjectFilePath+"SceneOperate\\";
  CString FileName = FilePath+"*.scn";
  ::GetFileList(m_FileList,FileName);
  ::StringListSortAsc(m_FileList);

	UpdateList();

  return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSceneFileSelect::UpdateList()
{
  m_ctlList.DeleteAllItems();

  int item = 0;
  POSITION pos = m_FileList.GetHeadPosition();
  while(pos)
  {
    CString filename = m_FileList.GetNext(pos);
    ::AddListItem(m_ctlList,item,0,::GetFormatText("%d",item+1));
    ::AddListItem(m_ctlList,item,1,filename);
    ::SetItemImage(m_ctlList,item,0);
    item++;
  }

  ::SelectListItem(m_ctlList,m_LastSelected);
}

void CDlgSceneFileSelect::OnDestroy() 
{
	CDialog::OnDestroy();
	
  POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
  if(pos)
    m_LastSelected = m_ctlList.GetNextSelectedItem(pos);
}

void CDlgSceneFileSelect::OnSatrtup() 
{
  if(m_ctlList.GetSelectedCount()==0)
  {
    ::AfxMessageBox("Ñ¡Ôñ´íÎó");
    return;
  }
  
  POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
  int item = m_ctlList.GetNextSelectedItem(pos);
  CString FileName = g_ProjectFilePath+"SceneOperate\\"+m_ctlList.GetItemText(item,1)+".scn";
  g_SceneOperateFileName = FileName;
	
	CDialog::OnOK();
}
