#ifndef DATAMODEL_H
#define DATAMODEL_H
#include "../trie.h"
#include <string>
#include <map>

using namespace std;

class DataModel{

public:
    map<string, int> words;
    map<string, int> temp;


    DataModel();
    bool readJson( Trie *t );
    int getValue(const string &key);
    void addWord(string key, int frequency = 1);
    void deleteWord(string key);
    bool saveJson();
};

#endif
