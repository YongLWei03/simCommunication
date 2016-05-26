#if !defined(AFX_DLGPARAMETERINPUT_H__91799CB1_260B_4F97_A2CD_0F08D471A3F7__INCLUDED_)
#define AFX_DLGPARAMETERINPUT_H__91799CB1_260B_4F97_A2CD_0F08D471A3F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLgParameterInput.h : header file
//

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDLgParameterInput dialog

class CDLgParameterInput : public CDialog
{
// Construction
public:
	CDLgParameterInput(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDLgParameterInput)
	enum { IDD = IDD_PARAMETER_INPUT };
	float	m_ParameterValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLgParameterInput)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDLgParameterInput)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPARAMETERINPUT_H__91799CB1_260B_4F97_A2CD_0F08D471A3F7__INCLUDED_)
