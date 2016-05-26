// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "simCommunication.h"

#include "MainFrm.h"
#include "simCommunicationSOEListView.h"
#include "simCommunicationVarListView.h"
#include "simCommunicationUnitListView.h"

#include "DlgUnitEdit.h"
#include "DlgOperateUnit.h"
#include "DlgOperateParameter.h"
#include "DlgMessageBox.h"
#include "DlgListSelect.h"
#include "DlgInlineVotlage.h"
#include "DlgPTVotlage.h"

#include "DlgSceneFileSelect.h"
#include "DlgFreeTransmissionList.h"
#include "DlgFaultTransmission.h"
#include "DlgPowerMonitor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL WINAPI CheckOperateCondition()
{
  BOOL Power1B;
  BOOL Power2B;

  if((g_Unit[3].VarGroup[0].Var[0].Value==0)&&(g_Unit[3].VarGroup[0].Var[1].Value==0)&&(g_Unit[3].VarGroup[0].Var[2].Value==0))
    Power1B = FALSE;
  else
    Power1B = TRUE;

  if((g_Unit[6].VarGroup[0].Var[0].Value==0)&&(g_Unit[6].VarGroup[0].Var[1].Value==0)&&(g_Unit[6].VarGroup[0].Var[2].Value==0))
    Power2B = FALSE;
  else
    Power2B = TRUE;

  if((Power1B==FALSE)&&(Power2B==FALSE))
  {
    ::AfxMessageBox("35kV母线电压异常");
    return FALSE;;
  }

  return TRUE;;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_COMMAND(ID_OPERATER_REVERT, OnOperaterRevert)
	ON_UPDATE_COMMAND_UI(ID_OPERATER_REVERT, OnUpdateOperaterRevert)
	ON_COMMAND(ID_OPERATER_ON, OnOperaterOn)
	ON_UPDATE_COMMAND_UI(ID_OPERATER_ON, OnUpdateOperaterOn)
	ON_COMMAND(ID_OPERATER_OFF, OnOperaterOff)
	ON_UPDATE_COMMAND_UI(ID_OPERATER_OFF, OnUpdateOperaterOff)
	ON_COMMAND(ID_OPERATER_LOCAL, OnOperaterLocal)
	ON_UPDATE_COMMAND_UI(ID_OPERATER_LOCAL, OnUpdateOperaterLocal)
	ON_COMMAND(ID_OPERATER_FAR, OnOperaterFar)
	ON_UPDATE_COMMAND_UI(ID_OPERATER_FAR, OnUpdateOperaterFar)
	ON_COMMAND(ID_OPERATER_PARAMETER, OnOperaterParameter)
	ON_UPDATE_COMMAND_UI(ID_OPERATER_PARAMETER, OnUpdateOperaterParameter)
	ON_COMMAND(ID_OPERATER_YX, OnOperaterYx)
	ON_UPDATE_COMMAND_UI(ID_OPERATER_YX, OnUpdateOperaterYx)
	ON_COMMAND(ID_EDIT_UNIT, OnEditUnit)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNIT, OnUpdateEditUnit)
	ON_COMMAND(ID_SHOW_WINDOW, OnShowWindow)
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_TRANSMISSION_FAULT, OnTransmissionFault)
	ON_UPDATE_COMMAND_UI(ID_TRANSMISSION_FAULT, OnUpdateTransmissionFault)
	ON_COMMAND(ID_FILE_LOAD, OnFileLoad)
	ON_COMMAND(ID_HANDCART_ON, OnHandcartOn)
	ON_UPDATE_COMMAND_UI(ID_HANDCART_ON, OnUpdateHandcartOn)
	ON_COMMAND(ID_HANDCART_OFF, OnHandcartOff)
	ON_UPDATE_COMMAND_UI(ID_HANDCART_OFF, OnUpdateHandcartOff)
	ON_COMMAND(ID_SCENE_OPERATE, OnSceneOperate)
	ON_UPDATE_COMMAND_UI(ID_SCENE_OPERATE, OnUpdateSceneOperate)
	ON_COMMAND(ID_PARAMETER_LOAD, OnParameterLoad)
	ON_COMMAND(ID_PARAMETER_SAVE, OnParameterSave)
	ON_COMMAND(ID_OPERATER_YX_LOCK, OnOperaterYxLock)
	ON_COMMAND(ID_OPERATER_YX_UNLOCK, OnOperaterYxUnlock)
	ON_COMMAND(ID_TRANSMISSION_FEED, OnTransmissionFeed)
	ON_UPDATE_COMMAND_UI(ID_TRANSMISSION_FEED, OnUpdateTransmissionFeed)
	ON_COMMAND(ID_POWER_CLEAR, OnPowerClear)
	ON_UPDATE_COMMAND_UI(ID_POWER_CLEAR, OnUpdatePowerClear)
	ON_COMMAND(ID_IN_VOTLAGE, OnInVotlage)
	ON_COMMAND(ID_PT_VOTLAGE, OnPtVotlage)
	ON_COMMAND(ID_POWER_MONITOR, OnPowerMonitor)
	ON_UPDATE_COMMAND_UI(ID_POWER_MONITOR, OnUpdatePowerMonitor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,           // status line indicator
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS);
	m_wndToolBar.LoadToolBar(IDR_MAINFRAME);
  m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	
	m_wndStatusBar.Create(this);
  m_wndStatusBar.SetIndicators(indicators,sizeof(indicators)/sizeof(UINT));

	g_pMainFrame->SetWindowText("通讯仿真 ["+g_PowerNameList[g_PowerID]+"]");

  m_TrayIcon.Create(this,WM_ICON_NOTIFY,"通讯仿真 ["+g_PowerNameList[g_PowerID]+"]",NULL,IDR_POPUP_SYSTEMTRAY);
  m_TrayIcon.SetMenuDefaultItem(0,TRUE);

	SetTimer(500,500,NULL);

  return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	m_wndSplitter.CreateStatic(this, 1, 2);
	m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CsimCommunicationUnitListView), CSize(420, 100), pContext);

	m_wndListSplitter.CreateStatic(&m_wndSplitter, 2, 1,	WS_CHILD | WS_VISIBLE | WS_BORDER,m_wndSplitter.IdFromRowCol(0, 1));
	m_wndListSplitter.CreateView(0, 0, RUNTIME_CLASS(CsimCommunicationVarListView), CSize(100, 480), pContext);
	m_wndListSplitter.CreateView(1, 0, RUNTIME_CLASS(CsimCommunicationSOEListView), CSize(100, 100), pContext);

	m_pUnitListView = (CsimCommunicationUnitListView*)m_wndSplitter.GetPane(0, 0);
	m_pVarListView = (CsimCommunicationVarListView*)m_wndListSplitter.GetPane(0, 0);
	m_pSOEListView = (CsimCommunicationSOEListView*)m_wndListSplitter.GetPane(1, 0);
  m_pUnitListView->m_pSOEListView=m_pSOEListView;
  m_pUnitListView->m_pVarListView=m_pVarListView;
  m_pSOEListView->m_pUnitListView=m_pUnitListView;
  m_pVarListView->m_pUnitListView=m_pUnitListView;
  m_pVarListView->SetFocus();

	return TRUE;
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
  if(g_NetworkManageClientSocket.m_pClientSocketThread->m_Run==TRUE)
  {
    CString FileName = "C:\\simPower2000\\simPower.exit";
    CFile File;
	  CFileStatus FileStatus;
    if(File.GetStatus(FileName,FileStatus)==TRUE)
    {
	    m_TrayIcon.HideIcon();
      ShowWindow(SW_HIDE);
      g_NetworkManageClientSocket.m_pClientSocketThread->m_Run = FALSE;
	    ShowWindow(SW_HIDE);
      Sleep(1000);
      return;
    }
  }

  ::ReadUnitFromShareMemory();

  if(g_SceneOperateFileName!="")
  {
    ::ClearSceneOperate(&g_SceneOperate);

    if(::LoadSceneOperateFile(&g_SceneOperate,g_SceneOperateFileName)==TRUE)
    {
      ::StartupSceneOperate(&g_SceneOperate);

      ::WriteUnitToShareMemory();

      g_ProtocolTH104[255].m_SendAllFloatDataTime = 0;
      g_ProtocolTH104[255].m_SendAllBitDataTime = 0;
      g_ProtocolTH104[255].m_SendAllLongDataTime = 0;
    }
    
    g_SceneOperateFileName = "";
  }
  if(g_SceneOperateID>=0)
  {
    POSITION pos  = g_SceneOperate.ScenePictureList.FindIndex(g_SceneOperateID);
    if(pos)
    {
      TagScenePictureStruct *pScenePicture = (TagScenePictureStruct*)g_SceneOperate.ScenePictureList.GetAt(pos);
      switch(pScenePicture->SceneType)
      {
        case 9://电压设置
        {
          CNewString ns(pScenePicture->SceneInlineVotlage,"\t","\r\n");
	        g_InlineVotlage.Ua1 = ns.GetFloatValue(0,0);
	        g_InlineVotlage.Ub1 = ns.GetFloatValue(0,1);
	        g_InlineVotlage.Uc1 = ns.GetFloatValue(0,2);
	        g_InlineVotlage.Ua2 = ns.GetFloatValue(0,3);
	        g_InlineVotlage.Ub2 = ns.GetFloatValue(0,4);
	        g_InlineVotlage.Uc2 = ns.GetFloatValue(0,5);
	        g_InlineVotlage.T1HiVoltageChangeRate = ns.GetFloatValue(1,0);
	        g_InlineVotlage.T1LoVoltageChangeRate = ns.GetFloatValue(1,1);
	        g_InlineVotlage.T2HiVoltageChangeRate = ns.GetFloatValue(1,2);
	        g_InlineVotlage.T2LoVoltageChangeRate = ns.GetFloatValue(1,3);
          break;
        }
        case 10://压互设置
        {
          CNewString ns(pScenePicture->ScenePTVotlage,"\t","\r\n");
	        g_PTVoltage.T1HiVoltageValueK    = ns.GetFloatValue(0,0);
	        g_PTVoltage.T1HiCurrentMaxValue  = ns.GetFloatValue(0,1);
	        g_PTVoltage.T2HiVoltageValueK    = ns.GetFloatValue(0,2);
	        g_PTVoltage.T2HiCurrentMaxValue  = ns.GetFloatValue(0,3);
	        g_PTVoltage.T1LoVoltageValueK   = ns.GetFloatValue(1,0);
	        g_PTVoltage.T1LoCurrentMaxValue = ns.GetFloatValue(1,1);
	        g_PTVoltage.T2LoVoltageValueK   = ns.GetFloatValue(2,0);
	        g_PTVoltage.T2LoCurrentMaxValue = ns.GetFloatValue(2,1);
          break;
        }
        case 11://电流设置
        {
          CNewString ns(pScenePicture->SceneInlineVotlage,"\t","\r\n");
	        g_Current.Reserved1 = ns.GetFloatValue(0,0);
	        g_Current.Reserved2 = ns.GetFloatValue(0,1);
	        g_Current.Reserved3 = ns.GetFloatValue(0,2);
	        g_Current.Reserved4 = ns.GetFloatValue(0,3);
	        g_Current.NoloadCurrentChangeMaxValue = ns.GetFloatValue(0,4);
          break;
        }
        case 12://补偿设置
        {
          CNewString ns(pScenePicture->SceneCompensation,"\t","\r\n");
          {
            g_Compensation[0].CompensationType = ns.GetIntegerValue(0,0);
            g_Compensation[0].CompensationUnitID = ns.GetIntegerValue(0,1);
            g_Compensation[0].CompensationBranchValueZ[0] = ns.GetFloatValue(0,2);
            g_Compensation[0].CompensationBranchValueZ[1] = ns.GetFloatValue(0,3);
            g_Compensation[0].CompensationBranchValueZ[2] = ns.GetFloatValue(0,4);
            g_Compensation[0].CompensationBranchValueZ[3] = ns.GetFloatValue(0,5);
            g_Compensation[0].CompensationBranchValueK[0] = ns.GetFloatValue(0,6);
            g_Compensation[0].CompensationBranchValueK[1] = ns.GetFloatValue(0,7);
            g_Compensation[0].CompensationBranchValueK[2] = ns.GetFloatValue(0,8);
            g_Compensation[0].CompensationBranchValueK[3] = ns.GetFloatValue(0,9);

            CString CompensationBranchVarI0 = ns.GetLine(1);
            CString CompensationBranchVarI1 = ns.GetLine(2);
            CString CompensationBranchVarI2 = ns.GetLine(3);
            CString CompensationBranchVarI3 = ns.GetLine(4);
            strcpy(g_Compensation[0].CompensationBranchVarI[0],CompensationBranchVarI0.GetBuffer(0));
            strcpy(g_Compensation[0].CompensationBranchVarI[1],CompensationBranchVarI1.GetBuffer(0));
            strcpy(g_Compensation[0].CompensationBranchVarI[2],CompensationBranchVarI2.GetBuffer(0));
            strcpy(g_Compensation[0].CompensationBranchVarI[3],CompensationBranchVarI3.GetBuffer(0));

            CString CompensationBranchVarQ0 = ns.GetLine(5);
            CString CompensationBranchVarQ1 = ns.GetLine(6);
            CString CompensationBranchVarQ2 = ns.GetLine(7);
            CString CompensationBranchVarQ3 = ns.GetLine(8);
            strcpy(g_Compensation[0].CompensationBranchVarQ[0],CompensationBranchVarQ0.GetBuffer(0));
            strcpy(g_Compensation[0].CompensationBranchVarQ[1],CompensationBranchVarQ1.GetBuffer(0));
            strcpy(g_Compensation[0].CompensationBranchVarQ[2],CompensationBranchVarQ2.GetBuffer(0));
            strcpy(g_Compensation[0].CompensationBranchVarQ[3],CompensationBranchVarQ3.GetBuffer(0));

            CString CompensationBranchPowerPathVarList0 = ns.GetLine(9);
            CString CompensationBranchPowerPathVarList1 = ns.GetLine(10);
            CString CompensationBranchPowerPathVarList2 = ns.GetLine(11);
            CString CompensationBranchPowerPathVarList3 = ns.GetLine(12);
            strcpy(g_Compensation[0].CompensationBranchPowerPathVarList[0],CompensationBranchPowerPathVarList0.GetBuffer(0));
            strcpy(g_Compensation[0].CompensationBranchPowerPathVarList[1],CompensationBranchPowerPathVarList1.GetBuffer(0));
            strcpy(g_Compensation[0].CompensationBranchPowerPathVarList[2],CompensationBranchPowerPathVarList2.GetBuffer(0));
            strcpy(g_Compensation[0].CompensationBranchPowerPathVarList[3],CompensationBranchPowerPathVarList3.GetBuffer(0));
          }
          {
            g_Compensation[1].CompensationType = ns.GetIntegerValue(13,0);
            g_Compensation[1].CompensationUnitID = ns.GetIntegerValue(13,1);
            g_Compensation[1].CompensationBranchValueZ[0] = ns.GetFloatValue(13,2);
            g_Compensation[1].CompensationBranchValueZ[1] = ns.GetFloatValue(13,3);
            g_Compensation[1].CompensationBranchValueZ[2] = ns.GetFloatValue(13,4);
            g_Compensation[1].CompensationBranchValueZ[3] = ns.GetFloatValue(13,5);
            g_Compensation[1].CompensationBranchValueK[0] = ns.GetFloatValue(13,6);
            g_Compensation[1].CompensationBranchValueK[1] = ns.GetFloatValue(13,7);
            g_Compensation[1].CompensationBranchValueK[2] = ns.GetFloatValue(13,8);
            g_Compensation[1].CompensationBranchValueK[3] = ns.GetFloatValue(0,9);

            CString CompensationBranchVarI0 = ns.GetLine(14);
            CString CompensationBranchVarI1 = ns.GetLine(15);
            CString CompensationBranchVarI2 = ns.GetLine(16);
            CString CompensationBranchVarI3 = ns.GetLine(17);
            strcpy(g_Compensation[1].CompensationBranchVarI[0],CompensationBranchVarI0.GetBuffer(0));
            strcpy(g_Compensation[1].CompensationBranchVarI[1],CompensationBranchVarI1.GetBuffer(0));
            strcpy(g_Compensation[1].CompensationBranchVarI[2],CompensationBranchVarI2.GetBuffer(0));
            strcpy(g_Compensation[1].CompensationBranchVarI[3],CompensationBranchVarI3.GetBuffer(0));

            CString CompensationBranchVarQ0 = ns.GetLine(18);
            CString CompensationBranchVarQ1 = ns.GetLine(19);
            CString CompensationBranchVarQ2 = ns.GetLine(20);
            CString CompensationBranchVarQ3 = ns.GetLine(21);
            strcpy(g_Compensation[1].CompensationBranchVarQ[0],CompensationBranchVarQ0.GetBuffer(0));
            strcpy(g_Compensation[1].CompensationBranchVarQ[1],CompensationBranchVarQ1.GetBuffer(0));
            strcpy(g_Compensation[1].CompensationBranchVarQ[2],CompensationBranchVarQ2.GetBuffer(0));
            strcpy(g_Compensation[1].CompensationBranchVarQ[3],CompensationBranchVarQ3.GetBuffer(0));

            CString CompensationBranchPowerPathVarList0 = ns.GetLine(22);
            CString CompensationBranchPowerPathVarList1 = ns.GetLine(23);
            CString CompensationBranchPowerPathVarList2 = ns.GetLine(24);
            CString CompensationBranchPowerPathVarList3 = ns.GetLine(25);
            strcpy(g_Compensation[1].CompensationBranchPowerPathVarList[0],CompensationBranchPowerPathVarList0.GetBuffer(0));
            strcpy(g_Compensation[1].CompensationBranchPowerPathVarList[1],CompensationBranchPowerPathVarList1.GetBuffer(0));
            strcpy(g_Compensation[1].CompensationBranchPowerPathVarList[2],CompensationBranchPowerPathVarList2.GetBuffer(0));
            strcpy(g_Compensation[1].CompensationBranchPowerPathVarList[3],CompensationBranchPowerPathVarList3.GetBuffer(0));
          }
          break;
        }
        case 13://传动设置
        {
          CNewString ns(pScenePicture->SceneTransmission,"\t","\r\n");
          BOOL EnableFreeTransmission = ns.GetIntegerValue(0,0);
          BOOL EnableFaultTransmission = ns.GetIntegerValue(0,1);
          if(EnableFreeTransmission==TRUE)
          {
            struct TagFreeTransmissionStruct *pFreeTransmission = new TagFreeTransmissionStruct;

            pFreeTransmission->FeederUnitID = ns.GetIntegerValue(1,0);
            pFreeTransmission->AverCurrent = ns.GetFloatValue(1,1);
            pFreeTransmission->AverAngle = ns.GetFloatValue(1,2);
	          pFreeTransmission->CurrentChangeRate = ns.GetFloatValue(1,3);
            pFreeTransmission->ContinuedSeconds = ns.GetIntegerValue(1,4);
	          pFreeTransmission->StartupDelaySecond = ns.GetIntegerValue(1,5);
            CString CurrentChangeRateList = ns.GetLine(2);
            strcpy(pFreeTransmission->CurrentChangeRateList,CurrentChangeRateList.GetBuffer(0));

            pFreeTransmission->BeginTime = CTime::GetCurrentTime() + CTimeSpan(0,0,0,pFreeTransmission->StartupDelaySecond);
            pFreeTransmission->FinishTime = pFreeTransmission->BeginTime + CTimeSpan(0,0,0,pFreeTransmission->ContinuedSeconds);

            ::AddFreeTransmission(pFreeTransmission);
          }
          if(EnableFaultTransmission==TRUE)
          {
            CString FaultTransmissionDataFileName = ns.GetLine(3);
            strcpy(g_FaultTransmission.FaultTransmissionDataFileName,FaultTransmissionDataFileName.GetBuffer(0));
            g_FaultTransmission.StartupDelaySecond = ns.GetIntegerValue(4,0);
            g_FaultTransmission.BeginTime = CTime::GetCurrentTime() + CTimeSpan(0,0,0,g_FaultTransmission.StartupDelaySecond);;
            g_FaultTransmission.FaultTransmissionState = 1;//准备读文件
          }
          break;
        }
        case 14://行车设置
        {
          CNewString ns(pScenePicture->SceneTrain,"\t","\r\n");
          BOOL EnableFreeTrain = ns.GetIntegerValue(0,0);
          BOOL EnableSimTrain = ns.GetIntegerValue(0,1);
          if(EnableFreeTrain==TRUE)
          {
            struct TagFreeTrainStruct *pFreeTrain = new TagFreeTrainStruct;
            pFreeTrain->InFeederUnitID = ns.GetIntegerValue(1,0);
            pFreeTrain->OutFeederUnitID = ns.GetIntegerValue(1,1);
            pFreeTrain->AverCurrent = ns.GetFloatValue(1,2);
            pFreeTrain->AverAngle = ns.GetFloatValue(1,3);
	          pFreeTrain->CurrentChangeRate = ns.GetFloatValue(1,4);
            pFreeTrain->InContinuedSeconds = ns.GetIntegerValue(1,5);
            pFreeTrain->OutContinuedSeconds = ns.GetIntegerValue(1,6);
            pFreeTrain->ChangeContinuedSeconds = ns.GetIntegerValue(1,7);
	          pFreeTrain->StartupDelaySecond = ns.GetIntegerValue(1,8);
            CString InCurrentChangeRateList = ns.GetLine(2);
            strcpy(pFreeTrain->InCurrentChangeRateList,InCurrentChangeRateList.GetBuffer(0));
            CString OutCurrentChangeRateList = ns.GetLine(3);
            strcpy(pFreeTrain->OutCurrentChangeRateList,OutCurrentChangeRateList.GetBuffer(0));

            pFreeTrain->InBeginTime = CTime::GetCurrentTime() + CTimeSpan(0,0,0,pFreeTrain->StartupDelaySecond);;
            pFreeTrain->InFinishTime = pFreeTrain->InBeginTime+CTimeSpan(0,0,0,pFreeTrain->InContinuedSeconds);
            pFreeTrain->ChangeBeginTime = pFreeTrain->InFinishTime;
            pFreeTrain->ChangeFinishTime = pFreeTrain->ChangeBeginTime+CTimeSpan(0,0,0,pFreeTrain->ChangeContinuedSeconds);
            pFreeTrain->OutBeginTime = pFreeTrain->ChangeFinishTime;
            pFreeTrain->OutFinishTime = pFreeTrain->OutBeginTime+CTimeSpan(0,0,0,pFreeTrain->OutContinuedSeconds);

            ::AddFreeTrain(pFreeTrain);
          }
          if(EnableSimTrain==TRUE)
          {
            CString SimTrainDataFileName = ns.GetLine(4);
            strcpy(g_SimTrain.SimTrainDataFileName,SimTrainDataFileName.GetBuffer(0));
            g_SimTrain.StartupDelaySecond = ns.GetIntegerValue(5,0);
            g_SimTrain.BeginTime = CTime::GetCurrentTime() + CTimeSpan(0,0,0,g_SimTrain.StartupDelaySecond);;
            g_SimTrain.SimTrainDataCount = 0;
            g_SimTrain.SimTrainDataCurrentPos = 0;
            g_SimTrain.SimTrainState = 1;//准备读文件
          }
          break;
        }
        case 15://场景设置
        {
          CNewString ns(pScenePicture->SceneFileLoad,"\t","\r\n");
          CString FileName = ns.GetWord(0,0);
          if(FileName!="")
          {
            CString SceneFileName = g_ProjectFilePath+"SceneOperate\\"+FileName+".scn";
            WORD StartupDelaySecond = ns.GetIntegerValue(0,1);

            g_SceneOperateFileName = SceneFileName;
          }
          break;
        }
      }
    }

    g_SceneOperateID = -1;
  }

  {
	  static BOOL m_Count;
    if(m_Count)
      m_Count = FALSE;
    else
      m_Count = TRUE;

    if(g_NetworkManageClientSocket.m_Connect==TRUE)
    {
      if(m_Count)
        m_TrayIcon.SetIcon(IDR_POWER_NORMAL0);
      else
        m_TrayIcon.SetIcon(IDR_POWER_NORMAL1);
    }
    else
    {
      if(m_Count)
        m_TrayIcon.SetIcon(IDR_POWER_FAIL0);
      else
        m_TrayIcon.SetIcon(IDR_POWER_FAIL1);
    }
  }

  CListCtrl &ctlUnitList = m_pUnitListView->GetListCtrl();

  m_pUnitListView->UpdateListView();
  m_pSOEListView->UpdateListView();
  m_pVarListView->UpdateListView();

  if(g_DistributionOperateMode==TRUE)//分散操作模式
    m_wndStatusBar.SetPaneText(1,"分散操作模式,允许所有人员进行监控操作");
  else
  {	
    for(int no=0;no<COMPUTER_COUNT;no++)
    {
      if((no!=0)&&(no!=255)&&(g_ProtocolTH104[no].m_OperateEnable==TRUE))
      {
        m_wndStatusBar.SetPaneText(1,::GetFormatText("集中操作模式,允许<%d#>学员进行监控操作",no));
        break;
      }
      else
        m_wndStatusBar.SetPaneText(1,"集中操作模式,只允许教员进行监控操作");
    }
  }
	if(g_NetworkManageClientSocket.m_pClientSocketThread->m_Run==FALSE)
  {
    if(g_SimCommunicationMain==TRUE)
      ::SaveUnitList();

    g_pPowerThread->KillThread();
    g_pPowerThread=NULL;

    g_pDlgMessageBox->DestroyWindow();
    delete g_pDlgMessageBox;

    g_NetworkManageClientSocket.m_pClientSocketThread->KillThread();
    g_NetworkManageClientSocket.m_pClientSocketThread=NULL;

    g_pServerSocketTH104Thread->KillThread();
    g_pServerSocketTH104Thread=NULL;

    for(int no=0;no<COMPUTER_COUNT;no++)
    {
      if(g_ProtocolTH104[no].m_pCommThread!=NULL)
      {
        //g_ProtocolTH104[no].m_pCommThread->SuspendThread();
        g_ProtocolTH104[no].m_pCommThread->KillThread();
        g_ProtocolTH104[no].m_pCommThread=NULL;
      }
    }
	  delete this;
    return;
  }

	CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnClose() 
{
  if(g_EnableStudentExitOperate==FALSE)
  {
    g_pDlgMessageBox->DoShow("禁止学员退出系统",5);
    return;
  }

  if(::AfxMessageBox("是否退出系统？",MB_YESNO)==IDNO)
    return;
	
  g_NetworkManageClientSocket.m_pClientSocketThread->m_Run = FALSE;
	ShowWindow(SW_HIDE);
  Sleep(1000);

  return;

	CFrameWnd::OnClose();
}

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch(wParam)
  {
    case PM_SET_PANE_TEXT:
    {  
      CString *pText = (CString *)lParam;
      if(m_wndStatusBar.GetSafeHwnd())
        m_wndStatusBar.SetPaneText(0,*pText);
      return TRUE;  
      break;
    }
  }	
	return CFrameWnd::OnCommand(wParam, lParam);
}

