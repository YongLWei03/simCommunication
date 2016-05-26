#if !defined(AFX_DLGFREETRANSMISSION_H__286E65D8_EB6C_4039_9EB8_345574096421__INCLUDED_)
#define AFX_DLGFREETRANSMISSION_H__286E65D8_EB6C_4039_9EB8_345574096421__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFreeTransmission.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgFreeTransmission dialog

class CDlgFreeTransmission : public CDialog
{
// Construction
public:
	CDlgFreeTransmission(CWnd* pParent = NULL);   // standard constructor

  struct TagFreeTransmissionStruct *m_pFreeTransmission;

// Dialog Data
	//{{AFX_DATA(CDlgFreeTransmission)
	enum { IDD = IDD_DLG_FREE_TRANSMISSION };
	CListCtrl	m_ctlList;
	CComboBox	m_ctlFeedUnitName;
	CEdit	m_ctlContinuedSeconds;
	CEdit	m_ctlCurrentChangeRate;
	CEdit	m_ctlStartupDelaySecond;
	CEdit	m_ctlAverCurrent;
	CEdit	m_ctlAverAngle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFreeTransmission)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFreeTransmission)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFREETRANSMISSION_H__286E65D8_EB6C_4039_9EB8_345574096421__INCLUDED_)
