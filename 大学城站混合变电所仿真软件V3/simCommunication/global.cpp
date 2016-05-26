#include "stdafx.h"
#include <math.h>
#include <io.h>
#include <direct.h>
#include <errno.h>
#include "global.h"
#include "SortClass.h"
#include <olestd.h>

BOOL WINAPI AddListTitle(CListCtrl& ctList,LPCTSTR strItem,int nItem,int nSubItem,int nMask,int nFmt)
{
	LV_COLUMN lvc;
	lvc.mask = nMask;
	lvc.fmt = nFmt;
	lvc.pszText = (LPTSTR) strItem;
	lvc.cx = ctList.GetStringWidth(lvc.pszText)+16;
	if(nMask & LVCF_SUBITEM){
		if(nSubItem != -1)
			lvc.iSubItem = nSubItem;
		else
			lvc.iSubItem = nItem;
	}
	return ctList.InsertColumn(nItem,&lvc);
}

BOOL WINAPI AddListTitle(CListCtrl& ctList,LPCTSTR strItem,int nItem)
{
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_CENTER;
	lvc.pszText = (LPTSTR) strItem;
	lvc.cx = ctList.GetStringWidth(lvc.pszText)+16;
  lvc.iSubItem = nItem;
	return ctList.InsertColumn(nItem,&lvc);
}

BOOL WINAPI AddListItem(CListCtrl& ctList,int nItem,int nSubItem,LPCTSTR strItem,int nImageIndex)
{
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nItem;
	lvItem.iSubItem = nSubItem;
	lvItem.pszText = (LPTSTR) strItem;
	if(nImageIndex != -1)
  {
		lvItem.mask |= LVIF_IMAGE;
		lvItem.iImage= nImageIndex;
	}
	if(nSubItem == 0)
		return ctList.InsertItem(&lvItem);
	return ctList.SetItem(&lvItem);
}

BOOL WINAPI AddListItem(CListCtrl& ctList,int nItem,int nSubItem,LPCTSTR strItem)
{
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nItem;
	lvItem.iSubItem = nSubItem;
	lvItem.pszText = (LPTSTR) strItem;
	if(nSubItem == 0)
		return ctList.InsertItem(&lvItem);
	return ctList.SetItem(&lvItem);
}

BOOL WINAPI SetItemText(CListCtrl& ctList,int nItem,int nSubItem,LPCTSTR strItem)
{
	LV_COLUMN lvc;
  lvc.mask = LVCF_TEXT|LVCF_WIDTH;
  char buf[80];
	lvc.pszText = (LPTSTR) buf;
  lvc.cchTextMax=80;
  ctList.GetColumn(nSubItem,&lvc);
	if(lvc.cx<=0)
    return FALSE;
  CString Str=ctList.GetItemText(nItem,nSubItem);
  if(strItem==Str)
    return FALSE;
	ctList.SetItemText(nItem,nSubItem,strItem);
  return TRUE;
}

BOOL WINAPI SetItemImage(CListCtrl& ctList,int nItem,int nImageIndex)
{
	LV_ITEM lvItem;
	lvItem.mask = LVIF_IMAGE;
	lvItem.iItem = nItem;
	lvItem.iSubItem = -1;
	lvItem.iImage= nImageIndex;
	return ctList.SetItem(&lvItem);
}

BOOL  WINAPI SortListItem(CListCtrl& ctlList,int nSubItem)
{
  if(ctlList.GetItemCount()<2)
    return FALSE;

  CString str1 = ctlList.GetItemText(0,nSubItem);
  CString str2 = ctlList.GetItemText(ctlList.GetItemCount()-1,nSubItem);
  if(str1==""&&str2=="")
    return TRUE;

  CWaitCursor Cursor;

  str1.MakeUpper();
  str2.MakeUpper();
  CString str = str1;

  if(str1==""||str2=="")
  {
    CSortClass sc(&ctlList, nSubItem, false);//文本
    if(str1>str2)
      sc.Sort(1);//升序
    else
      sc.Sort(0);//降序
    return TRUE;
  }

  if(str.GetLength()>=19&&str[4]=='/'&&str[7]=='/'&&str[10]=='-'&&str[13]==':'&&str[16]==':')  //时间
  {
    CSortClass sc(&ctlList, nSubItem, false);//文本
    if(str1>str2)
      sc.Sort(1);//升序
    else
      sc.Sort(0);//降序
    return TRUE;
  }

  unsigned char FirstChar = str.GetAt(0);
  if((FirstChar>='0'&&FirstChar<='9')||(FirstChar=='+')||(FirstChar=='-')||(FirstChar=='.'))
  {
    CSortClass sc(&ctlList, nSubItem, true);//数字
    if(atof(str1)>atof(str2))
      sc.Sort(1);//升序
    else
      sc.Sort(0);//降序
  }
  else
  {
    CSortClass sc(&ctlList, nSubItem, false);//文本
    if(str1>str2)
      sc.Sort(1);//升序
    else
      sc.Sort(0);//降序
  }

  return TRUE;
}

BOOL WINAPI SelectListItem(CListCtrl& ctList,int nItem)
{
	LV_ITEM lvItem;
	lvItem.mask = LVIF_STATE;
	lvItem.iItem = nItem;
	lvItem.iSubItem = -1;
	lvItem.stateMask = LVIS_SELECTED;
	lvItem.state = LVIS_SELECTED;
	ctList.EnsureVisible(nItem,true);
  return ctList.SetItem(&lvItem);
}

BOOL WINAPI SelectListFirstItem(CListCtrl& ctList)
{
	int nItem = ctList.GetItemCount();
  if(!nItem)
    return FALSE;
	return SelectListItem(ctList,0);
}

BOOL WINAPI SelectListLastItem(CListCtrl& ctList)
{
	int nItem = ctList.GetItemCount();
  if(!nItem)
    return FALSE;
	return SelectListItem(ctList,nItem-1);
}

BOOL WINAPI UnSelectListItem(CListCtrl& ctList,int nItem)
{
        LV_ITEM lvItem;
        lvItem.mask = LVIF_STATE;
        lvItem.iItem = nItem;
        lvItem.iSubItem = -1;
        lvItem.stateMask = LVIS_SELECTED;
        lvItem.state = 0;
        ctList.EnsureVisible(nItem,true);
  return ctList.SetItem(&lvItem);
}

BOOL WINAPI SelectListAllItem(CListCtrl& ctList)
{
  if(!ctList.GetItemCount())
    return FALSE;

  for(int nItem = 1; nItem<ctList.GetItemCount();nItem++)
    SelectListItem(ctList,nItem);

  SelectListItem(ctList,0);
  return TRUE;
}

