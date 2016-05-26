#if !defined(AFX_PAGEUNITLISTVIEW_H__7387D577_0981_11D4_BA60_0080C8D77339__INCLUDED_)
#define AFX_PAGEUNITLISTVIEW_H__7387D577_0981_11D4_BA60_0080C8D77339__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// simCommunicationListView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CsimCommunicationUnitListView view
class CsimCommunicationSOEListView;
class CsimCommunicationVarListView;
class CsimCommunicationUnitListView : public CListView
{
protected:
	CsimCommunicationUnitListView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CsimCommunicationUnitListView)
  void MakeListView();
  void UpdateListView();

// Attributes
public:
  CsimCommunicationSOEListView *m_pSOEListView;
  CsimCommunicationVarListView *m_pVarListView;
  CImageList m_ctlImage16;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CsimCommunicationUnitListView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CsimCommunicationUnitListView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CsimCommunicationUnitListView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUnitClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEUNITLISTVIEW_H__7387D577_0981_11D4_BA60_0080C8D77339__INCLUDED_)
