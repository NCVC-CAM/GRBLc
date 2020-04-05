// CustomControl.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "boost/lexical_cast.hpp"
#include "CustomControl.h"
#include "GRBLdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace boost;
using std::string;

BEGIN_MESSAGE_MAP(CIntEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CFloatEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CMyComboBox::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN ) {
		if ( pMsg->wParam == VK_RETURN ) {
			dynamic_cast<CGRBLdlg*>(GetParent())->SendCommand();
			SetEditSel(0, -1);
			return TRUE;
		}
		else if ( pMsg->wParam=='V' && GetKeyState(VK_CONTROL)<0 ) {
			Paste();
			return TRUE;
		}
	}
	return __super::PreTranslateMessage(pMsg);
}

int CMyComboBox::AddString(LPCTSTR lpszAddStr)
{
	if ( GetCount() >= MAXADD_CMD )
		DeleteString(0);
	return __super::AddString(lpszAddStr);
}

/////////////////////////////////////////////////////////////////////////////

BOOL CMyListBox::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN ) {
		if ( GetKeyState(VK_CONTROL) < 0 ) {
			switch ( pMsg->wParam ) {
			case 'A':
				SelItemRange(true, 0, GetCount());
				return TRUE;
			case 'C':
				CopyMessage();
				return TRUE;
			}
		}
	}
	return __super::PreTranslateMessage(pMsg);
}

void CMyListBox::CopyMessage(void)
{
	int		i, nAlloc=0, nCnt=GetSelCount();
	CArray<int, int>	aryListBoxSel;
	CString				strSel;
	CStringArray		arySel;

	aryListBoxSel.SetSize(nCnt);
	GetSelItems(nCnt, aryListBoxSel.GetData());
	ASSERT( nCnt == aryListBoxSel.GetCount() );
	for ( i=0; i<aryListBoxSel.GetCount(); i++ ) {
		GetText( aryListBoxSel[i], strSel );
		strSel += "\r\n";
		nAlloc += strSel.GetLength();
		arySel.Add(strSel);
	}
	if ( nAlloc == 0 )
		return;

	HGLOBAL	hMem = GlobalAlloc(GHND, nAlloc+1);		// 終端NULL
	if ( !hMem )
		return;
	LPTSTR	pLock  = (LPTSTR)GlobalLock(hMem);
	if ( !pLock ) {
		GlobalFree(hMem);
		return;
	}
	for ( i=0; i<arySel.GetCount(); i++ ) {
		lstrcpy(pLock, arySel[i]);
		pLock += arySel[i].GetLength();
	}
	GlobalUnlock(hMem);
	if ( !OpenClipboard() ) {
		GlobalFree(hMem);
		CString	strMsg;
		strMsg.Format("Unable to set Clipboard data. OpenClipboard() error:%d", GetLastError());
		AfxMessageBox(strMsg);
		return;
	}
	if ( !EmptyClipboard() ) {
		CloseClipboard();
		GlobalFree(hMem);
		CString	strMsg;
		strMsg.Format("Unable to set Clipboard data. EmptyClipboard() error:%d", GetLastError());
		AfxMessageBox(strMsg);
		return;
	}
	if ( !SetClipboardData(CF_TEXT, hMem) ) {
		CloseClipboard();
		GlobalFree(hMem);
		CString	strMsg;
		strMsg.Format("Unable to set Clipboard data. SetClipboardData() error:%d", GetLastError());
		AfxMessageBox(strMsg);
		return;
	}
	CloseClipboard();
}

/////////////////////////////////////////////////////////////////////////////
// CIntEdit

CIntEdit& CIntEdit::operator =(int n)
{
	ASSERT(::IsWindow(m_hWnd));
	SetWindowText(lexical_cast<string>(n).c_str());
	return (*this);
}

CIntEdit::operator int()
{
	ASSERT(::IsWindow(m_hWnd));
	CString	strNumber;
	GetWindowText(strNumber);
	return atoi(LPCTSTR(strNumber.Trim()));
}

void CIntEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	static	CString	strLeave("+-");
	TCHAR	tChar = (TCHAR)nChar;

	if ( (IsCharAlphaNumeric(tChar) && !IsCharAlpha(tChar)) ||
				strLeave.Find(tChar) >= 0 || nChar == VK_BACK )
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	else
		::MessageBeep(MB_ICONASTERISK);
}

/////////////////////////////////////////////////////////////////////////////
// CFloatEdit

CFloatEdit& CFloatEdit::operator =(float d)
{
	extern	LPCTSTR		g_szValue;	// "%.3f"

	ASSERT(::IsWindow(m_hWnd));
	CString	strBuf;
	strBuf.Format(g_szValue, d);
	SetWindowText(strBuf);
	return (*this);
}

CFloatEdit::operator float()
{
	ASSERT(::IsWindow(m_hWnd));
	CString	strNumber;
	GetWindowText(strNumber);
	return (float)atof(LPCTSTR(strNumber.Trim()));
}

void CFloatEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	static	CString	strLeave("+-.");
	TCHAR	tChar = (TCHAR)nChar;

	if ( (IsCharAlphaNumeric(tChar) && !IsCharAlpha(tChar)) ||
				strLeave.Find(tChar) >= 0 || nChar == VK_BACK )
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	else
		::MessageBeep(MB_ICONASTERISK);
}