BOOL WINAPI UnSelectListAllItem(CListCtrl& ctList)
{
  POSITION pos = ctList.GetFirstSelectedItemPosition();
  while(pos)
  {
    int item = ctList.GetNextSelectedItem(pos);
    UnSelectListItem(ctList,item);
  }

  return TRUE;
}

BOOL WINAPI DeleteSelectListItem(CListCtrl& ctlList)
{
  if(ctlList.GetSelectedCount()==0)
  {
    ctlList.DeleteAllItems();
    return TRUE;
  }
  if(ctlList.GetItemCount()==(int)ctlList.GetSelectedCount())
  {
    ctlList.DeleteAllItems();
    return TRUE;
  }
  int selectitem;
  {
    POSITION pos = ctlList.GetFirstSelectedItemPosition();
    if(pos==NULL)
      return FALSE;
    selectitem = ctlList.GetNextSelectedItem(pos);
  }
  {
    POSITION pos = ctlList.GetFirstSelectedItemPosition();
    while(pos)
    {
      int item = ctlList.GetNextSelectedItem(pos);
      ctlList.SetItemText(item,0,"");
    }
    for(int item=1;item<ctlList.GetItemCount();item++)
    {
      if(ctlList.GetItemText(item,0)=="")
      {
        ctlList.DeleteItem(item);
        item--;
      }
    }
    if(ctlList.GetItemText(0,0)=="")
      ctlList.DeleteItem(0);
  }

  if(selectitem<ctlList.GetItemCount())
    ::SelectListItem(ctlList,selectitem);
  else
    ::SelectListFirstItem(ctlList);
  ctlList.SetFocus();

  return TRUE;
}

CString WINAPI GetTextWithLength(CString text,int len)
{
  if(len>=text.GetLength())
    return text;
  CString rettext;
  for(int no=0;no<len;no++)
  {
    if((BYTE)text[no]<0xa0)
      rettext += text[no];
    else
    {
      rettext += text[no];
      no++;
      rettext += text[no];
    }
  }
  return rettext;
}

CString WINAPI GetFormatText(CString format,float value)
{
  if(format=="")
    return "";
  CString text;
  if(format.Find(';')==-1)
    text.Format(format,value);
  else
  {
    CNewString ns(format,";","\r\n");
    if(value)
      text = ns.GetWord(0,1);
    else
      text = ns.GetWord(0,0);
  }
  return text;
}

CString WINAPI GetFormatText(CString format,WORD value)
{
  if(format=="")
    format="%d";
  CString text;
  text.Format(format,value);
  return text;
}

CString WINAPI GetFormatText(CString format,DWORD value)
{
  if(format=="")
    format="%d";
  CString text;
  text.Format(format,value);
  return text;
}

CString WINAPI GetFormatText(CString format,int value)
{
  if(format=="")
    format="%d";
  CString text;
  text.Format(format,value);
  return text;
}

CString WINAPI GetColorText(COLORREF color)
{
  CString text;
  text.Format("%03d%03d%03d",GetRValue(color),GetGValue(color),GetBValue(color));
  return text;
}

COLORREF WINAPI GetColorFromString(CString color)
{
  if(color=="暗红")
    color="128000000";
  if(color=="暗绿")
    color="000128000";
  if(color=="暗兰")
    color="000000128";
  if(color=="亮红")
    color="255000000";
  if(color=="亮绿")
    color="000255000";
  if(color=="亮兰")
    color="000000255";
  if(color=="亮灰")
    color="160160160";
  if(color=="暗灰")
    color="080080080";
  if(color=="纯白")
    color="255255255";
  if(color=="纯黑")
    color="000000000";

  if(color.GetLength()!=9)
  color="000000000";
  int r=atoi(color.Mid(0,3));
  int g=atoi(color.Mid(3,3));
  int b=atoi(color.Mid(6,3));
  return(RGB(r,g,b));
}

CString WINAPI GetTimeText(CTime ct,WORD MSEL)
{
  if(ct<0)
    return("-");
    
  if(ct.GetYear()<=1990)
    return("-");
  CString str=ct.Format("%Y/%m/%d-%X")+::GetFormatText(".%03d",MSEL);
  return str;
}

CString WINAPI GetTimeText(CTime ct)
{
  if(ct<0)
    return("-");
    
  if(ct.GetYear()<=1990)
    return("-");
  CString str=ct.Format("%Y/%m/%d-%X");
  return str;
}

CString WINAPI GetTimeText(CTime ct,CString format)
{
  if(ct.GetYear()<=1990)
    return("-");
  CString str=ct.Format(format);
  return str;
}

CString WINAPI GetTimeText(DWORD value)
{
  if((int)value<0)
    return "-";

  CTimeSpan span(0,0,0,value);
  int day=span.GetDays();
  int hour=span.GetHours();
  int min=span.GetMinutes();
  int sec=span.GetSeconds();
  CString text;
  text.Format("%d-%02d:%02d:%02d",day,hour,min,sec);
  return text;
}

CString WINAPI GetCurrentTimeText()
{
  CString str=GetTimeText(CTime::GetCurrentTime());
  return str;
}

CString WINAPI GetTextHexFromBuffer(BYTE * buf,int buflen)
{
  CString total="";
  for(int pos=0;pos<buflen;pos++)
  {
    CString str;
    str.Format("%02X ",buf[pos]);
    total+=str;
    if(total.GetLength()>=4096)
    {
      total += "....";
      return total;
    }
  }
  return total;
}

int WINAPI SetBufFromString(BYTE * buf,CString text)
{
  CNewString ns(text," ");
  int pos = 0;
  for(int n=0;n<ns.GetWordCount(0);n++)
  { 
    CString p=ns.GetWord(0,n);
    if(p=="")
      continue;
    buf[pos]=HexCharToByte(p.GetBuffer(0));
    pos++;
  }
  return pos;
}

CString  WINAPI itostr(int value)
{
  CString str;
  str.Format("%d",value);
  return str;
}

CString  WINAPI ftostr(float value)
{
  CString str;
  str.Format("%0.0f",value);
  int sing = 8-str.GetLength();
  if(sing<1)
    sing=1;

  CString format;
  format.Format("%%0.%df",sing);
  str.Format(format,value);

  int dot=str.Find('.');
  for(int n=str.GetLength()-1;n>=dot;n--)
  {
    char ch = str.GetAt(n);
    if(ch!='0')
      break;
    str=str.Left(n);
  }
  if(str.GetAt(str.GetLength()-1)=='.')
    str=str.Left(str.GetLength()-1);

  return str;
}

BYTE WINAPI btobcd(BYTE value)
{
  if(value>99)
    return 0;
  int l=value%10;
  int h=value/10;
  return (h<<4)+l;;
}

