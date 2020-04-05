// ProbeOption.cpp : ŽÀ‘•ƒtƒ@ƒCƒ‹
//

#include "stdafx.h"
#include "NCVCaddin.h"
#include <string>
#include "boost/lexical_cast.hpp"
#include "ProbeOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace boost;
extern	LPCTSTR		g_szFuncName;	// "GRBLc"

static	const int	g_intDefault[] = {
	20,		// m_nZpFeed
	0,		// m_nZpSetMethod(G92)
	0,		// m_nZpWork(G54)
	20,		// m_nXYFeed
	0		// m_nXYWork(G54)
};
static	const float	g_floatDefault[] = {
	-20.0,	// m_fZpLength
	19.5,	// m_fZpThickness
	5.0,	// m_fZpDistance
	20.0,	// m_fXYLength
	5.0		// m_fXYDistance
};
//
static	LPCTSTR		g_szIntKey[] = {
	"ZProbeFeed", "ZProbeMethod", "ZProbeWork",
	"XYProbeFeed", "XYProbeWork"
};
static	LPCTSTR		g_szFloatKey[] = {
	"ZProbeLength", "ZProbeThickness", "ZProbeDistance",
	"XYProbeLength", "XYProbeDistance"
};

//////////////////////////////////////////////////////////////

CProbeOption::CProbeOption()
{
	DefaultSetting();
}

void CProbeOption::DefaultSetting(void)
{
	size_t	i;
	for ( i=0; i<probI_MAX; i++ )
		m_intOption[i] = g_intDefault[i];
	for ( i=0; i<probF_MAX; i++ )
		m_fltOption[i] = g_floatDefault[i];
}

bool CProbeOption::Read(void)
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

	for ( i=0; i<probI_MAX; i++ ) {
		lResult = reg.QueryDWORDValue(g_szIntKey[i], dwResult);
		if ( lResult == ERROR_SUCCESS )
			m_intOption[i] = dwResult;
	}
	for ( i=0; i<probF_MAX; i++ ) {
		ulResult = MAXREGBUF;
		lResult = reg.QueryStringValue(g_szFloatKey[i], szResult, &ulResult);
		if ( lResult == ERROR_SUCCESS )
			m_fltOption[i] = lexical_cast<float>(szResult);
	}

	reg.Close();
	return true;
}

bool CProbeOption::Write(void)
{
	HKEY	hKey;
	if ( NCVC_CreateRegKey(g_szFuncName, &hKey) != ERROR_SUCCESS )
		return false;

	size_t	i;
	CRegKey	reg(hKey);
	LONG	lResult = ERROR_SUCCESS;
	string	strBuf;

	for ( i=0; i<probI_MAX && lResult==ERROR_SUCCESS; i++ ) {
		lResult = reg.SetDWORDValue(g_szIntKey[i], m_intOption[i]);
	}
	for ( i=0; i<probF_MAX && lResult==ERROR_SUCCESS; i++ ) {
		strBuf = lexical_cast<string>(m_fltOption[i]);
		lResult = reg.SetStringValue(g_szFloatKey[i], strBuf.c_str());
	}

	reg.Close();
	return lResult==ERROR_SUCCESS ? true : false;
}
