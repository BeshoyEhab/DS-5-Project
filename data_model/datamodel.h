#ifndef DATAMODEL_H
#define DATAMODEL_H
#include <string>
#include <map>

using namespace std;

class DataModel{

private:
    map<string, int> words;
    map<string, int> temp;

public:
    DataModel();
    bool readJson();
    int getValue(const string &key);
    void addWord(string key, int frequency = 1);
    void deleteWord(string key);
    bool saveJson();
};

#endif
