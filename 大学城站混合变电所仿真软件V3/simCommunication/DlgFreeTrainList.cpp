// DlgFreeTrainList.cpp : implementation file
//

#include "stdafx.h"
#include "simcommunication.h"
#include "DlgFreeTrainList.h"
#include "DlgFreeTrain.h"
#include "DlgMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL WINAPI CheckOperateCondition();

static CPtrList g_FreeTrainManageList;
static WORD g_LastSelected = 0;
static BOOL g_SetCheck[1000];

/////////////////////////////////////////////////////////////////////////////
// CDlgFreeTrainList dialog


CDlgFreeTrainList::CDlgFreeTrainList(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFreeTrainList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFreeTrainList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgFreeTrainList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFreeTrainList)
	DDX_Control(pDX, IDC_LIST, m_ctlList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFreeTrainList, CDialog)
	//{{AFX_MSG_MAP(CDlgFreeTrainList)
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
// CDlgFreeTrainList message handlers

BOOL CDlgFreeTrainList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_ctlList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);
	m_ctlImage16.Create(16,20,ILC_COLOR4,10,1);
	m_ctlImage16.Add( AfxGetApp() -> LoadIcon(IDI_FILE));
  m_ctlList.SetImageList( &m_ctlImage16,LVSIL_SMALL);

	::AddListTitle(m_ctlList,"序号",0,0,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_CENTER);
	::AddListTitle(m_ctlList,"驶入馈线名称",1,1,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_CENTER);
	::AddListTitle(m_ctlList,"驶出馈线名称",2,2,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_CENTER);
	::AddListTitle(m_ctlList,"行车电流(A)",3,3,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_CENTER);
	::AddListTitle(m_ctlList,"电流相位(度)",4,4,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_CENTER);
	::AddListTitle(m_ctlList,"驶入持续时间(秒)",5,5,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_CENTER);
	::AddListTitle(m_ctlList,"换相续时间(秒)",6,6,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_CENTER);
	::AddListTitle(m_ctlList,"驶出持续时间(秒)",7,7,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_CENTER);
	::AddListTitle(m_ctlList,"电流抖动(%)",8,8,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_CENTER);
	::AddListTitle(m_ctlList,"启动时间(秒)",9,9,LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,LVCFMT_CENTER);
  m_ctlList.SetColumnWidth(0,64);

  LoadFreeTrainManageListFile();

	UpdateList();
  ::SelectListItem(m_ctlList,g_LastSelected);

  return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFreeTrainList::UpdateList()
{
  for(int no=0;no<m_ctlList.GetItemCount();no++)
	  g_SetCheck[no] = m_ctlList.GetCheck(no);
  while(m_ctlList.GetItemCount()<g_FreeTrainManageList.GetCount())
  {
    ::AddListItem(m_ctlList,0,0,"");
  }
  while(m_ctlList.GetItemCount()>g_FreeTrainManageList.GetCount())
  {
    m_ctlList.DeleteItem(0);
  }

  for(int item=0;item<g_FreeTrainManageList.GetCount();item++)
  {
    POSITION pos = g_FreeTrainManageList.FindIndex(item);
    struct TagFreeTrainStruct *pFreeTrain = (TagFreeTrainStruct *)g_FreeTrainManageList.GetAt(pos);

    ::SetItemText(m_ctlList,item,0,::GetFormatText("%d",item+1));
    ::SetItemText(m_ctlList,item,1,g_Unit[pFreeTrain->InFeederUnitID].UnitName);
    ::SetItemText(m_ctlList,item,2,g_Unit[pFreeTrain->OutFeederUnitID].UnitName);
    ::SetItemText(m_ctlList,item,3,::GetFormatText("%0.0f",pFreeTrain->AverCurrent));
    ::SetItemText(m_ctlList,item,4,::GetFormatText("%0.0f",pFreeTrain->AverAngle));
    ::SetItemText(m_ctlList,item,5,::GetFormatText("%d",pFreeTrain->InContinuedSeconds));
    ::SetItemText(m_ctlList,item,6,::GetFormatText("%d",pFreeTrain->ChangeContinuedSeconds));
    ::SetItemText(m_ctlList,item,7,::GetFormatText("%d",pFreeTrain->OutContinuedSeconds));
    ::SetItemText(m_ctlList,item,8,::GetFormatText("%0.1f",pFreeTrain->CurrentChangeRate*100));
    ::SetItemText(m_ctlList,item,9,::GetFormatText("%d",pFreeTrain->StartupDelaySecond));
    ::SetItemImage(m_ctlList,item,0);
	  m_ctlList.SetCheck(item,g_SetCheck[item]);
  }
}