int GetBit(BYTE value,int bit)
{
  if(bit<0)
    bit=0;
  if(bit>7)
    bit=7;
  if(value&(1<<bit))
    return 1;
  return 0;
}

int GetBit(WORD value,int bit)
{
  if(bit<0)
    bit=0;
  if(bit>15)
    bit=15;
  if(value&(1<<bit))
    return 1;
  return 0;
}

int GetBit(DWORD value,int bit)
{
  if(bit<0)
    bit=0;
  if(bit>31)
    bit=31;
  if(value&(1<<bit))
    return 1;
  return 0;
}

BYTE SetBit(BYTE value,int bit)
{
  if(bit<0)
    bit=0;
  if(bit>7)
    bit=7;
   BYTE mask = 1<<bit;
  value= value|mask;
  return value;
}

WORD SetBit(WORD value,int bit)
{
  if(bit<0)
    bit=0;
  if(bit>15)
    bit=15;
  WORD mask = 1<<bit;
  value= value|mask;
  return value;
}

DWORD SetBit(DWORD value,int bit)
{
  if(bit<0)
    bit=0;
  if(bit>31)
    bit=31;
  DWORD mask = 1<<bit;
  value= value|mask;
  return value;
}

BYTE ResetBit(BYTE value,int bit)
{
  if(bit<0)
    bit=0;
  if(bit>7)
    bit=7;
   BYTE mask = 1<<bit;
  mask = 0xffffffff ^ mask;
  value= value&mask;
  return value;
}

WORD ResetBit(WORD value,int bit)
{
  if(bit<0)
    bit=0;
  if(bit>15)
    bit=15;
  WORD mask = 1<<bit;
  mask = 0xffffffff ^ mask;
  value= value&mask;
  return value;
}

DWORD ResetBit(DWORD value,int bit)
{
  if(bit<0)
    bit=0;
  if(bit>31)
    bit=31;
  DWORD mask = 1<<bit;
  mask = 0xffffffff ^ mask;
  value= value&mask;
  return value;
}

BYTE WINAPI HexCharToByte(LPCTSTR buf)
{
  BYTE l=0x0f;
  BYTE h=0x0f;

  if(buf[1]>='0'&&buf[1]<='9')
    l=buf[1]-'0';
  else if(buf[1]>='A'&&buf[1]<='F')
    l=buf[1]-'A'+10;
  else if(buf[1]>='a'&&buf[1]<='f')
    l=buf[1]-'a'+10;

  if(buf[0]>='0'&&buf[0]<='9')
    h=buf[0]-'0';
  else if(buf[0]>='A'&&buf[0]<='F')
    h=buf[0]-'A'+10;
  else if(buf[0]>='a'&&buf[0]<='f')
    h=buf[0]-'a'+10;
  
  if(buf[1]==0x00)
    l=0;
  return (h*16+l);
}

BOOL FileCopy(CString FileNameSource,CString FileNameDect)
{
  CFile FileSource;
  CFile FileDest;
	if(!FileSource.Open(FileNameSource, CFile::modeRead, NULL))
    return 0;
	if(!FileDest.Open(FileNameDect, CFile::modeCreate | CFile::modeWrite, NULL))
    return 0;
  DWORD FileLen=FileSource.GetLength();
  BYTE buffer[1024];
  DWORD DoneLen=0;
  while(DoneLen<FileLen)
  {
    FileSource.Read(buffer,1024);
    FileDest.Write(buffer,1024);
    DoneLen+=1024;
    if(DoneLen>FileLen)
      DoneLen=FileLen;
    FileSource.Seek(DoneLen,CFile::begin);
    FileDest.Seek(DoneLen,CFile::begin);
  }
  FileSource.Close();
  FileDest.Close();
  return FileLen;
}

int CtrlListDataCopy(CListCtrl &m_ctlList)
{
  if(m_ctlList.GetItemCount()<=0)
  {
    AfxMessageBox("空表不能输出复制");
    return FALSE;
  }

	LV_COLUMN lvc;
  int Col=0;
  CString Str;
  lvc.mask = LVCF_TEXT|LVCF_WIDTH;
  char buf[80];
	lvc.pszText = (LPTSTR) buf;
  lvc.cchTextMax=80;

  CString title;
  while(m_ctlList.GetColumn(Col,&lvc))
  {
    Col++;
    if(lvc.cx)
	    title=title+lvc.pszText+"\t";
  }
  title=title+"\r\n";

  CString list=title;
  for(int no=0;no<m_ctlList.GetItemCount();no++)
  {
    int col=0;
    while(m_ctlList.GetColumn(col,&lvc))
    {
      if(lvc.cx)
      {
        CString text = m_ctlList.GetItemText(no,col);
        list += text+"\t";
      }
      col++;
    }
    list += "\r\n";
  }
  if(::OpenClipboard(NULL))
  {
    ::EmptyClipboard();
    ::CloseClipboard();
  }
  if(m_ctlList.OpenClipboard())
  {
    HANDLE hmem = ::GlobalAlloc(GMEM_FIXED,list.GetLength()+1);
    char *buf = (char*)::GlobalLock(hmem);
    strcpy(buf,list);
    ::SetClipboardData(CF_TEXT,hmem);
    ::GlobalUnlock(hmem);
    ::CloseClipboard();
    return TRUE;
  }
  return FALSE;
}

int CtrlListDataSave(CListCtrl &m_ctlList)
{
  if(m_ctlList.GetItemCount()<=0)
  {
    AfxMessageBox("空表不能输出保存");
    return FALSE;
  }

  CWaitCursor cursor;

  CString filename="*.xls";
  CFileDialog fdlg(FALSE,"xls",filename,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"表格文件(*.xls)|*.xls|文本文件(*.txt)|*.txt||");
  if(fdlg.DoModal()==IDCANCEL)
    return FALSE;
  filename=fdlg.GetPathName();

  int state = ::GetFileAttributes(filename);
  if(state>=0&&state&FILE_ATTRIBUTE_READONLY)
  {
    CString str=filename+" 为只读文件,是否继续保存?";
    if(AfxMessageBox(str,MB_YESNO)==IDNO)
      return FALSE;
    ::SetFileAttributes(filename,FILE_ATTRIBUTE_ARCHIVE|FILE_ATTRIBUTE_NORMAL);
  }

  CFile file;
  CFileException fe;
	if(!file.Open(filename, CFile::modeCreate | CFile::modeWrite, &fe))
	{
    CString str=filename+" 文件存储错误!";
    AfxMessageBox(str,MB_SYSTEMMODAL);
    return FALSE;
	}  
  char buffer[128];
  CArchive ar(&file,CArchive::store,128,buffer);

	LV_COLUMN lvc;
  int Col=0;
  CString Str;
  lvc.mask = LVCF_TEXT|LVCF_WIDTH;
  char buf[80];
	lvc.pszText = (LPTSTR) buf;
  lvc.cchTextMax=80;

  CString title;
  while(m_ctlList.GetColumn(Col,&lvc))
  {
    Col++;
    if(!lvc.cx)
      continue;
	  title=title+lvc.pszText+"\t";
  }
  title=title+"\r\n";
  ar.WriteString(title);
  for(int item=0;item<m_ctlList.GetItemCount();item++)
  {
    title = "";
    int subitem=0;
    while(m_ctlList.GetColumn(subitem,&lvc))
    {
      subitem++;
      if(!lvc.cx)
        continue;
      CString str = m_ctlList.GetItemText(item,subitem-1);
      title=title+str+"\t";
    }
    title=title+"\r\n";
    
    ar.WriteString(title);
  }
  ar.Flush();
  ar.Close();
  file.Close();

  return TRUE;
}

