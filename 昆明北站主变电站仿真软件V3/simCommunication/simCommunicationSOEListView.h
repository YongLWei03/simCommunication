#if !defined(AFX_PAGESOELISTVIEW_H__7387D577_0981_11D4_BA60_0080C8D77339__INCLUDED_)
#define AFX_PAGESOELISTVIEW_H__7387D577_0981_11D4_BA60_0080C8D77339__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// simCommunicationListView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CsimCommunicationSOEListView view
class CsimCommunicationUnitListView;
class CsimCommunicationSOEListView : public CListView
{
protected:
	CsimCommunicationSOEListView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CsimCommunicationSOEListView)
  void MakeListView();
  void UpdateListView();

// Attributes
public:
  CsimCommunicationUnitListView *m_pUnitListView;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CsimCommunicationSOEListView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CsimCommunicationSOEListView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CsimCommunicationSOEListView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESOELISTVIEW_H__7387D577_0981_11D4_BA60_0080C8D77339__INCLUDED_)
