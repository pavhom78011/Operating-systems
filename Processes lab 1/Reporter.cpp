#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "Employee.h"
using namespace std;
int main(int argc, char* argv[])
{
    multimap<string, employee> people;
    vector<double> salary;
    employee a;
    string str = argv[1];
    int payment = stoi(argv[2]);
    ofstream ofst(str + ".txt");
    str = argv[0];
    ifstream ifst(str + ".bin", ios::binary);
    if (ofst.is_open())
    {
        str = argv[0];
        ofst << "Отчёт по файлу " + str + '\n';
        int index{};
        while (ifst.read(reinterpret_cast<char*>(&a), sizeof(employee)))
        {
            salary.push_back(a.hours * payment);
            people.insert({ a.name, a });
        }
        for (auto& num : people)
        {
            ofst << "Num:" << num.second.num << " " << " Name:" << num.second.name << " " << " Hours:" << num.second.hours << " " << "Salary:" << salary[index] << endl;
            ++index;
        }
        cout << "Reporter worked";
    }
    else
    {
        cout << "Ошибка открытия файла!";
    }
    ofst.close();
    ifst.close();
    _getch();
}