int SaveCtrlListColumnWidth(CListCtrl &ctlList,CString ctlListName)
{
	LV_COLUMN lvc;
  int Col=0;
  CString Str;
  lvc.mask = LVCF_TEXT|LVCF_WIDTH;
  char buf[80];
	lvc.pszText = (LPTSTR) buf;
  lvc.cchTextMax=80;

  CString titlewidth;
  while(ctlList.GetColumn(Col,&lvc))
  {
    CString width;
    width.Format("%d;",lvc.cx);
    titlewidth = titlewidth + width;
    Col++;

  }
  
  AfxGetApp()->WriteProfileString(PROFILETOPIC,ctlListName,titlewidth);
  return TRUE;
}

int LoadCtrlListColumnWidth(CListCtrl &ctlList,CString ctlListName)
{
  CString titlewidth;
  titlewidth = AfxGetApp()->GetProfileString(PROFILETOPIC,ctlListName,"");
  if(titlewidth=="")
    return FALSE;

  CNewString ne(titlewidth,";");
  for(int no=0;no<ne.GetWordCount(0);no++)
  {
    int width = atoi(ne.GetWord(0,no));
    if(width>=0)
      ctlList.SetColumnWidth(no,width);
  }
  
  return TRUE;
}

void WINAPI SetWindowFont(CWnd *pWnd)
{
  CFont *oldfont=pWnd->GetFont();
  CFont *font=new CFont;
  font->CreateFont(12,0,0,0,0,0,0,0,1,0,0,0,1,"宋体");
  pWnd->SetFont(font);
  if(oldfont)
    oldfont->DeleteObject();
}

void StringListSortAsc(CStringList &StringList)
{
  int StringCount = StringList.GetCount();
  if(StringCount<=1)
    return;
  if(StringCount>10000)
    return;

  CString StringGroup[10000];

  for(int no=0;no<StringCount;no++)
  {
    POSITION pos = StringList.FindIndex(no);
    CString string = StringList.GetAt(pos);
    StringGroup[no] = string;
  }
  StringList.RemoveAll();

  for(int SortPos=1;SortPos<StringCount;SortPos++)
  {
    for(int sort=SortPos;sort>=1;sort--)
    {
      int pos1 = sort;    //当前项
      int pos0 = sort-1;  //前一项
      
      CString string1 = StringGroup[pos1];//当前项
      CString string0 = StringGroup[pos0];//前一项

      if(string1>=string0)
        break;

      
      CString tempstring = string1;
      StringGroup[pos1] = string0;
      StringGroup[pos0] = tempstring;
    }
  }

  for(no=0;no<StringCount;no++)
  {
    CString string = StringGroup[no];
    StringList.AddTail(string);
  }
}

void StringListSortDes(CStringList &StringList)
{
  int StringCount = StringList.GetCount();
  if(StringCount<=1)
    return;
  if(StringCount>10000)
    return;

  CString StringGroup[10000];

  for(int no=0;no<StringCount;no++)
  {
    POSITION pos = StringList.FindIndex(no);
    CString string = StringList.GetAt(pos);
    StringGroup[no] = string;
  }
  StringList.RemoveAll();

  for(int SortPos=1;SortPos<StringCount;SortPos++)
  {
    for(int sort=SortPos;sort>=1;sort--)
    {
      int pos1 = sort;    //当前项
      int pos0 = sort-1;  //前一项
      
      CString string1 = StringGroup[pos1];//当前项
      CString string0 = StringGroup[pos0];//前一项

      if(string1<=string0)
        break;

      
      CString tempstring = string1;
      StringGroup[pos1] = string0;
      StringGroup[pos0] = tempstring;
    }
  }

  for(no=0;no<StringCount;no++)
  {
    CString string = StringGroup[no];
    StringList.AddTail(string);
  }
}

void WINAPI AddStringToList(CComboBox &ctlList,CString str[])
{
  int pos=0;
  while(str[pos].GetLength())
    ctlList.AddString(str[pos++]);
}

BOOL WINAPI GetFileList(CStringList &FileList,CString FileName)
{
  CWaitCursor WaitCursor;

  CFileFind ff;
  int Loop=ff.FindFile(FileName);
  while(Loop)
  {
    Loop=ff.FindNextFile();
    if(ff.IsDirectory())
      continue;
    FileList.AddTail(ff.GetFileTitle());
  }
  
  return TRUE;
}

void WINAPI BitBlt(CBitmap *pdbitmap ,int x,int y,int w,int h,CBitmap *psbitmap,int sx,int sy,DWORD op )
{
  CDC sdc,ddc;
  CBitmap *soldbitmap,*doldbitmap;
  sdc.CreateCompatibleDC(NULL);
  ddc.CreateCompatibleDC(NULL);  
  soldbitmap=sdc.SelectObject(psbitmap);
  doldbitmap=ddc.SelectObject(pdbitmap);
  ddc.BitBlt(x,y,w,h,&sdc,sx,sy,op);
  sdc.SelectObject(soldbitmap);
  ddc.SelectObject(doldbitmap);
  sdc.DeleteDC();
  ddc.DeleteDC();
  
}

void WINAPI BitBlt(CDC *pdc ,int x,int y,int w,int h,CBitmap *psbitmap,int sx,int sy,DWORD op )
{
  CBitmap *oldbitmap;
	CDC memdc;
	memdc.CreateCompatibleDC(pdc);
	oldbitmap=memdc.SelectObject(psbitmap);
  pdc->BitBlt(x,y,w,h,&memdc,sx,sy,SRCCOPY);
  memdc.SelectObject(oldbitmap);
  memdc.DeleteDC();
}

