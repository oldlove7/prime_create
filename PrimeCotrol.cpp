#include "PrimeCotrol.h"
#include "resource.h"
#include <windows.h>


PrimeCotrol::PrimeCotrol()
{
}


PrimeCotrol::~PrimeCotrol()
{
}

BOOL PrimeCotrol::listview(HWND hDlg, TCHAR *str)
{
	TCHAR printTime[128];
	TCHAR printString[128];
	//ua_tcscpy(printTime, cTime);
	sTime();
	strcpy_s(printTime, cTime);
	wsprintf(printString, TEXT("%s %s"), printTime, str);
	SendMessage(GetDlgItem(hDlg, IDC_PRIME_LIST), LB_ADDSTRING, 0, (LPARAM)printString);
	return TRUE;
}

TCHAR PrimeCotrol::sTime()
{
	GetLocalTime(&st);
	wsprintf(cTime, TEXT("[%d:%d: %d]"), st.wHour, st.wMinute, st.wSecond);
	return(*cTime);
}
