//
//  Classification.h
//  NeuroProject
//
//  Created by Хайкова Светлана and Мансуров Рустам on 07/06/2018.
//  Copyright © 2018 Khaykova Svetlana and Mansurov Rustam. All rights reserved.
//

#ifndef Classification_h
#define Classification_h

// Классификация методом Парзеновского окна переменной ширины 

struct Item
{
    int type;
    vector <ld> coordinate;
};

int k;
Item newItem;
vector <Item> sample;

ld dist(vector <ld> a, vector <ld> b) // Расстояние между векторами
{
    int len = a.size();
    ld ans = 0;
    frn (i, len)
        ans += (a[i] - b[i]) * (a[i] - b[i]);
    return ans;
}

ld dist(Item a, Item b)
{
    return dist(a.coordinate, b.coordinate);
}

bool cmp(Item a, Item b)
{
    return dist(a, newItem) < dist(b, newItem);
}

ld K(ld u) // Функция ядра : косинус
{
    return PI / 4 * cos(PI * sqrt(u) / 2);
}

void classification(Item curItem) // Классификация нового элемента, добавление в базу
{
    newItem = curItem;
    sort(all(sample), cmp);
    map <int, ld> m; /// class, weight;
    m.clear();
    ld distK = dist(curItem, sample[k - 1]);
    int maxType = sample[0].type;
    int i = 0;
    while (dist(curItem, sample[i]) <= distK)
    {
        if (m.find(sample[i].type) == m.end())
            m[sample[i].type] = 0;
        m[sample[i].type] += K(dist(sample[i], curItem) / distK);
        if (m[maxType] < m[sample[i].type])
            maxType = sample[i].type;
        i++;
    }
    curItem.type = maxType;
    sample.pb(curItem);
    //    word[maxType];
    if (maxType == 10)
    {
        cout << "Хуй\n";
    }
    else cout << maxType << "\n";
}

int classificationLOO(Item curItem, int k) // Классификация уже известного элемента из базы
{
    newItem = curItem;
    sort(all(sample), cmp);
    map <int, ld> m; /// class, weight;
    m.clear();
    ld distK = dist(curItem, sample[k]);
    int maxType = sample[1].type;
    int i = 1;
    while (dist(curItem, sample[i]) <= distK)
//    frab(i, 1, k + 1)
    {
        if (m.find(sample[i].type) == m.end())
            m[sample[i].type] = 0;
        m[sample[i].type] += K(dist(sample[i], curItem) / distK);
        if (m[maxType] < m[sample[i].type])
            maxType = sample[i].type;
        i++;
    }
//    if (maxType != curItem.type)
//    {
//        cerr << k << " " << maxType << " " << curItem.type << "\n";
//    }
    return int(maxType != curItem.type);
}


int testKLOO(int k) // смотрим на количество ошибок при данном параметре К
{
    vector <Item> copySample = sample;
    int len = int(copySample.size());
    int numErr = 0;
    frn (i, len)
        numErr += classificationLOO(copySample[i], k);
    cerr << k << " " << numErr << "\n";
    return numErr;
}

/*
    LOO - Leave-one-out, подбираем размер Парзеновского окна, при котором достигается наименьшее количество ошибок
    "удаляем" очередной элемент из базы, смотрим, как он классифицируется, сверяем с действительным классом
*/

int LOO()
{
    int len = min (int(sample.size()), 50);
    pair <int, int> p = mp(INF, 0); /// number Errors, best K;
    frab(i, 1, len)
    {
        int curErr = testKLOO(i);
        if (curErr <= p.first)
            p = mp(curErr, i);
    }
    return p.second;
}

#endif /* Classification_h */
