#include "trie.h"
#include <iostream>
#include <map>
#include <unordered_map>
#include <string>

using namespace std;


Node::Node()
{
    freq = 0;
}





Trie::Trie()
{
    root = new Node();
}



void Trie::insert(string s,int f)
{
    Node* node = root;
    for (int i = 0; i < s.length(); i++)
    {
        if (!node->child[s[i]])
        {
            node->child[s[i]] = new Node();
        }
        node = node->child[s[i]];
    }
    node->freq =f;
    node->path = s.length();
}



bool Trie::contain(string s)
{
    Node* node = root;
    for (int i = 0; i < s.size(); i++)
    {
        if (!node->child[s[i]])
            return false;
        node = node->child[s[i]];
    }
    return true;
}




void Trie:: printAllWordsFromNode(Node* node, string prefix) {


    if (node->freq>0) {
        mab.insert(make_pair(node->freq, prefix));
        q.push(prefix);
        m.insert(make_pair(node->path, prefix));
        //cout << prefix << endl;

    }
    for (auto& pair : node->child) {
        printAllWordsFromNode(pair.second, prefix + pair.first);
    }

    map<int, string>::iterator it;

        for (it = mab.begin(); it != mab.end(); it++)
        {
           V.emplace_back(it->second);
        }
        mab.clear();

}

void Trie:: printSuggestions(const string prefix) {
    Node* node = root;
    for (int i = 0; i < prefix.size();i++) {
        if (node->child.find(prefix[i]) != node->child.end()) {
            node = node->child[prefix[i]];
        }
        else {
            cout << "No suggestions found for prefix: " << prefix << endl;
            return;
        }
    }
    printAllWordsFromNode(node, prefix);
}
