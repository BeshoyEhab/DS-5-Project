#ifndef DATAMODEL_H
#define DATAMODEL_H
#include "../trie.h"
#include <QString>

using namespace std;

class DataModel{

public:
    DataModel();
    bool readJson( Trie *t );
    bool saveJson(Trie* t);
    bool convertTxtToJson(const QString &txtFilePath, const QString &jsonFilePath);
};

#endif
