// ProbeParent.cpp : 実装ファイル
//

#include "stdafx.h"
#include "GRBLc.h"
#include "GRBLdlg.h"
#include "ProbeParent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CProbeDlg, CDialogEx)
IMPLEMENT_DYNAMIC(CProbeParent, CProbeDlg)

BEGIN_MESSAGE_MAP(CProbeDlg, CDialogEx)
END_MESSAGE_MAP()
BEGIN_MESSAGE_MAP(CProbeParent, CProbeDlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PROBE, &CProbeParent::OnSelchangeTabProbe)
END_MESSAGE_MAP()

extern	LPCTSTR		ERR_MPOS = "MPos cannot be obtained";

//////////////////////////////////////////////////////////////
// CProbeDlg

CProbeDlg::CProbeDlg(UINT nID, CGRBLdlg* pParent) : CDialogEx(nID, pParent)
{
	m_pParent = pParent;
}

BOOL CProbeDlg::PreTranslateMessage(MSG* pMsg)
{
	extern	int		g_inputJogKey[];

	if ( pMsg->message == WM_KEYDOWN ) {
		if ( GetKeyState(VK_CONTROL) < 0 ) {
			for ( int i=0; i<NCXYZ*2; i++ ) {
				if ( GetKeyState(g_inputJogKey[i]) < 0 ) {
					m_pParent->SendJogXYZ(i);
					return TRUE;
				}
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

//////////////////////////////////////////////////////////////
// CProbeParent ダイアログ

CProbeParent::CProbeParent(CGRBLdlg* pParent) : CProbeDlg(IDD_PROBE, pParent)
	,m_dlgZ(pParent), m_dlgXY(pParent)
{
}

CProbeParent::~CProbeParent()
{
}

void CProbeParent::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_PROBE, m_ctrlTab);
}

BOOL CProbeParent::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	AfxGetGRBLcApp()->GetProbeOption()->Read();

	m_ctrlTab.InsertItem(0, "Z-Probe");
	m_ctrlTab.InsertItem(1, "XY-Probe");
	m_dlgZ.Create(IDD_Z_PROBE, &m_ctrlTab);
	m_dlgXY.Create(IDD_XY_PROBE, &m_ctrlTab);

	CRect	rc;
	m_ctrlTab.GetClientRect(rc);
	m_ctrlTab.AdjustRect(FALSE, rc);
	rc.OffsetRect(-1,0);	// ?
	m_dlgZ.MoveWindow(rc);
	m_dlgXY.MoveWindow(rc);

	m_ctrlTab.SetCurSel(0);
	m_dlgZ.ShowWindow(SW_SHOW);
	m_dlgXY.ShowWindow(SW_HIDE);
//	m_dlgZ.SetFocus();

	return TRUE;
}

void CProbeParent::OnCancel()
{
	m_dlgZ.DestroyWindow();
	m_dlgXY.DestroyWindow();
	AfxGetGRBLcApp()->GetProbeOption()->Write();

	CDialogEx::OnCancel();
}

//////////////////////////////////////////////////////////////
// CProbeParent メッセージ ハンドラー

void CProbeParent::OnSelchangeTabProbe(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nSel = m_ctrlTab.GetCurSel();
	m_dlgZ.ShowWindow (nSel==0 ? SW_SHOW : SW_HIDE);
	m_dlgXY.ShowWindow(nSel==1 ? SW_SHOW : SW_HIDE);
	*pResult = 0;
}
