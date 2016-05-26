#if !defined(AFX_DLGSCENEFILESELECT_H__39BD70F0_8318_45EE_8401_A68802E1894E__INCLUDED_)
#define AFX_DLGSCENEFILESELECT_H__39BD70F0_8318_45EE_8401_A68802E1894E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSceneFileSelect.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSceneFileSelect dialog

class CDlgSceneFileSelect : public CDialog
{
// Construction
public:
	CDlgSceneFileSelect(CWnd* pParent = NULL);   // standard constructor

  CString m_FileName;

  CStringList m_FileList;
  CImageList m_ctlImage16;

  void UpdateList();

// Dialog Data
	//{{AFX_DATA(CDlgSceneFileSelect)
	enum { IDD = IDD_DLG_SCENE_FILE_SELECT };
	CListCtrl	m_ctlList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSceneFileSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSceneFileSelect)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSatrtup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSCENEFILESELECT_H__39BD70F0_8318_45EE_8401_A68802E1894E__INCLUDED_)
