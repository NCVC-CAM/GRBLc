#pragma once

#include "resource.h"
#include "NCVCaddin.h"
#include "CustomControl.h"
#include "SerialPort.h"
#include "boost/optional.hpp"

#define	DEF_BAUDRATE	9	// B115200

enum GRBLstat
{
	grblIDLE=0, grblRUN, grblHOLD, grblJOG, grblALARM,
	grblDOOR, grblCHECK, grblHOME, grblSLEEP
};
enum MSGadd
{
	msgAsIt, msgCmd, msgInfo
};

/////////////////////////////////////////////////////////////////////
// CGRBLdlg ダイアログ

class CGRBLdlg : public CDialogEx, public serial::SerialObserver
{
	NCVCHANDLE			m_hDoc;
	NCDATA				m_nc;
	serial::SerialPort*	m_pCom;
	std::string			m_strPiece,
						m_strSend;

	CWinThread*	m_pQueryThread;
	CWinThread*	m_pCycleThread;
	CEvent		m_eventMonitoring,
				m_eventSingle;
	CCriticalSection	m_exclusionSend;
	bool		m_bQueryThread,
				m_bCycleThread,
				m_bCycleThreadSuspend;
	boost::optional<double>	m_dWCO[NCXYZ];
	GRBLstat	m_statGRBL;
	CFont		m_fontAxis;

	bool	CmpString(const std::string&, const std::string&);
	bool	Disconnect(bool = false);
	void	EnableControl(bool);
	void	ResponseGRBL(const std::string&);

	DECLARE_DYNAMIC(CGRBLdlg)

public:
	CGRBLdlg(NCVCHANDLE);
	virtual ~CGRBLdlg();

	GRBLstat	GetStat(void) { return m_statGRBL; }
	bool		IsViewMPos(void) { return m_btWPos.GetCheck()==BST_UNCHECKED; }
	void	GetQueryStatus(const std::string&);
	void	SendJogXYZ(int);
	bool	SplitSendCommand(const CString&);
	bool	SendCommand(void);
	bool	SendCommand(const CString&);
	bool	SendCommand(const std::string&);
	bool	SendCommand(char);
	void	AddMessage(LPCTSTR, MSGadd);
	void	AddMessage(const std::string&, MSGadd = msgAsIt);

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif
	CComboBox	m_cbComPort;
	CComboBox	m_cbBaudrate;
	CMyComboBox	m_cbCommand;
	CMyListBox	m_lbMessage;
	CButton		m_btConnect;
	CButton		m_btCustomCmd[NCXYZ];
	CButton		m_btZeroAxis[NCXYZ];
	CButton		m_btJog[2][NCXYZ];
	CButton		m_btAlarmUnlock;
	CButton		m_btProbe;
	CButton		m_btHoming;
	CButton		m_btWPos;
	CStatic		m_stAxis[NCXYZ];
	CStatic		m_stStatus;
	CStatic		m_stLine;
	CFloatEdit	m_edJogStep;
	CIntEdit	m_edJogFeed;
	CListCtrl	m_lcSendList;
	BOOL		m_bSingleLock;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
//	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();

	virtual void notify( const std::string& str );	// class SerialObserver

public:
	afx_msg void OnConnect();
	afx_msg void OnMsgClear();
	afx_msg void OnKillfocusMsgLst();
	afx_msg void OnCustomCmd();
	afx_msg void OnSoftReset();
	afx_msg void OnZeroAxis();
	afx_msg void OnJogXYZ();
	afx_msg void OnAlarmUnlock();
	afx_msg void OnProbe();
	afx_msg void OnHomingCycle();
	afx_msg void OnCycleStart();
	afx_msg void OnFeedHold();
	afx_msg void OnSetting();
	afx_msg void OnCheckUpdate();
	afx_msg void OnToolTipNeedText(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetDispInfo_SendList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnCycleEnd(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()

	HKEY	EnumComPort(void);
	static	UINT	MonitoringComportThread(LPVOID);
	static	UINT	QueryThreadFunc(LPVOID);
	static	UINT	CycleStartThreadFunc(LPVOID);
	int		MainSendBlock(int, int, BOOL&);
	int		SearchSubprogram(int, int, int);
};

//

inline bool CGRBLdlg::CmpString(const std::string& s1, const std::string& s2)
{
	return s1.length()>=s2.length() && s1.substr(0,s2.length())==s2 ? true : false;
}

struct MONITORINGPARAM
{
	HKEY		hKey;
	CGRBLdlg*	pDlg;
	MONITORINGPARAM(HKEY h, CGRBLdlg* p) {
		hKey = h;
		pDlg = p;
	}
};
typedef	MONITORINGPARAM*	LPMONITORINGPARAM;
