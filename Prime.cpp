#include <Windows.h>
#include <iostream>
#include <conio.h>
#include "Prime.h"
#include "CQuery.h"

#define ESC 27

using namespace std;

CQuery qry1;

//생성자 초기화
PRIME::PRIME()
{
	/*
	count=0;
	PrimeNumber=0;
	X=0;
	first_prime=0;
	term_prime=0;
	*/
}

/*
//소수판별 함수
bool PRIME::isPrime(unsigned __int64 n)
{
    //printf("isPrime start \n");
    unsigned __int64 i=2;
	unsigned __int64 k=0;
    int Prime=0,check=0;
	k=n/2;
	if(n % 2 != 0)//홀수일 경우만 진행
	{
			for(i; i<=k; i++){
				if(n%i==0){
                 check++;//check 가 1이상이면 짝수기 때문
				// printf("prime check = %d \n",check);
                // printf("                      check=%d\n",check);			 
				}
            //printf("Random_p_q = %d i=%d \n",n,i);
			}
			//check 가 0이여야만 소수임
			if(check==0){
				//printf("prime ok=%d \n",n);
				return true;
			}
			else if(check>=1){
            //printf("prime false \n");
            check=0,i=2;
			}
	}
	return false;
}

//입력범위의 소수를 구하는 함수
void PRIME::prime_generation()
{
	//cout << a * a * a * a * a * a * a << endl;
	//int i=0,j=0,k=0,p_start=0, p_end=0;

	cout << "p_start?";
	cin >> p_start;
	cout << "p_end";
	cin >> p_end; 
	for(i=p_start; i<p_end ; i++){
		if(true==isPrime(i)){
			//cout<< i << endl;
			//printf("prime = %d \n",i);
			prime_array[j]=i;
			j++;
		}
	}
	
	for(k=0; k<=j; k++){
		cout<< prime_array[k] << endl;
		//printf("prime %d \n",prime_array[k]);
	}

	cout << j << endl;
}

//입력된 수를 소인수 분해하는 함수
void PRIME::getPrime()
{
	cout<<"분해할 소인수 입력 =";
	cin>>FirstInputNumber;
	InputNumber=FirstInputNumber;

	
	    
		for(i=2;i<=InputNumber;i++)
		{
			cout<<"i="<<i<<endl;
			if(InputNumber%i==0)
			{
				Prime[PrimeNumber]=i;
				PrimeNumber++;
				InputNumber=InputNumber/i;
				i=1;
				count++;
				cout<<"count="<<count<<endl;
				cout<<"PrimeNumber="<<Prime[PrimeNumber-1]<<endl;
			}
		}
		X=1;
		i=0;
		cout<<"prime print"<<endl;
		while(FirstInputNumber!=X)
		{
			//printf("%d ",Prime[i]);
			//Sleep(2000);
			cout<<"X="<<X<<endl;
			cout<<"i="<<i<<endl;
			cout<<"소수의 인수="<<Prime[i]<<endl;
			X=X*Prime[i];
			
			cout<<X<<"*"<<Prime[i]<<endl;
			i++;
			if(X==0)
			{
				cout<<"문제발생"<<endl;
				cout<<"x="<<X<<endl;
				cout<<"prime="<<Prime[0]<<endl;
				cout<<"prime="<<Prime[1]<<endl;
				cout<<"prime="<<Prime[2]<<endl;
				cout<<"prime="<<Prime[3]<<endl;
				cout<<"prime="<<Prime[4]<<endl;
				cout<<"prime="<<Prime[5]<<endl;
				cout<<"prime="<<Prime[6]<<endl;
				cout<<"prime="<<Prime[7]<<endl;
				return;
			}
		}
		count=0;//재실행을 위한 카운트 초기화	
}
//현재시간을 출력
void PRIME::show_time()
{
	char date[64], time[64]; 
     //_strdate(date); 
     //_strtime(time); 
     printf("%s %s\n", date, time); 
} 

//소수를 순차적으로 카운트하여 데이터 베이스에 저장하는 함수
void PRIME::prime_generation_db_insert()
{
	CQuery query;
    //숫자 범위 입력
	while(1)
	{
		//가장큰값 조회하여 1 더하기
	    p_start = select_max_prime()+1;
		
	    //cin >> p_start;
	    cout << "p_end = ";
	    cin >> p_end; 
		if(p_end<=p_start)
		{
			cout<<"저장된 수보다 작습니다"<<endl;
		}
		else{
			break;
		}
	}
	first_prime=select_max_prime();//가장큰수를 첫번째 변수에 저장
	for(i=p_start; i<p_end ; i++){
		//ESC가 입력되면 루프종료
		if(_kbhit())
		{
			ch=_getch();
			if(ch==ESC)
			{
				return;
			}
		}
		////////////////////////////////
		start = clock();//계산시작 시간
		//소수를 판별하여 데이터베이스에 저장
		if(true==isPrime(i)){
			prime=i;
            term_prime = prime - first_prime;
			//query.insert_query_prime(prime,term_prime);
			cout << "insert secces primeNumber = " << prime << endl;
			count++;
			cout << "count = " << count << endl;

			finish = clock();//계산종료 시간
			printf("calculation  time=%lf\n",(double)(finish - start) / CLOCKS_PER_SEC);
			first_prime = prime;
			show_time();//현재시간 출력

		}
	}
}

*/