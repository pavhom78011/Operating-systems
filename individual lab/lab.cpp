#include <iostream>
#include <Windows.h>
#include <random>
#include <vector>
#include <queue>
#include <map>
using namespace std;

int numofwaiters{}, numofcooks{}, numofvisitors{}, servedVisitors{}, currentOrder{}, arrivedVisitors{};
queue<int> orders, arrivedvisitors, cookeddishes, availableWaiters;
multimap<int, int> cookers, waitersorders, waiterdelievers;
HANDLE Hsemaphore;
HANDLE Hmutex = CreateMutex(NULL, FALSE, NULL);
HANDLE orderEvent, cookEvent;
HANDLE* Hcooks, * Hwaiters, * Hvisitors;
HANDLE* Heventsvisit;

static DWORD WINAPI cooker(LPVOID num)
{
	int id = *(int*)num;
	srand(id);
	WaitForSingleObject(cookEvent, INFINITE);
	for (;;)
	{
		WaitForSingleObject(Hmutex, INFINITE);
		if (orders.empty() && !(servedVisitors >= numofvisitors))
		{
			ReleaseMutex(Hmutex);
			continue;
		}
		if (!orders.empty())
		{
			int order = orders.front();
			orders.pop();
			ReleaseMutex(Hmutex);
			Sleep(rand() % 100 + 500);
			WaitForSingleObject(Hmutex, INFINITE);
			cout << "Cook " << id + 1 << " prepared order " << order << endl;
			pair<int, int> p(id + 1, order);
			cookers.insert(p);
			cookeddishes.push(order);
		}
		ReleaseMutex(Hmutex);
		SetEvent(cookEvent);
		WaitForSingleObject(Hmutex, INFINITE);
		if (servedVisitors >= numofvisitors)
		{
			ReleaseMutex(Hmutex);
			break;
		}
		ReleaseMutex(Hmutex);
	}
	return 0;
}

static DWORD WINAPI waiter(LPVOID num)
{
	int id = *(int*)num;
	srand(id);
	WaitForSingleObject(orderEvent, INFINITE);
	for (;;)
	{
		WaitForSingleObject(Hmutex, INFINITE);
		if (arrivedVisitors < numofvisitors / 1.5)
		{ 
			ReleaseMutex(Hmutex); 
			continue; 
		}
		if (!arrivedvisitors.empty() && !availableWaiters.empty() && availableWaiters.front() == (id + 1))
		{
			int order = ++currentOrder;
			orders.push(order);
			arrivedvisitors.pop();
			Sleep(rand() % 100 + 300);
			cout << "Waiter " << id + 1 << " took order " << order << endl;
			availableWaiters.push(id + 1);
			availableWaiters.pop();
			pair<int, int> p(id + 1, order);
			waitersorders.insert(p);
			SetEvent(cookEvent);
		}
		if (!cookeddishes.empty())
		{
			int deliever = cookeddishes.front();
			cookeddishes.pop();
			Sleep(rand() % 100 + 100);
			cout << "Waiter " << id + 1 << " delievered order " << deliever << endl;
			pair<int, int> p(id + 1, deliever);
			waiterdelievers.insert(p);
			++servedVisitors;
		}
		if (servedVisitors >= numofvisitors)
		{
			ReleaseMutex(Hmutex);
			break;
		}
		ReleaseMutex(Hmutex);
	}
	return 0;
}

static DWORD WINAPI visitor(LPVOID num)
{
	int id = *(int*)num;
	srand(id);
	WaitForSingleObject(Heventsvisit[id], INFINITE);
	WaitForSingleObject(Hmutex, INFINITE);
	Sleep(100);
	cout << "the guest " << id + 1 << " has arrived" << endl;
	++arrivedVisitors;
	arrivedvisitors.push(id + 1);
	ReleaseSemaphore(Hsemaphore, 1, NULL);
	if (id + 1 <= numofvisitors)
	{
		SetEvent(Heventsvisit[id + 1]);
		SetEvent(orderEvent);
	}
	ReleaseMutex(Hmutex);
	for (;;)
	{
		WaitForSingleObject(Hmutex, INFINITE);
		for (auto pair : waiterdelievers)
		{
			if (pair.second == id + 1)
			{
				Sleep(rand() % 100 + 300);
				cout << "visitor " << id + 1 << " has gone" << endl;
				return 0;
			}
		}
		ReleaseMutex(Hmutex);
	}
}

