#if !defined(AFX_DLGMESSAGEBOX_H__EA9F41C5_BA00_408F_9ABA_D9573E2D16F9__INCLUDED_)
#define AFX_DLGMESSAGEBOX_H__EA9F41C5_BA00_408F_9ABA_D9573E2D16F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMessageBox.h : header file
//

#include "resource.h"       // main symbols

class CDlgMessageBox;
extern CDlgMessageBox *g_pDlgMessageBox;

/////////////////////////////////////////////////////////////////////////////
// CDlgMessageBox dialog

class CDlgMessageBox : public CDialog
{
// Construction
public:
	CDlgMessageBox(CWnd* pParent = NULL);   // standard constructor
  void DoShow(CString Title,int DelaySecond);

// Dialog Data
	//{{AFX_DATA(CDlgMessageBox)
	enum { IDD = IDD_DLG_MESSAGE_BOX };
	CProgressCtrl	m_ctlProgress;
	CStatic	m_ctlTitle;
	CString	m_Title;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMessageBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgMessageBox)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMESSAGEBOX_H__EA9F41C5_BA00_408F_9ABA_D9573E2D16F9__INCLUDED_)
