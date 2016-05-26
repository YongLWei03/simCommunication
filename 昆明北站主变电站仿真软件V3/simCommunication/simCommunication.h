// simCommunication.h : main header file for the SIMCOMMUNICATION application
//

#if !defined(AFX_SIMCOMMUNICATION_H__AF621B81_8289_4D06_B641_877817AE5EAA__INCLUDED_)
#define AFX_SIMCOMMUNICATION_H__AF621B81_8289_4D06_B641_877817AE5EAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class CMainFrame;
extern CMainFrame *g_pMainFrame;
void WINAPI ShowPaneTextMessage(CString msg);

/////////////////////////////////////////////////////////////////////////////
// CSimCommunicationUnitApp:
// See simCommunication.cpp for the implementation of this class
//

class CSimCommunicationUnitApp : public CWinApp
{
public:
	CSimCommunicationUnitApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimCommunicationUnitApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSimCommunicationUnitApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMCOMMUNICATION_H__AF621B81_8289_4D06_B641_877817AE5EAA__INCLUDED_)