void CMainFrame::OnShowWindow() 
{
  ShowWindow(SW_SHOWMAXIMIZED);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);

  if(nType==SIZE_MINIMIZED)
    ShowWindow(SW_HIDE);
}

void CMainFrame::OnEditUnit() 
{
  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();
  POSITION pos = ctlList.GetFirstSelectedItemPosition();
  int UnitID = ctlList.GetNextSelectedItem(pos)+1;

  CDlgUnitEdit dlg;
  dlg.m_UnitType = g_Unit[UnitID].UnitType;
  dlg.m_UnitName = g_Unit[UnitID].UnitName;
  dlg.m_UnitCT = g_Unit[UnitID].UnitCT;
  dlg.m_UnitPT = g_Unit[UnitID].UnitPT;
  for(int vargroupno=0;vargroupno<VARGROUPCOUNT;vargroupno++)
  {
    for(int varno=0;varno<VARCOUNT;varno++)
    {
      dlg.m_VarName[vargroupno][varno] = g_Unit[UnitID].VarGroup[vargroupno].Var[varno].VarName;
      dlg.m_TextFormat[vargroupno][varno] = g_Unit[UnitID].VarGroup[vargroupno].Var[varno].TextFormat;
    }
  }

  if(dlg.DoModal()==IDOK)
  {
    POSITION pos = ctlList.GetFirstSelectedItemPosition();
    while(pos)
    {
      int UnitID = ctlList.GetNextSelectedItem(pos)+1;
      g_Unit[UnitID].UnitType = dlg.m_UnitType;
      g_Unit[UnitID].UnitName = dlg.m_UnitName;
      g_Unit[UnitID].UnitCT = dlg.m_UnitCT;
      g_Unit[UnitID].UnitPT = dlg.m_UnitPT;

      for(int vargroupno=0;vargroupno<VARGROUPCOUNT;vargroupno++)
      {
        for(int varno=0;varno<VARCOUNT;varno++)
        {
          g_Unit[UnitID].VarGroup[vargroupno].Var[varno].VarName = dlg.m_VarName[vargroupno][varno];
          g_Unit[UnitID].VarGroup[vargroupno].Var[varno].TextFormat = dlg.m_TextFormat[vargroupno][varno];
        }
      }
    }
  }
}

