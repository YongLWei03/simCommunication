#if !defined(AFX_DLGCURRENTDISTRIBUTION_H__7DB295EB_0878_4609_89EF_C38544F44DCE__INCLUDED_)
#define AFX_DLGCURRENTDISTRIBUTION_H__7DB295EB_0878_4609_89EF_C38544F44DCE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCurrentDistribution.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgCurrentDistribution dialog

class CDlgCurrentDistribution : public CDialog
{
// Construction
public:
	CDlgCurrentDistribution(CWnd* pParent = NULL);   // standard constructor

  void UpdateList();
  void DrawList();

  CString m_ChangeRageList;

// Dialog Data
	//{{AFX_DATA(CDlgCurrentDistribution)
	enum { IDD = IDD_DLG_CURRENT_DISTRIBUTION };
	CEdit	m_ctlValue;
	CListCtrl	m_ctlList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCurrentDistribution)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCurrentDistribution)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSetValue();
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditValue();
	afx_msg void OnSetValue1();
	afx_msg void OnSetValue2();
	afx_msg void OnSetValue3();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCURRENTDISTRIBUTION_H__7DB295EB_0878_4609_89EF_C38544F44DCE__INCLUDED_)
