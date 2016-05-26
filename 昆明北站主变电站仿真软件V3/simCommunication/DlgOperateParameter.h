#if !defined(AFX_DLGOPERATEPARAMETER_H__D8A48C3E_96D8_43DF_B9BD_9CCF0909616E__INCLUDED_)
#define AFX_DLGOPERATEPARAMETER_H__D8A48C3E_96D8_43DF_B9BD_9CCF0909616E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgOperateParameter.h : header file
//

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDlgOperateParameter dialog

class CDlgOperateParameter : public CDialog
{
// Construction
public:
	CDlgOperateParameter(CWnd* pParent = NULL);   // standard constructor

  WORD m_UnitID;

// Dialog Data
	//{{AFX_DATA(CDlgOperateParameter)
	enum { IDD = IDD_OPERATE_PARAMETER };
	CStatic	m_ctlGroupID;
	CListCtrl	m_ctlList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOperateParameter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgOperateParameter)
	afx_msg void OnSet();
	afx_msg void OnCall();
	afx_msg void OnSend();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnGroup0();
	afx_msg void OnGroup1();
	afx_msg void OnGroup2();
	afx_msg void OnGroup3();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOPERATEPARAMETER_H__D8A48C3E_96D8_43DF_B9BD_9CCF0909616E__INCLUDED_)
