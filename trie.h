#ifndef TRIE_H
#define TRIE_H

#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include "queue"

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
    //multimap<int, string>m;
    multimap<int,string> s;
    // multimap<int, multimap<int,string>,greater<int>>m;
    map<string, int>autoo;
    //queue <string> q;
    Node *root;

public:
    Trie();
    void insert(string,int);
    void add(string);
    bool remove(string);
    void increaseF(string);
    void autosave(string);
    bool contain(string);
    void printAllWordsFromNode(Node* node, string prefix);
    void printSuggestions(const string prefix,int num=4);
};







#endif
