// GRBLdlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include <string>
#include <vector>
#include "GRBLc.h"
#include "GRBLdlg.h"
#include "GRBLcOption.h"
#include "GRBLcSetup.h"
#include "ProbeParent.h"
#include "boost/xpressive/xpressive.hpp"				// regex
#include "boost/lexical_cast.hpp"
#include "boost/tokenizer.hpp"
#include "boost/format.hpp"
#include "boost/function.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/classification.hpp"	// is_any_of

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace boost;
using namespace std;
typedef	tokenizer< char_separator<char> >	TOKEN;

IMPLEMENT_DYNAMIC(CGRBLdlg, CDialogEx)

BEGIN_MESSAGE_MAP(CGRBLdlg, CDialogEx)
	ON_BN_CLICKED(IDC_CONNECT, &CGRBLdlg::OnConnect)
	ON_BN_CLICKED(IDC_MSGCLR, &CGRBLdlg::OnMsgClear)
	ON_BN_CLICKED(IDC_CMD_CUSTOM1, &CGRBLdlg::OnCustomCmd)
	ON_BN_CLICKED(IDC_CMD_CUSTOM2, &CGRBLdlg::OnCustomCmd)
	ON_BN_CLICKED(IDC_CMD_CUSTOM3, &CGRBLdlg::OnCustomCmd)
	ON_BN_CLICKED(IDC_CMD_SOFTRESET, &CGRBLdlg::OnSoftReset)
	ON_BN_CLICKED(IDC_SETTING, &CGRBLdlg::OnSetting)
	ON_BN_CLICKED(IDC_X_ZERO, &CGRBLdlg::OnZeroAxis)
	ON_BN_CLICKED(IDC_Y_ZERO, &CGRBLdlg::OnZeroAxis)
	ON_BN_CLICKED(IDC_Z_ZERO, &CGRBLdlg::OnZeroAxis)
	ON_BN_CLICKED(IDC_UNLOCK, &CGRBLdlg::OnAlarmUnlock)
	ON_BN_CLICKED(IDC_PROBE, &CGRBLdlg::OnProbe)
	ON_BN_CLICKED(IDC_HOMING, &CGRBLdlg::OnHomingCycle)
	ON_BN_CLICKED(IDC_START, &CGRBLdlg::OnCycleStart)
	ON_BN_CLICKED(IDC_HOLD, &CGRBLdlg::OnFeedHold)
	ON_BN_CLICKED(IDC_JOG_LEFT, &CGRBLdlg::OnJogXYZ)
	ON_BN_CLICKED(IDC_JOG_RIGHT, &CGRBLdlg::OnJogXYZ)
	ON_BN_CLICKED(IDC_JOG_FRONT, &CGRBLdlg::OnJogXYZ)
	ON_BN_CLICKED(IDC_JOG_BACK, &CGRBLdlg::OnJogXYZ)
	ON_BN_CLICKED(IDC_JOG_UP, &CGRBLdlg::OnJogXYZ)
	ON_BN_CLICKED(IDC_JOG_DOWN, &CGRBLdlg::OnJogXYZ)
	ON_BN_CLICKED(IDC_SINGLE, &CGRBLdlg::OnCheckUpdate)
	ON_LBN_KILLFOCUS(IDC_MSGLST, &CGRBLdlg::OnKillfocusMsgLst)
	ON_NOTIFY(TTN_NEEDTEXT, 0, &CGRBLdlg::OnToolTipNeedText)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_SEND_LIST, &CGRBLdlg::OnGetDispInfo_SendList)
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_CYCLE_END, &CGRBLdlg::OnCycleEnd)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////
// CGRBLdlg ダイアログ

CGRBLdlg::CGRBLdlg(NCVCHANDLE hDoc) : CDialogEx(IDD_DIALOG1, NULL)
	,m_eventQuery(FALSE, TRUE), m_eventCycleRecv(FALSE, TRUE)	// Manual event
{
	m_hDoc = hDoc;
	m_nc.dwSize = sizeof(NCDATA);
	m_pCom = NULL;
	m_bQueryThread = m_bCycleThread = m_bCycleThreadSuspend = false;
	m_pCycleThread = NULL;
	m_statGRBL = grblALARM;
	m_bSingleLock = FALSE;
}

CGRBLdlg::~CGRBLdlg()
{
#ifdef _DEBUG
	cout << "CGRBLdlg::~CGRBLdlg()\n";
#endif
}

void CGRBLdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COM, m_cbComPort);
	DDX_Control(pDX, IDC_BAUDRATE, m_cbBaudrate);
	DDX_Control(pDX, IDC_CONNECT, m_btConnect);
	DDX_Control(pDX, IDC_COMMAND, m_cbCommand);
	DDX_Control(pDX, IDC_MSGLST, m_lbMessage);
	DDX_Control(pDX, IDC_JOG_STEP, m_edJogStep);
	DDX_Control(pDX, IDC_JOG_FEED, m_edJogFeed);
	DDX_Control(pDX, IDC_SEND_LIST, m_lcSendList);
	DDX_Control(pDX, IDC_STATUS, m_stStatus);
	DDX_Control(pDX, IDC_PROCLINE, m_stLine);
	DDX_Control(pDX, IDC_UNLOCK, m_btAlarmUnlock);
	DDX_Control(pDX, IDC_PROBE, m_btProbe);
	DDX_Control(pDX, IDC_HOMING, m_btHoming);
	DDX_Control(pDX, IDC_WPOS, m_btWPos);
	for (int i=0; i<NCXYZ; i++) {
		DDX_Control(pDX, IDC_CMD_CUSTOM1 + i, m_btCustomCmd[i]);
		DDX_Control(pDX, IDC_X_ZERO + i, m_btZeroAxis[i]);
		DDX_Control(pDX, IDC_X_AXIS + i, m_stAxis[i]);
		DDX_Control(pDX, IDC_JOG_LEFT + i * 2, m_btJog[0][i]);
		DDX_Control(pDX, IDC_JOG_LEFT + i * 2 + 1, m_btJog[1][i]);
	}
	DDX_Check(pDX, IDC_SINGLE, m_bSingleLock);
}

BOOL CGRBLdlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HKEY hKey = EnumComPort();	// MonitoringComportThread.cpp
	if ( hKey ) {
		LPMONITORINGPARAM pParam = new MONITORINGPARAM(hKey, this);
		AfxBeginThread(MonitoringComportThread, pParam);
	}

	EnableToolTips();

	m_fontAxis.CreatePointFont(160, "MS ゴシック");
	for ( int i=0; i<NCXYZ; i++ ) {
		GetDlgItem(IDC_X_ZERO+i)->SetFont(&m_fontAxis);
		GetDlgItem(IDC_X_AXIS+i)->SetFont(&m_fontAxis);
	}

	CGRBLcOption* pOpt = AfxGetGRBLcApp()->GetOption();
	int nComPort, nBaudrate = m_cbBaudrate.SetCurSel(pOpt->GetIntOpt(grblI_Baudrate));
	if ( pOpt->GetStringOpt(grblS_Comport).empty() ) {
		nComPort = m_cbComPort.GetCurSel();	// Set EnumComPort()
	}
	else {
		nComPort  = m_cbComPort.SetCurSel(m_cbComPort.FindString(-1, pOpt->GetStringOpt(grblS_Comport).c_str()));
	}
	m_btWPos.SetCheck(pOpt->GetIntOpt(grblI_WPos)==1 ? BST_CHECKED : BST_UNCHECKED);
	m_edJogStep = pOpt->GetFloatOpt(grblF_JogStep);
	m_edJogFeed = pOpt->GetIntOpt  (grblI_JogFeed);
	if ( pOpt->GetIntOpt(grblI_AutoConnect) && nComPort>=0 && nBaudrate>=0 ) {
		m_btConnect.PostMessage(BM_CLICK);
	}

	m_lcSendList.ShowScrollBar(SB_VERT);
	CHeaderCtrl* pHeader = m_lcSendList.GetHeaderCtrl();
	LONG_PTR	lStyle = GetWindowLongPtr(pHeader->m_hWnd, GWL_STYLE);
	SetWindowLongPtr(pHeader->m_hWnd, GWL_STYLE, lStyle|HDS_HIDDEN);
	m_lcSendList.InsertColumn(0, "Code");
	CRect	rc;
	m_lcSendList.GetClientRect(&rc);
	m_lcSendList.SetColumnWidth(0, rc.Width());
	DWORD	dwStyle = m_lcSendList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	m_lcSendList.SetExtendedStyle(dwStyle);
	m_lcSendList.SetItemCountEx(NCVC_GetNCBlockDataSize(m_hDoc));

	return TRUE;
}

BOOL CGRBLdlg::DestroyWindow()
{
#ifdef _DEBUG
	cout << "CGRBLdlg::DestroyWindow()\n";
#endif
	m_fontAxis.DeleteObject();

	CGRBLcOption* pOpt = AfxGetGRBLcApp()->GetOption();
	pOpt->SetWPos(m_btWPos.GetCheck()==BST_CHECKED ? 1 : 0);
	pOpt->Write_NoSetup();

	return CDialogEx::DestroyWindow();
}
/*
void CGRBLdlg::PostNcDestroy()
{
	delete	this;
	CDialogEx::PostNcDestroy();
}
*/
void CGRBLdlg::OnOK()
{
	// Enterでダイアログを終了させない
	// 他のボタンにフォーカスがあるときはEnterキー有効
//	CDialogEx::OnOK();
}

void CGRBLdlg::OnCancel()
{
#ifdef _DEBUG
	cout << "CGRBLdlg::OnCancel()\n";
#endif
	m_eventMonitoring.SetEvent();
	if ( m_pCom ) {
		if ( !Disconnect() )
			return;
	}
//	DestroyWindow();
	CDialogEx::OnCancel();
}

