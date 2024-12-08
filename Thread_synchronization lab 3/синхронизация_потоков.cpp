#include <iostream>
#include <Windows.h>
#include <random>
using namespace std;

struct MyStruct 
{
    int* array;
    int size{};
    int finish{};
};

MyStruct a;
HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
HANDLE* hStop;
HANDLE* hFinish;
HANDLE* hContinue;
CRITICAL_SECTION cs;
bool* threadsState;

static DWORD WINAPI marker(LPVOID num) 
{
    WaitForSingleObject(hEvent, INFINITE);
    int doublenum = *(int*)num;
    srand(doublenum);
    for (int random{}, i{};;) 
    {
        random = rand() % a.size;
        EnterCriticalSection(&cs);
        if (a.array[random] == 0) 
        {
            Sleep(5);
            a.array[random] = doublenum;
            ++i;
            Sleep(5);
            LeaveCriticalSection(&cs);
        }
        else 
        {
            cout << "Serial number:" << doublenum << endl;
            cout << "Number of marked elements:" << i << endl;
            cout << "Index of an array element that cannot be marked: " << random << endl;
            LeaveCriticalSection(&cs);
            SetEvent(hStop[doublenum - 1]);
            ResetEvent(hEvent);
            WaitForSingleObject(hEvent, INFINITE);
            DWORD result = WaitForSingleObject(hFinish[a.finish - 1], INFINITE);
            if (result == WAIT_OBJECT_0 && doublenum == a.finish) 
            {
                EnterCriticalSection(&cs);
                for (int i{}; i < a.size; ++i) 
                {
                    if (a.array[i] == doublenum) 
                    {
                        a.array[i] = 0;
                    }
                }
                threadsState[doublenum - 1] = false;
                LeaveCriticalSection(&cs);
                ExitThread(0);
            }
            else 
            {
                WaitForSingleObject(hContinue[doublenum - 1], INFINITE);
            }
        }
    }
    return 0;
}

int main() {
    int numofthreads{}, k{};
    cout << "Array size:";
    cin >> a.size;
    a.array = new int[a.size];
    for (int i{}; i < a.size; ++i) 
    {
        a.array[i] = 0;
    }
    cout << "Number of threads marker:";
    cin >> numofthreads;
    int* threadnum = new int[numofthreads];
    threadsState = new bool[numofthreads];
    for (int i = 0; i < numofthreads; ++i)
    {
        threadsState[i] = true;
    }
    HANDLE* Hmarkers = new HANDLE[numofthreads];
    DWORD* Marker = new DWORD[numofthreads];
    hStop = new HANDLE[numofthreads];
    hFinish = new HANDLE[numofthreads];
    hContinue = new HANDLE[numofthreads];
    InitializeCriticalSection(&cs);
    for (int i{}; i < numofthreads; ++i) 
    {
        hStop[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (hStop[i] == NULL) 
        {
            return GetLastError();
        }
        hFinish[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (hFinish[i] == NULL) 
        {
            return GetLastError();
        }
        hContinue[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (hContinue[i] == NULL) 
        {
            return GetLastError();
        }
        threadnum[i] = i + 1;
        Hmarkers[i] = CreateThread(NULL, 0, marker, &threadnum[i], 0, &Marker[i]);
        if (Hmarkers[i] == NULL) 
        {
            return GetLastError();
        }
    }
    SetEvent(hEvent);
    for (;;)
    {
        for (int i = 0; i < numofthreads; ++i) 
        {
            if (threadsState[i])
            {
                WaitForSingleObject(hStop[i], INFINITE);
            }
        }
        cout << "Array:";
        for (int i{}; i < a.size; ++i) 
        {
            cout << a.array[i] << " ";
        }
        cout << endl;
        cout << "Sequence number of the stream to which it will be served signal to complete its work:";
        cin >> k;
        while (k < 1 || k > numofthreads || !threadsState[k - 1])
        {
            cout << "Enter correct number:";
            cin >> k;
        }
        a.finish = k;
        SetEvent(hFinish[k - 1]);
        SetEvent(hEvent);
        WaitForSingleObject(Hmarkers[k - 1], INFINITE);
        cout << "Array:";
        for (int i{}; i < a.size; ++i) 
        {
            cout << a.array[i] << " ";
        }
        cout << endl;
        for (int i{}; i < numofthreads; ++i) 
        {
            DWORD result = WaitForSingleObject(Hmarkers[i], 0);
            if (result == WAIT_OBJECT_0) 
            {
                cout << "Thread " << (i + 1) << " has terminated." << endl;
            }
            else 
            {
                cout << "Thread " << (i + 1) << " is still running." << endl;
            }
        }
        bool allThreadsTerminated = true;
        for (int i{}; i < numofthreads; ++i) 
        {
            if (threadsState[i])
            {
                allThreadsTerminated = false;
                break;
            }
        }
        if (allThreadsTerminated) 
        {
            break;
        }
        for (int i{}; i < numofthreads; ++i) 
        {
            if (i != k - 1 && threadsState[i])
            {
                SetEvent(hContinue[i]);
            }
        }
        for (int i{}; i < numofthreads; ++i) 
        {
            if (threadsState[i])
            {
                ResetEvent(hStop[i]);
                ResetEvent(hFinish[i]);
                ResetEvent(hContinue[i]);
            }
        }
    }
    for (int i{}; i < numofthreads; ++i) 
    {
        CloseHandle(hStop[i]);
        CloseHandle(hFinish[i]);
        CloseHandle(hContinue[i]);
        CloseHandle(Hmarkers[i]);
    }
    CloseHandle(hEvent);
    DeleteCriticalSection(&cs);
}