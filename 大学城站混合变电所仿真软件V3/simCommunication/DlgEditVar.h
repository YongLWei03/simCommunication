#if !defined(AFX_DLGEDITVAR_H__C632382F_6C16_4252_8ABC_9CBDA4C1B1AF__INCLUDED_)
#define AFX_DLGEDITVAR_H__C632382F_6C16_4252_8ABC_9CBDA4C1B1AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEditVar.h : header file
//

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDlgEditVar dialog

class CDlgEditVar : public CDialog
{
// Construction
public:
	CDlgEditVar(CWnd* pParent = NULL);   // standard constructor

  CString m_VarName[VARCOUNT];
  CString m_TextFormat[VARCOUNT];

// Dialog Data
	//{{AFX_DATA(CDlgEditVar)
	enum { IDD = IDD_EDIT_VAR };
	CEdit	m_ctlVarName;
	CEdit	m_ctlTextFormat;
	CListCtrl	m_ctlList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEditVar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEditVar)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSet();
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnReplace();
	afx_msg void OnChangeVarName();
	afx_msg void OnChangeTextFormat();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITVAR_H__C632382F_6C16_4252_8ABC_9CBDA4C1B1AF__INCLUDED_)
