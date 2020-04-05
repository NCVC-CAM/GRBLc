// GRBLc.h : GRBLc.DLL のメイン ヘッダー ファイル
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル
#include "GRBLdlg.h"
#include "GRBLcOption.h"
#include "ProbeOption.h"

// CGRBLcApp
// このクラスの実装に関しては GRBLc.cpp を参照してください。
//

class CGRBLcApp : public CWinApp
{
	CGRBLcOption	m_optGRBL;
	CProbeOption	m_optProbe;

public:
	CGRBLcApp();

//	CGRBLdlg*	m_pDlg;
	CGRBLcOption*	GetOption(void) {
		return &m_optGRBL;
	}
	CProbeOption*	GetProbeOption(void) {
		return &m_optProbe;
	}
//	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
