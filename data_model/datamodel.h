#include "trie.h"
#include <QString>

class DataModel{

private:
    Trie trie;

public:
    DataModel();
    void readJson(const QString& fileName);
    /*
    int getValue(const string &key);
    void addWord(string key, int frequency = 1);
    void deleteWord(string key);
    */
    void saveJson();
};
