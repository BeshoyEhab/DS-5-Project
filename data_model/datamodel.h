#include <string>
#include "trie.h"

using namespace std;

class DataModel{

private:
    Trie trie;

public:
    DataModel();
    void readJson();
    /*
    int getValue(const string &key);
    void addWord(string key, int frequency = 1);
    void deleteWord(string key);
    */
    void saveJson();
};
