// DlgFreeTransmissionList.cpp : implementation file
//

#include "stdafx.h"
#include "simcommunication.h"
#include "DlgFreeTransmissionList.h"
#include "DlgFreeTransmission.h"
#include "DlgMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL WINAPI CheckOperateCondition();

static CPtrList g_FreeTransmissionManageList;
static WORD g_LastSelected = 0;
static BOOL g_SetCheck[1000];

/////////////////////////////////////////////////////////////////////////////
// CDlgFreeTransmissionList dialog


CDlgFreeTransmissionList::CDlgFreeTransmissionList(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFreeTransmissionList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFreeTransmissionList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgFreeTransmissionList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFreeTransmissionList)
	DDX_Control(pDX, IDC_LIST, m_ctlList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFreeTransmissionList, CDialog)
	//{{AFX_MSG_MAP(CDlgFreeTransmissionList)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_CLON, OnClon)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_STARTUP, OnStartup)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFreeTransmissionList message handlers

BOOL CDlgFreeTransmissionList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_ctlList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);
	m_ctlImage16.Create(16,20,ILC_COLOR4,10,1);
	m_ctlImage16.Add( AfxGetApp() -> LoadIcon(IDI_FILE));
  m_ctlList.SetImageList( &m_ctlImage16,LVSIL_SMALL);

	::AddListTitle(m_ctlList,"序号",0,0,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_CENTER);
	::AddListTitle(m_ctlList,"馈线名称",1,1,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_CENTER);
	::AddListTitle(m_ctlList,"传动电流(A)",2,2,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_CENTER);
	::AddListTitle(m_ctlList,"电流相位(度)",3,3,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_CENTER);
	::AddListTitle(m_ctlList,"持续时间(秒)",4,4,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_CENTER);
	::AddListTitle(m_ctlList,"电流抖动(%)",5,5,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_CENTER);
	::AddListTitle(m_ctlList,"启动时间(秒)",6,6,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_CENTER);
  m_ctlList.SetColumnWidth(0,64);

  LoadFreeTrainManageListFile();

	UpdateList();
  ::SelectListItem(m_ctlList,g_LastSelected);

  return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFreeTransmissionList::UpdateList()
{
  for(int no=0;no<m_ctlList.GetItemCount();no++)
	  g_SetCheck[no] = m_ctlList.GetCheck(no);
  while(m_ctlList.GetItemCount()<g_FreeTransmissionManageList.GetCount())
  {
    ::AddListItem(m_ctlList,0,0,"");
  }
  while(m_ctlList.GetItemCount()>g_FreeTransmissionManageList.GetCount())
  {
    m_ctlList.DeleteItem(0);
  }

  for(int item=0;item<g_FreeTransmissionManageList.GetCount();item++)
  {
    POSITION pos = g_FreeTransmissionManageList.FindIndex(item);
    struct TagFreeTransmissionStruct *pFreeTransmission = (TagFreeTransmissionStruct *)g_FreeTransmissionManageList.GetAt(pos);

    ::SetItemText(m_ctlList,item,0,::GetFormatText("%d",item+1));
    ::SetItemText(m_ctlList,item,1,g_Unit[pFreeTransmission->FeederUnitID].UnitName);
    ::SetItemText(m_ctlList,item,2,::GetFormatText("%0.0f",pFreeTransmission->AverCurrent));
    ::SetItemText(m_ctlList,item,3,::GetFormatText("%0.0f",pFreeTransmission->AverAngle));
    ::SetItemText(m_ctlList,item,4,::GetFormatText("%d",pFreeTransmission->ContinuedSeconds));
    ::SetItemText(m_ctlList,item,5,::GetFormatText("%0.1f",pFreeTransmission->CurrentChangeRate*100));
    ::SetItemText(m_ctlList,item,6,::GetFormatText("%d",pFreeTransmission->StartupDelaySecond));
    ::SetItemImage(m_ctlList,item,0);
	  m_ctlList.SetCheck(item,g_SetCheck[item]);
  }
}

