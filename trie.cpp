#include "trie.h"
#include <iostream>
#include <map>
#include <algorithm>
#include <string>
#include <iostream>

using namespace std;


Node::Node()
{
    freq = -1;
}

Trie::Trie() {
    root = new Node();
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
    allwards.insert(make_pair(s,f));
}

void Trie::increaseFrequency(string s) {
    Node* node = root;
    for (char c : s) {
        node = node->child[c];
    }
    if (node->freq >= 0) {
        node->freq++;
        allwards[s]++;
        cout << "frequancy increased  " << s << " to " << node->freq << endl;
    }
}

void Trie::autosave(string s) {
    autoSaveMap[s]++;

    if (autoSaveMap[s] == 3) {
        insert(s, 3);
        cout << "new word saved " << s << endl;
    }
}

bool Trie::remove(string s) {

    Node* node = root;
    if (!contain(s))
        return false;
    for (char c : s)
        node = node->child[c];
    node->freq = 0;
    allwards.erase(s);
    return true;
}

bool Trie::contain(string s) {
    Node* node = root;
    for (char c : s) {
        if (node->child.find(c) == node->child.end()) {
            return false;
        }
        node = node->child[c];
    }
    return (node->freq >= 0);
}

void Trie::generateAllWordsFromNode(Node* node, string prefix) {


    if (node->freq > 0) {
        sortedWords.push_back({prefix, node->freq});
    }
    for (auto& pair : node->child) {
        if (pair.second)
            generateAllWordsFromNode(pair.second, prefix + pair.first);
    }
}

void Trie::generateSuggestions(const string prefix , int numofsug,bool useBFS,bool useFreq) {
    sortedWords.clear();
    suggestionsVector.clear();

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

    generateAllWordsFromNode(node, prefix);

    sort(sortedWords.begin(), sortedWords.end(), [useFreq, useBFS](const auto &a, const auto &b) {
        if (useFreq && a.second != b.second)
            return a.second > b.second;
        if (useBFS && a.first.length() != b.first.length())
            return a.first.length() < b.first.length();
        return a.first < b.first;
    });

    for (int i = 0; i < min(numofsug, (int)sortedWords.size()); i++) {
          suggestionsVector.push_back(sortedWords[i].first);
    }
}
