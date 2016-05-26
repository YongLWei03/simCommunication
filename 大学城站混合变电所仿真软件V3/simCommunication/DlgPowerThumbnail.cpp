// DlgPowerThumbnail.cpp : implementation file
//

#include "stdafx.h"
#include "simcommunication.h"
#include "DlgPowerThumbnail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef struct TagObjectStruct
{
  CRect ObjectRect;
  WORD  ObjectUnitID;
  WORD  ObjectVarID;
}
OBJECTSTRUCT;

/////////////////////////////////////////////////////////////////////////////
// CDlgPowerThumbnail dialog


CDlgPowerThumbnail::CDlgPowerThumbnail(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPowerThumbnail::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPowerThumbnail)
    m_PowerState = FALSE;
	//}}AFX_DATA_INIT
  for(int unitno=0;unitno<UNITCOUNT;unitno++)
  {
    for(int varno=0;varno<VARCOUNT;varno++)
      m_PowerStateList[unitno][varno] = 0;
  }
}


void CDlgPowerThumbnail::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPowerThumbnail)
	DDX_Control(pDX, IDC_EXPORT, m_ctlExport);
	DDX_Control(pDX, IDC_IMPORT, m_ctlImport);
	DDX_Control(pDX, IDC_WND, m_ctlWnd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPowerThumbnail, CDialog)
	//{{AFX_MSG_MAP(CDlgPowerThumbnail)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPowerThumbnail message handlers

BOOL CDlgPowerThumbnail::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_ctlWnd.SetWindowPos(&CWnd::wndTop,0,0,800,600,SWP_NOMOVE|SWP_SHOWWINDOW);
	
  CWaitCursor WaitCursor;

  LoadBitmapFile();
  LoadObjectFile();

  if(m_PowerState==TRUE)
  {
    SetWindowText("供电状态");
    m_ctlImport.ShowWindow(TRUE);
    m_ctlExport.ShowWindow(TRUE);
  }

  SetTimer(100,100,NULL);

  return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPowerThumbnail::LoadBitmapFile()
{
  CString FileNameList[5];
  FileNameList[0] = g_ProjectFilePath+"Thumbnail\\Power.0";
  FileNameList[1] = g_ProjectFilePath+"Thumbnail\\Power.1";
  FileNameList[2] = g_ProjectFilePath+"Thumbnail\\Power.2";
  FileNameList[3] = g_ProjectFilePath+"Thumbnail\\Power.3";
  FileNameList[4] = g_ProjectFilePath+"Thumbnail\\Power.4";

  CClientDC dc(&m_ctlWnd);
  for(int no=1;no<5;no++)
    ::LoadBitmapFile(FileNameList[no],m_bitmap[no]);
}

void CDlgPowerThumbnail::LoadObjectFile()
{
  CString FileNameList[5];
  FileNameList[0] = g_ProjectFilePath+"Thumbnail\\Power.0";
  FileNameList[1] = g_ProjectFilePath+"Thumbnail\\Power.1";
  FileNameList[2] = g_ProjectFilePath+"Thumbnail\\Power.2";
  FileNameList[3] = g_ProjectFilePath+"Thumbnail\\Power.3";
  FileNameList[4] = g_ProjectFilePath+"Thumbnail\\Power.4";

  CFile file;
  if(file.Open(FileNameList[0], CFile::modeRead,NULL)==TRUE)
  {
    char buffer[128];
    CArchive ar(&file,CArchive::load,128,buffer);
    WORD count;
    ar>>count;
    for(int no=0;no<count;no++)
    {
      TagObjectStruct *pObject = new TagObjectStruct;
      m_ObjectList.AddTail(pObject);
      ar>>pObject->ObjectRect;
      ar>>pObject->ObjectUnitID;
      ar>>pObject->ObjectVarID;
    }
    ar.Flush();
    ar.Close();
    file.Close();
  }
}

