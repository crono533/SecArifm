#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <list>

using namespace std;

class Range
{
public:
    unsigned int frequency; // частота символа
    int left, right;        // левая и правая границы дифпазона
    char symbol;            // поле предствляющиее символ

    Range(char sym, int freq)
    {
        symbol = sym;
        frequency = freq;
    }
};

void Create_frequencies_map(map<char, int> &symbolsMap, ifstream &inputFile)
{
    char c; // переменная для считывания символа из файла
    for (inputFile.get(c); !inputFile.eof(); inputFile.get(c))
        symbolsMap[c]++;
}

static bool Compare_rule(Range &l, Range &r)
{
    return l.frequency > r.frequency;
}

void Create_sorted_list_of_ranges(list<Range> &rangesOfSymbols, map<char, int> &symbolsMap)
{
    for (auto &pair : symbolsMap)
    {
        Range tmpSym(pair.first, pair.second);
        rangesOfSymbols.push_back(tmpSym);
    }

    rangesOfSymbols.sort(Compare_rule);

    int tmp = 0;
    for (auto &pair : rangesOfSymbols)
    {
        pair.left = tmp;
        pair.right = tmp + pair.frequency;
        tmp = pair.right;
    }
}

int main()
{
    int menuFlag = 0;
    cout << "Закодировать файл - 1." << endl
         << "Раскодировать файл - 2." << endl
         << "Введите команду: ";
    cin >> menuFlag;
    if (menuFlag == 1)
    {
        ifstream inputFile("input.txt", ios::binary); // открытие файла для чтения в бинарном виде
        if (!inputFile.is_open())
        {
            cout << "Не удалось отркрыть читаемый файл";
            return 0;
        }

        ofstream outputFile("encode.txt", ios::binary);
        if(!outputFile.is_open())
        {
            cout << "Не удалось открыть файл на запись";
            return 0;
        }

        map<char, int> symbolsMap;
        Create_frequencies_map(symbolsMap, inputFile);

        cout << "Частоты символов" << endl;
        for (auto &pair : symbolsMap)
        {
            cout << pair.first << " " << pair.second << endl;
        }

        list<Range> rangesOfSymbols; // список для диапазона символов
        Create_sorted_list_of_ranges(rangesOfSymbols, symbolsMap);
        for(auto &pair : rangesOfSymbols)
        {
            cout <<"Symool: "<< pair.symbol << " left: " << pair.left << " right " << pair.right <<" freq: "<<pair.frequency<< endl; 
        }
        //сейчас запишем таблицу частот
        size_t howManySymbolsInText = 0;
        for(const auto &pair : rangesOfSymbols)
        {
            howManySymbolsInText += pair.frequency;
        }
        //посчитали в цикле сколько символов в тексте, чтобы при декодировании остановится
        outputFile.write(reinterpret_cast<const char*>(&howManySymbolsInText), sizeof(howManySymbolsInText));

        //тут в цикле записали таблицу
        for (const auto &pair : rangesOfSymbols)
        {
            char sym = pair.symbol;
            int freq = pair.frequency;
            
            outputFile.write(&sym, sizeof(sym));

            // Преобразование указателя на freq в указатель на char
            outputFile.write(reinterpret_cast<const char *>(&freq), sizeof(freq));
        }
    }
    else if (menuFlag == 0)
    {
    }
    else
    {
        cout << "Неизвестная команда";
        return 0;
    }

    return 0;
}