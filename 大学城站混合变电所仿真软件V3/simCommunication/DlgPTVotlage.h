#if !defined(AFX_DLGPTVOTLAGE_H__8C18338A_7FCD_41C3_8A1D_D86D31B3B6D9__INCLUDED_)
#define AFX_DLGPTVOTLAGE_H__8C18338A_7FCD_41C3_8A1D_D86D31B3B6D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPTVotlage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgPTVotlage dialog

class CDlgPTVotlage : public CDialog
{
// Construction
public:
	CDlgPTVotlage(CWnd* pParent = NULL);   // standard constructor
  struct TagPTVotlageStruct *m_pPTVotlage;      //Ñ¹»¥²¹³¥

// Dialog Data
	//{{AFX_DATA(CDlgPTVotlage)
	enum { IDD = IDD_DLG_PT_VOLTAGE };
	CEdit	m_ctl2TTemp2;
	CEdit	m_ctl2TTemp1;
	CEdit	m_ctl1TTemp2;
	CEdit	m_ctl1TTemp1;
	CEdit	m_ctl2TGear;
	CEdit	m_ctl1TGear;
	CEdit	m_ctl1THiCurrentMaxValue;
	CEdit	m_ctl1THiVoltageValueK;
	CEdit	m_ctl2THiCurrentMaxValue;
	CEdit	m_ctl2THiVoltageValueK;
	CEdit	m_ctl1TLoCurrentMaxValue;
	CEdit	m_ctl1TLoVoltageValueK;
	CEdit	m_ctl2TLoCurrentMaxValue;
	CEdit	m_ctl2TLoVoltageValueK;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPTVotlage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPTVotlage)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPTVOTLAGE_H__8C18338A_7FCD_41C3_8A1D_D86D31B3B6D9__INCLUDED_)
