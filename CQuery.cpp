// CQuery 1.1
// ���̺귯�� �Ұ� �� ��� ����� CQuery.txt ���� ����


// �����ڵ�� �������� ���� ���� �� ���� �����ؾ� �Ѵ�.
//#define UNICODE
//#define _UNICODE

// API ������Ʈ�� ���� ��� ���� ��� ������ �����Ѵ�.
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "CQuery.h"

// MFC ������Ʈ�� ���� ��� stdafx.h�� CQuery.h�� �����Ѵ�.
// #include "stdafx.h"

// ������:���� �ʱ�ȭ�� ����Ѵ�.
CQuery::CQuery()
{
	AffectCount = -1;
	ret = SQL_SUCCESS;

	// ȯ�� �ڵ��� �Ҵ��ϰ� ���� �Ӽ��� �����Ѵ�.
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
}

// �ı���:���� �ڵ��� �����Ѵ�.
CQuery::~CQuery()
{
	if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	if (hDbc) SQLDisconnect(hDbc);
	if (hDbc) SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	if (hEnv) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}
BOOL CQuery::DisConnect()
{
	if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	if (hDbc) SQLDisconnect(hDbc);
	if (hDbc) SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	if (hEnv) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
	return TRUE;
}
// ���� �ڵ��� �Ҵ��ϰ� ������ �� ����ڵ���� ���� �Ҵ��Ѵ�.
// Type=CQ_MDB:ConStr�� MDB ������ ��θ� ������. ��� ������ ���� ���丮���� MDB�� ã�´�.
// Type=CQ_DSNFILE:ConStr�� SQL ������ ���� ������ ������ DSN ������ ���� ��θ� ������. 
// Type=CQ_DSN:SQLConnect �Լ��� DSN�� ���� �����Ѵ�.
// ���� �Ǵ� ��� �ڵ� �Ҵ翡 �����ϸ� FALSE�� �����Ѵ�.
BOOL CQuery::Connect(ConType Type, TCHAR *ConStr, TCHAR *UID, TCHAR *PWD)
{
	SQLTCHAR InCon[255];
	SQLTCHAR OutCon[255];
	SQLSMALLINT cbOutCon;

	int ii = 1;
	SQLRETURN Ret;
	SQLINTEGER NativeError;
	SQLTCHAR SqlState[6], Msg[255];
	SQLSMALLINT MsgLen;
	TCHAR str[256];

	// ���� Ÿ�Կ� ���� MDB �Ǵ� SQL ����, �Ǵ� DSN�� �����Ѵ�.
	SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
	switch (Type) {
	case CQ_MDB:
		wsprintf((TCHAR *)InCon, TEXT("DRIVER={Microsoft Access Driver (*.mdb)};DBQ=%s;"), ConStr);
		ret = SQLDriverConnect(hDbc, NULL, (SQLTCHAR *)InCon, sizeof(InCon) / sizeof(SQLTCHAR), OutCon,
			sizeof(OutCon) / sizeof(SQLTCHAR), &cbOutCon, SQL_DRIVER_NOPROMPT);
		break;
	case CQ_DSNFILE:
		wsprintf((TCHAR *)InCon, TEXT("FileDsn=%s"), ConStr);
		ret = SQLDriverConnect(hDbc, NULL, (SQLTCHAR *)InCon, sizeof(InCon) / sizeof(SQLTCHAR), OutCon,
			sizeof(OutCon) / sizeof(SQLTCHAR), &cbOutCon, SQL_DRIVER_NOPROMPT);
		break;
	case CQ_DSN:
		ret = SQLConnect(hDbc, (SQLTCHAR *)ConStr, SQL_NTS, (SQLTCHAR *)UID, SQL_NTS,
			(SQLTCHAR *)PWD, SQL_NTS);
		break;
	}

	// ���� ������ ���� ������ �����ش�.
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO)) {
		while (Ret = SQLGetDiagRec(SQL_HANDLE_DBC, hDbc, ii, SqlState, &NativeError,
			Msg, sizeof(Msg) / sizeof(SQLTCHAR), &MsgLen) != SQL_NO_DATA) {
			wsprintf(str, TEXT("SQLSTATE:%s, ��������:%s"), (LPCTSTR)SqlState, (LPCTSTR)Msg);
			::MessageBox(NULL, str, TEXT("���� ����"), 0);
			ii++;
		}
		return FALSE;
	}

	// ��� �ڵ��� �Ҵ��Ѵ�.
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO)) {
		hStmt = 0;
		return FALSE;
	}
	return TRUE;
}