void CMainFrame::OnUpdateEditUnit(CCmdUI* pCmdUI) 
{
  if(g_SimCommunicationMain==FALSE)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();
  if(ctlList.GetSelectedCount()==1)
    pCmdUI->Enable(TRUE);
  else
    pCmdUI->Enable(FALSE);
}

void CMainFrame::OnOperaterRevert() 
{
  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();

  POSITION pos = ctlList.GetFirstSelectedItemPosition();
  while(pos)
  {
    int UnitID = ctlList.GetNextSelectedItem(pos)+1;

    g_Unit[UnitID].VarGroup[1].Var[30].Value = 0;
    g_Unit[UnitID].VarGroup[1].Var[31].Value = 0;
  }

  ::WriteUnitToShareMemory();

  g_ProtocolTH104[255].m_SendAllFloatDataTime = 0;
  g_ProtocolTH104[255].m_SendAllBitDataTime = 0;
  g_ProtocolTH104[255].m_SendAllLongDataTime = 0;
}

void CMainFrame::OnUpdateOperaterRevert(CCmdUI* pCmdUI) 
{
  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();
  pCmdUI->Enable(ctlList.GetSelectedCount());
}

void CMainFrame::OnOperaterOn() 
{
  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();

  POSITION pos = ctlList.GetFirstSelectedItemPosition();
  while(pos)
  {
    WORD UnitID = ctlList.GetNextSelectedItem(pos)+1;

    float ActionValue[VARCOUNT];
    memset(&ActionValue,0x00,sizeof(ActionValue));

    switch(g_Unit[UnitID].UnitType)
    {
      case 1://差动
        break;
      case 2://高后备
      case 3://低后备
      case 4://线路
      case 5://电容器
      case 6://综合测控 
      {
        if((g_Unit[UnitID].VarGroup[1].Var[0].Value!=g_Unit[UnitID].VarGroup[1].Var[1].Value)&&(g_Unit[UnitID].VarLock[0]==FALSE))//无控断和无拒动
        {
          g_Unit[UnitID].VarGroup[1].Var[0].Value = 1;
          g_Unit[UnitID].VarGroup[1].Var[1].Value = 0;
          ::AddPresetVar(UnitID,1,3,0,CTime::GetCurrentTime());
          ::AddPresetVar(UnitID,1,3,1,CTime::GetCurrentTime()+CTimeSpan(10));
          g_Unit[UnitID].VarGroup[1].Var[23].Value = 0;
        }
        else
          g_Unit[UnitID].VarGroup[1].Var[23].Value = 1;
        ::ProductActionValue(UnitID,ActionValue);
        ::ProductSOEReport(UnitID,34,CTime::GetCurrentTime(),(WORD)::GetTickCount()%1000,1,ActionValue);
        break;
      }
    }
  }

  ::WriteUnitToShareMemory();

  g_ProtocolTH104[255].m_SendAllFloatDataTime = 0;
  g_ProtocolTH104[255].m_SendAllBitDataTime = 0;
  g_ProtocolTH104[255].m_SendAllLongDataTime = 0;
}