BOOL WINAPI ShowPicture(CDC *pDC,CRect PictureRect,BOOL FlipHor,BOOL FlipVer,CString FileName)
{
  CWaitCursor WaitCursor;

  HANDLE hFile = ::CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  DWORD  FileSize = GetFileSize(hFile, NULL);
  if(FileSize<=0)
    return FALSE;

  HGLOBAL hFileBuf = ::GlobalAlloc(GMEM_ZEROINIT|GMEM_FIXED,FileSize);
  if(hFileBuf==NULL)
    return FALSE;

  BYTE *FileBuf = (BYTE *)::GlobalLock(hFileBuf);
  DWORD  ReadFileSize;
  ::ReadFile(hFile,FileBuf,FileSize,&ReadFileSize,NULL);
  if(FileSize!=ReadFileSize)
    return FALSE;

  ::GlobalUnlock(hFileBuf);
  CloseHandle(hFile);

	IPicture *pIPicture = NULL;

	LPSTREAM pStream = NULL;
	HRESULT Result = CreateStreamOnHGlobal(hFileBuf, TRUE, &pStream );
	OleLoadPicture(pStream, 0, FALSE,IID_IPicture, (void **)&pIPicture);
  if(pIPicture==NULL)
    return FALSE;

	OLE_XSIZE_HIMETRIC cx;
	OLE_YSIZE_HIMETRIC cy;
	pIPicture->get_Width(&cx);
	pIPicture->get_Height(&cy);
	
  int PicWidth = MAP_LOGHIM_TO_PIX(cx, pDC->GetDeviceCaps(LOGPIXELSX));
	int PicHeight = MAP_LOGHIM_TO_PIX(cy, pDC->GetDeviceCaps(LOGPIXELSX));

  float ZoomScaleHor = (float)PictureRect.Width() / (float)PicWidth;
  float ZoomScaleVer = (float)PictureRect.Height() / (float)PicHeight;

  float ZoomScale;
  if(ZoomScaleHor<=ZoomScaleVer)
    ZoomScale = ZoomScaleHor;
  else
    ZoomScale = ZoomScaleVer;

  if(ZoomScale==0)
    ZoomScale = 1;
  
  PicWidth = (int)((float)PicWidth * ZoomScale);
  PicHeight = (int)((float)PicHeight * ZoomScale);

  int PicLeft;
  int PicTop;

  if(FlipHor)
  {
    PicLeft = PicWidth;
    PicWidth = -PicWidth;
  }
  else
  {
    PicLeft = 0;
    PicWidth = PicWidth;
  }

  if(FlipVer)
  {
    PicTop = 0;
    PicHeight = PicHeight;
  }
  else
  {
    PicTop = PicHeight;
    PicHeight = -PicHeight;
  }

  CBitmap Bitmap;
  Bitmap.CreateCompatibleBitmap(pDC,PictureRect.Width(),PictureRect.Height());

  CDC memdc;
  memdc.CreateCompatibleDC(pDC);
  CBitmap *oldbitmap=memdc.SelectObject(&Bitmap);
  pIPicture->Render(memdc.GetSafeHdc(),PicLeft, PicTop, PicWidth,PicHeight,0,0,cx,cy,NULL);

  memdc.SelectObject(oldbitmap);
  memdc.DeleteDC();

  PicWidth = (int)::fabs(PicWidth);
  PicHeight = (int)::fabs(PicHeight);
	
  int PicBoderWidth = (PictureRect.Width() - PicWidth) / 2;
  int PicBoderHeight = (PictureRect.Height() - PicHeight) / 2;
  if(PicBoderWidth<0)
    PicBoderWidth = 0;
  if(PicBoderHeight<0)
    PicBoderHeight = 0;

  PicLeft = PictureRect.left + PicBoderWidth;
  PicTop = PictureRect.top + PicBoderHeight * 2;

  BitBlt(pDC,PicLeft,PicTop,PicWidth,PicHeight,&Bitmap,0,0,SRCCOPY);
  Bitmap.DeleteObject();

  #ifdef _DEBUG
    return TRUE;
  #endif

  ::GlobalFree(hFileBuf);
	pStream->Release();
	pIPicture->Release();

  return TRUE;
}

void WINAPI DrawText(CDC *pDC,CRect rect,int align,int mode,int style,COLORREF textcolor,COLORREF backcolor,CString text,BOOL TextMultiLineDisplay = FALSE)
{
  if(text=="")
    return;

  CSize size=pDC->GetTextExtent(text,strlen(text));
  int px,py;
  switch(align)
  {
    case 0://左中
      if(!mode) //横
      {
        px=8;
        py=(rect.Height()-size.cy)/2;
      }
      else
      {
        px=size.cy+8;
        py=(rect.Height()-size.cx)/2;
      }
      break;
    case 1: //中中
      if(!mode) //横
      {
        if(TextMultiLineDisplay||(size.cx<=(rect.Width()-size.cy)))//一行显示
        {
          px=(rect.Width()-size.cx)/2;
          py=(rect.Height()-size.cy)/2;
        }
        else//判断可以多行显示
        {
          int line = (int)(((float)rect.Height() / (float)(size.cy))) ;
          if(line<=1)//高度不够，按单行显示
          {
            px=(rect.Width()-size.cx)/2;
            py=(rect.Height()-size.cy)/2;
          }
          else
          {
            CRect retrect;
            retrect.left = rect.left;
            retrect.right = rect.right;
            int textwidth = text.GetLength() / 2;

            retrect.top = rect.top;
            retrect.bottom = rect.top + rect.Height()*7/11;
            CString text1 = GetTextWithLength(text,textwidth);
            ::DrawText(pDC,retrect,align,mode,style,textcolor,backcolor,text1,TRUE);

            retrect.top = rect.top + rect.Height()*4/11;
            retrect.bottom = rect.bottom;
            CString text2 = text.Mid(text1.GetLength());
            ::DrawText(pDC,retrect,align,mode,style,textcolor,backcolor,text2,TRUE);

          }
        }
      }
      else
      {
        px=(rect.Width()-size.cy)/2+size.cy;
        py=(rect.Height()-size.cx)/2;
      }
      break;
    case 2: //右中
      if(!mode) //横
      {
        px=rect.Width()-size.cx-8;
        py=(rect.Height()-size.cy)/2;
      }
      else
      {
        px=rect.Width()-8;
        py=(rect.Height()-size.cx)/2;
      }
      break;
    case 3://左上
      if(!mode) //横
      {
        px=8;
        py=8;
      }
      else
      {
        px=size.cy+8;
        py=8;
      }
      break;
    case 4: //中上
      if(!mode) //横
      {
        px=(rect.Width()-size.cx)/2;
        py=8;
      }
      else
      {
        px=(rect.Width()-size.cy)/2+size.cy;
        py=8;
      }
      break;
    case 5: //右上
      if(!mode) //横
      {
        px=rect.Width()-size.cx-8;
        py=8;
      }
      else
      {
        px=rect.Width()-8;
        py=8;
      }
      break;
    case 6://左下
      if(!mode) //横
      {
        px=8;
        py=(rect.Height()-size.cy)-8;
      }
      else
      {
        px=size.cy+8;
        py=(rect.Height()-size.cx)-8;
      }
      break;
    case 7: //中下
      if(!mode) //横
      {
        px=(rect.Width()-size.cx)/2;
        py=(rect.Height()-size.cy)-8;
      }
      else
      {
        px=(rect.Width()-size.cy)/2+size.cy;
        py=(rect.Height()-size.cx)-8;
      }
      break;
    case 8: //右下
      if(!mode) //横
      {
        px=rect.Width()-size.cx-8;
        py=(rect.Height()-size.cy)-8;
      }
      else
      {
        px=rect.Width()-8;
        py=(rect.Height()-size.cx)-8;
      }
      break;
  }
  int left=rect.left;
  int top=rect.top;
  switch(style)
  {
    case 0: //normal
      pDC->SetTextColor(textcolor);
      pDC->TextOut(px+left,py+top,text);
      break;
    case 1:
      pDC->SetTextColor(backcolor);
      pDC->TextOut(px+1+left,py+1+top,text);
      pDC->SetTextColor(textcolor);
      pDC->TextOut(px+left,py+top,text);
      break;
    case 2:
      pDC->SetTextColor(backcolor);
      pDC->TextOut(px-1+left,py-1+top,text);
      pDC->TextOut(px+left,py-1+top,text);
      pDC->TextOut(px+1+left,py-1+top,text);
      pDC->TextOut(px-1+left,py+top,text);
      pDC->TextOut(px+1+left,py+top,text);
      pDC->TextOut(px-1+left,py+1+top,text);
      pDC->TextOut(px+left,py+1+top,text);
      pDC->TextOut(px+1+left,py+1+top,text);
      pDC->SetTextColor(textcolor);
      pDC->TextOut(px+left,py+top,text);
      break;
  }
}

