// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__CC5C5D7B_8E2C_44A9_80EC_C2F7CB370D78__INCLUDED_)
#define AFX_MAINFRM_H__CC5C5D7B_8E2C_44A9_80EC_C2F7CB370D78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SystemTray.h"

class CsimCommunicationSOEListView;
class CsimCommunicationVarListView;
class CsimCommunicationUnitListView;
class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:
	CSplitterWnd m_wndSplitter;
	CSplitterWnd m_wndListSplitter;
  CsimCommunicationSOEListView *m_pSOEListView;
  CsimCommunicationVarListView *m_pVarListView;
  CsimCommunicationUnitListView *m_pUnitListView;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CSystemTray m_TrayIcon;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnOperaterRevert();
	afx_msg void OnUpdateOperaterRevert(CCmdUI* pCmdUI);
	afx_msg void OnOperaterOn();
	afx_msg void OnUpdateOperaterOn(CCmdUI* pCmdUI);
	afx_msg void OnOperaterOff();
	afx_msg void OnUpdateOperaterOff(CCmdUI* pCmdUI);
	afx_msg void OnOperaterLocal();
	afx_msg void OnUpdateOperaterLocal(CCmdUI* pCmdUI);
	afx_msg void OnOperaterFar();
	afx_msg void OnUpdateOperaterFar(CCmdUI* pCmdUI);
	afx_msg void OnOperaterParameter();
	afx_msg void OnUpdateOperaterParameter(CCmdUI* pCmdUI);
	afx_msg void OnOperaterYx();
	afx_msg void OnUpdateOperaterYx(CCmdUI* pCmdUI);
	afx_msg void OnEditUnit();
	afx_msg void OnUpdateEditUnit(CCmdUI* pCmdUI);
	afx_msg void OnShowWindow();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileSave();
	afx_msg void OnTransmissionFault();
	afx_msg void OnUpdateTransmissionFault(CCmdUI* pCmdUI);
	afx_msg void OnFileLoad();
	afx_msg void OnHandcartOn();
	afx_msg void OnUpdateHandcartOn(CCmdUI* pCmdUI);
	afx_msg void OnHandcartOff();
	afx_msg void OnUpdateHandcartOff(CCmdUI* pCmdUI);
	afx_msg void OnSceneOperate();
	afx_msg void OnUpdateSceneOperate(CCmdUI* pCmdUI);
	afx_msg void OnParameterLoad();
	afx_msg void OnParameterSave();
	afx_msg void OnOperaterYxLock();
	afx_msg void OnOperaterYxUnlock();
	afx_msg void OnTransmissionFeed();
	afx_msg void OnUpdateTransmissionFeed(CCmdUI* pCmdUI);
	afx_msg void OnPowerClear();
	afx_msg void OnUpdatePowerClear(CCmdUI* pCmdUI);
	afx_msg void OnInVotlage();
	afx_msg void OnPtVotlage();
	afx_msg void OnPowerMonitor();
	afx_msg void OnUpdatePowerMonitor(CCmdUI* pCmdUI);
	afx_msg void OnTrainFree();
	afx_msg void OnUpdateTrainFree(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__CC5C5D7B_8E2C_44A9_80EC_C2F7CB370D78__INCLUDED_)
