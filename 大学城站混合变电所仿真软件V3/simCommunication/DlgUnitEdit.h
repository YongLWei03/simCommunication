#if !defined(AFX_DLGUNITEDIT_H__4002BFAF_B3C1_46F8_9389_A45638F5E3D0__INCLUDED_)
#define AFX_DLGUNITEDIT_H__4002BFAF_B3C1_46F8_9389_A45638F5E3D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgUnitEdit.h : header file
//

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDlgUnitEdit dialog

class CDlgUnitEdit : public CDialog
{
// Construction
public:
	CDlgUnitEdit(CWnd* pParent = NULL);   // standard constructor

  CString m_VarName[VARGROUPCOUNT][VARCOUNT];
  CString m_TextFormat[VARGROUPCOUNT][VARCOUNT];
  
// Dialog Data
	//{{AFX_DATA(CDlgUnitEdit)
	enum { IDD = IDD_EDIT_UNIT };
	CEdit	m_ctlUnitCT;
	CEdit	m_ctlUnitPT;
	CEdit	m_ctlUnitName;
	CComboBox	m_ctlUnitType;
	CString	m_UnitName;
	int		m_UnitType;
	float	m_UnitPT;
	float	m_UnitCT;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgUnitEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgUnitEdit)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnEditYc();
	afx_msg void OnEditYx();
	afx_msg void OnEditDd();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnReplase();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGUNITEDIT_H__4002BFAF_B3C1_46F8_9389_A45638F5E3D0__INCLUDED_)
