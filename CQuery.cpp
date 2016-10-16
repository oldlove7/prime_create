// CQuery 1.1
// 라이브러리 소개 및 사용 방법은 CQuery.txt 파일 참조


// 유니코드로 컴파일할 때는 다음 두 줄을 포함해야 한다.
//#define UNICODE
//#define _UNICODE

// API 프로젝트에 사용될 경우 다음 헤더 파일을 포함한다.
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "CQuery.h"

// MFC 프로젝트에 사용될 경우 stdafx.h에 CQuery.h를 포함한다.
// #include "stdafx.h"

// 생성자:각종 초기화를 담당한다.
CQuery::CQuery()
{
	AffectCount = -1;
	ret = SQL_SUCCESS;

	// 환경 핸들을 할당하고 버전 속성을 설정한다.
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
}

// 파괴자:연결 핸들을 해제한다.
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
// 연결 핸들을 할당하고 연결한 후 명령핸들까지 같이 할당한다.
// Type=CQ_MDB:ConStr은 MDB 파일의 경로를 가진다. 경로 생략시 현재 디렉토리에서 MDB를 찾는다.
// Type=CQ_DSNFILE:ConStr은 SQL 서버의 연결 정보를 가지는 DSN 파일의 완전 경로를 가진다. 
// Type=CQ_DSN:SQLConnect 함수로 DSN에 직접 연결한다.
// 연결 또는 명령 핸들 할당에 실패하면 FALSE를 리턴한다.
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

	// 연결 타입에 따라 MDB 또는 SQL 서버, 또는 DSN에 연결한다.
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

	// 접속 에러시 진단 정보를 보여준다.
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO)) {
		while (Ret = SQLGetDiagRec(SQL_HANDLE_DBC, hDbc, ii, SqlState, &NativeError,
			Msg, sizeof(Msg) / sizeof(SQLTCHAR), &MsgLen) != SQL_NO_DATA) {
			wsprintf(str, TEXT("SQLSTATE:%s, 진단정보:%s"), (LPCTSTR)SqlState, (LPCTSTR)Msg);
			::MessageBox(NULL, str, TEXT("진단 정보"), 0);
			ii++;
		}
		return FALSE;
	}

	// 명령 핸들을 할당한다.
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO)) {
		hStmt = 0;
		return FALSE;
	}
	return TRUE;
}

// SQL문을 실행한다. 실패시 진단 정보를 출력하고 FALSE를 리턴한다.
BOOL CQuery::ExecSQL(LPCTSTR szSQL)
{

	int c;
	// SQL문을 실행한다. SQL_NO_DATA를 리턴한 경우도 일단 성공으로 취급한다. 
	// 이 경우 Fetch에서 EOF를 리턴하도록 되어 있기 때문에 진단 정보를 출력할 필요는 없다.
	ret = SQLExecDirect(hStmt, (SQLTCHAR *)szSQL, SQL_NTS);
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO) && (ret != SQL_NO_DATA)) {	
		switch (ret)
		{
			//SQL_SUCCESS = 0 함수 호출 성공.
			//SQL_SUCCESS_WITH_INFO = 1 함수 호출 성공 & 경고 발생.정상 / 심각
			//SQL_ERROR = -1 일반적인 오류 발생. 심각
			//SQL_INVALID_HANDLE = -2 핸들 오류 발생.심각
			//SQL_NO_DATA = 100 읽을 데이터 없음.일반적으로 EOF 상태.정상
			//SQL_NEED_DATA = 99  추가 정보 필요.선택
			//SQL_STILL_EXECUTING = 2 비동기 실행중인 상태.정상
		    case -1:
				MessageBox(NULL, TEXT("데이터베이스 일반적인 에러 입니다."), TEXT("알림"), MB_OK);
				break;
			case -2:
				MessageBox(NULL, TEXT("ODBC 핸들오류 입니다."), TEXT("알림"), MB_OK);
				break;
			case 99:
				MessageBox(NULL, TEXT("데이터베이스 추가정보가 필요합니다."), TEXT("알림"), MB_OK);
				break;
		}
		//MessageBox(NULL, (LPCTSTR)hStmt, TEXT("알림"), MB_OK);
		PrintDiag();
		return FALSE;
	}

	// Update, Delete, Insert 명령시 영향받은 레코드 개수를 구해 놓는다.
	//64비트에서 컴파일시 에서나서 주석처리
	SQLRowCount(hStmt, &AffectCount);

	SQLNumResultCols(hStmt, &nCol);
	if (nCol > CQ_MAXCOL) {
		::MessageBox(NULL, TEXT("최대 컬럼 수를 초과했습니다"), TEXT("CQuery 치명적 에러"), MB_OK);
		return FALSE;
	}

	// nCol이 0인 경우는 Select문 이외의 다른 명령을 실행한 경우이므로 
	// 바인딩을 할 필요가 없다.
	if (nCol == 0) {
		Clear();
		return TRUE;
	}

	// 모든 컬럼을 문자열로 바인딩해 놓는다. Col배열은 zero base, 
	// 컬럼 번호는 one base임에 유의할 것
	for (c = 0; c<nCol; c++) {
		//64비트에서 컴파일시 에서나서 주석처리
		SQLBindCol(hStmt, c + 1, SQL_C_TCHAR, Col[c], 255, &lCol[c]);
		SQLDescribeCol(hStmt, c + 1, ColName[c], 30, NULL, NULL, NULL, NULL, NULL);
	}
	return TRUE;
}

