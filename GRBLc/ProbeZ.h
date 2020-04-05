#pragma once

#include "resource.h"
#include "CustomControl.h"

/////////////////////////////////////////////////////////////////////
// CProbeZ ダイアログ

class CProbeZ : public CProbeDlg
{
	DECLARE_DYNAMIC(CProbeZ)

public:
	CProbeZ(CGRBLdlg*);
	virtual ~CProbeZ();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Z_PROBE };
#endif
	CFloatEdit	m_edLength;
	CFloatEdit	m_edThickness;
	CFloatEdit	m_edDistance;
	CIntEdit	m_edFeed;
	int			m_nSetMethod;
	int			m_nWork;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK() {}
	virtual void OnCancel() {}

public:
	virtual BOOL DestroyWindow();
	afx_msg void OnStart();
	afx_msg void OnPause();
	afx_msg void OnRegist();

	DECLARE_MESSAGE_MAP()
};
