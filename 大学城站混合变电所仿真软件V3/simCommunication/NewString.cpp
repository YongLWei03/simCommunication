// NewString.cpp: implementation of the CNewString class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NewString.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WORD WINAPI GetUnitIDFromVarInfo(CString VarInfo)
{
  WORD UnitID = atoi(VarInfo.Mid(0,2));
  return UnitID;
}

WORD WINAPI GetVarIDFromVarInfo(CString VarInfo)
{
  WORD VarID = 0;
  if(VarInfo.GetLength()>=5)
    VarID = atoi(VarInfo.Mid(3,2));
  return VarID;
}

TagVarStruct* WINAPI GetVarFromVarInfo(CString VarInfo,WORD VarGroup)
{
  TagVarStruct *pVar = NULL;

  WORD UnitID = ::GetUnitIDFromVarInfo(VarInfo);
  WORD VarID = ::GetVarIDFromVarInfo(VarInfo);
  if(g_Unit[UnitID].UnitType>0)
    pVar = &g_Unit[UnitID].VarGroup[VarGroup].Var[VarID];
  
  return pVar;
}

void WINAPI GetVarListFromVarListInfo(CPtrList &VarList,CString VarListInfo,WORD VarGroup)
{
  CNewString ns(VarListInfo,".",";"); 
  for(int no=0;no<ns.GetLineCount();no++)
  {
    CString VarInfo = ns.GetLine(no);
    WORD UnitID = ::GetUnitIDFromVarInfo(VarInfo);
    WORD VarID = ::GetVarIDFromVarInfo(VarInfo);
    if(g_Unit[UnitID].UnitType>0)
    {
      TagVarStruct *pVar = &g_Unit[UnitID].VarGroup[VarGroup].Var[VarID];
      VarList.AddTail(pVar);
    }
  }
}

CString WINAPI GetUnitNameFromVarInfo(CString VarInfo)
{
  CString UnitName;

  WORD UnitID = ::GetUnitIDFromVarInfo(VarInfo);
  if(g_Unit[UnitID].UnitType>0)
    UnitName = g_Unit[UnitID].UnitName;

  return UnitName;
}

CString WINAPI GetVarNameFromVarInfo(CString VarInfo,WORD VarGroup)
{
  CString VarName;

  TagVarStruct *pVar = ::GetVarFromVarInfo(VarInfo,VarGroup);
  if(pVar)
    VarName = pVar->VarName;

  return VarName;
}

CString WINAPI GetVarInfoFromVar(TagVarStruct *pVar)
{
  CString VarInfo = "";
  if(pVar)
    VarInfo.Format("%02d.%02d;",pVar->UnitID,pVar->VarID);

  return VarInfo;
}

CString WINAPI GetVarListInfoFromVarList(CPtrList &VarList)
{
  CString VarListInfo = "";
  POSITION pos = VarList.GetHeadPosition();
  while(pos)
  {
    TagVarStruct *pVar = (TagVarStruct *)VarList.GetNext(pos);
    VarListInfo += ::GetVarInfoFromVar(pVar);
  }

  return VarListInfo;
}

CNewString::CNewString(CString page,CString wordmark,CString linemark)
{
  m_Page=page;
  m_LineMark=linemark;
  m_WordMark=wordmark;
  m_CurrentLine=-1;
  MakeLineList();
}

CNewString::~CNewString(void)
{
  m_LineList.RemoveAll();
  m_WordList.RemoveAll();
}
  
CString CNewString::GetPage()
{
  return m_Page;
}

void CNewString::SetPage(CString page)
{
  m_Page=page;
}

int CNewString::GetLineCount()
{
  return m_LineList.GetSize();
}

void CNewString::MakeLineList()
{
  CString page=m_Page;
  m_LineList.RemoveAll();
  
  while(page.GetLength())
  {
    int pos=page.Find(m_LineMark);
    if(pos<0)
    {  
      m_LineList.Add(page);
      return;
    }
    m_LineList.Add(page.Left(pos));
    page=page.Mid(pos+m_LineMark.GetLength());
  }
}

