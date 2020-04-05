// ProbeZ.cpp : 実装ファイル
//

#include "stdafx.h"
#include <string>
#include "GRBLc.h"
#include "GRBLdlg.h"
#include "ProbeOption.h"
#include "ProbeParent.h"
#include "boost/format.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace boost;

extern	LPCTSTR		ERR_MPOS;	// ProbeParent.cpp

IMPLEMENT_DYNAMIC(CProbeZ, CProbeDlg)

BEGIN_MESSAGE_MAP(CProbeZ, CProbeDlg)
	ON_BN_CLICKED(IDC_PROBE_START,  &CProbeZ::OnStart)
	ON_BN_CLICKED(IDC_PROBE_PAUSE,  &CProbeZ::OnPause)
	ON_BN_CLICKED(IDC_PROBE_REGIST, &CProbeZ::OnRegist)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////
// CProbeZ ダイアログ

CProbeZ::CProbeZ(CGRBLdlg* pParent) : CProbeDlg(IDD_Z_PROBE, NULL)
{
	m_pParent = pParent;
	m_nSetMethod = 0;
	m_nWork = 0;
}

CProbeZ::~CProbeZ()
{
}

void CProbeZ::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROBE_LENGTH, m_edLength);
	DDX_Control(pDX, IDC_PROBE_FEED, m_edFeed);
	DDX_Control(pDX, IDC_PROBE_THICKNESS, m_edThickness);
	DDX_Control(pDX, IDC_PROBE_DISTANCE, m_edDistance);
	DDX_Radio(pDX, IDC_ZPROBE_SETG92, m_nSetMethod);
	DDX_CBIndex(pDX, IDC_PROBE_WORK, m_nWork);
}

BOOL CProbeZ::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CProbeOption*	pOpt = AfxGetGRBLcApp()->GetProbeOption();
	m_edLength		= pOpt->m_fZpLength;
	m_edThickness	= pOpt->m_fZpThickness;
	m_edDistance	= pOpt->m_fZpDistance;
	m_edFeed		= pOpt->m_nZpFeed;
	m_nSetMethod	= pOpt->m_nZpSetMethod;
	m_nWork			= pOpt->m_nZpWork;

	UpdateData(FALSE);
	return TRUE;
}

BOOL CProbeZ::DestroyWindow()
{
	UpdateData();
	CProbeOption*	pOpt = AfxGetGRBLcApp()->GetProbeOption();
	pOpt->m_fZpLength	= m_edLength;
	pOpt->m_fZpThickness= m_edThickness;
	pOpt->m_fZpDistance	= m_edDistance;
	pOpt->m_nZpFeed		= m_edFeed;
	pOpt->m_nZpSetMethod= m_nSetMethod;
	pOpt->m_nZpWork		= m_nWork;

	return CDialogEx::DestroyWindow();
}

//////////////////////////////////////////////////////////////
// CProbeZ メッセージ ハンドラー

void CProbeZ::OnStart()
{
	extern	LPCTSTR		g_szValue;		// "%.3f"
	extern	LPCTSTR		g_szFeed;		// "F%d";

	if ( m_pParent->GetStat() == grblHOLD ) {
		m_pParent->SendCommand('~');
		m_pParent->AddMessage("~", msgCmd);
		return;
	}

	float	fLen  = m_edLength;
	int		nFeed = m_edFeed;
	string	strValue = (format(g_szValue) % fLen ).str(),
			strFeed  = (format(g_szFeed ) % nFeed).str(),
			strSend  = "G38.2Z" + strValue + strFeed;
	m_pParent->SendCommand(strSend);
	m_pParent->AddMessage(strSend, msgCmd);
}

void CProbeZ::OnPause()
{
	m_pParent->OnFeedHold();
}

void CProbeZ::OnRegist()
{
	extern	LPCTSTR		g_szValue;		// "%.3f"
	extern	LPCTSTR		g_szErrorAxis;	// "-.---";

	UpdateData();
	float	fThickness = m_edThickness,
			fDistance  = -copysignf(m_edDistance, m_edLength);
	string	strSend;
	if ( m_nSetMethod == 0 ) {
		// set G92
		strSend = "G92Z" + (format(g_szValue) % fThickness).str();
	}
	else {
		// set WorkOffset
		CString	strAxis;
		m_pParent->m_stAxis[NCA_Z].GetWindowText(strAxis);
		if ( strAxis==g_szErrorAxis || !m_pParent->IsViewMPos() ) {
			m_pParent->AddMessage(ERR_MPOS, msgInfo);
			return;
		}
		float	fz = (float)atof(strAxis) - fThickness;
		int		nWork  = m_nWork + 1;	// G54～G59 → P1～P6
		strSend = (format("G10L2P%dZ%.3f") % nWork % fz).str();
	}
	m_pParent->SendCommand(strSend);
	m_pParent->AddMessage(strSend, msgCmd);

	// これがないと error:20 おそらく EEPROM問題
	Sleep(AfxGetGRBLcApp()->GetOption()->GetIntOpt(grblI_QueryTime));

	strSend = "G91G00Z" + (format(g_szValue) % fDistance).str();
	m_pParent->SendCommand(strSend);
	m_pParent->AddMessage(strSend, msgCmd);
}
