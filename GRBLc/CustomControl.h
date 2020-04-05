// CustomControl.h : ヘッダー ファイル
//

#pragma once

//	コンボボックスのキー入力は
//	CGRBLdlg::PreTranslateMessage() で捉えられない(?)ので
//	コンボボックス自体で処理
#define	MAXADD_CMD		10
class CMyComboBox : public CComboBox
{
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	int		AddString(LPCTSTR);
};

class CMyListBox : public CListBox
{
	void	CopyMessage(void);
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

/////////////////////////////////////////////////////////////////////////////
// CIntEdit ウィンドウ

class CIntEdit : public CEdit
{
public:
	CIntEdit() {};

	operator int();
	CIntEdit& operator =(int);

protected:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CFloatEdit ウィンドウ

class CFloatEdit : public CEdit
{
public:
	CFloatEdit() {}

	operator float();
	CFloatEdit& operator =(float);

protected:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP()
};