BOOL CGRBLdlg::PreTranslateMessage(MSG* pMsg)
{
	extern	int		g_inputJogKey[];
	extern	char	g_inputCtrlKey[];

	int		i;
	boost::function<void(CGRBLdlg*)>	funcCtrl[] = {
		&CGRBLdlg::OnConnect, &CGRBLdlg::OnAlarmUnlock
	};

	if ( pMsg->message == WM_KEYDOWN ) {
		if ( GetKeyState(VK_ESCAPE) < 0 )
			return TRUE;	// ESCでダイアログを終了させない
		if ( GetKeyState(VK_CONTROL) < 0 ) {
			if ( GetKeyState(VK_HOME) < 0 ) {
				OnHomingCycle();
				return TRUE;
			}
			if ( GetKeyState(VK_SPACE) < 0 ) {
				OnCycleStart();
				return TRUE;
			}
			for ( i=0; i<SIZEOF(funcCtrl); i++ ) {
				if ( pMsg->wParam == g_inputCtrlKey[i] ) {
					funcCtrl[i](this);
					return TRUE;
				}
			}
			for ( i=0; i<NCXYZ*2; i++ ) {
				if ( GetKeyState(g_inputJogKey[i]) < 0 ) {
					SendJogXYZ(i);
					return TRUE;
				}
			}
		}
		if ( GetKeyState(VK_SPACE) < 0 ) {
			OnFeedHold();
			return TRUE;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

//////////////////////////////////////////////////////////////
// CGRBLdlg メッセージ ハンドラー

void CGRBLdlg::OnConnect()
{
	if ( m_pCom ) {
		if ( Disconnect() ) {
			m_btConnect.SetWindowText("Connect");
			EnableControl(true);
		}
		return;
	}

	int	nComPort  = m_cbComPort.GetCurSel(),
		nBaudrate = m_cbBaudrate.GetCurSel();
	if ( nComPort<0 || nBaudrate<0 ) return;

	CString	strCom, strBaudrate;
	m_cbComPort.GetLBText(nComPort, strCom);
	m_cbBaudrate.GetLBText(nBaudrate, strBaudrate);
	m_pCom = new serial::SerialPort;
	if ( !m_pCom->open(string(LPCTSTR(strCom)), atoi(LPCTSTR(strBaudrate))) ) {
		AfxMessageBox("Open connect error...");
		return;
	}

	m_pCom->attach(this);
	m_btConnect.SetWindowText("Disconnect");
	m_strPiece.clear();

	m_bQueryThread = true;
	m_eventQuery.ResetEvent();
	m_eventCycleRecv.ResetEvent();
	AfxBeginThread(QueryThreadFunc, this);
}

void CGRBLdlg::OnMsgClear()
{
	m_lbMessage.ResetContent();
}

void CGRBLdlg::OnCustomCmd()
{
	int	nID = GetFocus()->GetDlgCtrlID() - IDC_CMD_CUSTOM1;
	if ( 0<=nID && nID<NCXYZ ) {
		CGRBLcOption* pOpt = AfxGetGRBLcApp()->GetOption();
		CString	strCmd(pOpt->GetStringOpt(nID).c_str());
		if ( !strCmd.IsEmpty() ) {
			SplitSendCommand(strCmd);
		}
	}
	m_lbMessage.SetFocus();
}

void CGRBLdlg::OnSoftReset()
{
	if ( m_bCycleThread ) {
		m_bCycleThread = false;
		if ( m_bCycleThreadSuspend ) {
			m_bCycleThreadSuspend = false;
			m_pCycleThread->ResumeThread();
		}
		m_eventCycleRecv.SetEvent();
		m_eventCycle.Lock(EVENT_TIMEOUT);
	}
	SendCommand(char(0x18));
	AddMessage("0x18", msgCmd);
	m_lbMessage.SetFocus();
}

void CGRBLdlg::OnSetting()
{
	CGRBLcSetup		dlg(this);
	dlg.DoModal();
}

void CGRBLdlg::OnZeroAxis()
{
	extern	LPCTSTR		g_szXYZ;	// "XYZ"

	int	nID = GetFocus()->GetDlgCtrlID() - IDC_X_ZERO;
	if ( 0<=nID && nID<NCXYZ ) {
		string	strSend("G92");
		strSend += g_szXYZ[nID];
		strSend += '0';
		SendCommand(strSend);
		AddMessage(strSend, msgCmd);
	}
	m_lbMessage.SetFocus();
}

void CGRBLdlg::OnAlarmUnlock()
{
	static	string	strSend = "$X";
	SendCommand(strSend);
	AddMessage(strSend, msgCmd);
	m_lbMessage.SetFocus();
}

void CGRBLdlg::OnProbe()
{
	bool bUndo = m_btWPos.GetCheck()==BST_CHECKED ? true : false;
	m_btWPos.SetCheck(BST_UNCHECKED);
	CProbeParent	dlg(this);
	dlg.DoModal();
	if ( bUndo )
		m_btWPos.SetCheck(BST_CHECKED);
}

void CGRBLdlg::OnHomingCycle()
{
	static	string	strSend = "$H";
	SendCommand(strSend);
	AddMessage(strSend, msgCmd);
	m_lbMessage.SetFocus();
}

void CGRBLdlg::OnCycleStart()
{
	bool	bResult;
	switch ( m_statGRBL ) {
	case grblALARM:
		if ( m_pCom ) {
			AddMessage("Alarming. Please unlock.", msgInfo);
		}
		break;
	case grblHOLD:
	case grblDOOR:
		bResult = SendCommand('~');
		AddMessage("~", msgCmd);
		if ( bResult ) {
			if ( m_pCycleThread && m_bCycleThreadSuspend ) {
				m_bCycleThreadSuspend = false;
				m_pCycleThread->ResumeThread();
			}
		}
		break;
	case grblIDLE:
		if ( m_bCycleThread ) {
			DWORD	dwResult = WaitForSingleObject(m_eventSingle.m_hObject, 0);
			if ( dwResult == WAIT_TIMEOUT ) {
				m_eventSingle.SetEvent();
			}
		}
		else {
			EnableControl(false);
			m_bCycleThread = true;
			m_pCycleThread = AfxBeginThread(CycleStartThreadFunc, this);
			Sleep(AfxGetGRBLcApp()->GetOption()->GetIntOpt(grblI_QueryTime));
			m_eventCycleRecv.SetEvent();	// first time
		}
		break;
	}
	m_lcSendList.SetFocus();
}

void CGRBLdlg::OnFeedHold()
{
	if ( m_bCycleThread && m_statGRBL==grblHOLD ) {
		m_bCycleThread = false;
		if ( m_bCycleThreadSuspend ) {
			m_bCycleThreadSuspend = false;
			m_pCycleThread->ResumeThread();
		}
		DWORD	dwResult = WaitForSingleObject(m_eventSingle.m_hObject, 0);
		if ( dwResult == WAIT_TIMEOUT ) {
			m_eventSingle.SetEvent();
		}
		m_eventCycleRecv.SetEvent();
		SendCommand('~');
		AddMessage("CycleMode cancel", msgInfo);
		EnableControl(true);
	}
	else if ( m_statGRBL==grblJOG ) {
		SendCommand(char(0x85));
		AddMessage("0x85", msgCmd);
	}
	else {
		SendCommand('!');
		AddMessage("!", msgCmd);
	}
	m_lcSendList.SetFocus();
}

void CGRBLdlg::OnCheckUpdate()
{
	UpdateData();
}

void CGRBLdlg::OnJogXYZ()
{
	int	nID = GetFocus()->GetDlgCtrlID() - IDC_JOG_LEFT;
	if ( !(0<=nID && nID<NCXYZ*2) )
		return;
	SendJogXYZ(nID);
	m_lcSendList.SetFocus();
}

void CGRBLdlg::OnKillfocusMsgLst()
{
	m_lbMessage.SetSel(-1, FALSE);	// 選択解除
}

void CGRBLdlg::OnToolTipNeedText(NMHDR* pNMHDR, LRESULT* pResult)
{
	TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;

	if ( pTTT->uFlags & TTF_IDISHWND ) {
		int nID = ::GetDlgCtrlID((HWND)pTTT->hdr.idFrom);
		if ( IDC_CMD_CUSTOM1<=nID && nID<=IDC_CMD_CUSTOM3 ) {
			CGRBLcOption* pOpt = AfxGetGRBLcApp()->GetOption();
			string	strCmd = pOpt->GetStringOpt(nID-IDC_CMD_CUSTOM1);
			if ( !strCmd.empty() ) {
				lstrcpy(pTTT->szText, strCmd.c_str());
			}
		}
	}
}

void CGRBLdlg::OnGetDispInfo_SendList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	char	szBuf[1024];
	int		nLen;

	if ( pInfo->item.mask & LVIF_TEXT ) {
		if ( pInfo->item.iSubItem == 0 ) {
			nLen = NCVC_GetNCBlockData(m_hDoc, pInfo->item.iItem, szBuf, sizeof(szBuf));
			if ( nLen >= 0 ) {
				lstrcpy(pInfo->item.pszText, szBuf);
			}
		}
	}

	*pResult = 0;
}

HBRUSH CGRBLdlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if ( nCtlColor == CTLCOLOR_STATIC ) {
		int nID = pWnd->GetDlgCtrlID();
		if ( IsViewMPos() && IDC_X_AXIS<=nID && nID<=IDC_Z_AXIS ) {
			pDC->SetTextColor(RGB(255,0,0));
		}
	}

	return hbr;
}

LRESULT CGRBLdlg::OnCycleEnd(WPARAM, LPARAM)
{
	m_bCycleThread = false;
	m_pCycleThread = NULL;
	EnableControl(true);
	m_stLine.SetWindowText("");
	return 0;
}

//////////////////////////////////////////////////////////////

bool CGRBLdlg::Disconnect(bool bForce)
{
	if ( m_bCycleThread ) {
		int nRet = bForce ? IDYES : AfxMessageBox("サイクル実行中ですが切断しますか？", MB_YESNO);
		if ( nRet != IDYES )
			return false;
		m_bCycleThread = false;
		if ( m_bCycleThreadSuspend ) {
			m_bCycleThreadSuspend = false;
			m_pCycleThread->ResumeThread();
		}
		m_eventCycleRecv.SetEvent();
		m_eventCycle.Lock(EVENT_TIMEOUT);
	}
	m_bQueryThread = false;
	m_eventQuery.Lock();
#ifdef _DEBUG
	cout << "CGRBLdlg::Disconnect() m_eventQuery.Lock() Pass\n";
#endif

	m_pCom->detach(this);
	m_pCom->close();
	delete m_pCom;
	m_pCom = NULL;
#ifdef _DEBUG
	cout << "CGRBLdlg::Disconnect() m_pCom delete ok\n";
#endif

	GetQueryStatus(string());	// Axis "-.---"

	return true;
}

void CGRBLdlg::EnableControl(bool bEnable)
{
	m_cbCommand.EnableWindow(bEnable);
	for ( int i=0; i<NCXYZ; i++ ) {
		m_btCustomCmd[i].EnableWindow(bEnable);
		m_btZeroAxis[i].EnableWindow(bEnable);
		m_btJog[0][i].EnableWindow(bEnable);
		m_btJog[1][i].EnableWindow(bEnable);
	}
	m_btAlarmUnlock.EnableWindow(bEnable);
	m_btProbe.EnableWindow(bEnable);
	m_btHoming.EnableWindow(bEnable);
}

void CGRBLdlg::ResponseGRBL(const std::string& strRecv)
{
	static	string	strError = "error";
	if ( CmpString(strRecv, strError) ) {
		OnFeedHold();
		AddMessage(strRecv+" \""+m_strSend+"\"");
		AddMessage("Resume->[Cycle Start] Stop->[Feed Hold]", msgInfo);
		m_statGRBL = grblHOLD;	// Forced
	}
	m_eventCycleRecv.SetEvent();
}

void CGRBLdlg::AddMessage(LPCTSTR lpszMsg, MSGadd e)
{
	AddMessage(string(lpszMsg), e);
}

void CGRBLdlg::AddMessage(const std::string& s, MSGadd e)
{
	static	LPCTSTR	arAdd[] = {"", ">>", "--"};
	string	strMsg = arAdd[e] + s;

	if ( m_lbMessage.AddString(strMsg.c_str()) < 0 ) {
		m_lbMessage.ResetContent();
		m_lbMessage.AddString(strMsg.c_str());
	}
	m_lbMessage.SetTopIndex( m_lbMessage.GetCount()-1 );
}

void CGRBLdlg::GetQueryStatus(const string& strRecv)
{
	extern	LPCTSTR	g_szErrorAxis;	// "-.---"
	extern	LPCTSTR	g_szValue;		// "%.3f"
	static	LPCTSTR	szNotConnected = "not connected";
	static	string	arsPos[] = {
		"WPos:", "MPos:", "WCO:"
	};
	static	string	arsStat[] = {
		"Idle", "Run", "Hold", "Jog", "Alarm", "Door", "Check", "Home", "Sleep"
	};
	static	char_separator<char>	sep("|");

	size_t	i, j, nPosType = 99;
	string	strTok;

	if ( strRecv.empty() ) {
		for ( j=0; j<NCXYZ; j++ ) {
			m_dWCO[j].reset();
			m_stAxis[j].SetWindowText(g_szErrorAxis);
		}
		m_stStatus.SetWindowText(szNotConnected);
		return;
	}
	strTok = strRecv.substr(1, strRecv.length()-2);	// Remove '<'...'>'
	m_stStatus.SetWindowText(strTok.c_str());

	vector<string>		vAxis;
	optional<double>	dAxis[NCXYZ];
	TOKEN	tokens(strRecv, sep);
	TOKEN::iterator		it = tokens.begin();

	// Get status
	if ( it != tokens.end() ) {
		strTok = (*it).substr(1);	// Remove '<'
		for ( j=0; j<SIZEOF(arsStat); j++ ) {
			if ( CmpString(strTok, arsStat[j]) ) {
				m_statGRBL = (GRBLstat)j;
				break;
			}
		}
		if ( j >= SIZEOF(arsStat) )
			m_statGRBL = grblALARM;
		if ( m_statGRBL==grblIDLE && m_pCycleThread && m_bCycleThreadSuspend ) {
			// Assuming a return from grblDOOR
			DWORD	dwExitCode;
			GetExitCodeThread(m_pCycleThread->m_hThread, &dwExitCode);
			if ( dwExitCode == STILL_ACTIVE ) {
				m_bCycleThreadSuspend = false;
				m_pCycleThread->ResumeThread();
			}
		}
		++it;
	}

	// Get Axis
	for ( ; it!=tokens.end(); ++it ) {
		strTok = *it;
		for ( i=0; i<SIZEOF(arsPos); i++ ) {
			if ( CmpString(strTok, arsPos[i]) ) {
				algorithm::split(vAxis, strTok.substr(arsPos[i].length()), is_any_of(","));
				if ( i == 2 ) {
					// 2:WCO
					for ( j=0; j<NCXYZ && j<vAxis.size(); j++ ) {
						strTok = algorithm::trim_copy(vAxis[j]);
						if ( !strTok.empty() )
							m_dWCO[j] = atof(strTok.c_str());
					}
				}
				else {
					// 0:WPos 1:MPos
					nPosType = i;
					for ( j=0; j<NCXYZ && j<vAxis.size(); j++ ) {
						strTok = algorithm::trim_copy(vAxis[j]);
						if ( !strTok.empty() )
							dAxis[j] = atof(strTok.c_str());
					}
				}
			}
		}
	}

	// View Axis
	if ( IsViewMPos() ) {
		if ( nPosType == 0 ) {
			for ( j=0; j<NCXYZ; j++ ) {
				if ( m_dWCO[j] && dAxis[j] )
					dAxis[j] = dAxis[j].get() + m_dWCO[j].get();
				else
					dAxis[j].reset();
			}
		}
		else if ( nPosType != 1 ) {
			for ( j=0; j<NCXYZ; j++ )
				dAxis[j].reset();
		}
	}
	else {
		if ( nPosType == 1 ) {
			for ( j=0; j<NCXYZ; j++ ) {
				if ( m_dWCO[j] && dAxis[j] )
					dAxis[j] = dAxis[j].get() - m_dWCO[j].get();
				else
					dAxis[j].reset();
			}
		}
		else if ( nPosType != 0 ) {
			for ( j=0; j<NCXYZ; j++ )
				dAxis[j].reset();
		}
	}
	CString	strAxis;
	for ( j=0; j<NCXYZ; j++ ) {
		if ( dAxis[j] ) {
			strAxis.Format(g_szValue, dAxis[j].get());
		}
		else {
			strAxis = g_szErrorAxis;
		}
		m_stAxis[j].SetWindowText(strAxis);
	}
}

void CGRBLdlg::SendJogXYZ(int nID)
{
	extern	LPCTSTR		g_szXYZ;	// "XYZ"
	extern	LPCTSTR		g_szValue;	// "%.3f"
	extern	LPCTSTR		g_szFeed;	// "F%d";

	float	fStep = m_edJogStep;
	int		nFeed = m_edJogFeed;
	if ( nID==0 || nID==3 || nID==5 )	// X:Left Y:Back Z:Down
		fStep = -fStep;

	string	strValue = (format(g_szValue) % fStep).str(),
			strFeed  = (format(g_szFeed ) % nFeed).str();
	SendCommand(string("$J=G91") + g_szXYZ[nID/2] + strValue + strFeed);
}

bool CGRBLdlg::SplitSendCommand(const CString& strText)
{
	bool	bResult = true;
	vector<string>	vCmd;
	string strCmd;
	algorithm::split(vCmd, LPCTSTR(strText), is_any_of(";"));
	for ( auto it=vCmd.begin(); it!=vCmd.end() && bResult; ++it ) {
		strCmd = algorithm::trim_copy(*it);
		if ( !strCmd.empty() ) {
			bResult = SendCommand(strCmd);
			AddMessage(strCmd, msgCmd);
		}
	}
	return bResult;
}

bool CGRBLdlg::SendCommand(void)
{
	CString	strText;
	m_cbCommand.GetWindowText(strText);
	if ( strText.IsEmpty() )
		return true;
	m_cbCommand.AddString(strText);
	return SplitSendCommand(strText);
}

bool CGRBLdlg::SendCommand(const CString& strCommand)
{
	return SendCommand( string(LPCTSTR(strCommand)) );
}

bool CGRBLdlg::SendCommand(const string& strCommand)
{
	bool	bResult = false;
	if ( m_pCom && m_pCom->isConnect() ) {
		m_exclusionSend.Lock();
		bResult = m_pCom->send(strCommand+'\n');
		m_exclusionSend.Unlock();
		if ( !bResult )
			Disconnect(true);
	}
	return bResult;
}

bool CGRBLdlg::SendCommand(char c)
{
	bool	bResult = false;
	if ( m_pCom && m_pCom->isConnect() ) {
		m_exclusionSend.Lock();
		bResult = m_pCom->send(c);
		m_exclusionSend.Unlock();
		if ( !bResult )
			Disconnect(true);
	}
	return bResult;
}

void CGRBLdlg::notify(const string& strRecv)
{
	static	char_separator<char>	sep("\n");
	TOKEN	tokens(strRecv, sep);
	string	strTok;

	for ( TOKEN::iterator it=tokens.begin(); it!=tokens.end(); ++it ) {
		strTok = *it;
		if ( strTok.back() == '\r' ) {
			strTok = m_strPiece + strTok.substr(0, strTok.length()-1);
			if ( !strTok.empty() && strTok.front()=='<' ) {
				GetQueryStatus(strTok);
			}
			else if ( m_bCycleThread && !strTok.empty() && strTok.front()!='[' ) {
				ResponseGRBL(strTok);
			}
			else {
				AddMessage(strTok);
			}
			m_strPiece.clear();
		}
		else {
			m_strPiece += strTok;
		}
	}
}

//////////////////////////////////////////////////////////////

UINT CGRBLdlg::QueryThreadFunc(LPVOID pParam)
{
#ifdef _DEBUG
	cout << "CGRBLdlg::QueryThreadFunc() start\n";
#endif
	CGRBLdlg* pDlg = reinterpret_cast<CGRBLdlg*>(pParam);
	CGRBLcOption* pOpt = AfxGetGRBLcApp()->GetOption();
	Sleep(pOpt->GetIntOpt(grblI_QueryTime));

	while ( pDlg->m_bQueryThread && pDlg->m_pCom && pDlg->m_pCom->isConnect() ) {
		pDlg->m_exclusionSend.Lock();
		pDlg->m_pCom->send('?');
		pDlg->m_exclusionSend.Unlock();
		Sleep(pOpt->GetIntOpt(grblI_QueryTime));
	}

	pDlg->m_eventQuery.SetEvent();
#ifdef _DEBUG
	cout << "CGRBLdlg::QueryThreadFunc() end\n";
#endif
	return 0;
}

UINT CGRBLdlg::CycleStartThreadFunc(LPVOID pParam)
{
#ifdef _DEBUG
	cout << "CGRBLdlg::CycleStartThreadFunc() start\n";
#endif
	CTime		ts = CTime::GetCurrentTime(), te;
	CTimeSpan	tSpan;
	CGRBLdlg* pDlg = reinterpret_cast<CGRBLdlg*>(pParam);
	CGRBLcOption* pOpt = AfxGetGRBLcApp()->GetOption();
	int		nEndCode = 0,
			nMaxLoop = NCVC_GetNCBlockDataSize(pDlg->m_hDoc);
	BOOL	bTrace = true;

	pDlg->AddMessage("Cyelc Start", msgInfo);
	if ( pOpt->GetIntOpt(grblI_WithTrace) ) {
		NCVC_TracePause(pDlg->m_hDoc);
		NCVC_TraceStart(pDlg->m_hDoc);
		int nowTraceObj = NCVC_TraceNextDraw(pDlg->m_hDoc) - 1;
		if ( nowTraceObj >= 0 )
			bTrace = NCVC_GetNCData(pDlg->m_hDoc, nowTraceObj, &pDlg->m_nc);
	}
	else
		bTrace = false;

	// Main Loop
	for ( int i=0; i<nMaxLoop && pDlg->m_bCycleThread && nEndCode!=30; i++ ) {
		nEndCode = pDlg->MainSendBlock(i, nMaxLoop, bTrace);
	}

	pDlg->PostMessage(WM_CYCLE_END);
	if ( pDlg->m_bCycleThread ) {
		te = CTime::GetCurrentTime();
		tSpan = te - ts;
		CString	strTime( tSpan.Format("Total machining time %H:%M:%S") );
		pDlg->AddMessage(strTime, msgInfo);
	}
	if ( pOpt->GetIntOpt(grblI_WithTrace) )
		NCVC_TraceStop(pDlg->m_hDoc);

	pDlg->m_eventCycle.SetEvent();
#ifdef _DEBUG
	cout << "CGRBLdlg::CycleStartThreadFunc() end\n";
#endif
	return 0;
}

int CGRBLdlg::MainSendBlock(int nIndex, int nMaxLoop, BOOL& bTrace)
{
	using namespace boost::xpressive;
	static	sregex	reIgnore  = ( '(' >> *(~as_xpr(')')) >> ')' ) | ( (as_xpr('O')|'N') >> +_d );
	static	sregex	reOBS = (s1= bos >> '/' >> *_d) >> (s2= *_);
	static	sregex	reMcode = 'M' >> (s1= +_d) >> !(s2= 'P' >> +_d) >> !(s3= 'L' >> +_d);

	int		nResult = 0, nowTraceObj, nM99line;
	char	szBlock[MAXREGBUF];
	string	strBlock;
	smatch	what;
	CString	strLine;
	CGRBLcOption* pOpt = AfxGetGRBLcApp()->GetOption();

	// Get NC block
	NCVC_GetNCBlockData(m_hDoc, nIndex, szBlock, sizeof(szBlock));
	strBlock = szBlock;

	// Ignore code ( ex. comment )
	strBlock = regex_replace(strBlock, reIgnore, "");	// Remove '(...)' or O_ or N_
	algorithm::trim(strBlock);
	if ( strBlock.empty() || strBlock.front()=='%' )
		return 0;

	// optional block skip
	if ( regex_match(strBlock, what, reOBS) ) {
		size_t	nOBS = what[1].length()==1 ? 0 : atoi(what[1].str().substr(1).c_str());
		if ( pOpt->IsOBScheck(nOBS) ) {
			AddMessage("Optional Block Skip \""+strBlock+"\"", msgInfo);
			return 0;
		}
		strBlock = what[2].str();
	}

	// Select send block
	if ( m_bCycleThread ) {
		m_lcSendList.SetItemState(nIndex, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_lcSendList.EnsureVisible(nIndex, false);
		strLine.Format("Line=%d/%d %s", nIndex+1, nMaxLoop, strBlock.c_str());
		m_stLine.SetWindowText(strLine);
	}

	// Check Grbl
	do {
		Sleep(pOpt->GetIntOpt(grblI_QueryTime));
	} while ( m_bCycleThread && m_statGRBL==grblRUN );
	m_eventCycleRecv.Lock(EVENT_TIMEOUT);		// wait response
	m_eventCycleRecv.ResetEvent();
	switch ( m_statGRBL ) {
	case grblALARM:
		m_bCycleThread = false;	// thread end (not send)
		break;
	case grblHOLD:
	case grblDOOR:
		m_bCycleThreadSuspend = true;
		m_pCycleThread->SuspendThread();
		break;
	}
	if ( !m_bCycleThread )
		return nResult;

	// NCVC trace
	while ( bTrace && m_bCycleThread && m_nc.nLine<nIndex ) {
		NCVC_TraceStart(m_hDoc);
		nowTraceObj = NCVC_TraceNextDraw(m_hDoc) - 1;
		if ( nowTraceObj >= 0 )
			bTrace = NCVC_GetNCData(m_hDoc, nowTraceObj, &m_nc);
		else
			m_nc.nLine = nMaxLoop;	// end loop
	}

	// Check Mcode
	if ( regex_match(strBlock, what, reMcode) ) {
		switch ( atoi(what[1].str().c_str()) ) {
		case 2:
		case 30:
			nResult = 30;	// end code
			break;
		case 98:
			if ( what[2].length() > 1 ) {	// P_
				int nProgNo = atoi(what[2].str().substr(1).c_str());
				int nNewIndex = SearchSubprogram(nIndex, nMaxLoop, nProgNo);
				if ( nNewIndex >= 0 ) {
					strBlock = what[3].str();
					int nRepeat = strBlock.length() > 1 ? atoi(strBlock.substr(1).c_str()) : 1;
					while ( nRepeat-- && bTrace && m_bCycleThread ) {
						for ( int i=nNewIndex; i<nMaxLoop && bTrace && m_bCycleThread; i++ ) {
							nResult = MainSendBlock(i, nMaxLoop, bTrace);
							if ( nResult==30 || nResult==99 )
								break;
						}
					}
					return nResult;	// Not send 'M98'
				}
			}
			break;
		case 99:
			// Next 'M99' object
			nM99line = m_nc.nLine;
			while ( bTrace && m_bCycleThread && m_nc.nLine==nM99line ) {
				NCVC_TraceStart(m_hDoc);
				nowTraceObj = NCVC_TraceNextDraw(m_hDoc) - 1;
				if ( nowTraceObj >= 0 )
					bTrace = NCVC_GetNCData(m_hDoc, nowTraceObj, &m_nc);
			}
			return 99;
		}
	}

	// Send command
	if ( m_bCycleThread ) {
		m_strSend = strBlock;
		if ( SendCommand(strBlock) ) {
			if ( m_bSingleLock ) {
				m_eventSingle.Lock();
			}
		}
		else {
			m_bCycleThread = false;
		}
	}

	return nResult;
}

int CGRBLdlg::SearchSubprogram(int nIndex, int nMaxLoop, int nProgNo)
{
	using namespace boost::xpressive;
	static	sregex	reComment = '(' >> *(~as_xpr(')')) >> ')';
			sregex	reProg = sregex::compile("O0*"+lexical_cast<string>(nProgNo)+"(\\D|$)");

	char	szBlock[MAXREGBUF];
	string	strBlock;

	for ( int i=nIndex+1; i<nMaxLoop && m_bCycleThread; i++ ) {
		NCVC_GetNCBlockData(m_hDoc, i, szBlock, sizeof(szBlock));
		strBlock = szBlock;
		strBlock = regex_replace(strBlock, reComment, "");
		if ( regex_search(strBlock, reProg) )
			return i;
	}

	return -1;
}
