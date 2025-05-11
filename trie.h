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

    vector<pair<string, int>> mab;
    map<string ,int>allwards;
    vector<string> V;
    multimap<int,string> s;
    map<string, int>autoo;
    Node *root;

public:
    Trie();
    void allWords(Node* node, string currentWord, map<string,int> words);
    vector<string> getAllWords();
    void insert(string,int);
    void add(string);
    bool remove(string);
    void increaseF(string);
    void autosave(string);
    bool contain(string);
    void printAllWordsFromNode(Node* node, string prefix);
    void printSuggestions(const string prefix,int num=4,bool BFS=false,bool Freq=true);
};







#endif
