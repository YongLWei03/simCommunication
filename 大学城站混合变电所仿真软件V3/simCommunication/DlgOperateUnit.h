#if !defined(AFX_DLGOPERATEUNIT_H__D08919ED_08E1_409E_A356_B0CEAB0CC6A6__INCLUDED_)
#define AFX_DLGOPERATEUNIT_H__D08919ED_08E1_409E_A356_B0CEAB0CC6A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgOperateUnit.h : header file
//

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDlgOperateUnit dialog

class CDlgOperateUnit : public CDialog
{
// Construction
public:
	CDlgOperateUnit(CWnd* pParent = NULL);   // standard constructor

  TagVarStruct m_VarList[VARCOUNT];

  void UpdateList();

// Dialog Data
	//{{AFX_DATA(CDlgOperateUnit)
	enum { IDD = IDD_OPERATE_UNIT };
	CListCtrl	m_ctlList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOperateUnit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgOperateUnit)
	virtual BOOL OnInitDialog();
	afx_msg void OnOn();
	afx_msg void OnOff();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOPERATEUNIT_H__D08919ED_08E1_409E_A356_B0CEAB0CC6A6__INCLUDED_)