// SQL문을 실행한다. 가변 인수를 조립한 후 ExecSQL 함수를 호출한다.
BOOL CQuery::Exec(LPCTSTR szSQL, ...)
{
	TCHAR buf[2048];
	va_list marker;

	va_start(marker, szSQL);
	_vstprintf_s(buf, szSQL, marker);

	return ExecSQL(buf);
}
// 디비 접속 체크
BOOL CQuery::StatusDB()
{
	TCHAR szSQL[255];
	wsprintf(szSQL, TEXT("SELECT count(prime) from prime"));
	// SQL문을 실행한다. SQL_NO_DATA를 리턴한 경우도 일단 성공으로 취급한다. 
	// 이 경우 Fetch에서 EOF를 리턴하도록 되어 있기 때문에 진단 정보를 출력할 필요는 없다.
	ret = SQLExecDirect(hStmt, (SQLTCHAR *)szSQL, SQL_NTS);
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO) && (ret != SQL_NO_DATA)) {
		//PrintDiag();
		return FALSE;
	}
	return TRUE;
}

// 결과셋에서 한 행을 가져온다.
SQLRETURN CQuery::Fetch()
{
	ret = SQLFetch(hStmt);
	return ret;
}

// 커서를 닫고 바인딩 정보를 해제한다.
void CQuery::Clear()
{
	SQLCloseCursor(hStmt);
	SQLFreeStmt(hStmt, SQL_UNBIND);
}

// 컬럼 이름으로부터 컬럼 인덱스를 찾는다. 없을 경우 -1을 리턴한다.
int CQuery::FindCol(TCHAR *name)
{
	int i;
	for (i = 0; i<nCol; i++) {
		if (lstrcmpi(name, (LPCTSTR)ColName[i]) == 0)
			return i + 1;
	}
	return -1;
}

// nCol의 컬럼값을 정수로 읽는다. NULL일 경우 CQ_NULL을 리턴한다.
int CQuery::GetInt(int nCol)
{
	// 컬럼이 없거나 컬럼 번호를 초과한 경우
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

// SQL문을 실행하고 결과셋의 첫 칼럼에서 정수값을 읽어 리턴한다. 
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

	// 데이터가 없는 경우 CQ_NORESULT를 리턴한다.
	if (Fetch() == CQ_EOT) {
		Clear();
		return CQ_NORESULT;
	}
	i = GetInt(1);
	Clear();

	return i;
}

// nCol의 컬럼값을 문자열로 읽는다.
// buf의 길이는 최소한 MAXBUFFER이어야 하며 길이 점검은 하지 않는다.
int CQuery::GetStr(int nCol, TCHAR *buf, int buflen /* =MAXBUFFER */)
{
	// 컬럼이 없거나 컬럼 번호를 초과한 경우
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

// SQL문을 실행하고 결과셋의 첫 칼럼에서 문자열을 읽어 리턴한다.
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

	// 데이터가 없는 경우 CQ_NORESULT를 리턴한다.
	if (Fetch() == CQ_EOT) {
		Clear();
		return CQ_NORESULT;
	}

	// GetStr이 리턴하는 에러 코드(컬럼없음, NULL) 를 다시 리턴한다.
	Result = GetStr(1, buf);
	Clear();
	return Result;
}

