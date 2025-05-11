#ifndef DATAMODEL_H
#define DATAMODEL_H
#include "../trie.h"
#include <string>
#include <map>

using namespace std;

class DataModel{

public:
    map<string, int> words;

    DataModel();
    bool readJson( Trie *t );
    bool saveJson(Trie* t);
};

#endif
