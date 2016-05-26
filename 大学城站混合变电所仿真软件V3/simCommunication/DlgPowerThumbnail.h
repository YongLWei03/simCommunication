#if !defined(AFX_DLGPOWERTHUMBNAIL_H__68A74C06_7D81_44C3_AF36_1CE6386F9ED1__INCLUDED_)
#define AFX_DLGPOWERTHUMBNAIL_H__68A74C06_7D81_44C3_AF36_1CE6386F9ED1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPowerThumbnail.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgPowerThumbnail dialog

class CDlgPowerThumbnail : public CDialog
{
// Construction
public:
	CDlgPowerThumbnail(CWnd* pParent = NULL);   // standard constructor

  void LoadBitmapFile();
  void LoadObjectFile();
  void UpdateView();

  BOOL m_PowerState;
  float m_PowerStateList[UNITCOUNT][VARCOUNT];

  CBitmap m_bitmap[5];
  CPtrList m_ObjectList;

// Dialog Data
	//{{AFX_DATA(CDlgPowerThumbnail)
	enum { IDD = IDD_DLG_POWER_THUMBNAIL };
	CButton	m_ctlExport;
	CButton	m_ctlImport;
	CStatic	m_ctlWnd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPowerThumbnail)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPowerThumbnail)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnImport();
	afx_msg void OnExport();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPOWERTHUMBNAIL_H__68A74C06_7D81_44C3_AF36_1CE6386F9ED1__INCLUDED_)
