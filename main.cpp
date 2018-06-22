//
//  main.cpp
//  NeuroProject
//
//  Created by Хайкова Светлана and Мансуров Рустам on 24.04.2018.
//  Copyright © 2018 Khaykova Svetlana and Mansurov Rustam. All rights reserved.
//

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <queue>
#include <set>
#include <map>
#include <algorithm>
#include <stack>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <fstream>

#define pb push_back
#define mp make_pair
#define all(a) a.begin(), a.end()
#define frn(i, n) for (int i = 0; i < (int)(n); i++)
#define frd(i, n) for (int i = (int)(n - 1); i >= 0; i--)
#define frs(it, n) for(auto it = n.begin(); it != n.end(); it++)
#define frab(i, a, b) for(int i = a; i < b; i++)


typedef long long ll;
typedef long double ld;
typedef unsigned int ui;
typedef unsigned short int usi;
typedef unsigned long long ull;

const ll LINF = 1e18;
const int INF = 1e9;
const int M = 1e9 + 7;
const double EPS = 1e-9;
const double PI = acos(-1.0);

using namespace std;

string fileName;
string curDirectory;
ui numInterv;

#include "ControlAudio.h"
#include "Classification.h"

void func();
void readData();
void test (int testNum);

///    Для запуска создать файл с именем "test" + номер(натуральное число) + ".wav"

int main(int argc, const char * argv[])
{
    std::ios_base::sync_with_stdio(0);
    //    freopen("aout", "w", stdout);
    
    func();
    
    numInterv = 19;
    
    readData();
    
    int testNum;
    cin >> testNum;
    test(testNum);
    
    return 0;
}


void func() // добавляет слова из файла в базу
{
    ofstream f1("aoutH");
    f1.close();

    fileName = "aoutH";
    curDirectory = "/Users/strizh78/Projects/NeuroProject/source/";
    
    string nameFile;
    cout << "Enter name of base-file:\n";
    cin >> nameFile;
    string name = "/Users/strizh78/Downloads/" + nameFile + ".wav";
    ControlAudio au(name);
    
    ofstream out("aout", ios_base::app);

    ifstream in1;
    
    int answer;
    cout << "Enter answer (as int) for file samples:\n";
    cin >> answer;
    in1.open("aoutH");
    
        while (!in1.eof())
        {
            ld temp;
            in1 >> temp;
            if (!in1.eof())
            {
                out << temp << ' ';
                frn (i, numInterv - 3)
                {
                    in1 >> temp;
                    out << temp << ' ';
                }
                out << 10 << '\n';
            }
        }
        in1.close();
    out.close();
}

void readData() // загружает данные из базы в классификатор
{
    ifstream fin("aout");
    
    ld temp;
    while (fin >> temp)
    {
        Item a;
        a.coordinate.pb(temp);
        frn (i, numInterv - 3)
        {
            fin >> temp;
            a.coordinate.pb(temp);
        }
        fin >> a.type;
        sample.pb(a);
    }
    k = 5;
// не раскоменчивать, работает парадоксально долго, за время работы на базе из 600 элементов можно детей родить, ибо работа за 5 степень * логарифм от количества элементов
//    k = LOO();
    cerr << numInterv << ' ' << k << '\n';
    fin.close();
}

void test (int testNum) // классифицирет слова из тестового файла "test" + testNum
{
    curDirectory = "/Users/strizh78/Projects/NeuroProject/test/";
    string name = "/Users/strizh78/Downloads/test" + to_string(testNum) + ".wav";
    ControlAudio au(name);
    frs (word, au.words)
    {
        cerr << word->start << ' ' << word->end << '\n';
        Item a;
        frn (i, (word->MFCC).size())
        {
            a.coordinate.pb((word->MFCC)[i]);
        }
        classification(a);
    }
}