void CNewString::AddLine(CString line)
{
  m_LineList.Add(line);
  MakePage();
}

void CNewString::RemoveLine(int lineno)
{
  if(lineno>=GetLineCount())
    return ;
  m_LineList.RemoveAt(lineno);
  MakePage();
}

void CNewString::InsertLine(int lineno,CString line)
{
  if(lineno>=GetLineCount())
    return ;
  m_LineList.InsertAt(lineno,line);
  MakePage();
}

void CNewString::SetLine(int lineno,CString line)
{
  while(lineno>=GetLineCount())
    m_LineList.Add("");
  m_LineList[lineno]=line;
  MakePage();
}

CString CNewString::GetLine(int lineno)
{
  if(lineno<0)
    lineno=0;
  if(lineno>=GetLineCount())
    return "";
  return m_LineList[lineno];  
}

int CNewString::GetIntegerValue(int lineno)
{
  CString word = GetLine(lineno);
  return (int)atoi(word);
}

float CNewString::GetFloatValue(int lineno)
{
  CString word = GetLine(lineno);
  return (float)atof(word);
}

COLORREF CNewString::GetColorValue(int lineno)
{
  CString word = GetLine(lineno);
  return GetColorFromString(word);
}

int CNewString::GetWordCount(int lineno)
{
  MakeWordList(lineno);
  return m_WordList.GetSize();
}

void CNewString::MakeWordList(int lineno)
{
  if(m_CurrentLine == lineno)
    return;
  m_CurrentLine = lineno;

  m_WordList.RemoveAll();
  if(lineno>=GetLineCount())
    return;
  CString line=m_LineList[lineno];
  while(line.GetLength())
  {
    CString temp;
    int pos=line.Find(m_WordMark);
    if(pos<0)
    {  
      m_WordList.Add(line);
      return ;
    }
    m_WordList.Add(line.Left(pos));
    line=line.Mid(pos+m_WordMark.GetLength());
  }
}

CString CNewString::GetWord(int lineno,int wordno)
{
  if(lineno<0)
    lineno=0;
  if(wordno<0)
    wordno=0;

  MakeWordList(lineno);
  if(wordno>=m_WordList.GetSize())
    return "";
  return m_WordList[wordno];
}

int CNewString::GetIntegerValue(int lineno,int wordno)
{
  CString word = GetWord(lineno,wordno);
  return (int)atoi(word);
}

float CNewString::GetFloatValue(int lineno,int wordno)
{
  CString word = GetWord(lineno,wordno);
  return (float)atof(word);
}

COLORREF CNewString::GetColorValue(int lineno,int wordno)
{
  CString word = GetWord(lineno,wordno);
  return GetColorFromString(word);
}

void CNewString::SetWord(int lineno,int wordno,CString word)
{
  while(lineno>=GetLineCount())
    m_LineList.Add("");
  MakeWordList(lineno);
  while(wordno>=m_WordList.GetSize())
    m_WordList.Add("");
  m_WordList[wordno]=word;
  MakeLine(lineno);
}

void CNewString::AddWord(int lineno,CString word)
{
  if(lineno>=GetLineCount())
    return ;
  MakeWordList(lineno);
  m_WordList.Add(word);
  MakeLine(lineno);
}

void CNewString::InsertWord(int lineno,int wordno,CString word)
{
  if(lineno>=GetLineCount())
    return ;
  MakeWordList(lineno);
  m_WordList.InsertAt(wordno,word);
  MakeLine(lineno);
}

void CNewString::MakePage()
{
  m_CurrentLine = -1;
  m_Page="";
  for(int l=0;l<GetLineCount();l++)
    m_Page+=m_LineList.GetAt(l)+m_LineMark;
}

void CNewString::MakeLine(int lineno)
{
  if(lineno>=GetLineCount())
    return;
  MakeWordList(lineno);
  CString line="";
  for(int w=0;w<m_WordList.GetSize();w++)
    line+=m_WordList.GetAt(w)+m_WordMark;
  m_LineList[lineno]=line;
  MakePage();
}