void WINAPI Line(CDC *pdc,int x1,int y1,int x2,int y2)
{
  pdc->MoveTo(x1,y1);
  pdc->LineTo(x2,y2);
}

CString WINAPI GetDataFormat(CString format)
{
   if(format=="")
     return "";
   if(format=="00H")
     return "%02X";
   if(format=="0000H")
     return "%04X";
   if(format=="00000000H")
     return "%08X";
   if(format=="$00H")
     return "$%02X";
   if(format=="$0000H")
     return "$%04X";
   if(format=="$00000000H")
     return "$%08X";

   for(int i=0;i<format.GetLength();i++)
     if(format[i]>='0'&&format[i]<='9')
       break;
   CString mark=format.Left(i);
   if(mark!=format)
     format=format.Mid(i);
   else
     return format;

   for(i=0;i<format.GetLength();i++)
     if((format[i]<'0'||format[i]>'9')&&format[i]!='.')
       break;
   CString unit=format.Mid(i);
   format=format.Left(i);

   char buf[80];
   int a=format.Find(".");
   if(a<0)
   {
     CString fmt;
     if(mark.Right(1)=='-'||mark.Right(1)=='+')
     {
       sprintf(buf,"%d",format.GetLength()+1);
       fmt=buf;
       mark=mark.Left(mark.GetLength()-1);
       return(mark+"%+0"+fmt+"d"+unit);
     }  
     sprintf(buf,"%d",format.GetLength());
     fmt=buf;
     return(mark+"%0"+fmt+".0f"+unit);
   }
   else
   {
     int b=format.GetLength()-a-1;
     CString fmt;
     if(mark.Right(1)=='-'||mark.Right(1)=='+')
     {
       sprintf(buf,"%d",format.GetLength()+1);
       fmt=buf;
       sprintf(buf,"%d",b);
       fmt=fmt+"."+buf;
       mark=mark.Left(mark.GetLength()-1);
       return(mark+"%+0"+fmt+"f"+unit);
     }  
     sprintf(buf,"%d",format.GetLength());
     fmt=buf;
     sprintf(buf,"%d",b);
     fmt=fmt+"."+buf;
     return(mark+"%0"+fmt+"f"+unit);
   }

}

#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')

BOOL WINAPI SaveDIB(HANDLE hDib, CFile& file)
{
	BITMAPFILEHEADER bmfHdr; // Header for Bitmap file
	LPBITMAPINFOHEADER lpBI;   // Pointer to DIB info structure
	DWORD dwDIBSize;

	if (hDib == NULL)
		return FALSE;

	/*
	 * Get a pointer to the DIB memory, the first of which contains
	 * a BITMAPINFO structure
	 */
	lpBI = (LPBITMAPINFOHEADER) ::GlobalLock((HGLOBAL) hDib);
	if (lpBI == NULL)
		return FALSE;

	if (!IS_WIN30_DIB(lpBI))
	{
		::GlobalUnlock((HGLOBAL) hDib);
		return FALSE;       // It's an other-style DIB (save not supported)
	}

	/*
	 * Fill in the fields of the file header
	 */

	/* Fill in file type (first 2 bytes must be "BM" for a bitmap) */
	bmfHdr.bfType = DIB_HEADER_MARKER;  // "BM"

	// Calculating the size of the DIB is a bit tricky (if we want to
	// do it right).  The easiest way to do this is to call GlobalSize()
	// on our global handle, but since the size of our global memory may have
	// been padded a few bytes, we may end up writing out a few too
	// many bytes to the file (which may cause problems with some apps).
	//
	// So, instead let's calculate the size manually (if we can)
	//
	// First, find size of header plus size of color table.  Since the
	// first DWORD in both BITMAPINFOHEADER and BITMAPCOREHEADER conains
	// the size of the structure, let's use this.

	dwDIBSize = *(LPDWORD)lpBI + ::PaletteSize((LPSTR)lpBI);  // Partial Calculation

	// Now calculate the size of the image

	if ((lpBI->biCompression == BI_RLE8) || (lpBI->biCompression == BI_RLE4))
	{
		// It's an RLE bitmap, we can't calculate size, so trust the
		// biSizeImage field

		dwDIBSize += lpBI->biSizeImage;
	}
	else
	{
		DWORD dwBmBitsSize;  // Size of Bitmap Bits only

		// It's not RLE, so size is Width (DWORD aligned) * Height

		dwBmBitsSize = WIDTHBYTES((lpBI->biWidth)*((DWORD)lpBI->biBitCount)) * lpBI->biHeight;

		dwDIBSize += dwBmBitsSize;

		// Now, since we have calculated the correct size, why don't we
		// fill in the biSizeImage field (this will fix any .BMP files which
		// have this field incorrect).

		lpBI->biSizeImage = dwBmBitsSize;
	}


	// Calculate the file size by adding the DIB size to sizeof(BITMAPFILEHEADER)

	bmfHdr.bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;

	/*
	 * Now, calculate the offset the actual bitmap bits will be in
	 * the file -- It's the Bitmap file header plus the DIB header,
	 * plus the size of the color table.
	 */
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + lpBI->biSize
											  + PaletteSize((LPSTR)lpBI);

	TRY
	{
		// Write the file header
		file.Write((LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER));
		//
		// Write the DIB header and the bits
		//
		file.WriteHuge(lpBI, dwDIBSize);
	}
	CATCH (CFileException, e)
	{
		::GlobalUnlock((HGLOBAL) hDib);
		THROW_LAST();
	}
	END_CATCH

	::GlobalUnlock((HGLOBAL) hDib);
	return TRUE;
}

