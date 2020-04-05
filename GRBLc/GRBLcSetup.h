#pragma once

#include "resource.h"
#include "NCVCaddin.h"
#include "CustomControl.h"

class CGRBLdlg;

/////////////////////////////////////////////////////////////////////
// CGRBLcSetup ダイアログ

class CGRBLcSetup : public CDialogEx
{
	DECLARE_DYNAMIC(CGRBLcSetup)

public:
	CGRBLcSetup(CGRBLdlg*);
	virtual ~CGRBLcSetup();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GRBLC_SETUP };
#endif
	BOOL		m_bAutoConnect;
	BOOL		m_bWithTrace;
	CIntEdit	m_edQueryTime;
	CComboBox	m_cbComPort;
	CComboBox	m_cbBaudrate;
	CFloatEdit	m_edJogStep;
	CIntEdit	m_edJogFeed;
	CString		m_strCmd[NCXYZ];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
};