int main()
{
	cout << "Enter numbers of cooks:";
	cin >> numofcooks;
	while (numofcooks < 1)
	{
		cout << "enter a number greater than or equal to 1:";
		cin >> numofcooks;
	}
	cout << "Enter numbers of waiters:";
	cin >> numofwaiters;
	while (numofwaiters < 1)
	{
		cout << "enter a number greater than or equal to 1:";
		cin >> numofwaiters;
	}
	for (int i{}; i < numofwaiters; ++i)
	{
		availableWaiters.push(i + 1);
	}
	cout << "Enter numbers of visitors:";
	cin >> numofvisitors;
	while (numofvisitors < 1)
	{
		cout << "enter a number greater than or equal to 1:";
		cin >> numofvisitors;
	}
	int* cookmassiv = new int[numofcooks];
	int* waitermassiv = new int[numofwaiters];
	int* visitormassiv = new int[numofvisitors];
	Hcooks = new HANDLE[numofcooks];
	Hwaiters = new HANDLE[numofwaiters];
	Hvisitors = new HANDLE[numofvisitors];
	Heventsvisit = new HANDLE[numofvisitors];
	orderEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	Hsemaphore = CreateSemaphore(NULL, 0, numofvisitors, NULL);
	for (int i{}; i < numofcooks; ++i)
	{
		cookmassiv[i] = i;
		Hcooks[i] = CreateThread(NULL, 0, cooker, &cookmassiv[i], 0, NULL);
	}
	for (int i{}; i < numofwaiters; ++i)
	{
		waitermassiv[i] = i;
		Hwaiters[i] = CreateThread(NULL, 0, waiter, &waitermassiv[i], 0, NULL);
	}
	for (int i{}; i < numofvisitors; ++i)
	{
		visitormassiv[i] = i;
		Heventsvisit[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		Hvisitors[i] = CreateThread(NULL, 0, visitor, &visitormassiv[i], 0, NULL);
	}
	SetEvent(Heventsvisit[0]);
	WaitForMultipleObjects(numofvisitors, Hvisitors, TRUE, INFINITE);
	WaitForMultipleObjects(numofwaiters, Hwaiters, TRUE, INFINITE);
	WaitForMultipleObjects(numofcooks, Hcooks, TRUE, INFINITE);
	if (numofvisitors == servedVisitors)
	{
		cout << "All visitors are serverd. Restraunt is closed." << endl;
	}
	else
	{
		cout << "ERROR" << endl;
	}
	cout << "Orders:" << endl;
	for (auto pair : waitersorders)
	{
		cout << "waiter " << pair.first << " processed order " << pair.second << endl;
	}
	cout << "Cooks:" << endl;
	for (auto pair : cookers)
	{
		cout << "cooker " << pair.first << " processed order " << pair.second << endl;
	}
	cout << "Delievers:" << endl;
	for (auto pair : waiterdelievers)
	{
		cout << "waiter " << pair.first << " deliever order " << pair.second << endl;
	}
	for (int i = 0; i < numofvisitors; ++i)
	{
		CloseHandle(Heventsvisit[i]);
		CloseHandle(Hvisitors[i]);
	}
	for (int i = 0; i < numofwaiters; ++i)
	{
		CloseHandle(Hwaiters[i]);
	}
	for (int i = 0; i < numofcooks; ++i)
	{
		CloseHandle(Hcooks[i]);
	}
	CloseHandle(Hsemaphore);
	CloseHandle(Hmutex);
}