void CMainFrame::OnUpdateOperaterOn(CCmdUI* pCmdUI) 
{
  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();
  pCmdUI->Enable(ctlList.GetSelectedCount()>0);
}

void CMainFrame::OnOperaterOff() 
{
  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();

  POSITION pos = ctlList.GetFirstSelectedItemPosition();
  while(pos)
  {
    WORD UnitID = ctlList.GetNextSelectedItem(pos)+1;

    float ActionValue[VARCOUNT];
    memset(&ActionValue,0x00,sizeof(ActionValue));

    switch(g_Unit[UnitID].UnitType)
    {
      case 1://差动
        break;
      case 2://高后备
      case 3://低后备
      case 4://线路
      case 5://电容器
      case 6://综合测控 
      {
        if((g_Unit[UnitID].VarGroup[1].Var[0].Value!=g_Unit[UnitID].VarGroup[1].Var[1].Value)&&(g_Unit[UnitID].VarLock[0]==FALSE))//无控断和无拒动
        {
          g_Unit[UnitID].VarGroup[1].Var[0].Value = 0;
          g_Unit[UnitID].VarGroup[1].Var[1].Value = 1;
          g_Unit[UnitID].VarGroup[1].Var[23].Value = 0;
        }
        else
          g_Unit[UnitID].VarGroup[1].Var[23].Value = 1;
        ::ProductActionValue(UnitID,ActionValue);
        ::ProductSOEReport(UnitID,35,CTime::GetCurrentTime(),(WORD)::GetTickCount()%1000,1,ActionValue);
        break;
      }
    }
  }

  ::WriteUnitToShareMemory();

  g_ProtocolTH104[255].m_SendAllFloatDataTime = 0;
  g_ProtocolTH104[255].m_SendAllBitDataTime = 0;
  g_ProtocolTH104[255].m_SendAllLongDataTime = 0;
}

