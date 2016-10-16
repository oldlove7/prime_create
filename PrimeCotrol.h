#include <windows.h>
class PrimeCotrol
{
private:
	SYSTEMTIME st;
	
	TCHAR cTime[16];
public:
	
	PrimeCotrol();
	~PrimeCotrol();
	BOOL listview(HWND hDlg, TCHAR *str);
	BOOL editbox(HWND hDlg, TCHAR *str);
	TCHAR sTime();
};