BOOL CDlgFreeTrainList::LoadFreeTrainManageListFile()
{
  CString FileName = g_ProjectFilePath + "FeedTrain.lst";
  CFile file;
	if(!file.Open(FileName, CFile::modeRead))
  {
    ::AfxMessageBox(FileName + " 文件读取失败");
    return FALSE;
  }
  else
  {
    CArchive ar(&file,CArchive::load);

    g_FreeTrainManageList.RemoveAll();

    CString title = "FreeTrainList 1.0";
    ar>>title;

    WORD Count = g_FreeTrainManageList.GetCount();
    ar>>Count;
    
    for(int no=0;no<Count;no++)
    {
      struct TagFreeTrainStruct *pFreeTrain = new TagFreeTrainStruct;
      g_FreeTrainManageList.AddTail(pFreeTrain);
      ar>>pFreeTrain->InFeederUnitID;
      ar>>pFreeTrain->OutFeederUnitID;
      ar>>pFreeTrain->AverCurrent;
      ar>>pFreeTrain->AverAngle;
      ar>>pFreeTrain->CurrentChangeRate;
      ar>>pFreeTrain->InContinuedSeconds;
      ar>>pFreeTrain->ChangeContinuedSeconds;
      ar>>pFreeTrain->OutContinuedSeconds;
      ar>>pFreeTrain->StartupDelaySecond;
      CString InCurrentChangeRateList;
      CString OutCurrentChangeRateList;
      ar>>InCurrentChangeRateList;
      ar>>OutCurrentChangeRateList;
      strcpy(pFreeTrain->InCurrentChangeRateList,InCurrentChangeRateList.GetBuffer(0));
      strcpy(pFreeTrain->OutCurrentChangeRateList,OutCurrentChangeRateList.GetBuffer(0));
      ar>>g_SetCheck[no];
    }

    ar.Flush();
    ar.Close();
    file.Close();
  }	

  return TRUE;
}

void CDlgFreeTrainList::OnLoad() 
{
  if(::AfxMessageBox("是否读取除自由行车管理文件？",MB_YESNO)==IDNO)
    return;

  LoadFreeTrainManageListFile();

  POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
  g_LastSelected = m_ctlList.GetNextSelectedItem(pos);
	UpdateList();
  ::SelectListItem(m_ctlList,g_LastSelected);
}

void CDlgFreeTrainList::OnSave() 
{
  if(::AfxMessageBox("是否保存除自由行车管理文件？",MB_YESNO)==IDNO)
    return;

  CString FileName = g_ProjectFilePath + "FeedTrain.lst";
	CFile file;
	if(!file.Open(FileName, CFile::modeCreate | CFile::modeWrite))
  {
    AfxMessageBox(FileName+" 保存文件失败!",MB_SYSTEMMODAL);
    return;
  }

  char buffer[1024];
  CArchive ar(&file,CArchive::store,1024,buffer);

  CString title = "FreeTrainList 1.0";
  ar<<title;

  WORD Count = g_FreeTrainManageList.GetCount();
  ar<<Count;
    
  for(int no=0;no<Count;no++)
  {
    POSITION pos = g_FreeTrainManageList.FindIndex(no);
    struct TagFreeTrainStruct *pFreeTrain = (TagFreeTrainStruct *)g_FreeTrainManageList.GetAt(pos);
    ar<<pFreeTrain->InFeederUnitID;
    ar<<pFreeTrain->OutFeederUnitID;
    ar<<pFreeTrain->AverCurrent;
    ar<<pFreeTrain->AverAngle;
    ar<<pFreeTrain->CurrentChangeRate;
    ar<<pFreeTrain->InContinuedSeconds;
    ar<<pFreeTrain->ChangeContinuedSeconds;
    ar<<pFreeTrain->OutContinuedSeconds;
    ar<<pFreeTrain->StartupDelaySecond;
    CString InCurrentChangeRateList = pFreeTrain->InCurrentChangeRateList;
    CString OutCurrentChangeRateList = pFreeTrain->OutCurrentChangeRateList;
    ar<<InCurrentChangeRateList;
    ar<<OutCurrentChangeRateList;
	  g_SetCheck[no] = m_ctlList.GetCheck(no);
    ar<<g_SetCheck[no];
  }

  ar.Flush();
  ar.Close();
  file.Close();
}

