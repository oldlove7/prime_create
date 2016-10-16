//#include <sql.h>
#include <sqlext.h>

//접속정보
#define Dns TEXT("prime_db")
#define Id  TEXT("root")
#define Pass TEXT("old7302")

// CQuery에서 사용하는 매크로 상수들
#define CQ_MAXCOL 100				// 최대 컬럼수
#define CQ_MAXBUFFER 255			// 바인딩 변수의 버퍼 크기
#define CQ_BLOBBATCH 4096			// BLOB 입출력 단위

#define CQ_NULL -100				// NULL 필드값
#define CQ_NOCOL -102				// 없는 컬럼임.
#define CQ_NORESULT -103			// 결과셋이 하나도 없다.
#define CQ_ERROR -104				// 일반적인 에러
#define CQ_NOERROR 1				// 성공.
#define CQ_EOT SQL_NO_DATA			// Fetch 함수에서 데이터가 더 이상 없다는 뜻임. End of Table
// 연결 타입
enum ConType { CQ_MDB = 1, CQ_DSNFILE, CQ_DSN };

class CQuery
{
private:
	SQLHENV hEnv;									// 환경 핸들
	SQLHDBC hDbc;									// 연결 핸들
	SQLRETURN ret;									// 최후 실행한 SQL문의 결과값
	TCHAR Col[CQ_MAXCOL][CQ_MAXBUFFER + 1];			// 바인딩될 컬럼 정보

	int FindCol(TCHAR *name);						// 컬럼의 이름으로부터 번호를 찾는다.

	void StrToTime(TCHAR *str, SYSTEMTIME *st);		// 문자열을 시간으로 변환한다.

public:
	SQLINTEGER AffectCount;							// 영향받은 레코드 개수
	SQLHSTMT hStmt;									// 명령 핸들. 직접 사용 가능
	SQLSMALLINT nCol;								// 컬럼 개수
	SQLTCHAR ColName[CQ_MAXCOL][50];				// 컬럼의 이름들
	SQLINTEGER lCol[CQ_MAXCOL];						// 컬럼의 길이/상태 정보

	void PrintDiag();								// 진단 정보 출력
	CQuery();										// 생성자
	~CQuery();										// 파괴자:연결 핸들을 해제한다.
	// 데이터 소스에 연결한다.
	BOOL Connect(ConType Type, TCHAR *ConStr, TCHAR *UID = NULL, TCHAR *PWD = NULL);
	BOOL DisConnect();
	BOOL ExecSQL(LPCTSTR szSQL);					// SQL문을 실행한다.
	BOOL Exec(LPCTSTR szSQL, ...);					// SQL문을 실행한다.
	SQLRETURN Fetch();								// 한 행 가져오기
	BOOL StatusDB();                                // DB접속 체크
	void Clear();									// 커서 해제 및 언바인딩

	// 정수형 컬럼 읽기
	int GetInt(int nCol);
	int GetInt(TCHAR *sCol) { return GetInt(FindCol(sCol)); }
	int ExecGetInt(LPCTSTR szSQL, ...);

	// 문자열형 컬럼 읽기
	int GetStr(int nCol, TCHAR *buf, int buflen = CQ_MAXBUFFER);
	int GetStr(TCHAR *sCol, TCHAR *buf, int buflen = CQ_MAXBUFFER) { return GetStr(FindCol(sCol), buf, buflen); }
	int ExecGetStr(LPCTSTR szSQL, TCHAR *buf, ...);

	// 실수형 컬럼 읽기
	double GetDouble(int nCol);
	double GetDouble(TCHAR *sCol) { return GetDouble(FindCol(sCol)); }

	// 날짜형 컬럼 읽기
	int GetDate(int nCol, SYSTEMTIME *st);
	int GetDate(TCHAR *sCol, SYSTEMTIME *st) { return GetDate(FindCol(sCol), st); }

	// 블랍 입출력
	int ReadBlob(LPCTSTR szSQL, void *buf);
	int WriteBlob(LPCTSTR szSQL, void *buf, int size);




};
