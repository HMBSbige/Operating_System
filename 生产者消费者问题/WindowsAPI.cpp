#include <iostream>
#include <iomanip>
#include <windows.h>
#include <new>
using namespace std;
#define N 20
#define nThread 4
//#define DEBUG
HANDLE Empty, full, mutex;
int x = 0;
int y = 0;
int *buffer;

void output(){
	register int i;
	for (i = 0; i<N; ++i){
		if(i!=0)
			cout<< " ";
		cout << setw(3)<<buffer[i];
	}
	cout << endl;
}
DWORD WINAPI produce(LPVOID param)
{
	
	for (int j = 0;j < ceil((double)N/ (double)nThread); ++j){
		WaitForSingleObject(Empty, INFINITE);
		WaitForSingleObject(mutex, INFINITE);
		cout <<"生产者ID:" << " " << GetCurrentThreadId() << "\t " << j << "\t";
		buffer[y++%N] = y +1;
		output();
		ReleaseSemaphore(mutex, 1, NULL);
		ReleaseSemaphore(full, 1, NULL);
	}
	return 0;
}
DWORD WINAPI consume(LPVOID param)
{
	for (int j = 0; j < ceil((double)N / (double)nThread); ++j){
		WaitForSingleObject(full, INFINITE);
		WaitForSingleObject(mutex, INFINITE);
		cout << "消费者ID:" << " " <<GetCurrentThreadId() << "\t " << j << "\t";
		buffer[x++%N] = 0;
		output();
		ReleaseSemaphore(mutex, 1, NULL);
		ReleaseSemaphore(Empty, 1, NULL);		
	} 
	return 0;
}
int main()
{
	int i;
	Empty = CreateSemaphore(NULL, N, N, L"空计数信号量");
	full = CreateSemaphore(NULL, 0, N, L"满计数信号量");
	mutex = CreateSemaphore(NULL, 1, 1, L"互斥信号量");

	buffer = new int[N];
	for (i=0;i<N;++i){
		buffer[i] = 0;
	}

	DWORD *ThreadId = new DWORD[2*nThread];
	HANDLE* ThreadHandle = new HANDLE[2*nThread];

	for (i=0;i<nThread ;++i){
		ThreadHandle[i + nThread ] = CreateThread(NULL, 0, produce, NULL, 0, &ThreadId[i + nThread]);
		ThreadHandle[i] = CreateThread(NULL, 0, consume, NULL, 0, &ThreadId[i]);
	}

	WaitForMultipleObjects(2*nThread, ThreadHandle, TRUE, INFINITE);
#ifdef DEBUG
	system("pause");
#endif // DEBUG
	return 0;
}