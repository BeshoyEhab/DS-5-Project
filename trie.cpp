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





Trie::Trie() {
    root = new Node();
    autoo.clear();
}


void Trie::insert(string s, int f) {
    Node* node = root;
    for (char c : s) {
        if (!node->child[c]) {
            node->child[c] = new Node();
        }
        node = node->child[c];
    }
    node->freq = f;
}


void Trie::increaseF(string s) {
    Node* node = root;
    for (char c : s) {
        node = node->child[c];
    }
    if (node->freq > 0) {
        node->freq++;
        cout << " frequancy increased  " << s << " to " << node->freq << endl;
    }
}

void Trie::autosave(string s) {
    autoo[s]++;

    if (autoo[s] == 3) {
        insert(s, 3);
        cout << "new word saved " << s << endl;
    }
}


bool Trie::contain(string s) {
    Node* node = root;
    for (char c : s) {
        if (node->child.find(c) == node->child.end()) {
            return false;
        }
        node = node->child[c];
    }
    return (node->freq > 0);
}



void Trie:: printAllWordsFromNode(Node* node, string prefix) {


    if (node->freq>0) {
        mab.insert(make_pair(node->freq, prefix));
        q.push(prefix);
        m.insert(make_pair(node->path, prefix));


    }
    for (auto& pair : node->child) {
        printAllWordsFromNode(pair.second, prefix + pair.first);
    }


}

void Trie::printSuggestions(const string prefix) {
    mab.clear();
    V.clear();

    Node* node = root;
    for (int i = 0; i < prefix.size(); i++) {
        if (node->child.find(prefix[i]) != node->child.end()) {
            node = node->child[prefix[i]];
        }
        else {
            cout << "No suggestions found for prefix: " << prefix << endl;
            return;
        }
    }

    printAllWordsFromNode(node, prefix);


    int count = 0;
    for (auto it = mab.begin(); it != mab.end() && count < 4; ++it, ++count) {
        V.push_back(it->second);
    }
}
