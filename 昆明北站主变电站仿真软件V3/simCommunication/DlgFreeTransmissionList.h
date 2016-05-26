#if !defined(AFX_DLGFREETRANSMISSIONLIST_H__02AA619B_B675_4E36_A0E8_06C7386F8363__INCLUDED_)
#define AFX_DLGFREETRANSMISSIONLIST_H__02AA619B_B675_4E36_A0E8_06C7386F8363__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFreeTransmissionList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgFreeTransmissionList dialog

class CDlgFreeTransmissionList : public CDialog
{
// Construction
public:
	CDlgFreeTransmissionList(CWnd* pParent = NULL);   // standard constructor
  CImageList m_ctlImage16;

  void UpdateList();
  BOOL LoadFreeTrainManageListFile();

// Dialog Data
	//{{AFX_DATA(CDlgFreeTransmissionList)
	enum { IDD = IDD_DLG_FREE_TRANSMISSION_LIST };
	CListCtrl	m_ctlList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFreeTransmissionList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFreeTransmissionList)
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

#endif // !defined(AFX_DLGFREETRANSMISSIONLIST_H__02AA619B_B675_4E36_A0E8_06C7386F8363__INCLUDED_)