// nCol의 실수형 값을 읽는다. 이 함수가 리턴하는 에러값은 정수형으로 캐스팅한 후 비교한다.
double CQuery::GetDouble(int nCol)
{
	TCHAR *dummyptr;
	TCHAR tmp[128];
	double tmpdouble;

	// 컬럼이 없거나 컬럼 번호를 초과한 경우
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

// yyyy-mm-dd hh:mm:ss 포멧(또는 날짜만)으로 되어 있는 날짜를 날짜 구조체에 넣는다.
void CQuery::StrToTime(TCHAR *str, SYSTEMTIME *st)
{
	TCHAR Year[5] = { 0, }, Month[3] = { 0, }, Day[3] = { 0, };
	TCHAR Hour[3] = { 0, }, Minute[3] = { 0, }, Second[3] = { 0, };

	// 날짜 포멧이 아닌 경우 에러 처리. 최소한 년월일 요소는 있어야 한다.
	if (lstrlen(str) < 10)
		return;

	// 날짜만을 추출하므로 시간 정보는 리셋시켜야 한다.
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

	// 시간 정보가 있어야만 시간값을 변환한다. 아니면 0으로 남겨둔다.
	if (lstrlen(str) > 10) {
		st->wHour = _tstoi(Hour);
		st->wMinute = _tstoi(Minute);
		st->wSecond = _tstoi(Second);
	}
}

// 날짜 필드를 읽어온다. 문자열로 일단 읽은 후 SYSTEMTIME형으로 바꾼다.
// ODBC에 날짜를 읽는 별도의 기능이 있으나 바인딩을 날짜형으로 따로 해 주어야 하는
// 번거로움이 있기 때문에 문자열로 바인딩해 놓고 컨버팅하는 방법을 사용하고 있다.
int CQuery::GetDate(int nCol, SYSTEMTIME *st)
{
	TCHAR tmp[128];

	// 컬럼이 없거나 컬럼 번호를 초과한 경우
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

// 에러 발생시 진단 정보를 출력한다.
// 이 정보는 Assert에 해당하므로 릴리즈에서는 진단이 뜨지 않도록 에러 처리를 해야 한다.
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
		wsprintf(str, TEXT("SQLSTATE:%s, 진단정보:%s"), (LPCTSTR)SqlState, (LPCTSTR)Msg);
		::MessageBox(NULL, str, TEXT("진단 정보"), 0);

		ii++;
	}
}

// BLOB 데이터를 buf에 채운다. 이때 buf는 충분한 크기의 메모리를 미리 할당해 
// 놓아야 한다. NULL일 경우 0을 리턴하고 에러 발생시 -1을 리턴한다. 성공시 읽은 
// 총 바이트 수를 리턴한다. szSQL은 하나의 BLOB 필드를 읽는 Select SQL문이어야 한다.
int CQuery::ReadBlob(LPCTSTR szSQL, void *buf)
{
	//64비트에서 컴파일시 에서나서 주석처리
	SQLTCHAR BinaryPtr[CQ_BLOBBATCH];
	SQLINTEGER LenBin;
	char *p;
	int nGet;
	int TotalGet = 0;

	// 명령을 실행하기 전에 항상 Clear를 먼저 한다.
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

// buf의 BLOB 데이터를 저장한다. szSQL은 하나의 BLOB 데이터를 저장하는 Update, Insert
// SQL문이어야 한다.
int CQuery::WriteBlob(LPCTSTR szSQL, void *buf, int size)
{

	//64비트에서 컴파일시 에서나서 주석처리
	SQLINTEGER cbBlob;
	char tmp[CQ_BLOBBATCH], *p;
	SQLPOINTER pToken;
	int nPut;

	// 명령을 실행하기 전에 항상 Clear를 먼저 한다.
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
