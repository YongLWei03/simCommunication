#if !defined(AFX_DLGFREETRAINLIST_H__0581C5F1_D57D_4BB8_84D7_3F970F27FA1A__INCLUDED_)
#define AFX_DLGFREETRAINLIST_H__0581C5F1_D57D_4BB8_84D7_3F970F27FA1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFreeTrainList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgFreeTrainList dialog

class CDlgFreeTrainList : public CDialog
{
// Construction
public:
	CDlgFreeTrainList(CWnd* pParent = NULL);   // standard constructor
  CImageList m_ctlImage16;

  void UpdateList();
  BOOL LoadFreeTrainManageListFile();

// Dialog Data
	//{{AFX_DATA(CDlgFreeTrainList)
	enum { IDD = IDD_DLG_FREE_TRAIN_LIST };
	CListCtrl	m_ctlList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFreeTrainList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFreeTrainList)
	virtual BOOL OnInitDialog();
	afx_msg void OnLoad();
	afx_msg void OnSave();
	afx_msg void OnNew();
	afx_msg void OnEdit();
	afx_msg void OnClon();
	afx_msg void OnDel();
	virtual void OnCancel();
	afx_msg void OnStartup();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFREETRAINLIST_H__0581C5F1_D57D_4BB8_84D7_3F970F27FA1A__INCLUDED_)