// SQL���� �����Ѵ�. ���н� ���� ������ ����ϰ� FALSE�� �����Ѵ�.
BOOL CQuery::ExecSQL(LPCTSTR szSQL)
{

	int c;
	// SQL���� �����Ѵ�. SQL_NO_DATA�� ������ ��쵵 �ϴ� �������� ����Ѵ�. 
	// �� ��� Fetch���� EOF�� �����ϵ��� �Ǿ� �ֱ� ������ ���� ������ ����� �ʿ�� ����.
	ret = SQLExecDirect(hStmt, (SQLTCHAR *)szSQL, SQL_NTS);
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO) && (ret != SQL_NO_DATA)) {	
		switch (ret)
		{
			//SQL_SUCCESS = 0 �Լ� ȣ�� ����.
			//SQL_SUCCESS_WITH_INFO = 1 �Լ� ȣ�� ���� & ��� �߻�.���� / �ɰ�
			//SQL_ERROR = -1 �Ϲ����� ���� �߻�. �ɰ�
			//SQL_INVALID_HANDLE = -2 �ڵ� ���� �߻�.�ɰ�
			//SQL_NO_DATA = 100 ���� ������ ����.�Ϲ������� EOF ����.����
			//SQL_NEED_DATA = 99  �߰� ���� �ʿ�.����
			//SQL_STILL_EXECUTING = 2 �񵿱� �������� ����.����
		    case -1:
				MessageBox(NULL, TEXT("�����ͺ��̽� �Ϲ����� ���� �Դϴ�."), TEXT("�˸�"), MB_OK);
				break;
			case -2:
				MessageBox(NULL, TEXT("ODBC �ڵ���� �Դϴ�."), TEXT("�˸�"), MB_OK);
				break;
			case 99:
				MessageBox(NULL, TEXT("�����ͺ��̽� �߰������� �ʿ��մϴ�."), TEXT("�˸�"), MB_OK);
				break;
		}
		//MessageBox(NULL, (LPCTSTR)hStmt, TEXT("�˸�"), MB_OK);
		PrintDiag();
		return FALSE;
	}

	// Update, Delete, Insert ��ɽ� ������� ���ڵ� ������ ���� ���´�.
	//64��Ʈ���� �����Ͻ� �������� �ּ�ó��
	SQLRowCount(hStmt, &AffectCount);

	SQLNumResultCols(hStmt, &nCol);
	if (nCol > CQ_MAXCOL) {
		::MessageBox(NULL, TEXT("�ִ� �÷� ���� �ʰ��߽��ϴ�"), TEXT("CQuery ġ���� ����"), MB_OK);
		return FALSE;
	}

	// nCol�� 0�� ���� Select�� �̿��� �ٸ� ����� ������ ����̹Ƿ� 
	// ���ε��� �� �ʿ䰡 ����.
	if (nCol == 0) {
		Clear();
		return TRUE;
	}

	// ��� �÷��� ���ڿ��� ���ε��� ���´�. Col�迭�� zero base, 
	// �÷� ��ȣ�� one base�ӿ� ������ ��
	for (c = 0; c<nCol; c++) {
		//64��Ʈ���� �����Ͻ� �������� �ּ�ó��
		SQLBindCol(hStmt, c + 1, SQL_C_TCHAR, Col[c], 255, &lCol[c]);
		SQLDescribeCol(hStmt, c + 1, ColName[c], 30, NULL, NULL, NULL, NULL, NULL);
	}
	return TRUE;
}

