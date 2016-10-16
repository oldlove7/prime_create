#include <windows.h>
#include "CQuery.h"
#include "resource.h"
#include "PrimeCotrol.h"
#include "Prime.h"
#include <conio.h>
#include <stdio.h>
#include <CommCtrl.h>

 
BOOL CALLBACK MainDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);

HINSTANCE g_hInst;
HWND hDlgMain;
HWND hList, hFName, hFPrice, hFKorean, hPrimeList, hMaxPrime, hMaxPrime2, hEditbox, hPrimeCount, hPrimeTerm, hPrimeTermTime, hProgress;
HWND hCount, hStat;
HANDLE hThreadPrime = 0,hThreadDBstat=0,hThreadDBconn=0;//������ �ڵ�
CQuery qry;
PrimeCotrol primelistview;


int threadstatus;//������ ���� ���� 0: ���� 1: ����
int dbstat;//��� ���� üũ ���� 0: ���� -1: ������

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance
		  ,LPSTR lpszCmdParam,int nCmdShow)
{

	g_hInst=hInstance;
	/*
	if (qry.Connect(CQ_DSN, Dns, Id, Pass) == TRUE) {
		wsprintf(szTmp, TEXT("������ ���̽� �����"));
		primelistview.listview(NULL, szTmp);
	}
	else
	{
		MessageBox(NULL, "������ ���̽��� ������ �� �����ϴ�", "����", MB_OK);
		return 0;
	}
	*/
	DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOGMAIN), HWND_DESKTOP, MainDlgProc);
	

	return 0;
}
//������ ���̽� �����Լ�
BOOL DBCONN(HWND hDlg)
{
	TCHAR szDBconn[128];
	if (qry.Connect(CQ_DSN, Dns, Id, Pass) == TRUE)
	{
		wsprintf(szDBconn, TEXT("������ ���̽� ���� �õ�"));
		primelistview.listview(hDlg, szDBconn);
		wsprintf(szDBconn, TEXT("������ ���̽� �����"));
		primelistview.listview(hDlg, szDBconn);
		dbstat = 0;
	}
	else
	{
		wsprintf(szDBconn, TEXT("������ ���̽��� ������ �� �����ϴ�."));
		primelistview.listview(hDlg, szDBconn);
		//MessageBox(NULL, "������ ���̽��� ������ �� �����ϴ�", "����", MB_OK);
		dbstat = -1;
		return FALSE;
	}
	return TRUE;
}
// hWnd �����츦 ȭ�� �߾����� ������.
void CenterWindow(HWND hWnd)
{
	RECT rcTemp1, rcTemp2 ;
	LONG iX, iY, iWidth, iHeight ;

	GetWindowRect(GetDesktopWindow(),&rcTemp1) ;
	GetWindowRect(hWnd,&rcTemp2) ;
	iWidth = rcTemp2.right - rcTemp2.left ;
	iHeight = rcTemp2.bottom - rcTemp2.top ;
	iX = LONG((rcTemp1.right - iWidth) / 2) ;
	iY = LONG((rcTemp1.bottom - iHeight) / 2) ;
	//MoveWindow(hWnd,iX,iY,iWidth,iHeight,TRUE) ;
}

