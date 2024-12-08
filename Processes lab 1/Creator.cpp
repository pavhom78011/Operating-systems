#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <fstream>
#include <string>
#include "Employee.h"
using namespace std;
int main(int argc, char* argv[])
{
    int num = stoi(argv[1]);
    string name = argv[2];
    ofstream ofst(name + ".bin", ios::binary);
    if (ofst.is_open())
    {
        for (; num != 0; --num)
        {
            employee a;
            cout << "Employee identification number:";
            cin >> a.num;
            while (cin.fail())
            {
                cin.clear();
                cin.ignore();
                cout << "Please enter a number:";
                cin >> a.num;
            }
            cout << "Name:";
            cin >> a.name;
            cout << "Hours:";
            cin >> a.hours;
            while (cin.fail())
            {
                cin.clear();
                cin.ignore();
                cout << "Please enter a number:";
                cin >> a.hours;
            }
            ofst.write(reinterpret_cast<char*>(&a), sizeof(employee));
        }
    }
    else
    {
        cout << "Error opening file!";
    }
    ofst.close();
}