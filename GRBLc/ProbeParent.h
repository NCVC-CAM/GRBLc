#pragma once

#include "resource.h"

class CGRBLdlg;

/////////////////////////////////////////////////////////////////////
// CProbeDlg

class CProbeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProbeDlg)
	DECLARE_MESSAGE_MAP()
protected:
	CGRBLdlg*	m_pParent;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	CProbeDlg(UINT, CGRBLdlg*);
};

#include "ProbeZ.h"
#include "ProbeXY.h"

/////////////////////////////////////////////////////////////////////
// CProbeParent ダイアログ

class CProbeParent : public CProbeDlg
{
	CProbeZ		m_dlgZ;
	CProbeXY	m_dlgXY;

	DECLARE_DYNAMIC(CProbeParent)

public:
	CProbeParent(CGRBLdlg*);
	virtual ~CProbeParent();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROBE };
#endif
	CTabCtrl m_ctrlTab;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK() {}
	virtual void OnCancel();

public:
	afx_msg void OnSelchangeTabProbe(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};
