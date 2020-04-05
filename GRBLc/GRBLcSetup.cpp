// GRBLcSetup.cpp : 実装ファイル
//

#include "stdafx.h"
#include "GRBLc.h"
#include "GRBLcOption.h"
#include "GRBLcSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CGRBLcSetup, CDialogEx)

BEGIN_MESSAGE_MAP(CGRBLcSetup, CDialogEx)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////
// CGRBLcSetup ダイアログ

CGRBLcSetup::CGRBLcSetup(CGRBLdlg* pDlg) : CDialogEx(IDD_GRBLC_SETUP, pDlg)
{
}

CGRBLcSetup::~CGRBLcSetup()
{
}

void CGRBLcSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_AUTOCONNECT, m_bAutoConnect);
	DDX_Check(pDX, IDC_WITHTRACE, m_bWithTrace);
	DDX_Control(pDX, IDC_QUERYTIME, m_edQueryTime);
	DDX_Control(pDX, IDC_COM, m_cbComPort);
	DDX_Control(pDX, IDC_BAUDRATE, m_cbBaudrate);
	DDX_Control(pDX, IDC_JOG_STEP, m_edJogStep);
	DDX_Control(pDX, IDC_JOG_FEED, m_edJogFeed);
	for (int i=0; i<NCXYZ; i++)
		DDX_Text(pDX, IDC_CMD1+i, m_strCmd[i]);
}

BOOL CGRBLcSetup::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CGRBLcOption*	pOpt = AfxGetGRBLcApp()->GetOption();
	int		i;
	CString	strBuf;
	CGRBLdlg* pDlg = dynamic_cast<CGRBLdlg*>(GetParent());
	m_bAutoConnect = pOpt->m_bAutoConnect;
	m_bWithTrace   = pOpt->m_bWithTrace;
	m_edQueryTime  = pOpt->m_nQueryTime;
	for ( i=0; i<pDlg->m_cbComPort.GetCount(); i++ ) {
		pDlg->m_cbComPort.GetLBText(i, strBuf);
		m_cbComPort.AddString(strBuf);
	}
	m_cbComPort.SetCurSel(m_cbComPort.FindString(-1, pOpt->m_strOption[grblS_Comport].c_str()));
	m_cbBaudrate.SetCurSel(pDlg->m_cbBaudrate.GetCurSel());
	m_edJogStep = pOpt->m_fJogStep;
	m_edJogFeed = pOpt->m_nJogFeed;
	for ( i=0; i<NCXYZ; i++ )
		m_strCmd[i] = pOpt->m_strOption[i].c_str();

	UpdateData(FALSE);
	return TRUE;
}

//////////////////////////////////////////////////////////////
// CGRBLcSetup メッセージ ハンドラー

void CGRBLcSetup::OnOK()
{
	UpdateData();

	CGRBLcOption*	pOpt = AfxGetGRBLcApp()->GetOption();
	pOpt->m_bAutoConnect = m_bAutoConnect;
	pOpt->m_bWithTrace	 = m_bWithTrace;
	pOpt->m_nQueryTime	 = m_edQueryTime;
	pOpt->m_nBaudrate = m_cbBaudrate.GetCurSel();
	int nSel = m_cbComPort.GetCurSel();
	if ( nSel >= 0 ) {
		CString	strBuf;
		m_cbComPort.GetLBText(nSel, strBuf);
		pOpt->m_strOption[grblS_Comport] = LPCTSTR(strBuf);
	}
	else {
		pOpt->m_strOption[grblS_Comport].empty();
	}
	pOpt->m_fJogStep = m_edJogStep;
	pOpt->m_nJogFeed = m_edJogFeed;
	for ( int i=0; i<NCXYZ; i++ )
		pOpt->m_strOption[i] = LPCTSTR(m_strCmd[i]);
	pOpt->Write();

	CDialogEx::OnOK();
}
