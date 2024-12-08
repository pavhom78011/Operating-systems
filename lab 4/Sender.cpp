#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <fstream>
#include <string>
using namespace std;

HANDLE readyEvent, mutex, waitIfEmpty, waitToWrite, AllFinished, finishedEvent;

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		return 0;
	}
	int index = atoi(argv[2]);
	string eventName = "event_" + to_string(index);
	string namebin = argv[1];
	readyEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, (LP)eventName.c_str());
	string ename = to_string(index);
	finishedEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, (LP)ename.c_str());
	waitToWrite = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"start");
	waitIfEmpty = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"empty");
	mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"syncMutex");
	AllFinished = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, L"allFinished");
	if (readyEvent == NULL)
	{
		return GetLastError();
	}
	SetEvent(readyEvent);
	ofstream ofst(namebin + ".bin", ios::app | ios::binary);
	WaitForSingleObject(waitToWrite, INFINITE);
	for (int i{};;)
	{
		WaitForSingleObject(mutex, INFINITE);
		cout << "send a message to the Receiver process or stop working(1 or 0):" << endl;
		cin >> i;
		while (i != 0 && i != 1)
		{
			cout << "enter 1 or 0:" << endl;
			cin >> i;
		}
		if (i == 1)
		{
			if (WaitForSingleObject(AllFinished, 0) == WAIT_OBJECT_0)
			{
				cout << "write a message(less than 20 characters):" << endl;
				string str;
				cin >> str;
				while (str.length() >= 20)
				{
					cout << "rewrite:" << endl;
					cin >> str;
				}
				ofst << str << endl;
				WaitForSingleObject(AllFinished, INFINITE);
			}
			else if (WaitForSingleObject(AllFinished, 0) == WAIT_TIMEOUT)
			{
				ResetEvent(waitToWrite);
				cout << "limit" << endl;
				SetEvent(finishedEvent);
				PulseEvent(waitIfEmpty);
				ReleaseMutex(mutex);
				WaitForSingleObject(waitToWrite, INFINITE);
				ResetEvent(finishedEvent);
			}
			PulseEvent(waitIfEmpty);
			ReleaseMutex(mutex);
		}
		else if (i == 0)
		{	
			SetEvent(finishedEvent);
			PulseEvent(waitIfEmpty);
			ReleaseMutex(mutex);
			break;
		}
		WaitForSingleObject(readyEvent, INFINITE);
	}
	CloseHandle(readyEvent);
	CloseHandle(mutex);
	CloseHandle(waitIfEmpty);
	CloseHandle(AllFinished);
	CloseHandle(finishedEvent);
	CloseHandle(waitToWrite);
}