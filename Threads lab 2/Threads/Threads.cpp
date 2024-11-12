#include <iostream>
#include <Windows.h>
using namespace std;
struct MyStruct
{
    int* array;
    int size{};
    int minelement{};
    int maxelement{};
    int averagevalue{};
};
static DWORD WINAPI min_max(LPVOID array2)
{
    MyStruct* b = (MyStruct*)array2;
    int minelement(b->array[0]), maxelement(b->array[0]);
    for (int i{}; i < b->size; ++i)
    {
        minelement = minelement > b->array[i] ? b->array[i] : minelement;
        Sleep(7);
        maxelement = maxelement < b->array[i] ? b->array[i] : maxelement;
        Sleep(7);
    }
    cout << "Min element:" << minelement << endl;
    cout << "Max element:" << maxelement << endl;
    b->minelement = minelement;
    b->maxelement = maxelement;
    return 0;
}
static DWORD WINAPI average(LPVOID array2)
{
    MyStruct* b = (MyStruct*)array2;
    int averagevalue{};
    for (int i{}; i < b->size; ++i)
    {
        averagevalue += b->array[i];
        Sleep(12);
    }
    averagevalue /= b->size;
    cout <<"Average value:" << averagevalue << endl;
    b->averagevalue = averagevalue;
    return 0;
}
int main()
{
    MyStruct a;
    cout << "integer array size:";
    cin >> a.size;
    while (cin.fail())
    {
        cin.clear();
        cin.ignore();
        cout << "Please enter a number:";
        cin >> a.size;
    }
    if (a.size <= 0)
    {
        return 0;
    }
    cout << "array elements:";
    a.array = new int[a.size];
    for (int i{}; i < a.size; ++i)
    {
        cin >> a.array[i];
    }
    HANDLE Hmin_max;
    DWORD IDmin_max;
    HANDLE Haverage;
    DWORD IDaverage;
    Hmin_max = CreateThread(NULL, 0, min_max, &a, 0, &IDmin_max);
    if (Hmin_max == NULL)
    {
        return GetLastError();
    }
    Haverage = CreateThread(NULL, 0, average, &a, 0, &IDaverage);
    if (Haverage == NULL)
    {
        return GetLastError();
    }
    WaitForSingleObject(Hmin_max, INFINITE);
    WaitForSingleObject(Haverage, INFINITE);
    CloseHandle(Hmin_max);
    CloseHandle(Haverage);
    for (int i{}; i < a.size; ++i)
    {
        if (a.array[i] == a.maxelement || a.array[i] == a.minelement)
        {
            a.array[i] = a.averagevalue;
        }
    }
    cout << "Modified massiv:" << endl;
    for (int i{}; i < a.size; ++i)
    {
        cout << a.array[i] << " ";
    }
    return 0;
}