void CDlgFreeTrainList::OnNew() 
{
  static WORD InFeederUnitID = 0;
  static WORD OutFeederUnitID = 0;
  static float AverCurrent = 300;
  static float AverAngle = 30;
  static float CurrentChangeRate = (float)0.1;
  static WORD InContinuedSeconds = 30;
  static WORD ChangeContinuedSeconds = 5;
  static WORD OutContinuedSeconds = 30;
  static CString InCurrentChangeRateList;
  static CString OutCurrentChangeRateList;

  CDlgFreeTrain dlg;
  struct TagFreeTrainStruct *pFreeTrain = new TagFreeTrainStruct;
  pFreeTrain->InFeederUnitID = InFeederUnitID;
  pFreeTrain->OutFeederUnitID = OutFeederUnitID;
  pFreeTrain->AverCurrent = AverCurrent;
  pFreeTrain->AverAngle = AverAngle;
  pFreeTrain->CurrentChangeRate = CurrentChangeRate;
  pFreeTrain->InContinuedSeconds = InContinuedSeconds;
  pFreeTrain->ChangeContinuedSeconds = ChangeContinuedSeconds;
  pFreeTrain->OutContinuedSeconds = OutContinuedSeconds;
  strcpy(pFreeTrain->InCurrentChangeRateList,InCurrentChangeRateList.GetBuffer(0));
  strcpy(pFreeTrain->OutCurrentChangeRateList,OutCurrentChangeRateList.GetBuffer(0));
  pFreeTrain->StartupDelaySecond = g_StartupDelaySecond;
  dlg.m_pFreeTrain = pFreeTrain;
  if(dlg.DoModal()==IDOK)
  {
    InFeederUnitID = pFreeTrain->InFeederUnitID;
    OutFeederUnitID = pFreeTrain->OutFeederUnitID;
    AverCurrent = pFreeTrain->AverCurrent;
    AverAngle = pFreeTrain->AverAngle;
    CurrentChangeRate = pFreeTrain->CurrentChangeRate;

    InContinuedSeconds = pFreeTrain->InContinuedSeconds;
    ChangeContinuedSeconds = pFreeTrain->ChangeContinuedSeconds;
    OutContinuedSeconds = pFreeTrain->OutContinuedSeconds;
    InCurrentChangeRateList = pFreeTrain->InCurrentChangeRateList;
    OutCurrentChangeRateList = pFreeTrain->OutCurrentChangeRateList;
    g_StartupDelaySecond = pFreeTrain->StartupDelaySecond;

    pFreeTrain->InBeginTime = CTime::GetCurrentTime() + CTimeSpan(0,0,0,pFreeTrain->StartupDelaySecond);;
    pFreeTrain->InFinishTime = pFreeTrain->InBeginTime+CTimeSpan(0,0,0,pFreeTrain->InContinuedSeconds);
    pFreeTrain->ChangeBeginTime = pFreeTrain->InFinishTime;
    pFreeTrain->ChangeFinishTime = pFreeTrain->ChangeBeginTime+CTimeSpan(0,0,0,pFreeTrain->ChangeContinuedSeconds);
    pFreeTrain->OutBeginTime = pFreeTrain->ChangeFinishTime;
    pFreeTrain->OutFinishTime = pFreeTrain->OutBeginTime+CTimeSpan(0,0,0,pFreeTrain->OutContinuedSeconds);

    g_FreeTrainManageList.AddTail(pFreeTrain);
  
	  UpdateList();
    ::SelectListLastItem(m_ctlList);
  }
}

