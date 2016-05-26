#if !defined(AFX_DLGFREETRAIN_H__353F9033_E61D_4F71_B60B_AB2C6D2BCE9D__INCLUDED_)
#define AFX_DLGFREETRAIN_H__353F9033_E61D_4F71_B60B_AB2C6D2BCE9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFreeTrain.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgFreeTrain dialog

class CDlgFreeTrain : public CDialog
{
// Construction
public:
	CDlgFreeTrain(CWnd* pParent = NULL);   // standard constructor

  struct TagFreeTrainStruct *m_pFreeTrain;

// Dialog Data
	//{{AFX_DATA(CDlgFreeTrain)
	enum { IDD = IDD_DLG_FREE_TRAIN };
	CComboBox	m_ctlTrainDirect;
	CListCtrl	m_ctlOutList;
	CListCtrl	m_ctlInList;
	CEdit	m_ctlCurrentChangeRate;
	CEdit	m_ctlStartupDelaySecond;
	CEdit	m_ctlAverCurrent;
	CEdit	m_ctlOutContinuedSeconds;
	CEdit	m_ctlInContinuedSeconds;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFreeTrain)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFreeTrain)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnInSet();
	afx_msg void OnOutSet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFREETRAIN_H__353F9033_E61D_4F71_B60B_AB2C6D2BCE9D__INCLUDED_)