HANDLE WINAPI ReadDIBFile(CFile& file)
{
	BITMAPFILEHEADER bmfHeader;
	DWORD dwBitsSize;
	HANDLE hDIB;
	LPSTR pDIB;

	/*
	 * get length of DIB in bytes for use when reading
	 */

	dwBitsSize = file.GetLength();

	/*
	 * Go read the DIB file header and check if it's valid.
	 */
	if ((file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) !=
		sizeof(bmfHeader)) || (bmfHeader.bfType != DIB_HEADER_MARKER))
	{
		return NULL;
	}
	/*
	 * Allocate memory for DIB
	 */
	hDIB = (HANDLE) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize);
	if (hDIB == 0)
	{
		return NULL;
	}
	pDIB = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);

	/*
	 * Go read the bits.
	 */
	if (file.ReadHuge(pDIB, dwBitsSize - sizeof(BITMAPFILEHEADER)) !=
		dwBitsSize - sizeof(BITMAPFILEHEADER) )
	{
		::GlobalUnlock((HGLOBAL) hDIB);
		::GlobalFree((HGLOBAL) hDIB);
		return NULL;
	}
	::GlobalUnlock((HGLOBAL) hDIB);
	return hDIB;
}

BOOL WINAPI PaintDIB(HDC     hDC,
					LPRECT  lpDCRect,
					HANDLE    hDIB,
					LPRECT  lpDIBRect,
					CPalette* pPal)
{
	LPSTR    lpDIBHdr;            // Pointer to BITMAPINFOHEADER
	LPSTR    lpDIBBits;           // Pointer to DIB bits
	BOOL     bSuccess=FALSE;      // Success/fail flag
	HPALETTE hPal=NULL;           // Our DIB's palette
	HPALETTE hOldPal=NULL;        // Previous palette

	/* Check for valid DIB handle */
	if (hDIB == NULL)
		return FALSE;

	/* Lock down the DIB, and get a pointer to the beginning of the bit
	 *  buffer
	 */
	lpDIBHdr  = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);
	lpDIBBits = ::FindDIBBits(lpDIBHdr);

	// Get the DIB's palette, then select it into DC
	if (pPal != NULL)
	{
		hPal = (HPALETTE) pPal->m_hObject;

		// Select as background since we have
		// already realized in forground if needed
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);
	}

	/* Make sure to use the stretching mode best for color pictures */
	//::SetStretchBltMode(hDC, COLORONCOLOR);

	/* Determine whether to call StretchDIBits() or SetDIBitsToDevice() */
	  bSuccess = ::StretchDIBits(hDC,                          // hDC
							   lpDCRect->left,                 // DestX
							   lpDCRect->top,                  // DestY
							   RECTWIDTH(lpDCRect),            // nDestWidth
							   RECTHEIGHT(lpDCRect),           // nDestHeight
							   lpDIBRect->left,                // SrcX
							   lpDIBRect->top,                 // SrcY
							   RECTWIDTH(lpDIBRect),           // wSrcWidth
							   RECTHEIGHT(lpDIBRect),          // wSrcHeight
							   lpDIBBits,                      // lpBits
							   (LPBITMAPINFO)lpDIBHdr,         // lpBitsInfo
							   DIB_RGB_COLORS,                 // wUsage
							   SRCCOPY);                       // dwROP

   ::GlobalUnlock((HGLOBAL) hDIB);

	/* Reselect old palette */
	if (hOldPal != NULL)
	{
		::SelectPalette(hDC, hOldPal, TRUE);
	}

   return bSuccess;
}

BOOL WINAPI CreateDIBPalette(HANDLE hDIB, CPalette* pPal)
{
	LPLOGPALETTE lpPal;      // pointer to a logical palette
	HANDLE hLogPal;          // handle to a logical palette
	HPALETTE hPal = NULL;    // handle to a palette
	int i;                   // loop index
	WORD wNumColors;         // number of colors in color table
	LPSTR lpbi;              // pointer to packed-DIB
	LPBITMAPINFO lpbmi;      // pointer to BITMAPINFO structure (Win3.0)
	LPBITMAPCOREINFO lpbmc;  // pointer to BITMAPCOREINFO structure (old)
	BOOL bWinStyleDIB;       // flag which signifies whether this is a Win3.0 DIB
	BOOL bResult = FALSE;

	/* if handle to DIB is invalid, return FALSE */

	if (hDIB == NULL)
	  return FALSE;

   lpbi = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);

   /* get pointer to BITMAPINFO (Win 3.0) */
   lpbmi = (LPBITMAPINFO)lpbi;

   /* get pointer to BITMAPCOREINFO (old 1.x) */
   lpbmc = (LPBITMAPCOREINFO)lpbi;

   /* get the number of colors in the DIB */
   wNumColors = ::DIBNumColors(lpbi);

   if (wNumColors != 0)
   {
		/* allocate memory block for logical palette */
		hLogPal = ::GlobalAlloc(GHND, sizeof(LOGPALETTE)
									+ sizeof(PALETTEENTRY)
									* wNumColors);

		/* if not enough memory, clean up and return NULL */
		if (hLogPal == 0)
		{
			::GlobalUnlock((HGLOBAL) hDIB);
			return FALSE;
		}

		lpPal = (LPLOGPALETTE) ::GlobalLock((HGLOBAL) hLogPal);

		/* set version and number of palette entries */
		lpPal->palVersion = PALVERSION;
		lpPal->palNumEntries = (WORD)wNumColors;

		/* is this a Win 3.0 DIB? */
		bWinStyleDIB = IS_WIN30_DIB(lpbi);
		for (i = 0; i < (int)wNumColors; i++)
		{
			if (bWinStyleDIB)
			{
				lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;
				lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
				lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;
				lpPal->palPalEntry[i].peFlags = 0;
			}
			else
			{
				lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;
				lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;
				lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;
				lpPal->palPalEntry[i].peFlags = 0;
			}
		}

		/* create the palette and get handle to it */
		bResult = pPal->CreatePalette(lpPal);
		::GlobalUnlock((HGLOBAL) hLogPal);
		::GlobalFree((HGLOBAL) hLogPal);
	}

	::GlobalUnlock((HGLOBAL) hDIB);

	return bResult;
}

