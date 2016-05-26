#if !defined(AFX_COMMTHREAD_H__57477DE4_233C_11D4_BA64_0080C8D77339__INCLUDED_)
#define AFX_COMMTHREAD_H__57477DE4_233C_11D4_BA64_0080C8D77339__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CommThread.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCommThread thread

class CCommThread : public CWinThread
{
	DECLARE_DYNCREATE(CCommThread)
public:
	CCommThread();
	virtual ~CCommThread();
  BOOL m_Run;       //运行标志
  BOOL m_Running;   //正在运行标志
  BOOL m_Pause;     //暂停标志
  BOOL m_Pausing;   //正在暂停标志
  void KillThread();//结束线程

  WORD m_ComputerID;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCommThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMTHREAD_H__57477DE4_233C_11D4_BA64_0080C8D77339__INCLUDED_)

