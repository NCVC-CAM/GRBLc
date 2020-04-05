#include "stdafx.h"
#include "GRBLdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////

HKEY CGRBLdlg::EnumComPort(void)
{
	HKEY	hKey;
	if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM",
				0, KEY_READ|KEY_NOTIFY, &hKey) != ERROR_SUCCESS ) {
		AfxMessageBox("シリアルポートの列挙に失敗しました");
		return NULL;
	}

	TCHAR	szEntry[_MAX_PATH];
	BYTE	szValue[_MAX_PATH];
	DWORD	dwIndex, dwEntry, dwValue, dwType;
	LONG	lResult = ERROR_SUCCESS;
	for ( dwIndex=0; lResult!=ERROR_NO_MORE_ITEMS; dwIndex++ ) {
		dwEntry = sizeof(szEntry);
		dwValue = sizeof(szValue);
		lResult = RegEnumValue(hKey, dwIndex, szEntry, &dwEntry, NULL,
							&dwType, szValue, &dwValue);
		if ( lResult==ERROR_SUCCESS && dwType==REG_SZ )
			m_cbComPort.AddString((LPCTSTR)szValue);
	}
	if ( m_cbComPort.GetCount() > 0 )
		m_cbComPort.SetCurSel(0);

	return hKey;
}

UINT CGRBLdlg::MonitoringComportThread(LPVOID pVoid)
{
#ifdef _DEBUG
	std::cout << "CGRBLdlg::MonitoringComportThread() start\n";
#endif
	LPMONITORINGPARAM pParam = reinterpret_cast<LPMONITORINGPARAM>(pVoid);
	CGRBLdlg*	pDlg = pParam->pDlg;
	HKEY		hKey = pParam->hKey;
	delete	pParam;

	TCHAR	szEntry[_MAX_PATH];
	BYTE	szValue[_MAX_PATH];
	DWORD	dwResult, dwIndex, dwEntry, dwValue, dwType;
	LONG	lResult = ERROR_SUCCESS;
	CEvent	eventReg(FALSE, TRUE);
	HANDLE	hEvent[] = {eventReg.m_hObject, pDlg->m_eventMonitoring.m_hObject};
	bool	bResult = true;

	while ( bResult ) {
		if ( RegNotifyChangeKeyValue(hKey, false, REG_NOTIFY_CHANGE_LAST_SET, hEvent[0], true) != ERROR_SUCCESS )
			break;
		dwResult = WaitForMultipleObjects(SIZEOF(hEvent), hEvent, FALSE, INFINITE);
		if ( dwResult-WAIT_OBJECT_0 == 0 ) {
#ifdef _DEBUG
			std::cout << "RegNotifyChangeKeyValue() event return\n";
#endif
			pDlg->m_cbComPort.ResetContent();
			for ( dwIndex=0, lResult=ERROR_SUCCESS; lResult!=ERROR_NO_MORE_ITEMS; dwIndex++ ) {
				dwEntry = sizeof(szEntry);
				dwValue = sizeof(szValue);
				lResult = RegEnumValue(hKey, dwIndex, szEntry, &dwEntry, NULL,
									&dwType, szValue, &dwValue);
				if ( lResult==ERROR_SUCCESS && dwType==REG_SZ )
					pDlg->m_cbComPort.AddString((LPCTSTR)szValue);
			}
			eventReg.ResetEvent();
		}
		else {
			bResult = false;
			break;
		}
	}

	RegCloseKey(hKey);
#ifdef _DEBUG
	std::cout << "CGRBLdlg::MonitoringComportThread() end\n";
#endif
	return 0;
}
