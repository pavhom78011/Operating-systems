#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <fstream>
#include <string>
#include "Employee.h"
using namespace std;
static void PrintBinaryFile(string name2)
{
	ifstream ifst(name2 + ".bin", ios::binary);
	if (ifst.is_open())
	{
		cout << "Contents of the binary file:\n";
		string str;
		while (!ifst.eof())
		{
			getline(ifst, str);
			cout << str << endl;
		}
		ifst.clear();
		ifst.seekg(0, ios::beg);
		employee a{};
		cout << "Contents of the binary file without krakozyabr:\n";
		while (ifst.read(reinterpret_cast<char*>(&a), sizeof(employee)))
		{
			cout << "Num:" << a.num << " Name:" << a.name << " Hours:" << a.hours << endl;
		}
	}
	else
	{
		cout << "Error opening binary file\n";
		return;
	}
	ifst.close();
}
static void PrintTextFile(string name2)
{
	setlocale(LC_ALL, "rus");
	ifstream ifst(name2 + ".txt");
	string str;
	if (ifst.is_open())
	{
		while (!ifst.eof())
		{
			getline(ifst, str);
			cout << str << endl;
		}
	}
}
int main()
{
	char Path[300];
	GetModuleFileNameA(NULL, Path, MAX_PATH);
	string directory(Path);
	directory = directory.substr(0, directory.find_last_of("\\/"));
	SetCurrentDirectoryA(directory.c_str());
	int num{};
	char name[1000]{};
	cout << "Enter the name of the binary file:";
	cin >> name;
	cout << "Enter the number of entries in this file:";
	cin >> num;
	while(cin.fail())
	{
		cin.clear();
		cin.ignore();
		cout << "Please enter a number:";
		cin >> num;
	}
	char AppName1[] = "Creator.exe";
	char line[500]{};
	sprintf_s(line, "\"%s\" %d %s", AppName1, num, name);
	STARTUPINFOA startup;
	PROCESS_INFORMATION info{};
	ZeroMemory(&startup, sizeof(STARTUPINFOA));
	startup.cb = sizeof(STARTUPINFOA);
	if (!CreateProcessA(NULL, line, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startup, &info))
	{
		_cputs("Process not created!");
		_getch();
		return 0;
	}
	WaitForSingleObject(info.hProcess, INFINITE);
	CloseHandle(info.hThread);
	CloseHandle(info.hProcess);
	PrintBinaryFile(name);

	char name2[1000]{};
	int payment{};
	cout << "Report file name:";
	cin >> name2;
	cout << "Payment per hour of work:";
	cin >> payment;
	while (cin.fail())
	{
		cin.clear();
		cin.ignore();
		cout << "Please enter a payment(number):";
		cin >> payment;
	}
	char AppName2[] = "Reporter.exe";
	char line2[500]{};
	sprintf_s(line2, "%s %s %d", name, name2, payment);
	STARTUPINFOA startup2;
	PROCESS_INFORMATION info2{};
	ZeroMemory(&startup2, sizeof(STARTUPINFOA));
	startup2.cb = sizeof(STARTUPINFOA);
	if (!CreateProcessA(AppName2, line2, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startup2, &info2))
	{
		DWORD error = GetLastError();
		cout << "Process not created! Error code: " << error << endl;
		_getch();
		return 0;
	}
	WaitForSingleObject(info2.hProcess, INFINITE);
	CloseHandle(info2.hThread);
	CloseHandle(info2.hProcess);
	cout << "Report:" << '\n';
	PrintTextFile(name2);
	_getch();
}