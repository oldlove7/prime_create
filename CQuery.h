//#include <sql.h>
#include <sqlext.h>

//��������
#define Dns TEXT("prime_db")
#define Id  TEXT("root")
#define Pass TEXT("old7302")

// CQuery���� ����ϴ� ��ũ�� �����
#define CQ_MAXCOL 100				// �ִ� �÷���
#define CQ_MAXBUFFER 255			// ���ε� ������ ���� ũ��
#define CQ_BLOBBATCH 4096			// BLOB ����� ����

#define CQ_NULL -100				// NULL �ʵ尪
#define CQ_NOCOL -102				// ���� �÷���.
#define CQ_NORESULT -103			// ������� �ϳ��� ����.
#define CQ_ERROR -104				// �Ϲ����� ����
#define CQ_NOERROR 1				// ����.
#define CQ_EOT SQL_NO_DATA			// Fetch �Լ����� �����Ͱ� �� �̻� ���ٴ� ����. End of Table
// ���� Ÿ��
enum ConType { CQ_MDB = 1, CQ_DSNFILE, CQ_DSN };

class CQuery
{
private:
	SQLHENV hEnv;									// ȯ�� �ڵ�
	SQLHDBC hDbc;									// ���� �ڵ�
	SQLRETURN ret;									// ���� ������ SQL���� �����
	TCHAR Col[CQ_MAXCOL][CQ_MAXBUFFER + 1];			// ���ε��� �÷� ����

	int FindCol(TCHAR *name);						// �÷��� �̸����κ��� ��ȣ�� ã�´�.

	void StrToTime(TCHAR *str, SYSTEMTIME *st);		// ���ڿ��� �ð����� ��ȯ�Ѵ�.

public:
	SQLINTEGER AffectCount;							// ������� ���ڵ� ����
	SQLHSTMT hStmt;									// ��� �ڵ�. ���� ��� ����
	SQLSMALLINT nCol;								// �÷� ����
	SQLTCHAR ColName[CQ_MAXCOL][50];				// �÷��� �̸���
	SQLINTEGER lCol[CQ_MAXCOL];						// �÷��� ����/���� ����

	void PrintDiag();								// ���� ���� ���
	CQuery();										// ������
	~CQuery();										// �ı���:���� �ڵ��� �����Ѵ�.
	// ������ �ҽ��� �����Ѵ�.
	BOOL Connect(ConType Type, TCHAR *ConStr, TCHAR *UID = NULL, TCHAR *PWD = NULL);
	BOOL DisConnect();
	BOOL ExecSQL(LPCTSTR szSQL);					// SQL���� �����Ѵ�.
	BOOL Exec(LPCTSTR szSQL, ...);					// SQL���� �����Ѵ�.
	SQLRETURN Fetch();								// �� �� ��������
	BOOL StatusDB();                                // DB���� üũ
	void Clear();									// Ŀ�� ���� �� ����ε�

	// ������ �÷� �б�
	int GetInt(int nCol);
	int GetInt(TCHAR *sCol) { return GetInt(FindCol(sCol)); }
	int ExecGetInt(LPCTSTR szSQL, ...);

	// ���ڿ��� �÷� �б�
	int GetStr(int nCol, TCHAR *buf, int buflen = CQ_MAXBUFFER);
	int GetStr(TCHAR *sCol, TCHAR *buf, int buflen = CQ_MAXBUFFER) { return GetStr(FindCol(sCol), buf, buflen); }
	int ExecGetStr(LPCTSTR szSQL, TCHAR *buf, ...);

	// �Ǽ��� �÷� �б�
	double GetDouble(int nCol);
	double GetDouble(TCHAR *sCol) { return GetDouble(FindCol(sCol)); }

	// ��¥�� �÷� �б�
	int GetDate(int nCol, SYSTEMTIME *st);
	int GetDate(TCHAR *sCol, SYSTEMTIME *st) { return GetDate(FindCol(sCol), st); }

	// ��� �����
	int ReadBlob(LPCTSTR szSQL, void *buf);
	int WriteBlob(LPCTSTR szSQL, void *buf, int size);




};