BOOL CDlgFreeTransmissionList::LoadFreeTrainManageListFile()
{
  CString FileName = g_ProjectFilePath + "FeedTransmission.lst";
  CFile file;
	if(!file.Open(FileName, CFile::modeRead))
  {
    ::AfxMessageBox(FileName + " 文件读取失败");
    return FALSE;
  }
  else
  {
    CArchive ar(&file,CArchive::load);

    g_FreeTransmissionManageList.RemoveAll();

    CString title = "FeedTransmissionList 1.0";
    ar>>title;

    WORD Count = g_FreeTransmissionManageList.GetCount();
    ar>>Count;
    
    for(int no=0;no<Count;no++)
    {
      struct TagFreeTransmissionStruct *pFreeTransmission = new TagFreeTransmissionStruct;
      g_FreeTransmissionManageList.AddTail(pFreeTransmission);
      ar>>pFreeTransmission->FeederUnitID;
      ar>>pFreeTransmission->AverCurrent;
      ar>>pFreeTransmission->AverAngle;
      ar>>pFreeTransmission->CurrentChangeRate;
      ar>>pFreeTransmission->ContinuedSeconds;
      ar>>pFreeTransmission->StartupDelaySecond;
      CString CurrentChangeRateList;
      ar>>CurrentChangeRateList;
      strcpy(pFreeTransmission->CurrentChangeRateList,CurrentChangeRateList.GetBuffer(0));
      ar>>g_SetCheck[no];
    }

    ar.Flush();
    ar.Close();
    file.Close();
  }	
  
  return TRUE;
}

void CDlgFreeTransmissionList::OnLoad() 
{
  if(::AfxMessageBox("是否读取馈线传动管理文件？",MB_YESNO)==IDNO)
    return;

  LoadFreeTrainManageListFile();

  POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
  g_LastSelected = m_ctlList.GetNextSelectedItem(pos);
	UpdateList();
  ::SelectListItem(m_ctlList,g_LastSelected);
}

void CDlgFreeTransmissionList::OnSave() 
{
  if(::AfxMessageBox("是否保存馈线传动管理文件？",MB_YESNO)==IDNO)
    return;

  CString FileName = g_ProjectFilePath + "FeedTransmission.lst";
	CFile file;
	if(!file.Open(FileName, CFile::modeCreate | CFile::modeWrite))
  {
    AfxMessageBox(FileName+" 保存文件失败!",MB_SYSTEMMODAL);
    return;
  }

  char buffer[1024];
  CArchive ar(&file,CArchive::store,1024,buffer);

  CString title = "FeedTransmissionList 1.0";
  ar<<title;

  WORD Count = g_FreeTransmissionManageList.GetCount();
  ar<<Count;
    
  for(int no=0;no<Count;no++)
  {
    POSITION pos = g_FreeTransmissionManageList.FindIndex(no);
    struct TagFreeTransmissionStruct *pFreeTransmission = (TagFreeTransmissionStruct *)g_FreeTransmissionManageList.GetAt(pos);
    ar<<pFreeTransmission->FeederUnitID;
    ar<<pFreeTransmission->AverCurrent;
    ar<<pFreeTransmission->AverAngle;
    ar<<pFreeTransmission->CurrentChangeRate;
    ar<<pFreeTransmission->ContinuedSeconds;
    ar<<pFreeTransmission->StartupDelaySecond;
    CString CurrentChangeRateList = pFreeTransmission->CurrentChangeRateList;
    ar<<CurrentChangeRateList;
	  g_SetCheck[no] = m_ctlList.GetCheck(no);
    ar<<g_SetCheck[no];
  }

  ar.Flush();
  ar.Close();
  file.Close();
}

void CDlgFreeTransmissionList::OnNew() 
{
  static WORD FeederUnitID = 0;
  static float AverCurrent = 300;
  static float AverAngle = 30;
  static float CurrentChangeRate = (float)0.1;
  static WORD ContinuedSeconds = 30;
  static CString CurrentChangeRateList;

  CDlgFreeTransmission dlg;
  struct TagFreeTransmissionStruct *pFreeTransmission = new TagFreeTransmissionStruct;
  pFreeTransmission->FeederUnitID = FeederUnitID;
  pFreeTransmission->AverCurrent = AverCurrent;
  pFreeTransmission->AverAngle = AverAngle;
  pFreeTransmission->CurrentChangeRate = CurrentChangeRate;
  strcpy(pFreeTransmission->CurrentChangeRateList,CurrentChangeRateList.GetBuffer(0));
  pFreeTransmission->ContinuedSeconds = ContinuedSeconds;
  pFreeTransmission->StartupDelaySecond = g_StartupDelaySecond;
  dlg.m_pFreeTransmission = pFreeTransmission;
  if(dlg.DoModal()==IDOK)
  {
    FeederUnitID = pFreeTransmission->FeederUnitID;
    AverCurrent = pFreeTransmission->AverCurrent;
    AverAngle = pFreeTransmission->AverAngle;
    CurrentChangeRate = pFreeTransmission->CurrentChangeRate;
    CurrentChangeRateList = pFreeTransmission->CurrentChangeRateList;
    ContinuedSeconds = pFreeTransmission->ContinuedSeconds;
    g_StartupDelaySecond = pFreeTransmission->StartupDelaySecond;

    g_FreeTransmissionManageList.AddTail(pFreeTransmission);
  
	  UpdateList();
    ::SelectListLastItem(m_ctlList);
  }
}

