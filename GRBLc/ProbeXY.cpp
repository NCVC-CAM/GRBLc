// ProbeXY.cpp : 実装ファイル
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

IMPLEMENT_DYNAMIC(CProbeXY, CProbeDlg)

BEGIN_MESSAGE_MAP(CProbeXY, CProbeDlg)
	ON_BN_CLICKED(IDC_PROBE_X_CENTER, &CProbeXY::OnProbeXon)
	ON_BN_CLICKED(IDC_PROBE_Y_CENTER, &CProbeXY::OnProbeYon)
	ON_BN_CLICKED(IDC_PROBE_C_CENTER, &CProbeXY::OnProbe3on)
	ON_BN_CLICKED(IDC_PROBE_AXIS_X1, &CProbeXY::OnCurrentX)
	ON_BN_CLICKED(IDC_PROBE_AXIS_X2, &CProbeXY::OnCurrentX)
	ON_BN_CLICKED(IDC_PROBE_AXIS_X3, &CProbeXY::OnCurrentX)
	ON_BN_CLICKED(IDC_PROBE_AXIS_Y1, &CProbeXY::OnCurrentY)
	ON_BN_CLICKED(IDC_PROBE_AXIS_Y2, &CProbeXY::OnCurrentY)
	ON_BN_CLICKED(IDC_PROBE_AXIS_Y3, &CProbeXY::OnCurrentY)
	ON_BN_CLICKED(IDC_PROBE_START,  &CProbeXY::OnStart)
	ON_BN_CLICKED(IDC_PROBE_PAUSE,  &CProbeXY::OnPause)
	ON_BN_CLICKED(IDC_PROBE_SET,    &CProbeXY::OnSet)
	ON_BN_CLICKED(IDC_PROBE_REGIST, &CProbeXY::OnRegist)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////
// CProbeXY ダイアログ

CProbeXY::CProbeXY(CGRBLdlg* pParent) : CProbeDlg(IDD_XY_PROBE, NULL)
{
	m_pParent = pParent;
	m_nProbeMode = 2;
	m_nWork = 0;
	m_nSetX = m_nSetY = 0;
}

CProbeXY::~CProbeXY()
{
}

void CProbeXY::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_PROBE_WORK, m_nWork);
	DDX_Radio(pDX, IDC_PROBE_X_CENTER, m_nProbeMode);
	DDX_Radio(pDX, IDC_PROBE_AXIS_X1, m_nSetX);
	DDX_Radio(pDX, IDC_PROBE_AXIS_Y1, m_nSetY);
	DDX_Control(pDX, IDC_PROBE_LENGTH, m_edLength);
	DDX_Control(pDX, IDC_PROBE_FEED, m_edFeed);
	DDX_Control(pDX, IDC_PROBE_DISTANCE, m_edDistance);
	for (int i=0; i<SIZEOF(m_btAxisX); i++) {
		DDX_Control(pDX, IDC_PROBE_AXIS_X1+i, m_btAxisX[i]);
		DDX_Control(pDX, IDC_PROBE_AXIS_Y1+i, m_btAxisY[i]);
	}
	for (int i=0; i<SIZEOF(m_stAxisX); i++) {
		DDX_Control(pDX, IDC_PROBE_X1+i, m_stAxisX[i]);
		DDX_Control(pDX, IDC_PROBE_Y1+i, m_stAxisY[i]);
	}
}

BOOL CProbeXY::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CProbeOption*	pOpt = AfxGetGRBLcApp()->GetProbeOption();
	m_edLength		= pOpt->m_fXYLength;
	m_edDistance	= pOpt->m_fXYDistance;
	m_edFeed		= pOpt->m_nXYFeed;
	m_nWork			= pOpt->m_nXYWork;

	OnProbe3on();
	UpdateData(FALSE);
	return TRUE;
}

BOOL CProbeXY::DestroyWindow()
{
	UpdateData();
	CProbeOption*	pOpt = AfxGetGRBLcApp()->GetProbeOption();
	pOpt->m_fXYLength	= m_edLength;
	pOpt->m_fXYDistance	= m_edDistance;
	pOpt->m_nXYFeed		= m_edFeed;
	pOpt->m_nXYWork		= m_nWork;

	return CDialogEx::DestroyWindow();
}

string CProbeXY::GetProbeModeXY(void)
{
	extern	LPCTSTR		g_szXYZ;		// "XYZ"
	int		nXY;
	string	strResult;

	if ( m_nProbeMode == CIRCLE_MODE ) {
		// Center of Circle
		nXY = m_nSetX==1 ? 1 : 0;
	}
	else {
		// Center of [X|Y]
		nXY = m_nProbeMode;
	}

	strResult = g_szXYZ[nXY];
	return strResult;
}

