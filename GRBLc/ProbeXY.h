#pragma once

#include "resource.h"
#include "CustomControl.h"

#define	X_MODE			0
#define	Y_MODE			1
#define	CIRCLE_MODE		2

/////////////////////////////////////////////////////////////////////
// CProbeXY ダイアログ

class CProbeXY : public CProbeDlg
{
	std::string	GetProbeModeXY(void);
	int		RectCenter(void);
	int		CircleCenter(void);

	DECLARE_DYNAMIC(CProbeXY)

public:
	CProbeXY(CGRBLdlg*);
	virtual ~CProbeXY();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_XY_PROBE };
#endif
	CFloatEdit	m_edLength;
	CFloatEdit	m_edDistance;
	CIntEdit	m_edFeed;
	CButton		m_btAxisX[3];
	CButton		m_btAxisY[3];
	CStatic		m_stAxisX[4];
	CStatic		m_stAxisY[4];
	int			m_nProbeMode;
	int			m_nWork;
	int			m_nSetX;
	int			m_nSetY;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK() {}
	virtual void OnCancel() {}

public:
	virtual BOOL DestroyWindow();
	afx_msg void OnProbeXon();
	afx_msg void OnProbeYon();
	afx_msg void OnProbe3on();
	afx_msg void OnCurrentX();
	afx_msg void OnCurrentY();
	afx_msg void OnStart();
	afx_msg void OnPause();
	afx_msg void OnSet();
	afx_msg void OnRegist();

	DECLARE_MESSAGE_MAP()
};
