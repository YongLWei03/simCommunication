#if !defined(AFX_DLGINLINEVOTLAGE_H__F7F9283F_C077_4F78_A37F_2DF16F462454__INCLUDED_)
#define AFX_DLGINLINEVOTLAGE_H__F7F9283F_C077_4F78_A37F_2DF16F462454__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgInlineVotlage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgInlineVotlage dialog

class CDlgInlineVotlage : public CDialog
{
// Construction
public:
	CDlgInlineVotlage(CWnd* pParent = NULL);   // standard constructor

  struct TagInlineVotlageStruct *m_pInlineVotlage;

// Dialog Data
	//{{AFX_DATA(CDlgInlineVotlage)
	enum { IDD = IDD_DLG_INLINE_VOTLAGE };
	CEdit	m_ctl2TLoVoltageChangeRate;
	CEdit	m_ctl1TLoVoltageChangeRate;
	CEdit	m_ctlEditUa1;
	CEdit	m_ctlEditUb1;
	CEdit	m_ctlEditUc1;
	CEdit	m_ctlEditUa2;
	CEdit	m_ctlEditUb2;
	CEdit	m_ctlEditUc2;
	CEdit	m_ctl1THiVoltageChangeRate;
	CEdit	m_ctl2THiVoltageChangeRate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInlineVotlage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgInlineVotlage)
	virtual BOOL OnInitDialog();
	afx_msg void OnNormalUa1();
	afx_msg void OnNormalUb1();
	afx_msg void OnNormalUc1();
	afx_msg void OnLostUa1();
	afx_msg void OnLostUb1();
	afx_msg void OnLostUc1();
	afx_msg void OnLowUa1();
	afx_msg void OnLowUb1();
	afx_msg void OnLowUc1();
	afx_msg void OnNormalUa2();
	afx_msg void OnNormalUb2();
	afx_msg void OnNormalUc2();
	afx_msg void OnLostUa2();
	afx_msg void OnLostUb2();
	afx_msg void OnLostUc2();
	afx_msg void OnLowUa2();
	afx_msg void OnLowUb2();
	afx_msg void OnLowUc2();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINLINEVOTLAGE_H__F7F9283F_C077_4F78_A37F_2DF16F462454__INCLUDED_)
