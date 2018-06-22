//
//  MathCalc.h
//  NeuroProject
//
//  Created by Хайкова Светлана and Мансуров Рустам on 04.06.2018.
//  Copyright © 2018 Khaykova Svetlana and Mansurov Rustam. All rights reserved.
//

#ifndef MathCalc_h
#define MathCalc_h

ld max (ld a, ld b)
{
    if (a > b)
        return a;
    return b;
}

ld min (ld a, ld b)
{
    if (a < b)
        return a;
    return b;
}

// Вычисляет энтропию фрейма
//* normalizedData - нормализованные данные WAV файла
//* start - начало фрейма (в данных WAV файла)
//* finish - конец фрейма (в данных WAV файла)
double entropyCalc(vector <ld> normalizedData, ui start, ui finish)
{
    int numInterv = 75;
    ld maxRaw = -1, minRaw = 1;

//    frab (i, start, finish)
//    {
//        minRaw = min(minRaw, normalizedData[i]);
//        maxRaw = max(maxRaw, normalizedData[i]);
//    }
    
    ld del = (maxRaw - minRaw) / numInterv; // в стиле Лебедева - все между минимумом и макс., делим на интервалы
    
    vector <ld> p(numInterv); // плотность
    // Вычисляем плотность:
    frab (j, start, finish)
    {
        ld value = (normalizedData[j]);
        ld index = floor((value - minRaw) / del);
        
        if (index < 0) index = 0;
        if (index > numInterv - 1) index = numInterv - 1;
        
        p[index]++;
    }
    
    frn (i, p.size())
    {
        minRaw = min(minRaw, p[i]);
        maxRaw = max(maxRaw, p[i]);
    }
    
    // Нормируем плотность
//    ld size = max (abs(minRaw), abs(maxRaw));
    ld size = finish - start + 1;
    frn (i, p.size())
        p[i] /= size;
    
    // Считаем энтропию:
    ld entropy = 0;
    frn (i, p.size())
    {
        if (p[i] == 0)
            continue;
        entropy += p[i] * log2(p[i]);
    }
    return entropy;
}

// рассчитывает мел фильтры
//* beg - начало диапазона частот
//* end - конец диапазона частот
//* numInterv - желаемое количество интервалов
vector <vector <ld> > setMelFilters(vector <ld> normalizedData, ld beg, ld end,
                                int numInterv, int sampleRate, Word w, int start, int fin)
{
    // получаем m[i]:
    ld begM = 1127 * log (1 + beg / 700.0);
    ld endM = 1127 * log (1 + end / 700.0);
    ld del = ld(endM - begM) / (numInterv - 1);
    
    vector <ld> m;
    frn (i, numInterv)
        m.pb(begM + del * i);
    
    // получаем h[i]:
    vector <ld> h;
    frn (i, numInterv)
        h.pb(700 * (exp(m[i] / 1127.0) - 1));
    
    // получаем f[i]:
    vector <ld> f;
    frn (i, numInterv)
        f.pb(floor((fin - start + 1) * h[i] / sampleRate));
    
    // теперь считаем mel-фильтры:
    vector <vector <ld> > H;
    
    H.resize(f.size());
    
    frab (i, 1, numInterv - 1)
    {
        frab (j, 0, fin - start + 1)
        {
            ld koef = 0;
            if (j >= f[i - 1] && j <= f[i])
                koef = (j - f[i - 1]) / (f[i] - f[i - 1]);
            else if (j >= f[i] && j <= f[i + 1])
                koef = (f[i + 1] - j) / (f[i + 1] - f[i]);
            H[i - 1].pb(koef);
        }
    }
    // возвращаем H:
    return H;
}


struct Complex
{
    ld Re;
    ld Im;
    Complex(ld Re = 0, ld Im = 0) : Re(Re), Im(Im){};
};

vector <ld> x;