// SQL���� �����Ѵ�. ���� �μ��� ������ �� ExecSQL �Լ��� ȣ���Ѵ�.
BOOL CQuery::Exec(LPCTSTR szSQL, ...)
{
	TCHAR buf[2048];
	va_list marker;

	va_start(marker, szSQL);
	_vstprintf_s(buf, szSQL, marker);

	return ExecSQL(buf);
}
// ��� ���� üũ
BOOL CQuery::StatusDB()
{
	TCHAR szSQL[255];
	wsprintf(szSQL, TEXT("SELECT count(prime) from prime"));
	// SQL���� �����Ѵ�. SQL_NO_DATA�� ������ ��쵵 �ϴ� �������� ����Ѵ�. 
	// �� ��� Fetch���� EOF�� �����ϵ��� �Ǿ� �ֱ� ������ ���� ������ ����� �ʿ�� ����.
	ret = SQLExecDirect(hStmt, (SQLTCHAR *)szSQL, SQL_NTS);
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO) && (ret != SQL_NO_DATA)) {
		//PrintDiag();
		return FALSE;
	}
	return TRUE;
}

// ����¿��� �� ���� �����´�.
SQLRETURN CQuery::Fetch()
{
	ret = SQLFetch(hStmt);
	return ret;
}

// Ŀ���� �ݰ� ���ε� ������ �����Ѵ�.
void CQuery::Clear()
{
	SQLCloseCursor(hStmt);
	SQLFreeStmt(hStmt, SQL_UNBIND);
}

// �÷� �̸����κ��� �÷� �ε����� ã�´�. ���� ��� -1�� �����Ѵ�.
int CQuery::FindCol(TCHAR *name)
{
	int i;
	for (i = 0; i<nCol; i++) {
		if (lstrcmpi(name, (LPCTSTR)ColName[i]) == 0)
			return i + 1;
	}
	return -1;
}

// nCol�� �÷����� ������ �д´�. NULL�� ��� CQ_NULL�� �����Ѵ�.
int CQuery::GetInt(int nCol)
{
	// �÷��� ���ų� �÷� ��ȣ�� �ʰ��� ���
	if ((nCol > this->nCol) || (nCol == -1)) {
		return CQ_NOCOL;
	}

	if (lCol[nCol - 1] == SQL_NULL_DATA) {
		return CQ_NULL;
	}
	else {
		return _tstoi(Col[nCol - 1]);
	}
}

// SQL���� �����ϰ� ������� ù Į������ �������� �о� �����Ѵ�. 
int CQuery::ExecGetInt(LPCTSTR szSQL, ...)
{

	int i;
	TCHAR buf[2048];
	va_list marker;

	va_start(marker, szSQL);
	_vstprintf_s(buf, szSQL, marker);

	if (ExecSQL(buf) == FALSE) {
		return CQ_ERROR;
	}

	// �����Ͱ� ���� ��� CQ_NORESULT�� �����Ѵ�.
	if (Fetch() == CQ_EOT) {
		Clear();
		return CQ_NORESULT;
	}
	i = GetInt(1);
	Clear();

	return i;
}

// nCol�� �÷����� ���ڿ��� �д´�.
// buf�� ���̴� �ּ��� MAXBUFFER�̾�� �ϸ� ���� ������ ���� �ʴ´�.
int CQuery::GetStr(int nCol, TCHAR *buf, int buflen /* =MAXBUFFER */)
{
	// �÷��� ���ų� �÷� ��ȣ�� �ʰ��� ���
	if ((nCol > this->nCol) || (nCol == -1)) {
		return CQ_NOCOL;
	}

	if (lCol[nCol - 1] == SQL_NULL_DATA) {
		return CQ_NULL;
	}
	else {
		lstrcpyn(buf, Col[nCol - 1], buflen);
		return CQ_NOERROR;
	}
}

// SQL���� �����ϰ� ������� ù Į������ ���ڿ��� �о� �����Ѵ�.
int CQuery::ExecGetStr(LPCTSTR szSQL, TCHAR *buf, ...)
{
	int Result;
	TCHAR tbuf[2048];
	va_list marker;

	va_start(marker, buf);
	_vstprintf_s(tbuf, szSQL, marker);

	if (ExecSQL(tbuf) == FALSE) {
		return CQ_ERROR;
	}

	// �����Ͱ� ���� ��� CQ_NORESULT�� �����Ѵ�.
	if (Fetch() == CQ_EOT) {
		Clear();
		return CQ_NORESULT;
	}

	// GetStr�� �����ϴ� ���� �ڵ�(�÷�����, NULL) �� �ٽ� �����Ѵ�.
	Result = GetStr(1, buf);
	Clear();
	return Result;
}

