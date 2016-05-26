// Global.h: interface for the CNewObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBAL_H__E31411F6_5A16_11D3_BD91_0000B48F2506__INCLUDED_)
#define AFX_GLOBAL_H__E31411F6_5A16_11D3_BD91_0000B48F2506__INCLUDED_

#define PROFILETOPIC "Power2000"

#ifndef _INC_DIBAPI
#define _INC_DIBAPI

DECLARE_HANDLE(HDIB);

#define PALVERSION   0x300

#define RECORDMAX   5000

#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))
#define RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top)

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
//----------------------------------------------------//
#endif //!_INC_DIBAPI

//---------------------位图函数-----------------------//
BOOL      WINAPI  PaintDIB (HDC, LPRECT, HANDLE, LPRECT, CPalette* pPal);
BOOL      WINAPI  CreateDIBPalette(HANDLE hDIB, CPalette* cPal);
LPSTR     WINAPI  FindDIBBits (LPSTR lpbi);
DWORD     WINAPI  DIBWidth (LPSTR lpDIB);
DWORD     WINAPI  DIBHeight (LPSTR lpDIB);
WORD      WINAPI  PaletteSize (LPSTR lpbi);
WORD      WINAPI  DIBNumColors (LPSTR lpbi);
HANDLE    WINAPI  CopyHandle (HANDLE h);

BOOL      WINAPI  SaveDIB (HANDLE hDib, CFile& file);
HANDLE    WINAPI  ReadDIBFile(CFile& file);

BOOL      WINAPI  LoadBitmapFile(CString FileName,CBitmap &Bitmap);
HANDLE	  WINAPI  LoadBitmapFile(CString FileName);
void      WINAPI SaveBitmapFile(CString FileName,CBitmap &Bitmap);
//----------------------------------------------------//

//---------------------列表控件函数-------------------//
BOOL WINAPI AddListTitle(CListCtrl& ctList,LPCTSTR strItem,int nItem,int nSubItem ,int nMask,int nFmt);
BOOL WINAPI AddListTitle(CListCtrl& ctList,LPCTSTR strItem,int nItem);
BOOL WINAPI AddListItem(CListCtrl& ctList,int nItem,int nSubItem,LPCTSTR strItem,int nImageIndex);
BOOL WINAPI AddListItem(CListCtrl& ctList,int nItem,int nSubItem,LPCTSTR strItem);
BOOL WINAPI SetItemText(CListCtrl& ctList,int nItem,int nSubItem,LPCTSTR strItem);
BOOL WINAPI SetItemImage(CListCtrl& ctList,int nItem,int nImageIndex);
BOOL WINAPI SortListItem(CListCtrl& ctlList,int nSubItem);
BOOL WINAPI SelectListItem(CListCtrl& ctList,int nItem);
BOOL WINAPI SelectListFirstItem(CListCtrl& ctList);
BOOL WINAPI SelectListLastItem(CListCtrl& ctList);
BOOL WINAPI UnSelectListItem(CListCtrl& ctList,int nItem);
BOOL WINAPI SelectListAllItem(CListCtrl& ctList);
BOOL WINAPI UnSelectListAllItem(CListCtrl& ctList);
BOOL WINAPI DeleteSelectListItem(CListCtrl& ctList);
//----------------------------------------------------//

CString WINAPI GetTextWithLength(CString text,int len);
CString WINAPI GetFormatText(CString format,float value);
CString WINAPI GetFormatText(CString format,WORD value);
CString WINAPI GetFormatText(CString format,DWORD value);
CString WINAPI GetFormatText(CString format,int value);
CString WINAPI GetDataFormat(CString format);

CString WINAPI GetTimeText(CTime ct,WORD MSEL);
CString WINAPI GetTimeText(CTime ct,CString format);
CString WINAPI GetTimeText(CTime ct);
CString WINAPI GetTimeText(DWORD value);
CString WINAPI GetCurrentTimeText();
CString  WINAPI itostr(int value);
CString  WINAPI ftostr(float value);
BYTE WINAPI btobcd(BYTE value);

int GetBit(BYTE value,int bit);
int GetBit(WORD value,int bit);
int GetBit(DWORD value,int bit);
BYTE SetBit(BYTE value,int bit);
WORD SetBit(WORD value,int bit);
DWORD SetBit(DWORD value,int bit);
BYTE ResetBit(BYTE value,int bit);
WORD ResetBit(WORD value,int bit);
DWORD ResetBit(DWORD value,int bit);

//----字符串排序函数---------//
void StringListSortAsc(CStringList &StringList);
void StringListSortDes(CStringList &StringList);
//----------------------------------------------------//
BOOL FileCopy(CString FileNameSource,CString FileNameDect);

int CtrlListDataCopy(CListCtrl &m_ctlList);
int CtrlListDataSave(CListCtrl &ctlList);
int LoadCtrlListColumnWidth(CListCtrl &ctlList,CString ctlListName);
int SaveCtrlListColumnWidth(CListCtrl &ctlList,CString ctlListName);

COLORREF WINAPI GetColorFromString(CString color);
CString WINAPI GetColorText(COLORREF color);

CString WINAPI GetTextHexFromBuffer(BYTE * buf,int buflen);
int WINAPI SetBufFromString(BYTE * buf,CString text);
BYTE WINAPI HexCharToByte(LPCTSTR buf);

int LoadCtrlListColumnWidth(CListCtrl &ctlList,CString ctlListName);
int SaveCtrlListColumnWidth(CListCtrl &ctlList,CString ctlListName);

void WINAPI AddStringToList(CComboBox &ctlList,CString str[]);
BOOL WINAPI GetFileList(CStringList &FileList,CString FileName);

void WINAPI SetWindowFont(CWnd *pWnd);

BOOL WINAPI ShowPicture(CDC *pDC,CRect PictureRect,BOOL FlipHor,BOOL FlipVer,CString FileName);
void WINAPI BitBlt(CBitmap *pdbitmap ,int x,int y,int w,int h,CBitmap *psbitmap,int sx,int sy,DWORD op );
void WINAPI BitBlt(CDC *pdc ,int x,int y,int w,int h,CBitmap *psbitmap,int sx,int sy,DWORD op );
void WINAPI DrawText(CDC *pDC,CRect rect,int align,int mode,int style,COLORREF textcolor,COLORREF backcolor,CString text,BOOL TextMultiLineDisplay);
void WINAPI Line(CDC *pdc,int x1,int y1,int x2,int y2);

#endif // !defined(AFX_GLOBAL_H__E31411F6_5A16_11D3_BD91_0000B48F2506__INCLUDED_)