void CMainFrame::OnUpdateOperaterOff(CCmdUI* pCmdUI) 
{
  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();
  pCmdUI->Enable(ctlList.GetSelectedCount()>0);
}

void CMainFrame::OnOperaterLocal() 
{
  CPtrList DataList;

  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();

  POSITION pos = ctlList.GetFirstSelectedItemPosition();
  while(pos)
  {
    WORD UnitID = ctlList.GetNextSelectedItem(pos)+1;

    switch(g_Unit[UnitID].UnitType)
    {
      case 1://变压器差动保护
      case 2://变压器高后备保护
      case 4://线路保护
      case 5://电容器保护
        g_Unit[UnitID].VarGroup[1].Var[5].Value = 1;
        break;
      case 3://变压器低后备保护
        g_Unit[UnitID].VarGroup[1].Var[10].Value = 1;
        g_Unit[UnitID].VarGroup[1].Var[11].Value = 1;
        break;
    }
  }

  ::WriteUnitToShareMemory();

  g_ProtocolTH104[255].m_SendAllFloatDataTime = 0;
  g_ProtocolTH104[255].m_SendAllBitDataTime = 0;
  g_ProtocolTH104[255].m_SendAllLongDataTime = 0;
}

void CMainFrame::OnUpdateOperaterLocal(CCmdUI* pCmdUI) 
{
  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();
  pCmdUI->Enable(ctlList.GetSelectedCount()>0);
}