//����ū�Ҽ��� �˻��Ͽ� ����Ʈ �ڽ��� ���
int select_max_prime()
{
	TCHAR szMax[256];
	TCHAR maxP[128];
	int maxPrime = 0;
	wsprintf(szMax, TEXT("select max(prime) from prime"));
	maxPrime = qry.ExecGetInt(szMax);
	wsprintf(maxP, TEXT("%d"), maxPrime);
	//_itow_s(i, maxP, 10);
	SetWindowText(hMaxPrime, maxP);
	return maxPrime;
}
//����� ���� ����ū�Ҽ� �˻�
int select_max_prime1()
{
	TCHAR szMaxPrime[256];
	//TCHAR maxP[128];
	int maxPrime = 0;
	wsprintf(szMaxPrime, TEXT("select max(prime) from prime"));
	maxPrime = qry.ExecGetInt(szMaxPrime);
	//wsprintf(maxP, TEXT("%d"), maxPrime);

	//_itow_s(i, maxP, 10);
	//SetWindowText(hMaxPrime, maxP);
	return maxPrime;
}
//�Ҽ������� ū���� �˻�
int select_term_prime(int select)
{
	//select 1 -> �Ҽ������� ū���� �˻��Ͽ� ����Ʈ �ڽ��� ���
	//select 2 -> �Ҽ������� ū���� �˻��Ͽ� ����
	TCHAR szTerm[256];
	TCHAR primeTerm[128];
	int maxPrime = 0;
	int maxterm = 0;
	switch (select){
	case 1:
		wsprintf(szTerm, TEXT("SELECT MAX(term_prime) from prime"));
		maxPrime = qry.ExecGetInt(szTerm);
		wsprintf(primeTerm, TEXT("%d"), maxPrime);
		//_itow_s(i, maxP, 10);
		SetWindowText(hPrimeTerm, primeTerm);
		return 0;
	case 2:
		wsprintf(szTerm, TEXT("SELECT MAX(term_prime) from prime"));
		maxterm = qry.ExecGetInt(szTerm);
		//wsprintf(primeTerm, TEXT("%d"), maxPrime);
		//_itow_s(i, maxP, 10);
		//SetWindowText(hPrimeTerm, primeTerm);
		return maxterm;
	}
	return 0;
}
//�Էµ� �Ҽ� ���� ���
BOOL select_max_count()
{
	TCHAR szMaxCount[256];
	TCHAR primecount[128];
	int count = 0;
	wsprintf(szMaxCount, TEXT("SELECT count(term_prime) from prime"));
	count = qry.ExecGetInt(szMaxCount);
	wsprintf(primecount, TEXT("%d"), count);
	//_itow_s(i, maxP, 10);
	SetWindowText(hCount, primecount);
	return TRUE;
}

//�Ҽ��Ǻ� �Լ�
BOOL isPrime(unsigned int n)
{
	//printf("isPrime start \n");
	unsigned int i = 2;
	unsigned int k = 0;
	int Prime = 0, check = 0;
	k = n / 2;
	if (n % 2 != 0)//Ȧ���� ��츸 ����
	{
		for (i; i <= k; i++){
			if (n%i == 0){
				check++;//check �� 1�̻��̸� ¦���� ����
				// printf("prime check = %d \n",check);
				// printf("                      check=%d\n",check);			 
			}
			//printf("Random_p_q = %d i=%d \n",n,i);
		}
		//check �� 0�̿��߸� �Ҽ���
		if (check == 0){
			//printf("prime ok=%d \n",n);
			return true;
		}
		else if (check >= 1){
			//printf("prime false \n");
			check = 0, i = 2;
		}
	}
	return false;
}
//�Ҽ��� �����ͺ��̽��� �����Լ�
BOOL insert_query_prime(int prime, int term_prime)
{
	TCHAR szQuery[256];
	wsprintf(szQuery, TEXT("insert into prime values('%d','%d')"), prime, term_prime);
	//MessageBox(NULL, (LPCTSTR)szTmp, TEXT("�˸�"), MB_OK);
	qry.Exec(szQuery);
	
	return TRUE;
}
//�Ҽ� ��� �ð� ���
BOOL primetemtime(LARGE_INTEGER st, LARGE_INTEGER ed, LARGE_INTEGER freq)
{
	double gap;
	TCHAR szTerTime[256];
	gap = ((double)(ed.QuadPart - st.QuadPart)) / ((double)freq.QuadPart);
	sprintf_s(szTerTime, TEXT("%.4f"), gap);
	SetWindowText(hPrimeTermTime, szTerTime);
	return TRUE;
}
//�Ҽ��� ���������� ī��Ʈ�Ͽ� ������ ���̽��� �����ϴ� �Լ�
void prime_generation_db_insert(HWND hDlg)
{
	int p_start, p_end;
	int first_prime = 0;
	int prime = 0, term_prime = 0, term_max = 0;
	int i,progrescnn=0;
	TCHAR szCount[256];
	TCHAR szTermCount[128];
	int count = 0;
	TCHAR maxprime2[256];
	LARGE_INTEGER st, ed, freq;

	p_start = select_max_prime1() + 1;
	GetWindowText(hMaxPrime2, maxprime2, 256);
	p_end = atoi(maxprime2);
	term_max = select_term_prime(2); 
	if (p_end <= p_start)
	{
		MessageBox(NULL, TEXT("���۰��� �۽��ϴ�"), TEXT("�˸�"), MB_OK);
		return;
	}
	//p_end = 999999999;
	first_prime = select_max_prime();//����ū���� ù��° ������ ����
	//���α׷��� 
	//SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 500));
	SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, p_end-p_start));
	for (i = p_start; i<p_end; i++)
	{
		QueryPerformanceCounter(&st); //������ �ð�
		//���� 1�̸� ������ ��������
		if (threadstatus == 1){
			MessageBox(NULL, TEXT("�Ҽ��Է� ������ ����"), TEXT("�˸�"), MB_OK);
			threadstatus = 0;
			return;
		}
		
		progrescnn++;//���α׷����� ����
		///////////////////////////////
		SendMessage(hProgress, PBM_SETPOS, progrescnn, 0);
		//�Ҽ��� �Ǻ��Ͽ� �����ͺ��̽��� ����
		if ( isPrime(i)==TRUE)
		{
			prime = i;
			term_prime = prime - first_prime;
			if (term_prime > term_max)
			{
				sprintf_s(szTermCount, TEXT("%d ���� ū�� %d �� ����"),term_max,term_prime);
				MessageBox(NULL, szTermCount, TEXT("�˸�"), MB_OK);
				return;
			}
			insert_query_prime(prime,term_prime);

			//wsprintf(str, TEXT("insert secces primeNumber = %d"),prime);
			//primelistview.listview(hDlg, str);
			count++;
			_ultoa_s(count, szCount, 10);//���ڸ� ���ڷ� ��ȯ
			SetWindowText(hPrimeCount, szCount);//ī��Ʈ�� �ڽ��� ���
			//wsprintf(str, TEXT("insert secces primeNumber = %d"), count);
			//primelistview.listview(hDlg, str);
			
			//printf("calculation  time=%lf\n", (double)(finish - start) / CLOCKS_PER_SEC);
			first_prime = prime;
			//show_time();//����ð� ���
			//MessageBox(NULL, TEXT("�Է¼���"), TEXT("�˸�"), MB_OK);
				
			QueryPerformanceCounter(&ed);//������� �ð�
			QueryPerformanceFrequency(&freq);//���ļ� ���
			primetemtime(st, ed, freq);//�ð��� ��� �Լ�ȣ��
		}
		
	}
	progrescnn = 0;
}

