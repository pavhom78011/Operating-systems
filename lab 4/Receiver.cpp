#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <fstream>
#include <string>
using namespace std;

HANDLE* readyEvents;
HANDLE* finishedEvents;
HANDLE mutex = CreateMutex(NULL, NULL, L"syncMutex");
HANDLE waitIfEmpty = CreateEvent(NULL, TRUE, FALSE, L"empty");
HANDLE waitToWrite = CreateEvent(NULL, TRUE, FALSE, L"start");
HANDLE AllFinished;

int main()
{
	char appName[] = "Sender.exe";
	char Path[300];
	GetModuleFileNameA(NULL, Path, MAX_PATH);
	string directory(Path);
	directory = directory.substr(0, directory.find_last_of("\\/"));
	SetCurrentDirectoryA(directory.c_str());
	int num{}, numofsenders{};
	char name[1000]{};
	cout << "Enter the name of the binary file:";
	cin >> name;
	cout << "Enter the number of entries in this file:";
	cin >> num;
	while (num <= 0)
	{
		cout << "enter not zero: ";
		cin >> num;
	}
	string strname(name);
	ofstream ofst(strname + ".bin", ios::binary);
	cout << "number of Sender processes:";
	cin >> numofsenders;
	while (numofsenders <= 0)
	{
		cout << "enter not zero: ";
		cin >> numofsenders;
	}
	AllFinished = CreateSemaphore(NULL, num * 2, num * 2, L"allFinished");
	readyEvents = new HANDLE[numofsenders];
	finishedEvents = new HANDLE[numofsenders];
	for (int i = 0; i < numofsenders; ++i)
	{
		string eventName = "event_" + to_string(i);
		readyEvents[i] = CreateEvent(NULL, TRUE, FALSE, (LP)eventName.c_str());
		string ename = to_string(i);
		finishedEvents[i] = CreateEvent(NULL, TRUE, FALSE, (LP)ename.c_str());
		if (readyEvents[i] == NULL)
		{
			return GetLastError();
		}
	}
	STARTUPINFOA* startup = new STARTUPINFOA[numofsenders];
	PROCESS_INFORMATION* info = new PROCESS_INFORMATION[numofsenders];
	for (int i{}; i < numofsenders; ++i)
	{
		ZeroMemory(&startup[i], sizeof(STARTUPINFOA));
		startup[i].cb = sizeof(STARTUPINFOA);
		char line[500]{};
		sprintf_s(line, "%s %s %d", appName, name, i);
		if (!CreateProcessA(NULL, line, NULL, NULL, FALSE, FALSE, NULL, NULL, &startup[i], &info[i]))
		{
			return GetLastError();
		}
	}
	WaitForMultipleObjects(numofsenders, readyEvents, TRUE, INFINITE);
	for (int i{}; i < numofsenders; ++i)
	{
		ResetEvent(readyEvents[i]);
	}
	SetEvent(waitToWrite);
	for (int i{};;)
	{
		WaitForSingleObject(waitIfEmpty, INFINITE);
		WaitForSingleObject(mutex, INFINITE);
		cout << "read message from binary file or complete work(1 or 0):";
		cin >> i;
		while (i != 0 && i != 1)
		{
			cout << "enter 1 or 0:" << endl;
			cin >> i;
		}
		if (i)
		{
			cout << "binary file:" << endl;
			ifstream ifs(strname + ".bin");
			while (!ifs.eof())
			{
				string str;
				ifs >> str;
				cout << str << endl;
			}
			cout << "clear file or not(1 or 0): ";
			int t;
			cin >> t;
			if (t)
			{
				std::ofstream ofs(strname + ".bin", std::ofstream::out | std::ofstream::trunc);
				ofs.close();
				while (ReleaseSemaphore(AllFinished, 1, NULL)) 
				{

				}
				PulseEvent(waitToWrite);
			}
			for (int i{}, k{}; i < numofsenders; ++i)
			{
				if (WaitForSingleObject(finishedEvents[i], 0) == WAIT_OBJECT_0)
				{
					++k;
				}
				if (k == numofsenders)
				{
					return 0;
				}
			}
			ReleaseMutex(mutex);
		}
		else
		{
			for (int i{}; i < numofsenders; ++i)
			{
				TerminateProcess(info[i].hProcess, 0);
			}
			ReleaseMutex(mutex);
			break;
		}
		for (int i{}; i < numofsenders; ++i)
		{
			PulseEvent(readyEvents[i]);
		}
	}
	for (int i{}; i < numofsenders; ++i)
	{
		CloseHandle(info[i].hThread);
		CloseHandle(info[i].hProcess);
		CloseHandle(readyEvents[i]);
		CloseHandle(finishedEvents[i]);
	}
	CloseHandle(mutex);
	CloseHandle(AllFinished);
	CloseHandle(waitToWrite);
	CloseHandle(waitIfEmpty);
	return 0;
}