void CDlgPowerThumbnail::UpdateView()
{
	CClientDC dc(&m_ctlWnd); // device context for painting
  CBitmap bitmap;
  bitmap.CreateCompatibleBitmap(&dc,800,800);
  CDC memdc;
  memdc.CreateCompatibleDC(&dc);
	CBitmap *oldbitmap=memdc.SelectObject(&bitmap);

  ::BitBlt(&memdc,0,0,800,600,&m_bitmap[1],0,0,SRCCOPY);
  memdc.FillSolidRect(0,0,800,1,0x00000000);
  memdc.FillSolidRect(0,0,1,600,0x00000000);
  memdc.FillSolidRect(800-1,0,1,600,0x00000000);
  memdc.FillSolidRect(0,600-1,800,1,0x00000000);

  if(m_PowerState==TRUE)//供电状态
  {
    POSITION pos = m_ObjectList.GetHeadPosition();
    while(pos)
    {
      TagObjectStruct *pObject = (TagObjectStruct *)m_ObjectList.GetNext(pos);
    
      WORD bitmapno = 0;
      WORD UnitID = pObject->ObjectUnitID;
      WORD VarID = pObject->ObjectVarID%1000;
      if(m_PowerStateList[UnitID][VarID]!=0)//合位
        bitmapno = 3;
      else
        bitmapno = 1;

      ::BitBlt(&memdc,pObject->ObjectRect.left,pObject->ObjectRect.top,pObject->ObjectRect.Width()+1,pObject->ObjectRect.Height()+1,&m_bitmap[bitmapno],pObject->ObjectRect.left,pObject->ObjectRect.top,SRCCOPY);
    }
  }

  dc.BitBlt(0,0,800,600,&memdc,0,0,SRCCOPY);

  memdc.SelectObject(oldbitmap);
  memdc.DeleteDC();
  bitmap.DeleteObject();;
}

void CDlgPowerThumbnail::OnTimer(UINT nIDEvent) 
{
  UpdateView();

	CDialog::OnTimer(nIDEvent);
}

void CDlgPowerThumbnail::OnDestroy() 
{
	CDialog::OnDestroy();
	
  for(int no=1;no<5;no++)
    m_bitmap[no].DeleteObject();

  while(m_ObjectList.GetCount())
    delete (TagObjectStruct*) m_ObjectList.RemoveHead();
}

void CDlgPowerThumbnail::OnImport() 
{
  for(int unitno=0;unitno<UNITCOUNT;unitno++)
    for(int varno=0;varno<VARCOUNT;varno++)
      m_PowerStateList[unitno][varno] = g_Unit[unitno].VarGroup[1].Var[varno].Value;
}

void CDlgPowerThumbnail::OnExport() 
{
  for(int unitno=0;unitno<UNITCOUNT;unitno++)
    for(int varno=0;varno<VARCOUNT;varno++)
      g_Unit[unitno].VarGroup[1].Var[varno].Value = m_PowerStateList[unitno][varno];
}

void CDlgPowerThumbnail::OnMouseMove(UINT nFlags, CPoint point) 
{
  ::GetCursorPos(&point);
  m_ctlWnd.ScreenToClient(&point);
  
  HCURSOR hcursor;
  POSITION pos = m_ObjectList.GetTailPosition();
  while(pos)
  {
    TagObjectStruct *pObject = (TagObjectStruct *)m_ObjectList.GetPrev(pos);
    if(pObject->ObjectRect.PtInRect(point))
    {
      hcursor = (AfxGetApp()->LoadCursor("PUSH"));
      break;
    }
    else
      hcursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
  }
  SetCursor(hcursor);

	CDialog::OnMouseMove(nFlags, point);
}

void CDlgPowerThumbnail::OnLButtonUp(UINT nFlags, CPoint point) 
{
  ::GetCursorPos(&point);
  m_ctlWnd.ScreenToClient(&point);
  
  if(m_PowerState==TRUE)//供电状态
  {
    POSITION pos = m_ObjectList.GetTailPosition();
    while(pos)
    {
      TagObjectStruct *pObject = (TagObjectStruct *)m_ObjectList.GetPrev(pos);
      if(pObject->ObjectRect.PtInRect(point))
      {
        WORD UnitID = pObject->ObjectUnitID;
        WORD VarID = pObject->ObjectVarID%1000;
        if(m_PowerStateList[UnitID][VarID]==0)
          m_PowerStateList[UnitID][VarID] = 1;
        else
          m_PowerStateList[UnitID][VarID] = 0;

        if(VarID==0)
          m_PowerStateList[UnitID][VarID+1] = !m_PowerStateList[UnitID][VarID];
        
        break;
      }
    }
  }
  	
	CDialog::OnLButtonUp(nFlags, point);
}
