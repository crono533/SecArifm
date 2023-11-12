#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <stdbool.h>
#include <iterator>
#include <fstream>

using namespace std;

const unsigned short l = 0, h = -1;
const unsigned short First_qtr = (h + 1) / 4;
const unsigned short Half = First_qtr * 2;
const unsigned short Thride_qtr = First_qtr * 3;

class Range
{
public:
    unsigned int a; // частота
    int L, R; //лева и правая граниац
    char c; // символ

    Range();
    Range(char, int);
    void PrintRange();
};

static bool comp(Range &l, Range &r)
{
    return l.a > r.a;
}

Range::Range(char _c, int _a)
{
    c = _c;
    a = _a;
}

list<Range> CreateList(map<char, int> &m)
{
    list<Range> ls;
    for (auto it = m.begin(); it != m.end(); it++)
    {
        Range p(it->first, it->second);
        ls.push_back(p);
    }
    ls.sort(comp);
    int tmp = 0;
    for (auto it = ls.begin(); it != ls.end(); it++)
    {
        it->L = tmp;
        it->R = tmp + it->a;
        tmp = it->R;
    }
    return ls;
}

unsigned long long HeadReader(map<char, int> *MK, ifstream &fin)
{
    int key, q;
    unsigned long long len = 0;
    fin.read((char *)&q, sizeof(q));
    char s;
    for (int i = 0; i < q; i++)
    {
        fin.read((char *)&s, sizeof(s));
        fin.read((char *)&key, sizeof(key));
        (*MK)[s] = key;
        len += key;
    }
    cout << "LEN: " << len << endl;
    return len;
}

void Range::PrintRange()
{
    cout << c << " : " << L << " - " << R << endl;
}

void PrintList(list<Range> ls)
{
    for (auto it = ls.begin(); it != ls.end(); it++)
    {
        it->PrintRange();
    }
}

void BitsPlusFollow(ofstream &fout, bool bit, int bits_to_follow, int *count)
{
    fout.write((char *)&bit, 1);
    for (; bits_to_follow > 0; bits_to_follow--)
    {
        fout.write((char *)&bit, 1);
    }
}


unsigned short H = h, L = l;
unsigned short l0 = l, h0 = h;

int main()
{
    int menuFlag = 0;
    cout << "Закодировать файл - 1." << endl
         << "Раскодировать файл - 2." << endl
         << "Введите команду: ";
    cin >> menuFlag;

    if (menuFlag == 1)
    {
        ofstream fn("input.txt", ios::app);
        if (!fn.is_open())
        {
            cout << "error";
            return 0;
        }
        
        ofstream fout("encode.txt", ios::binary);
        if (!fout.is_open())
        {
            cout << "error";
            return 0;
        }

        fn << " " << '\n';
        fn.clear();
        fn.close();

        ifstream fin("input.txt", ios::binary);
        if (!fin.is_open())
        {
            cout << "error";
            return 0;
        }

        map<char, int> m;
        list<Range> t;
        char c;
        unsigned long long len = 0;
        for (fin.get(c); !fin.eof(); fin.get(c))
            m[c]++;
        fin.clear();
        fin.seekg(0);

        t = CreateList(m);
        PrintList(t);

        int k = m.size();
        fout.write((char *)&k, sizeof(k));

        for(auto &pair : m)
        {
            cout << "Symbol: " << pair.first << " freq: " << pair.second << endl;
        }


        for (auto it = m.begin(); it != m.end(); it++)
        {
            fout.write((char *)&it->first, sizeof(it->first));
            fout.write((char *)&it->second, sizeof(it->second));
        }

        list<Range>::iterator it;

        int delitel = t.back().R;
        //cout << delitel << " lal" << endl;
        int bits_to_follow = 0;
        char tmp = 0;
        int count = 0;
        char s;
        while (fin.get(s))
        {
            for (it = t.begin(); it->c != s; it++) //ищем символ в листе (диапазон символа)
                ;
            int d = H - L + 1; //-1 - 1 + 1 = -1 на первой итерации ; H = -1 ; L = 1 ; d = -1 ;
    /*-1*/  H = L + it->R * d / delitel - 1; // delidel = 9 на первой итерации
    /*0*/   L = L + it->L * d / delitel;
            for (;;)
            {
                if (H < Half)
                {
                    count++;
                    if (count == 8)
                    {
                        fout.put(tmp);
                        tmp = 0;
                        count = 0;
                    }
                    for (; bits_to_follow > 0; bits_to_follow--)
                    {
                        tmp = tmp | (1 << (7 - count));
                        count++;
                        if (count == 8)
                        {
                            fout.put(tmp);
                            tmp = 0;
                            count = 0;
                        }
                    }
                }
                else if (L >= Half)
                {
                    tmp = tmp | (1 << (7 - count));
                    count++;
                    if (count == 8)
                    {
                        fout.put(tmp);
                        tmp = 0;
                        count = 0;
                    }
                    for (; bits_to_follow > 0; bits_to_follow--)
                    {
                        count++;
                        if (count == 8)
                        {
                            fout.put(tmp);
                            tmp = 0;
                            count = 0;
                        }
                    }
                    L -= Half;
                    H -= Half;
                }
                else if ((L >= First_qtr) && (H < Thride_qtr))
                {
                    bits_to_follow++;
                    L -= First_qtr;
                    H -= First_qtr;
                }
                else
                    break;
                L <<= 1;
                H <<= 1;
                H++;
            }
        }

        fin.close();
        fout.close();
    }
    else if (menuFlag == 2)
    {
        ifstream fin("encode.txt", ios::binary);
        if (!fin.is_open())
        {
            cout << "error";
            return 0;
        }
        ofstream fout("decode.txt", ios::binary);
        if (!fout.is_open())
        {
            cout << "error";
            return 0;
        }

        map<char, int> m;
        unsigned long long len = HeadReader(&m, fin);

        for(auto &pair : m)
        {
            cout << "Symbol: " << pair.first << " freq: " << pair.second << endl;
        }


        list<Range> t = CreateList(m);
        PrintList(t);
        list<Range>::iterator it;

        int delitel = t.back().R;
        unsigned short value = (fin.get() << 8) | fin.get();
        char tmp = fin.get();
        int count = 0;
        int q = 0;
        while (len)
        {
            int d = h0 - l0 + 1;
            unsigned int freq = (((((unsigned long long)value - l0 + 1) * delitel)) - 1) / (d);
            for (it = t.begin(); it->R <= freq; it++)
                ;

            h0 = l0 + it->R * d / delitel - 1;
            l0 = l0 + it->L * d / delitel;
            for (;;)
            {
                if (h0 < Half)
                    ;
                else if (l0 >= Half)
                {
                    value -= Half;
                    l0 -= Half;
                    h0 -= Half;
                }
                else if ((l0 >= First_qtr) && (h0 < Thride_qtr))
                {
                    value -= First_qtr;
                    l0 -= First_qtr;
                    h0 -= First_qtr;
                }
                else
                    break;
                l0 <<= 1;
                h0 <<= 1;
                h0++;
                value <<= 1;
                value += ((tmp & (1 << (7 - count))) >> (7 - count));
                count++;
                // cout<<it->c<<" : "<<l0<<" - "<<h0<<endl;
                if (count == 8)
                {
                    tmp = fin.get();
                    count = 0;
                }
            }
            fout.write((char*)(&(it->c)), sizeof(it->c));
            len--;
            cout << "LEN IN MAIN:" << len << endl;
        }

        fin.close();
        fout.close();
    }
    else
        cout << "Неизвестная команда" << endl;

    return 0;
}