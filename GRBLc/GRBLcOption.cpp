// GRBLcOption.cpp : ŽÀ‘•ƒtƒ@ƒCƒ‹
//

#include "stdafx.h"
#include <string>
#include "boost/lexical_cast.hpp"
#include "boost/format.hpp"
#include "NCVCaddin.h"
#include "GRBLcOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace boost;
extern	LPCTSTR		g_szFuncName;	// "GRBLc"

static	const int	g_intDefault[] = {
	0,		// m_bAutoConnect
	0,		// m_bWithTrace
	500,	// m_nQueryTime
	9,		// m_nBaudrate(115200)
	100,	// m_nJogFeed
	1		// m_nWPos
};
static	const float	g_floatDefault[] = {
	10.0	// m_fJogStep
};
static	LPCTSTR		g_szDefault[] = {
	"$$", "$#", "$G",	// CustomCommand
	""					// ComPort
};
//
static	LPCTSTR		g_szIntKey[] = {
	"AutoConnect", "WithTrace", "QueryTime", "Baudrate", "JogFeed",
	"WPos"
};
static	LPCTSTR		g_szFloatKey[] = {
	"JogStep"
};
static	LPCTSTR		g_szStringKey[] = {
	"CustomCmd1", "CustomCmd2", "CustomCmd3",
	"Comport"
};

//////////////////////////////////////////////////////////////

CGRBLcOption::CGRBLcOption()
{
	DefaultSetting();
}

void CGRBLcOption::DefaultSetting(void)
{
	size_t	i;
	for ( i=0; i<grblI_MAX; i++ )
		m_intOption[i] = g_intDefault[i];
	for ( i=0; i<grblF_MAX; i++ )
		m_fltOption[i] = g_floatDefault[i];
	for ( i=0; i<grblS_MAX; i++ )
		m_strOption[i] = g_szDefault[i];
}

int CGRBLcOption::GetIntOpt(size_t n)
{
	ASSERT(0<=n && n<grblI_MAX);
	return m_intOption[n];
}

float CGRBLcOption::GetFloatOpt(size_t n)
{
	ASSERT(0<=n && n<grblF_MAX);
	return m_fltOption[n];
}

string CGRBLcOption::GetStringOpt(size_t n)
{
	ASSERT(0<=n && n<grblS_MAX);
	return m_strOption[n];
}

bool CGRBLcOption::IsOBScheck(size_t n)
{
	return n<m_bitOBS.size() ? m_bitOBS[n] : false;
}

bool CGRBLcOption::Read(void)
{
	HKEY	hKey;
	if ( NCVC_CreateRegKey(g_szFuncName, &hKey) != ERROR_SUCCESS )
		return false;

	size_t	i;
	CRegKey	reg(hKey);
	LONG	lResult = ERROR_SUCCESS;
	DWORD	dwResult;
	TCHAR	szResult[MAXREGBUF];
	ULONG	ulResult;

	for ( i=0; i<grblI_MAX; i++ ) {
		lResult = reg.QueryDWORDValue(g_szIntKey[i], dwResult);
		if ( lResult == ERROR_SUCCESS )
			m_intOption[i] = dwResult;
	}
	for ( i=0; i<grblF_MAX; i++ ) {
		ulResult = MAXREGBUF;
		lResult = reg.QueryStringValue(g_szFloatKey[i], szResult, &ulResult);
		if ( lResult == ERROR_SUCCESS )
			m_fltOption[i] = lexical_cast<float>(szResult);
	}
	for ( i=0; i<grblS_MAX; i++ ) {
		ulResult = MAXREGBUF;
		lResult = reg.QueryStringValue(g_szStringKey[i], szResult, &ulResult);
		if ( lResult == ERROR_SUCCESS )
			m_strOption[i] = szResult;
	}
	reg.Close();

	m_bitOBS.reset();
	if ( NCVC_GetSelectMachineFileName(szResult, MAXREGBUF) > 0 ) {
		string	strKey;
		for ( i=0; i<m_bitOBS.size(); i++ ) {
			strKey = (format("OBS%d") % i).str();
			if ( GetPrivateProfileInt("Settings", strKey.c_str(), 0, szResult) )
				m_bitOBS.set(i);
		}
	}

	return true;
}

bool CGRBLcOption::Write(void)
{
	HKEY	hKey;
	if ( NCVC_CreateRegKey(g_szFuncName, &hKey) != ERROR_SUCCESS )
		return false;

	size_t	i;
	CRegKey	reg(hKey);
	LONG	lResult = ERROR_SUCCESS;
	string	strBuf;

	for ( i=0; i<grblI_MAX && lResult==ERROR_SUCCESS; i++ ) {
		lResult = reg.SetDWORDValue(g_szIntKey[i], m_intOption[i]);
	}
	for ( i=0; i<grblF_MAX && lResult==ERROR_SUCCESS; i++ ) {
		strBuf = lexical_cast<string>(m_fltOption[i]);
		lResult = reg.SetStringValue(g_szFloatKey[i], strBuf.c_str());
	}
	for ( i=0; i<grblS_MAX && lResult==ERROR_SUCCESS; i++ ) {
		lResult = reg.SetStringValue(g_szStringKey[i], m_strOption[i].c_str());
	}

	reg.Close();
	return lResult==ERROR_SUCCESS ? true : false;
}

bool CGRBLcOption::Write_NoSetup(void)
{
	HKEY	hKey;
	if ( NCVC_CreateRegKey(g_szFuncName, &hKey) != ERROR_SUCCESS )
		return false;

	CRegKey	reg(hKey);
	LONG	lResult = ERROR_SUCCESS;

	lResult = reg.SetDWORDValue(g_szIntKey[grblI_WPos], m_nWPos);

	reg.Close();
	return lResult==ERROR_SUCCESS ? true : false;
}