void CMainFrame::OnOperaterFar() 
{
  CPtrList DataList;

  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();

  TagVarStruct Var[UNITCOUNT];

  POSITION pos = ctlList.GetFirstSelectedItemPosition();
  while(pos)
  {
    WORD UnitID = ctlList.GetNextSelectedItem(pos)+1;

    switch(g_Unit[UnitID].UnitType)
    {
      case 1://变压器差动保护
      case 2://变压器高后备保护
      case 4://线路保护
      case 5://电容器保护
        g_Unit[UnitID].VarGroup[1].Var[5].Value = 0;
        break;
      case 3://变压器低后备保护
        g_Unit[UnitID].VarGroup[1].Var[10].Value = 0;
        g_Unit[UnitID].VarGroup[1].Var[11].Value = 0;
        break;
    }
  }

  ::WriteUnitToShareMemory();

  g_ProtocolTH104[255].m_SendAllFloatDataTime = 0;
  g_ProtocolTH104[255].m_SendAllBitDataTime = 0;
  g_ProtocolTH104[255].m_SendAllLongDataTime = 0;
}

void CMainFrame::OnUpdateOperaterFar(CCmdUI* pCmdUI) 
{
  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();
  pCmdUI->Enable(ctlList.GetSelectedCount()>0);
}