void CDlgFreeTrainList::OnEdit() 
{
  if(m_ctlList.GetSelectedCount()!=1)
    return;

  POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
  int item = m_ctlList.GetNextSelectedItem(pos);
  TagFreeTrainStruct *pFreeTrain = (TagFreeTrainStruct *)g_FreeTrainManageList.GetAt(g_FreeTrainManageList.FindIndex(item));

  CDlgFreeTrain dlg;
  dlg.m_pFreeTrain = pFreeTrain;
  if(dlg.DoModal()==IDOK)
	  UpdateList();
}

void CDlgFreeTrainList::OnClon() 
{
  if(m_ctlList.GetSelectedCount()!=1)
    return;

  POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
  int item = m_ctlList.GetNextSelectedItem(pos);
  struct TagFreeTrainStruct *pFreeTrain = (TagFreeTrainStruct *)g_FreeTrainManageList.GetAt(g_FreeTrainManageList.FindIndex(item));
  struct TagFreeTrainStruct *pNewFreeTrain = new TagFreeTrainStruct;
  memcpy(pNewFreeTrain,pFreeTrain,sizeof(TagFreeTrainStruct));
  g_FreeTrainManageList.AddTail(pNewFreeTrain);

	UpdateList();
  ::SelectListLastItem(m_ctlList);
}

void CDlgFreeTrainList::OnDel() 
{
  if(m_ctlList.GetSelectedCount()!=1)
    return;

  if(::AfxMessageBox("是否删除自由行车？",MB_YESNO)==IDNO)
    return;

  POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
  int item = m_ctlList.GetNextSelectedItem(pos);
  g_FreeTrainManageList.RemoveAt(g_FreeTrainManageList.FindIndex(item));

  g_LastSelected = m_ctlList.GetNextSelectedItem(pos);
	g_LastSelected++;
  UpdateList();
  ::SelectListItem(m_ctlList,g_LastSelected);
	
}

void CDlgFreeTrainList::OnCancel() 
{
  POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
  g_LastSelected = m_ctlList.GetNextSelectedItem(pos);
	
	CDialog::OnCancel();
}

void CDlgFreeTrainList::OnStartup() 
{
  if(::CheckOperateCondition()==FALSE)
    return;
	
  if(::AfxMessageBox("是否启动除自由行车？",MB_YESNO)==IDNO)
    return;

  for(int item=0;item<m_ctlList.GetItemCount();item++)
  {
	  if(m_ctlList.GetCheck(item))
    {
      POSITION pos = g_FreeTrainManageList.FindIndex(item);
      struct TagFreeTrainStruct *pFreeTrain = (TagFreeTrainStruct *)g_FreeTrainManageList.GetAt(pos);
      
      pFreeTrain->InBeginTime = CTime::GetCurrentTime() + CTimeSpan(0,0,0,pFreeTrain->StartupDelaySecond);;
      pFreeTrain->InFinishTime = pFreeTrain->InBeginTime+CTimeSpan(0,0,0,pFreeTrain->InContinuedSeconds);
      pFreeTrain->ChangeBeginTime = pFreeTrain->InFinishTime;
      pFreeTrain->ChangeFinishTime = pFreeTrain->ChangeBeginTime+CTimeSpan(0,0,0,pFreeTrain->ChangeContinuedSeconds);
      pFreeTrain->OutBeginTime = pFreeTrain->ChangeFinishTime;
      pFreeTrain->OutFinishTime = pFreeTrain->OutBeginTime+CTimeSpan(0,0,0,pFreeTrain->OutContinuedSeconds);

      struct TagFreeTrainStruct *pNewFreeTrain = new TagFreeTrainStruct;
      memcpy(pNewFreeTrain,pFreeTrain,sizeof(TagFreeTrainStruct));

      ::AddFreeTrain(pNewFreeTrain);

      g_pDlgMessageBox->DoShow("准备启动自由行车",3);
    }
  }

  OnCancel();
}

void CDlgFreeTrainList::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEdit();
	
	*pResult = 0;
}
