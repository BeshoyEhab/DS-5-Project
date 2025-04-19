#ifndef DATAMODEL_H
#define DATAMODEL_H
#include <string>
#include <map>

using namespace std;

class DataModel{

private:
    map<string, int> words;

public:
    DataModel();
    bool readJson(const string &fileName);
    int getValue(const string &key);    // used to open the folder the first time you create the folder
    void addWord(string word);
    void deleteWord(string word);
};

#endif
