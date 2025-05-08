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

    multimap<int, string ,greater<int>> mab;
    vector <string> V;
    multimap<int, string>m;
    map<string, int>autoo;
    queue <string> q;
    Node *root;
public:
    Trie();
    void insert(string,int);
    bool contain(string);
    void printAllWordsFromNode(Node* node, string prefix);
    void printSuggestions(const string prefix);





};







#endif // TRIE_H
