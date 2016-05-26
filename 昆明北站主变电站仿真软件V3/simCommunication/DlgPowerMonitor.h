#if !defined(AFX_DLGPOWERMONITOR_H__8246BABE_18F8_43DA_A41A_AFB76F69EAE4__INCLUDED_)
#define AFX_DLGPOWERMONITOR_H__8246BABE_18F8_43DA_A41A_AFB76F69EAE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPowerMonitor.h : header file
//

class CDlgPowerMonitor;
extern CDlgPowerMonitor *g_pDlgPowerMonitor;

/////////////////////////////////////////////////////////////////////////////
// CDlgPowerMonitor dialog

class CDlgPowerMonitor : public CDialog
{
// Construction
public:
	CDlgPowerMonitor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPowerMonitor)
	enum { IDD = IDD_DLG_POWER_MONITOR };
	CListCtrl	m_ctlTransmissionList;
	CListCtrl	m_ctlFeedList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPowerMonitor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPowerMonitor)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPOWERMONITOR_H__8246BABE_18F8_43DA_A41A_AFB76F69EAE4__INCLUDED_)
