#if !defined(AFX_PAGEVARLISTVIEW_H__7387D577_0981_11D4_BA60_0080C8D77339__INCLUDED_)
#define AFX_PAGEVARLISTVIEW_H__7387D577_0981_11D4_BA60_0080C8D77339__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// simCommunicationListView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CsimCommunicationVarListView view
class CsimCommunicationUnitListView;
class CsimCommunicationVarListView : public CListView
{
protected:
	CsimCommunicationVarListView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CsimCommunicationVarListView)
  void MakeListView();
  void UpdateListView();

// Attributes
public:
  CsimCommunicationUnitListView *m_pUnitListView;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CsimCommunicationVarListView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CsimCommunicationVarListView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CsimCommunicationVarListView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnOperaterYxOn();
	afx_msg void OnUpdateOperaterYxOn(CCmdUI* pCmdUI);
	afx_msg void OnOperaterYxOff();
	afx_msg void OnUpdateOperaterYxOff(CCmdUI* pCmdUI);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOperaterYxLock();
	afx_msg void OnOperaterYxUnlock();
	afx_msg void OnUpdateOperaterYxLock(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOperaterYxUnlock(CCmdUI* pCmdUI);
	afx_msg void OnOperaterYxAbnormal();
	afx_msg void OnUpdateOperaterYxAbnormal(CCmdUI* pCmdUI);
	afx_msg void OnOperaterYxNormal();
	afx_msg void OnUpdateOperaterYxNormal(CCmdUI* pCmdUI);
	afx_msg void OnOperaterYxThumbnail();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEVARLISTVIEW_H__7387D577_0981_11D4_BA60_0080C8D77339__INCLUDED_)