void CDlgFreeTransmissionList::OnEdit() 
{
  if(m_ctlList.GetSelectedCount()!=1)
    return;

  POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
  int item = m_ctlList.GetNextSelectedItem(pos);
  struct TagFreeTransmissionStruct *pFreeTransmission = (TagFreeTransmissionStruct *)g_FreeTransmissionManageList.GetAt(g_FreeTransmissionManageList.FindIndex(item));

  CDlgFreeTransmission dlg;
  dlg.m_pFreeTransmission = pFreeTransmission;
  if(dlg.DoModal()==IDOK)
	  UpdateList();
}

void CDlgFreeTransmissionList::OnClon() 
{
  if(m_ctlList.GetSelectedCount()!=1)
    return;

  POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
  int item = m_ctlList.GetNextSelectedItem(pos);
  struct TagFreeTransmissionStruct *pFreeTransmission = (TagFreeTransmissionStruct *)g_FreeTransmissionManageList.GetAt(g_FreeTransmissionManageList.FindIndex(item));
  struct TagFreeTransmissionStruct *pNewFreeTransmission = new TagFreeTransmissionStruct;
  memcpy(pNewFreeTransmission,pFreeTransmission,sizeof(TagFreeTransmissionStruct));
  g_FreeTransmissionManageList.AddTail(pNewFreeTransmission);

	UpdateList();
  ::SelectListLastItem(m_ctlList);
}

void CDlgFreeTransmissionList::OnDel() 
{
  if(m_ctlList.GetSelectedCount()!=1)
    return;

  if(::AfxMessageBox("是否删除馈线传动？",MB_YESNO)==IDNO)
    return;

  POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
  int item = m_ctlList.GetNextSelectedItem(pos);
  g_FreeTransmissionManageList.RemoveAt(g_FreeTransmissionManageList.FindIndex(item));

  g_LastSelected = m_ctlList.GetNextSelectedItem(pos);
	g_LastSelected++;
  UpdateList();
  ::SelectListItem(m_ctlList,g_LastSelected);
	
}

void CDlgFreeTransmissionList::OnCancel() 
{
  POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
  g_LastSelected = m_ctlList.GetNextSelectedItem(pos);
	
	CDialog::OnCancel();
}

void CDlgFreeTransmissionList::OnStartup() 
{
  if(::CheckOperateCondition()==FALSE)
    return;
	
  if(::AfxMessageBox("是否启动馈线传动？",MB_YESNO)==IDNO)
    return;

  for(int item=0;item<m_ctlList.GetItemCount();item++)
  {
	  if(m_ctlList.GetCheck(item))
    {
      POSITION pos = g_FreeTransmissionManageList.FindIndex(item);
      struct TagFreeTransmissionStruct *pFreeTransmission = (TagFreeTransmissionStruct *)g_FreeTransmissionManageList.GetAt(pos);
      
      pFreeTransmission->BeginTime = CTime::GetCurrentTime() + CTimeSpan(0,0,0,pFreeTransmission->StartupDelaySecond);
      pFreeTransmission->FinishTime = pFreeTransmission->BeginTime + CTimeSpan(0,0,0,pFreeTransmission->ContinuedSeconds);

      struct TagFreeTransmissionStruct *pNewFreeTransmission = new TagFreeTransmissionStruct;
      memcpy(pNewFreeTransmission,pFreeTransmission,sizeof(TagFreeTransmissionStruct));
      ::AddFreeTransmission(pNewFreeTransmission);

      g_pDlgMessageBox->DoShow("准备启动馈线传动",3);
    }
  }

  OnCancel();
}

void CDlgFreeTransmissionList::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEdit();
	
	*pResult = 0;
}