//�����带 �����Ͽ� �Ҽ� �Է�
DWORD WINAPI ThreadFuncPrime(LPVOID prime)
{
	TCHAR szPrimeInsert[256];

	if (dbstat == -1)
	{
		MessageBox(NULL, TEXT("�����ͺ��̽� ����ȵ�"), TEXT("�˸�"), MB_OK);
		return 0;
	}

	wsprintf(szPrimeInsert, TEXT("�Ҽ��Է� ������ ����"));
	primelistview.listview(hDlgMain, szPrimeInsert);

	prime_generation_db_insert(NULL);

	wsprintf(szPrimeInsert, TEXT("�Ҽ��Է� ������ ����"));
	primelistview.listview(hDlgMain, szPrimeInsert);
	return 0;
}
//�����ͺ��̽� ���� üũ ������
DWORD WINAPI ThreadFuncDBstat(LPVOID dbstattmp)
{
	TCHAR szStat[256];
	wsprintf(szStat, TEXT("������ ���̽� ���� üũ ������ ����"));
	primelistview.listview(hDlgMain, szStat);
	
	while (1)
	{
		if (qry.StatusDB() == TRUE)
		{
			//wsprintf(szTmp, TEXT("������ ���̽�����: �����"));
			//primelistview.listview(hDlgMain, szTmp);
			
		}
		else
		{
			wsprintf(szStat, TEXT("������ ���̽�����: ����ȵ�"));
			primelistview.listview(hDlgMain, szStat);
			if (threadstatus == 0)
			{
				threadstatus = 1;
				wsprintf(szStat, TEXT("�Ҽ��Է� ������ ������"));
				primelistview.listview(hDlgMain, szStat);
			}
			dbstat = -1;
		}
		Sleep(5000);
	}
	return 0;
}
//�����ͺ��̽� ���� ������
DWORD WINAPI ThreadFuncDBconn(LPVOID dbconn)
{
	TCHAR szConn[256];
	wsprintf(szConn, TEXT("������ ���̽� ���� ������ ����"));
	primelistview.listview(hDlgMain, szConn);
	while (1)
	{
		if (dbstat == -1)
		{
			wsprintf(szConn, TEXT("������ ���̽� ���ӽõ�"));
			primelistview.listview(hDlgMain, szConn);
			if (DBCONN(NULL) == TRUE)
			{
				wsprintf(szConn, TEXT("������ ���̽� �����"));
				primelistview.listview(hDlgMain, szConn);
			}
		}
		Sleep(5000);
		//ExitThread(1);
	}
	return 0;
}
BOOL CALLBACK MainDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{

	DWORD ThreadID;

	switch (iMessage) {
	case WM_INITDIALOG:
		
		hDlgMain = hDlg;
		hList=GetDlgItem(hDlgMain,IDC_LISTCIGAR);
		hFName=GetDlgItem(hDlgMain,IDC_FNAME);
		hFPrice=GetDlgItem(hDlgMain,IDC_FPRICE);
		hFKorean=GetDlgItem(hDlgMain,IDC_FKOREAN);
		hPrimeList = GetDlgItem(hDlgMain, IDC_PRIME_LIST);
		hMaxPrime = GetDlgItem(hDlgMain, IDC_MAX_PRIME);
		hMaxPrime2 = GetDlgItem(hDlgMain, IDC_MAX_PRIME2);
		hPrimeCount = GetDlgItem(hDlgMain, IDC_PRIME_COUNT);
		hPrimeTerm = GetDlgItem(hDlgMain, IDC_PRIME_TERM_EDIT);
		hPrimeTermTime = GetDlgItem(hDlgMain, IDC_PRIME_TERM_TIME);
		hProgress = GetDlgItem(hDlgMain, IDC_PROGRESS);
		hCount = GetDlgItem(hDlgMain, IDC_COUNT);
		CenterWindow(hDlg);
		DBCONN(hDlg);
		hThreadDBstat = CreateThread(NULL, 0, ThreadFuncDBstat, NULL, 0, &ThreadID);
		hThreadDBconn = CreateThread(NULL, 0, ThreadFuncDBconn, NULL, 0, &ThreadID);
		
		//DBExecuteSQL();
		return TRUE;
	case WM_CREATE:
		//InitCommonControls();
		//hStat = CreateStatusWindow(WS_CHILD | WS_VISIBLE, "dldld", hDlg, 0);
		return TRUE;
	case WM_TIMER:
		switch (wParam){
		case 1:
			primelistview.sTime();
			break;
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
		case IDCANCEL:
			threadstatus = 1;
			EndDialog(hDlgMain,IDOK);
			break;
		case IDC_BTNADD:
			
			break;
		case IDC_BTNDEL:
			
			break;
		case IDC_BTNUPDATE:
			//hThreadDBstat = CreateThread(NULL, 0, ThreadFuncDBstat, NULL, 0, &ThreadID);
			//wsprintf(maxP, TEXT("%d"), maxPrime);
			//SetWindowText(hMaxPrime, maxP);
			//UpdateRecord();
			break;
		case IDC_PRIME_MAX:
			select_max_prime();
			break;
		case IDC_PRIME_INSERT:
			//SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 1000));
			hThreadPrime = CreateThread(NULL, 0, ThreadFuncPrime, NULL, 0, &ThreadID);
			

			//prime_generation_db_insert(hDlg,NULL, NULL);
			break;
		case IDC_STOP:
			threadstatus = 1;
			/*
			TerminateThread(hThread, 0);
			MessageBox(NULL, TEXT("������ ��������"), TEXT("�˸�"), MB_OK);
			CloseHandle(hThread);		
			/*
			result = GetExitCodeThread(hThread, &code);
			code1 = code;
			if (result != 0) {
				if (code1 == STILL_ACTIVE) {
					TerminateThread(hThread, 0);
					MessageBox(NULL, TEXT("�Ҽ��Է� ������ ����"), TEXT("�˸�"), MB_OK);
					CloseHandle(hThread);
				}
			}*/
			break;
		case IDC_PRIME_TERM:
			select_term_prime(1);
			break;
		case IDC_BUTTON_COUNT:
			select_max_count();
			break;
		}
		return FALSE;
	}
	return FALSE;
}