int CProbeXY::RectCenter(void)
{
	extern	LPCTSTR		g_szErrorAxis;	// "-.---";
	extern	LPCTSTR		g_szValue;		// "%.3f"

	double	d[2];
	CString	strAxis;

	CStatic*	pValue;
	CStatic*	pResult;
	if ( m_nProbeMode == X_MODE ) {
		pValue  =  m_stAxisX;
		pResult = &m_stAxisX[3];
	}
	else {
		pValue  =  m_stAxisY;
		pResult = &m_stAxisY[3];
	}

	for ( int i=0; i<2; i++, pValue++ ) {
		pValue->GetWindowText(strAxis);
		if ( strAxis==g_szErrorAxis || !m_pParent->IsViewMPos() ) {
			m_pParent->AddMessage(ERR_MPOS, msgInfo);
			return i;
		}
		d[i] = atof(strAxis);
	}
	d[0] = (d[1]-d[0])/2.0+d[0];
	strAxis.Format(g_szValue, d[0]);
	pResult->SetWindowText(strAxis);

	return 0;
}

int CProbeXY::CircleCenter(void)
{
	extern	LPCTSTR		g_szErrorAxis;	// "-.---";
	extern	LPCTSTR		g_szValue;		// "%.3f"

	double	x[3], y[3];
	CString	strAxis;

	for ( int i=0; i<3; i++ ) {
		m_stAxisX[i].GetWindowText(strAxis);
		if ( strAxis==g_szErrorAxis || !m_pParent->IsViewMPos() ) {
			m_pParent->AddMessage(ERR_MPOS, msgInfo);
			return i;
		}
		x[i] = atof(strAxis);
		m_stAxisY[i].GetWindowText(strAxis);
		if ( strAxis==g_szErrorAxis || !m_pParent->IsViewMPos() ) {
			m_pParent->AddMessage(ERR_MPOS, msgInfo);
			return i;
		}
		y[i] = atof(strAxis);
	}

	double	i, j, k, l, c1, c2, d;
	i = 2.0 * (x[1]-x[0]);
	j = 2.0 * (y[1]-y[0]);
	k = 2.0 * (x[2]-x[0]);
	l = 2.0 * (y[2]-y[0]);
	c1 = x[1]*x[1]-x[0]*x[0]+y[1]*y[1]-y[0]*y[0];
	c2 = x[2]*x[2]-x[0]*x[0]+y[2]*y[2]-y[0]*y[0];

	d = k*j-i*l;
	if ( fabs(d) < 0.001 ) {
		m_pParent->AddMessage("Calculation error", msgInfo);
		m_stAxisX[3].SetWindowText(g_szErrorAxis);
		m_stAxisY[3].SetWindowText(g_szErrorAxis);
		return 0;
	}
	x[0] = (j*c2-l*c1)/d;
	y[0] = (k*c1-i*c2)/d;
	strAxis.Format(g_szValue, x[0]);
	m_stAxisX[3].SetWindowText(strAxis);
	strAxis.Format(g_szValue, y[0]);
	m_stAxisY[3].SetWindowText(strAxis);

	return 0;
}

//////////////////////////////////////////////////////////////
// CProbeXY メッセージ ハンドラー

void CProbeXY::OnProbeXon()
{
	UpdateData();
	if ( m_nSetX == 2 ) {
		m_nSetX = m_nSetY = 0;
		UpdateData(FALSE);
	}
	for (size_t i=0; i<SIZEOF(m_btAxisX); i++) {
		m_btAxisX[i].ShowWindow(i!=2 ? SW_SHOW : SW_HIDE);
		m_btAxisY[i].ShowWindow(SW_HIDE);
	}
	for (size_t i=0; i<SIZEOF(m_stAxisX); i++) {
		m_stAxisX[i].ShowWindow(i!=2 ? SW_SHOW : SW_HIDE);
		m_stAxisY[i].ShowWindow(SW_HIDE);
	}
}

void CProbeXY::OnProbeYon()
{
	UpdateData();
	if ( m_nSetY == 2 ) {
		m_nSetX = m_nSetY = 0;
		UpdateData(FALSE);
	}
	for (size_t i=0; i<SIZEOF(m_btAxisX); i++) {
		m_btAxisX[i].ShowWindow(SW_HIDE);
		m_btAxisY[i].ShowWindow(i!=2 ? SW_SHOW : SW_HIDE);
	}
	for (size_t i=0; i<SIZEOF(m_stAxisX); i++) {
		m_stAxisX[i].ShowWindow(SW_HIDE);
		m_stAxisY[i].ShowWindow(i!=2 ? SW_SHOW : SW_HIDE);
	}
}