void CMainFrame::OnOperaterYx() 
{
  CDlgOperateUnit dlg;
  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();
  POSITION pos = ctlList.GetFirstSelectedItemPosition();
  if(pos)
  {
    WORD UnitID = ctlList.GetNextSelectedItem(pos)+1;
    for(int varno=0;varno<VARCOUNT;varno++)
    {
      dlg.m_VarList[varno].UnitID = UnitID;
      dlg.m_VarList[varno].VarGroupID = 1;
      dlg.m_VarList[varno].VarID = varno;
      dlg.m_VarList[varno].VarName = g_Unit[UnitID].VarGroup[1].Var[varno].VarName;
      dlg.m_VarList[varno].TextFormat = g_Unit[UnitID].VarGroup[1].Var[varno].TextFormat;
      dlg.m_VarList[varno].Value = g_Unit[UnitID].VarGroup[1].Var[varno].Value;
    }
    if(dlg.DoModal()==IDCANCEL)
      return;

    for(varno=0;varno<VARCOUNT;varno++)
      g_Unit[UnitID].VarGroup[1].Var[varno].Value = dlg.m_VarList[varno].Value;
  }

  ::WriteUnitToShareMemory();

  g_ProtocolTH104[255].m_SendAllFloatDataTime = 0;
  g_ProtocolTH104[255].m_SendAllBitDataTime = 0;
  g_ProtocolTH104[255].m_SendAllLongDataTime = 0;
}

void CMainFrame::OnUpdateOperaterYx(CCmdUI* pCmdUI) 
{
  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();
  pCmdUI->Enable(ctlList.GetSelectedCount()>0);
}

void CMainFrame::OnOperaterParameter() 
{
  CDlgOperateParameter dlg;
  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();
  POSITION pos = ctlList.GetFirstSelectedItemPosition();
  if(pos)
  {
    WORD UnitID = ctlList.GetNextSelectedItem(pos)+1;
    dlg.m_UnitID = UnitID;
    dlg.DoModal();
  }
}

void CMainFrame::OnUpdateOperaterParameter(CCmdUI* pCmdUI) 
{
  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();
  pCmdUI->Enable(ctlList.GetSelectedCount()==1);
}

void CMainFrame::OnTransmissionFeed() 
{
  CDlgFreeTransmissionList dlg;
  dlg.DoModal();
}

void CMainFrame::OnUpdateTransmissionFeed(CCmdUI* pCmdUI) 
{
  if(g_SimTrain.SimTrainState!=0)
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);
}

void CMainFrame::OnTransmissionFault() 
{
  if(::CheckOperateCondition()==FALSE)
    return;

  static CString FaultTransmissionDataFileName;

  CDlgFaultTransmission dlg;
  dlg.m_StartupDelaySecond = g_StartupDelaySecond;
  dlg.m_FileName = FaultTransmissionDataFileName;
  if(dlg.DoModal()==IDCANCEL)
    return;

  FaultTransmissionDataFileName = dlg.m_FileName;
  strcpy(g_FaultTransmission.FaultTransmissionDataFileName,FaultTransmissionDataFileName.GetBuffer(0));
  g_FaultTransmission.StartupDelaySecond = g_StartupDelaySecond = dlg.m_StartupDelaySecond;
  g_FaultTransmission.BeginTime = CTime::GetCurrentTime() + CTimeSpan(0,0,0,g_FaultTransmission.StartupDelaySecond);;
  g_FaultTransmission.FaultTransmissionState = 1;//准备读文件

  g_pDlgMessageBox->DoShow("准备启动故障传动数据文件 <"+FaultTransmissionDataFileName+">",3);
}

void CMainFrame::OnUpdateTransmissionFault(CCmdUI* pCmdUI) 
{
  if(g_SimCommunicationMain==FALSE)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

  pCmdUI->Enable(TRUE);
}

void CMainFrame::OnPowerClear() 
{
  ::ClearPower();
}

void CMainFrame::OnUpdatePowerClear(CCmdUI* pCmdUI) 
{
  if((g_FreeTransmissionList.GetCount()>0)||(g_FreeTrainList.GetCount()>0)||(g_SimTrain.SimTrainState!=0))
    pCmdUI->Enable(TRUE);
  else
    pCmdUI->Enable(FALSE);
}

void CMainFrame::OnPowerMonitor() 
{
  g_pDlgPowerMonitor->ShowWindow(SW_SHOW);
  g_pDlgPowerMonitor->BringWindowToTop();
}

void CMainFrame::OnUpdatePowerMonitor(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(TRUE);
}

