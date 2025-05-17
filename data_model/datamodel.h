#ifndef DATAMODEL_H
#define DATAMODEL_H
#include "../trie.h"

using namespace std;

class DataModel{

public:
    DataModel();
    bool readJson( Trie *t );
    bool saveJson(Trie* t);
};

#endif
