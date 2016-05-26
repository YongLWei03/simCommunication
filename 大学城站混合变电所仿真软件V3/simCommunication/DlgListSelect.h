#if !defined(AFX_DLGLISTSELECT_H__05D2DCA4_C719_4E12_961C_F292503B6E98__INCLUDED_)
#define AFX_DLGLISTSELECT_H__05D2DCA4_C719_4E12_961C_F292503B6E98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgListSelect.h : header file
//

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDlgListSelect dialog

class CDlgListSelect : public CDialog
{
// Construction
public:
	CDlgListSelect(CWnd* pParent = NULL);   // standard constructor
  WORD    m_DataWaveType;
  CString m_TextTitle;
  CString m_TextSelected;
  CStringList m_TextList;
  CImageList m_ctlImage16;

// Dialog Data
	//{{AFX_DATA(CDlgListSelect)
	enum { IDD = IDD_DLG_LIST_SELECT };
	CListCtrl	m_ctlList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgListSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgListSelect)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLISTSELECT_H__05D2DCA4_C719_4E12_961C_F292503B6E98__INCLUDED_)