LPSTR WINAPI FindDIBBits(LPSTR lpbi)
{
	return (lpbi + *(LPDWORD)lpbi + ::PaletteSize(lpbi));
}

DWORD WINAPI DIBWidth(LPSTR lpDIB)
{
	LPBITMAPINFOHEADER lpbmi;
	LPBITMAPCOREHEADER lpbmc;

	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	if (IS_WIN30_DIB(lpDIB))
		return lpbmi->biWidth;
	else  
		return (DWORD)lpbmc->bcWidth;
}

DWORD WINAPI DIBHeight(LPSTR lpDIB)
{
	LPBITMAPINFOHEADER lpbmi;
	LPBITMAPCOREHEADER lpbmc;

	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	if (IS_WIN30_DIB(lpDIB))
		return lpbmi->biHeight;
	else  
		return (DWORD)lpbmc->bcHeight;
}

WORD WINAPI PaletteSize(LPSTR lpbi)
{
   if (IS_WIN30_DIB (lpbi))
	  return (WORD)(::DIBNumColors(lpbi) * sizeof(RGBQUAD));
   else
	  return (WORD)(::DIBNumColors(lpbi) * sizeof(RGBTRIPLE));
}

WORD WINAPI DIBNumColors(LPSTR lpbi)
{
	WORD wBitCount;  

	if (IS_WIN30_DIB(lpbi))
	{
		DWORD dwClrUsed;

		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;
		if (dwClrUsed != 0)
			return (WORD)dwClrUsed;
	}

	if (IS_WIN30_DIB(lpbi))
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	else
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;

	switch (wBitCount)
	{
		case 1:
			return 2;

		case 4:
			return 16;

		case 8:
			return 256;

		default:
			return 0;
	}
}

HGLOBAL WINAPI CopyHandle (HGLOBAL h)
{
	if (h == NULL)
		return NULL;

	DWORD dwLen = ::GlobalSize((HGLOBAL) h);
	HGLOBAL hCopy = ::GlobalAlloc(GHND, dwLen);

	if (hCopy != NULL)
	{
		void* lpCopy = ::GlobalLock((HGLOBAL) hCopy);
		void* lp     = ::GlobalLock((HGLOBAL) h);
		memcpy(lpCopy, lp, dwLen);
		::GlobalUnlock(hCopy);
		::GlobalUnlock(h);
	}

	return hCopy;
}

BOOL WINAPI LoadBitmapFile(CString FileName,CBitmap &Bitmap)
{
	int cxDIB,cyDIB;
	
	FileName.MakeUpper();
	
	CFile file;    
	CFileException fe;

	if(!file.Open(FileName, CFile::modeRead, &fe))
    return FALSE;
	
	HANDLE m_hDIB = ::ReadDIBFile(file);

	file.Close();

	if (m_hDIB == NULL)
    return FALSE;

	LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL) m_hDIB);
	cxDIB = (int) ::DIBWidth(lpDIB);         // Size of DIB - x
	cyDIB = (int) ::DIBHeight(lpDIB);        // Size of DIB - y
	LPSTR lpDIBBits = ::FindDIBBits(lpDIB);
  ::GlobalUnlock((HGLOBAL) m_hDIB);
	
	
	CWnd wnd;
	CClientDC dc(wnd.GetDesktopWindow());
	CDC memdc;
  memdc.CreateCompatibleDC(&dc);

	Bitmap.CreateCompatibleBitmap(&dc,cxDIB,cyDIB);
	CBitmap *oldbitmap = memdc.SelectObject(&Bitmap);
	CRect rect(0,0,cxDIB,cyDIB);
	::PaintDIB(memdc.m_hDC,&rect,m_hDIB,&rect,NULL);
	memdc.SelectObject(oldbitmap);
  memdc.DeleteDC();
  ::GlobalFree((HGLOBAL) m_hDIB);
    
  return TRUE;
}

HANDLE WINAPI LoadBitmapFile(CString FileName)
{
	FileName.MakeUpper();
	
	CFile file;    
	CFileException fe;

	if(!file.Open(FileName, CFile::modeRead, &fe))
    return FALSE;
	
	HANDLE m_hDIB = ::ReadDIBFile(file);

	file.Close();

	return m_hDIB;
}

void WINAPI SaveBitmapFile(CString FileName,CBitmap &Bitmap)
{
	CWaitCursor WaitCursor;

  CClientDC dc(NULL);
	BITMAP btm;
	Bitmap.GetBitmap(&btm);
	DWORD size=btm.bmWidthBytes*btm.bmHeight;
	LPSTR lpData=(LPSTR)GlobalAlloc(GPTR,size);
/////////////////////////////////////////////
	BITMAPINFOHEADER bih;
	bih.biBitCount=btm.bmBitsPixel;
	bih.biClrImportant=0;
	bih.biClrUsed=0;
	bih.biCompression=0;
	bih.biHeight=btm.bmHeight;
	bih.biPlanes=1;
	bih.biSize=sizeof(BITMAPINFOHEADER);
	bih.biSizeImage=size;
	bih.biWidth=btm.bmWidth;
	bih.biXPelsPerMeter=0;
	bih.biYPelsPerMeter=0;
///////////////////////////////////
	GetDIBits(dc,Bitmap,0,bih.biHeight,lpData,(BITMAPINFO*)&bih,DIB_RGB_COLORS);
//	bm.GetBitmapBits(size,lpData);	//此函数在处理5-5-5模式的16位色下会出现颜色混乱
//////////////////////////////
	BITMAPFILEHEADER bfh;
	bfh.bfReserved1=bfh.bfReserved2=0;
	bfh.bfType=((WORD)('M'<< 8)|'B');
	bfh.bfSize=54+size;
	bfh.bfOffBits=54;
	CFile bf;
	if(bf.Open(FileName,CFile::modeCreate|CFile::modeWrite)){
		bf.WriteHuge(&bfh,sizeof(BITMAPFILEHEADER));
		bf.WriteHuge(&bih,sizeof(BITMAPINFOHEADER));
		bf.WriteHuge(lpData,size);
		bf.Close();
	}
	GlobalFree(lpData);
}
