#include <Windows.h>
#include <iostream>
#include <conio.h>
#include "Prime.h"
#include "CQuery.h"

#define ESC 27

using namespace std;

CQuery qry1;

//������ �ʱ�ȭ
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
//�Ҽ��Ǻ� �Լ�
bool PRIME::isPrime(unsigned __int64 n)
{
    //printf("isPrime start \n");
    unsigned __int64 i=2;
	unsigned __int64 k=0;
    int Prime=0,check=0;
	k=n/2;
	if(n % 2 != 0)//Ȧ���� ��츸 ����
	{
			for(i; i<=k; i++){
				if(n%i==0){
                 check++;//check �� 1�̻��̸� ¦���� ����
				// printf("prime check = %d \n",check);
                // printf("                      check=%d\n",check);			 
				}
            //printf("Random_p_q = %d i=%d \n",n,i);
			}
			//check �� 0�̿��߸� �Ҽ���
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

//�Է¹����� �Ҽ��� ���ϴ� �Լ�
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

//�Էµ� ���� ���μ� �����ϴ� �Լ�
void PRIME::getPrime()
{
	cout<<"������ ���μ� �Է� =";
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
			cout<<"�Ҽ��� �μ�="<<Prime[i]<<endl;
			X=X*Prime[i];
			
			cout<<X<<"*"<<Prime[i]<<endl;
			i++;
			if(X==0)
			{
				cout<<"�����߻�"<<endl;
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
		count=0;//������� ���� ī��Ʈ �ʱ�ȭ	
}
//����ð��� ���
void PRIME::show_time()
{
	char date[64], time[64]; 
     //_strdate(date); 
     //_strtime(time); 
     printf("%s %s\n", date, time); 
} 

//�Ҽ��� ���������� ī��Ʈ�Ͽ� ������ ���̽��� �����ϴ� �Լ�
void PRIME::prime_generation_db_insert()
{
	CQuery query;
    //���� ���� �Է�
	while(1)
	{
		//����ū�� ��ȸ�Ͽ� 1 ���ϱ�
	    p_start = select_max_prime()+1;
		
	    //cin >> p_start;
	    cout << "p_end = ";
	    cin >> p_end; 
		if(p_end<=p_start)
		{
			cout<<"����� ������ �۽��ϴ�"<<endl;
		}
		else{
			break;
		}
	}
	first_prime=select_max_prime();//����ū���� ù��° ������ ����
	for(i=p_start; i<p_end ; i++){
		//ESC�� �ԷµǸ� ��������
		if(_kbhit())
		{
			ch=_getch();
			if(ch==ESC)
			{
				return;
			}
		}
		////////////////////////////////
		start = clock();//������ �ð�
		//�Ҽ��� �Ǻ��Ͽ� �����ͺ��̽��� ����
		if(true==isPrime(i)){
			prime=i;
            term_prime = prime - first_prime;
			//query.insert_query_prime(prime,term_prime);
			cout << "insert secces primeNumber = " << prime << endl;
			count++;
			cout << "count = " << count << endl;

			finish = clock();//������� �ð�
			printf("calculation  time=%lf\n",(double)(finish - start) / CLOCKS_PER_SEC);
			first_prime = prime;
			show_time();//����ð� ���

		}
	}
}

*/