// nCol�� �Ǽ��� ���� �д´�. �� �Լ��� �����ϴ� �������� ���������� ĳ������ �� ���Ѵ�.
double CQuery::GetDouble(int nCol)
{
	TCHAR *dummyptr;
	TCHAR tmp[128];
	double tmpdouble;

	// �÷��� ���ų� �÷� ��ȣ�� �ʰ��� ���
	if ((nCol > this->nCol) || (nCol == -1)) {
		return CQ_NOCOL;
	}

	if (lCol[nCol - 1] == SQL_NULL_DATA) {
		return CQ_NULL;
	}
	else {
		lstrcpy(tmp, Col[nCol - 1]);
		tmpdouble = _tcstod(tmp, &dummyptr);
		return tmpdouble;
	}
}

// yyyy-mm-dd hh:mm:ss ����(�Ǵ� ��¥��)���� �Ǿ� �ִ� ��¥�� ��¥ ����ü�� �ִ´�.
void CQuery::StrToTime(TCHAR *str, SYSTEMTIME *st)
{
	TCHAR Year[5] = { 0, }, Month[3] = { 0, }, Day[3] = { 0, };
	TCHAR Hour[3] = { 0, }, Minute[3] = { 0, }, Second[3] = { 0, };

	// ��¥ ������ �ƴ� ��� ���� ó��. �ּ��� ����� ��Ҵ� �־�� �Ѵ�.
	if (lstrlen(str) < 10)
		return;

	// ��¥���� �����ϹǷ� �ð� ������ ���½��Ѿ� �Ѵ�.
	memset(st, 0, sizeof(SYSTEMTIME));
	lstrcpyn(Year, str, 5);
	lstrcpyn(Month, str + 5, 3);
	lstrcpyn(Day, str + 8, 3);
	lstrcpyn(Hour, str + 11, 3);
	lstrcpyn(Minute, str + 14, 3);
	lstrcpyn(Second, str + 17, 3);

	st->wYear = _tstoi(Year);
	st->wMonth = _tstoi(Month);
	st->wDay = _tstoi(Day);

	// �ð� ������ �־�߸� �ð����� ��ȯ�Ѵ�. �ƴϸ� 0���� ���ܵд�.
	if (lstrlen(str) > 10) {
		st->wHour = _tstoi(Hour);
		st->wMinute = _tstoi(Minute);
		st->wSecond = _tstoi(Second);
	}
}

// ��¥ �ʵ带 �о�´�. ���ڿ��� �ϴ� ���� �� SYSTEMTIME������ �ٲ۴�.
// ODBC�� ��¥�� �д� ������ ����� ������ ���ε��� ��¥������ ���� �� �־�� �ϴ�
// ���ŷο��� �ֱ� ������ ���ڿ��� ���ε��� ���� �������ϴ� ����� ����ϰ� �ִ�.
int CQuery::GetDate(int nCol, SYSTEMTIME *st)
{
	TCHAR tmp[128];

	// �÷��� ���ų� �÷� ��ȣ�� �ʰ��� ���
	if ((nCol > this->nCol) || (nCol == -1)) {
		return CQ_NOCOL;
	}

	if (lCol[nCol - 1] == SQL_NULL_DATA) {
		return CQ_NULL;
	}
	else {
		lstrcpy(tmp, Col[nCol - 1]);
		StrToTime(tmp, st);
		return CQ_NOERROR;
	}
}

