// NewString.h: interface for the CNewString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEWSTRING_H__E8CD0860_5D59_11D3_9D51_D85FC20A0395__INCLUDED_)
#define AFX_NEWSTRING_H__E8CD0860_5D59_11D3_9D51_D85FC20A0395__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct TagVarStruct;

WORD WINAPI GetUnitIDFromVarInfo(CString VarInfo);
WORD WINAPI GetVarIDFromVarInfo(CString VarInfo);
TagVarStruct* WINAPI GetVarFromVarInfo(CString VarInfo,WORD VarGroup);
void WINAPI GetVarListFromVarListInfo(CPtrList &VarList,CString VarListInfo,WORD VarGroup);
CString WINAPI GetUnitNameFromVarInfo(CString VarInfo);
CString WINAPI GetVarNameFromVarInfo(CString VarInfo,WORD VarGroup);
CString WINAPI GetVarInfoFromVar(TagVarStruct *pVar);
CString WINAPI GetVarListInfoFromVarList(CPtrList &VarList);

class CNewString
{

// Construction
public:
	CNewString(CString page="",CString wordmark="/",CString linemark="\r\n");	// standard constructor

	~CNewString();	// standard constructor
  
	  CStringArray m_LineList;
	  CStringArray m_WordList;
	  int     m_CurrentLine;
    CString m_Page;
	  CString m_LineMark;
	  CString m_WordMark;
	  
	  CString GetPage();
	  void SetPage(CString page);
	  int GetLineCount();

	  CString GetLine(int lineno);
	  int GetIntegerValue(int lineno);
	  float GetFloatValue(int lineno);
    COLORREF GetColorValue(int lineno);
	  void SetLine(int lineno,CString line);
	  void AddLine(CString line);
	  void InsertLine(int lineno,CString line);
	  void RemoveLine(int lineno);

	  CString GetWord(int lineno,int wordno);
	  int GetIntegerValue(int lineno,int wordno);
	  float GetFloatValue(int lineno,int wordno);
    COLORREF GetColorValue(int lineno,int wordno);
	  void SetWord(int lineno,int wordno,CString word);
	  void AddWord(int lineno,CString word);
	  void InsertWord(int lineno,int wordno,CString word);

	  int GetWordCount(int lineno);

	  void MakeLineList();
	  void MakeWordList(int lineno);
	  void MakeLine(int lineno);
	  void MakePage();             

};

#endif // !defined(AFX_NEWSTRING_H__E8CD0860_5D59_11D3_9D51_D85FC20A0395__INCLUDED_)