Complex operator*(Complex a, Complex b)
{
    Complex ans;
    ans.Re = a.Re * b.Re - a.Im * b.Im;
    ans.Im = a.Im * b.Re + a.Re * b.Im;
    return ans;
}
Complex operator+(Complex a, Complex b)
{
    Complex ans;
    ans.Re = a.Re + b.Re;
    ans.Im = a.Im + b.Im;
    return ans;
}
Complex operator-(Complex a, Complex b)
{
    Complex ans;
    ans.Re = a.Re - b.Re;
    ans.Im = a.Im - b.Im;
    return ans;
}

vector <ld> fft(vector <ld> data, int st, int fin) // fast fourier transform
{
    vector <ld> X;
    
    int N = fin - st + 1;
    frab (i, st, fin + 1)
        x.pb(data[i]);
    
    vector <int> rev;
    vector <Complex> a;
    vector <Complex> w;
    
    int k = 1;
    int head = -1;
    
    while (k < N)
        k *= 2;
    
    frab (i, N, k)
        x.pb(0.0);
    
    N = k;
    k = round(log2(N));
    rev.resize(x.size());
    rev[0] = 0;
    
    frab (i, 1, N)
    {
        if (!(i & (i - 1))) head++;
        rev[i] = rev[i ^ (1 << head)];
        rev[i] |= 1 << (k - head - 1);
    }
    
    frn (i, N)
    {
        ld angle = -2.0 * PI * i / N;
        w.pb(Complex(cos(angle), sin(angle)));
    }
    
    frn (i, N)
        a.pb(Complex(x[rev[i]]));
    
    for (int len = 2; len <= N; len *= 2)
    {
        vector <Complex> tmp;
        tmp.resize(N);
        ld angle = -2 * PI / len;
        Complex wlen(cos(angle), sin(angle));
        for (int i = 0; i < N; i += len)
        {
            Complex w(1);
            frn (j, len / 2)
            {
                Complex u = a[i + j];
                Complex v = a[i + j + len / 2] * w;
                a[i + j] = u + v;
                a[i + j + len / 2] = u - v;
                w = w * wlen;
            }
        }
    }
    
    x.clear();
    
    frn (i, N)
    {
        ld H = 0.54 - 0.46 * cos(2 * PI * i / (N - 1));
        a[i] = a[i] * Complex(H);
        X.pb(a[i].Re * a[i].Re + a[i].Im * a[i].Im);
    }
    
    return X;
}


// рассчитывает MFCC коэффициенты для каждого слова
void MFCCcalc(ControlAudio &au)
{
    ofstream file(fileName, ios_base::app);
    frs (word, au.words)
    {
        int start = au.frames[word->start].start;
        int finish = au.frames[word->end].end;
        ui m = numInterv - 2;
        vector <ld> X = fft(au.wavData.getNormalizedData(), start, finish);
        vector <vector <ld> > H = setMelFilters(au.wavData.getNormalizedData(), 300, 8000, numInterv,
                                    au.wavData.getHeader().sampleRate, *word, start, finish);
        
        
        vector <ld> S;
        frn (i, m)
        {
            ld sum = 0;
            frn (j, finish - start + 1)
            sum += (X[j] * H[i][j]);
            if (sum > 0)
                S.pb(log2(sum));
            else S.pb(0);
        }

        vector <ld> C;
        frn (i, m)
        {
            ld sum = 0;
            frn (j, m)
                sum += (S[j] * cos(PI * i * (j + 1 / 2) / m));
            C.pb(sum);
        }

        word->MFCC = C;
        frn (i, C.size())
        {
            file << C[i] << ' ';
        }
        file << '\n';
    }
    file.close();
}


#endif /* MathCalc_h */
//106.929 27.5486 -10.312 12.3775 11.3448 11.1353 5.40018 7.62436 -1.42236 3.97227 4.90971 3.29991
//129.169 21.3799 -4.46486 19.1751 9.34703 19.6084 4.28198 11.1747 2.14947 5.88863 6.26937 7.18264
