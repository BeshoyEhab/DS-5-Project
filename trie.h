#ifndef TRIE_H
#define TRIE_H

#include <map>
#include <string>
#include <vector>

using namespace std;

class Node
{
public:
    map <char,Node*> child;
    int freq;
    int path;
    Node();
};

class Trie
{
public:
    vector<pair<string, int>> sortedWords;
    map<string ,int>allwards;
    vector<string> suggestionsVector;
    map<string, int>autoSaveMap;
    Node *root;
    Trie();
    void allWords(Node* node, string currentWord, map<string,int> words);
    vector<string> getAllWords();
    void insert(string,int f = 1);
    bool remove(string);
    void increaseFrequency(string);
    void autosave(string);
    bool contain(string);
    void generateAllWordsFromNode(Node* node, string prefix);
    void generateSuggestions(const string prefix,int num=4,bool BFS=false,bool Freq=true);
};

#endif
