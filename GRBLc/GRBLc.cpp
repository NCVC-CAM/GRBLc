// GRBLc.cpp : DLL の初期化ルーチンです。
//

#include "stdafx.h"
#include "NCVCaddin.h"
#include "GRBLc.h"
#include "boost/system/system_error.hpp"
#include "boost/system/error_code.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern	LPCTSTR		g_szFuncName = "GRBLc";
extern	LPCTSTR		g_szValue = "%.3f";
extern	LPCTSTR		g_szFeed = "F%d";
extern	LPCTSTR		g_szXYZ = "XYZ";
extern	LPCTSTR		g_szErrorAxis = "-.---";
extern	int			g_inputJogKey[] = {
	VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN, VK_PRIOR, VK_NEXT
};
extern	char		g_inputCtrlKey[] = {
	'O', 'R'
};

BEGIN_MESSAGE_MAP(CGRBLcApp, CWinApp)
END_MESSAGE_MAP()

// CGRBLcApp コンストラクション

CGRBLcApp::CGRBLcApp()
{
//	m_pDlg = NULL;
}

// 唯一の CGRBLcApp オブジェクトです。
CGRBLcApp theApp;
#ifdef _DEBUG
DbgConsole	theDebug;	// デバッグ用コンソール
#endif

// CGRBLcApp 初期化
/*
BOOL CGRBLcApp::InitInstance()
{
	BOOL	bResult = CWinApp::InitInstance();
	if ( bResult ) {
		m_optGRBL.Read();
	}
	return bResult;
}
*/
/////////////////////////////////////////////////////////////////////////////
// NCVC ｱﾄﾞｲﾝ関数

NCADDIN BOOL NCVC_Initialize(NCVCINITIALIZE* nci)
{
#ifdef _DEBUG
	std::cout << "--GRBLc.dll CALL NCVC_Initialize\n";
#endif

	// ｱﾄﾞｲﾝの必要情報
	nci->dwSize = sizeof(NCVCINITIALIZE);
	nci->dwType = NCVCADIN_FLG_NCDFILE;
	nci->lpszMenuName[NCVCADIN_ARY_NCDFILE] = "GRBLc...";
	nci->lpszFuncName[NCVCADIN_ARY_NCDFILE] = g_szFuncName;
	nci->lpszAddinName	= g_szFuncName;
	nci->lpszCopyright	= "MNCT-S K.Magara";
	nci->lpszSupport	= "http://s-gikan2.maizuru-ct.ac.jp/";

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// NCVC ｲﾍﾞﾝﾄ関数

NCADDIN void GRBLc(void)
{
	NCVCHANDLE hDoc = NCVC_GetDocument(NULL);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
/*
	if ( AfxGetGRBLcApp()->m_pDlg )
		AfxGetGRBLcApp()->m_pDlg->SetFocus();
	else {
		AfxGetGRBLcApp()->m_pDlg = new CGRBLdlg;	// deleteは CGRBLdlg::PostNcDestroy()
		AfxGetGRBLcApp()->m_pDlg->Create(IDD_DIALOG1);
	}
*/
	try {
		AfxGetGRBLcApp()->GetOption()->Read();
		CGRBLdlg	dlg(hDoc);
		dlg.DoModal();
	}
	catch (boost::system::system_error& e) {
		CString	strMsg("例外を検出しました。処理を中断します。\ncode=");
		strMsg += e.what();
		AfxMessageBox(strMsg);
	}
}