// ���� �߻��� ���� ������ ����Ѵ�.
// �� ������ Assert�� �ش��ϹǷ� ��������� ������ ���� �ʵ��� ���� ó���� �ؾ� �Ѵ�.
void CQuery::PrintDiag()
{
	int ii;
	SQLRETURN Ret;
	SQLINTEGER NativeError;
	SQLTCHAR SqlState[6], Msg[255];
	SQLSMALLINT MsgLen;
	TCHAR str[256];

	ii = 1;
	while (Ret = SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, ii, SqlState, &NativeError,
		Msg, sizeof(Msg) / sizeof(TCHAR), &MsgLen) != SQL_NO_DATA) {
		wsprintf(str, TEXT("SQLSTATE:%s, ��������:%s"), (LPCTSTR)SqlState, (LPCTSTR)Msg);
		::MessageBox(NULL, str, TEXT("���� ����"), 0);

		ii++;
	}
}

// BLOB �����͸� buf�� ä���. �̶� buf�� ����� ũ���� �޸𸮸� �̸� �Ҵ��� 
// ���ƾ� �Ѵ�. NULL�� ��� 0�� �����ϰ� ���� �߻��� -1�� �����Ѵ�. ������ ���� 
// �� ����Ʈ ���� �����Ѵ�. szSQL�� �ϳ��� BLOB �ʵ带 �д� Select SQL���̾�� �Ѵ�.
int CQuery::ReadBlob(LPCTSTR szSQL, void *buf)
{
	//64��Ʈ���� �����Ͻ� �������� �ּ�ó��
	SQLTCHAR BinaryPtr[CQ_BLOBBATCH];
	SQLINTEGER LenBin;
	char *p;
	int nGet;
	int TotalGet = 0;

	// ����� �����ϱ� ���� �׻� Clear�� ���� �Ѵ�.
	Clear();

	ret = SQLExecDirect(hStmt, (SQLTCHAR *)szSQL, SQL_NTS);
	if (ret != SQL_SUCCESS) {
		PrintDiag();
		return -1;
	}

	while ((ret = SQLFetch(hStmt)) != SQL_NO_DATA) {
		p = (char *)buf;
		while ((ret = SQLGetData(hStmt, 1, SQL_C_BINARY, BinaryPtr, sizeof(BinaryPtr),
			&LenBin)) != SQL_NO_DATA) {
			if (LenBin == SQL_NULL_DATA) {
				Clear();
				return 0;
			}
			if (ret == SQL_SUCCESS)
				nGet = LenBin;
			else
				nGet = CQ_BLOBBATCH;
			TotalGet += nGet;
			memcpy(p, BinaryPtr, nGet);
			p += nGet;
		}
	}
	Clear();
	return TotalGet;

}

// buf�� BLOB �����͸� �����Ѵ�. szSQL�� �ϳ��� BLOB �����͸� �����ϴ� Update, Insert
// SQL���̾�� �Ѵ�.
int CQuery::WriteBlob(LPCTSTR szSQL, void *buf, int size)
{

	//64��Ʈ���� �����Ͻ� �������� �ּ�ó��
	SQLINTEGER cbBlob;
	char tmp[CQ_BLOBBATCH], *p;
	SQLPOINTER pToken;
	int nPut;

	// ����� �����ϱ� ���� �׻� Clear�� ���� �Ѵ�.
	Clear();

	cbBlob = SQL_LEN_DATA_AT_EXEC(size);
	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_LONGVARBINARY,
		size, 0, (SQLPOINTER)1, 0, &cbBlob);
	SQLExecDirect(hStmt, (SQLTCHAR *)szSQL, SQL_NTS);
	ret = SQLParamData(hStmt, &pToken);
	while (ret == SQL_NEED_DATA) {
		if (ret == SQL_NEED_DATA) {
			if ((INT_PTR)pToken == 1) {
				for (p = (char *)buf; p<(char *)buf + size; p += CQ_BLOBBATCH) {
					nPut = (int)min(CQ_BLOBBATCH, (char *)buf + size - p);
					memcpy(tmp, p, nPut);
					if (SQLPutData(hStmt, (PTR)tmp, nPut) != SQL_SUCCESS) {
						Clear();
						return CQ_ERROR;
					}
				}
			}
		}
		ret = SQLParamData(hStmt, &pToken);
	}
	Clear();
	return CQ_NOERROR;

}
