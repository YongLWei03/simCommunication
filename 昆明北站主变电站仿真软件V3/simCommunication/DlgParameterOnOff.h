#if !defined(AFX_DLGPARAMETERONOFF_H__BAAD3212_1931_4502_9769_5F482C9BC740__INCLUDED_)
#define AFX_DLGPARAMETERONOFF_H__BAAD3212_1931_4502_9769_5F482C9BC740__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgParameterOnOff.h : header file
//

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDlgParameterOnOff dialog

class CDlgParameterOnOff : public CDialog
{
// Construction
public:
	CDlgParameterOnOff(CWnd* pParent = NULL);   // standard constructor
  
  float m_ParameterValue;


// Dialog Data
	//{{AFX_DATA(CDlgParameterOnOff)
	enum { IDD = IDD_PARAMETER_ONOFF };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgParameterOnOff)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgParameterOnOff)
	afx_msg void OnParameterOn();
	afx_msg void OnParameterOff();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPARAMETERONOFF_H__BAAD3212_1931_4502_9769_5F482C9BC740__INCLUDED_)