void CProbeXY::OnProbe3on()
{
	for (size_t i=0; i<SIZEOF(m_btAxisX); i++) {
		m_btAxisX[i].ShowWindow(SW_SHOW);
		m_btAxisY[i].ShowWindow(SW_SHOW);
	}
	for (size_t i=0; i<SIZEOF(m_stAxisX); i++) {
		m_stAxisX[i].ShowWindow(SW_SHOW);
		m_stAxisY[i].ShowWindow(SW_SHOW);
	}
}

void CProbeXY::OnCurrentX()
{
	UpdateData();
	m_nSetY = m_nSetX;
	UpdateData(FALSE);
}

void CProbeXY::OnCurrentY()
{
	UpdateData();
	m_nSetX = m_nSetY;
	UpdateData(FALSE);
}

void CProbeXY::OnStart()
{
	extern	LPCTSTR		g_szValue;		// "%.3f"
	extern	LPCTSTR		g_szFeed;		// "F%d"

	if ( m_pParent->GetStat() == grblHOLD ) {
		m_pParent->SendCommand('~');
		m_pParent->AddMessage("~", msgCmd);
		return;
	}

	UpdateData();
	float	fLen  = m_edLength;
	int		nFeed = m_edFeed;
	string	strValue = (format(g_szValue) % fLen ).str(),
			strFeed  = (format(g_szFeed ) % nFeed).str(),
			strSend  = "G38.2" + GetProbeModeXY() + strValue + strFeed;
	m_pParent->SendCommand(strSend);
	m_pParent->AddMessage(strSend, msgCmd);
}

void CProbeXY::OnPause()
{
	m_pParent->OnFeedHold();
}

void CProbeXY::OnSet()
{
	extern	LPCTSTR		g_szValue;		// "%.3f"
	extern	LPCTSTR		g_szErrorAxis;	// "-.---";
	CString	strAxis;

	UpdateData();

	m_pParent->m_stAxis[NCA_X].GetWindowText(strAxis);
	if ( strAxis==g_szErrorAxis || !m_pParent->IsViewMPos() ) {
		m_pParent->AddMessage(ERR_MPOS, msgInfo);
		return;
	}
	m_stAxisX[m_nSetX].SetWindowText(strAxis);

	m_pParent->m_stAxis[NCA_Y].GetWindowText(strAxis);
	if ( strAxis==g_szErrorAxis || !m_pParent->IsViewMPos() ) {
		m_pParent->AddMessage(ERR_MPOS, msgInfo);
		return;
	}
	m_stAxisY[m_nSetY].SetWindowText(strAxis);

	float	fDistance  = -copysignf(m_edDistance, m_edLength);
	string	strSend = "G91G00" + GetProbeModeXY() + (format(g_szValue) % fDistance).str();
	m_pParent->SendCommand(strSend);
	m_pParent->AddMessage(strSend, msgCmd);

	m_nSetX++;	m_nSetY++;
	if ( m_nProbeMode == CIRCLE_MODE ) {
		if ( m_nSetX > 2 ) {
			m_nSetX = m_nSetY = CircleCenter();
		}
	}
	else {
		if ( m_nSetX > 1 ) {
			m_nSetX = m_nSetY = RectCenter();
		}
	}
	UpdateData(FALSE);
}

void CProbeXY::OnRegist()
{
	extern	LPCTSTR		g_szErrorAxis;	// "-.---";

	UpdateData();
	CString	strAxis;
	float	fx, fy;
	int		nWork  = m_nWork + 1;	// G54～G59 → P1～P6

	m_stAxisX[3].GetWindowText(strAxis);
	if ( strAxis==g_szErrorAxis || !m_pParent->IsViewMPos() ) {
		m_pParent->AddMessage(ERR_MPOS, msgInfo);
		return;
	}
	fx = (float)atof(strAxis);
	m_stAxisY[3].GetWindowText(strAxis);
	if ( strAxis==g_szErrorAxis || !m_pParent->IsViewMPos() ) {
		m_pParent->AddMessage(ERR_MPOS, msgInfo);
		return;
	}
	fy = (float)atof(strAxis);

	string	strSend;
	switch ( m_nProbeMode ) {
	case X_MODE:
		strSend = (format("G10L2P%dX%.3f") % nWork % fx).str();
		break;
	case Y_MODE:
		strSend = (format("G10L2P%dY%.3f") % nWork % fy).str();
		break;
	case CIRCLE_MODE:
		strSend = (format("G10L2P%dX%.3fY%.3f") % nWork % fx % fy).str();
		break;
	}
	if ( !strSend.empty() ) {
		m_pParent->SendCommand(strSend);
		m_pParent->AddMessage(strSend, msgCmd);
	}
}
