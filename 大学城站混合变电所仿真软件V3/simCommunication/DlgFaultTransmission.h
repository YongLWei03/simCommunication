#if !defined(AFX_DLGFAULTTRANSMISSION_H__E19DCA99_2FFB_4888_B682_EBC924E03DE2__INCLUDED_)
#define AFX_DLGFAULTTRANSMISSION_H__E19DCA99_2FFB_4888_B682_EBC924E03DE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFaultTransmission.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgFaultTransmission dialog

class CDlgFaultTransmission : public CDialog
{
// Construction
public:
	CDlgFaultTransmission(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgFaultTransmission)
	enum { IDD = IDD_DLG_FAULT_TRANSMISSION };
	CEdit	m_ctlFileName;
	CString	m_FileName;
	UINT	m_StartupDelaySecond;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFaultTransmission)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFaultTransmission)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnFileSelect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFAULTTRANSMISSION_H__E19DCA99_2FFB_4888_B682_EBC924E03DE2__INCLUDED_)