void CMainFrame::OnFileLoad() 
{
  if(::AfxMessageBox("是否读取配置？",MB_YESNO)==IDNO)
    return;

  ::LoadUnitList();
  ::LoadPowerPath();
  m_pUnitListView->MakeListView();
}

void CMainFrame::OnFileSave() 
{
  if(g_SimCommunicationMain==FALSE)
    return;

  if(::AfxMessageBox("是否保存配置？",MB_YESNO)==IDNO)
    return;

  ::SaveUnitList();
}

void CMainFrame::OnParameterLoad() 
{
  if(::AfxMessageBox("是否读取保护定值？",MB_YESNO)==IDNO)
    return;

  ::LoadParameterList();
}

void CMainFrame::OnParameterSave() 
{
  if(g_SimCommunicationMain==FALSE)
    return;

  if(::AfxMessageBox("是否保存保护定值？",MB_YESNO)==IDNO)
    return;

  ::SaveParameterList();
}

void CMainFrame::OnInVotlage() 
{
  CDlgInlineVotlage dlg;
  dlg.m_pInlineVotlage = &g_InlineVotlage;
  dlg.DoModal();
}

void CMainFrame::OnPtVotlage() 
{
  CDlgPTVotlage dlg;
  dlg.m_pPTVotlage = &g_PTVoltage;
  dlg.DoModal();
}

void CMainFrame::OnHandcartOn() 
{
  CPtrList DataList;

  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();

  TagVarStruct Var[UNITCOUNT];

  POSITION pos = ctlList.GetFirstSelectedItemPosition();
  while(pos)
  {
    WORD UnitID = ctlList.GetNextSelectedItem(pos)+1;

    switch(g_Unit[UnitID].UnitType)
    {
      case 1://变压器差动保护
      case 2://变压器高后备保护
      case 4://线路保护
      case 5://电容器保护
        g_Unit[UnitID].VarGroup[1].Var[4].Value = 1;
        break;
      case 3://变压器低后备保护
        g_Unit[UnitID].VarGroup[1].Var[8].Value = 1;
        g_Unit[UnitID].VarGroup[1].Var[9].Value = 1;
        break;
    }
  }

  ::WriteUnitToShareMemory();

  g_ProtocolTH104[255].m_SendAllFloatDataTime = 0;
  g_ProtocolTH104[255].m_SendAllBitDataTime = 0;
  g_ProtocolTH104[255].m_SendAllLongDataTime = 0;
}

void CMainFrame::OnUpdateHandcartOn(CCmdUI* pCmdUI) 
{
  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();
  pCmdUI->Enable(ctlList.GetSelectedCount()>0);
}

void CMainFrame::OnHandcartOff() 
{
  CPtrList DataList;

  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();

  TagVarStruct Var[UNITCOUNT];

  POSITION pos = ctlList.GetFirstSelectedItemPosition();
  while(pos)
  {
    WORD UnitID = ctlList.GetNextSelectedItem(pos)+1;

    switch(g_Unit[UnitID].UnitType)
    {
      case 1://变压器差动保护
      case 2://变压器高后备保护
      case 4://线路保护
      case 5://电容器保护
        g_Unit[UnitID].VarGroup[1].Var[4].Value = 0;
        break;
      case 3://变压器低后备保护
        g_Unit[UnitID].VarGroup[1].Var[8].Value = 0;
        g_Unit[UnitID].VarGroup[1].Var[9].Value = 0;
        break;
    }
  }

  ::WriteUnitToShareMemory();

  g_ProtocolTH104[255].m_SendAllFloatDataTime = 0;
  g_ProtocolTH104[255].m_SendAllBitDataTime = 0;
  g_ProtocolTH104[255].m_SendAllLongDataTime = 0;
}

void CMainFrame::OnUpdateHandcartOff(CCmdUI* pCmdUI) 
{
  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();
  pCmdUI->Enable(ctlList.GetSelectedCount()>0);
}

void CMainFrame::OnSceneOperate() 
{
  CDlgSceneFileSelect dlg;
  dlg.DoModal();
}

void CMainFrame::OnUpdateSceneOperate(CCmdUI* pCmdUI) 
{
  if(g_SimCommunicationMain==FALSE)
  {
    pCmdUI->Enable(FALSE);
    return;
  }
  pCmdUI->Enable(TRUE);
}

void CMainFrame::OnOperaterYxLock() 
{
  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();

  POSITION pos = ctlList.GetFirstSelectedItemPosition();
  while(pos)
  {
    WORD UnitID = ctlList.GetNextSelectedItem(pos)+1;

    switch(g_Unit[UnitID].UnitType)
    {
      case 1://变压器差动保护
      case 2://变压器高后备保护
      case 4://线路保护
      case 5://电容器保护
        g_Unit[UnitID].VarLock[0] = 1;
        g_Unit[UnitID].VarLock[1] = 1;
        break;
      case 3://变压器低后备保护
        g_Unit[UnitID].VarLock[0] = 1;
        g_Unit[UnitID].VarLock[1] = 1;
        g_Unit[UnitID].VarLock[2] = 1;
        g_Unit[UnitID].VarLock[3] = 1;
        break;
    }
  }
}

void CMainFrame::OnOperaterYxUnlock() 
{
  CListCtrl &ctlList=m_pUnitListView->GetListCtrl();

  POSITION pos = ctlList.GetFirstSelectedItemPosition();
  while(pos)
  {
    WORD UnitID = ctlList.GetNextSelectedItem(pos)+1;

    switch(g_Unit[UnitID].UnitType)
    {
      case 1://变压器差动保护
      case 2://变压器高后备保护
      case 4://线路保护
      case 5://电容器保护
        g_Unit[UnitID].VarLock[0] = 0;
        g_Unit[UnitID].VarLock[1] = 0;
        break;
      case 3://变压器低后备保护
        g_Unit[UnitID].VarLock[0] = 0;
        g_Unit[UnitID].VarLock[1] = 0;
        g_Unit[UnitID].VarLock[2] = 0;
        g_Unit[UnitID].VarLock[3] = 0;
        break;
    }
  }
}


