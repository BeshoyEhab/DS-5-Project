#include "trie.h"
#include <iostream>
#include <map>
#include <algorithm>
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
    allwards.insert(make_pair(s,f));
}


void Trie::increaseF(string s) {
    Node* node = root;
    for (char c : s) {
        node = node->child[c];
    }
    if (node->freq > 0) {
        node->freq++;
        allwards[s]++;
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


void Trie::add(string s) {
    if(!contain(s))
        insert(s,1);
}


bool Trie::remove(string s) {

    Node* node = root;
    if(contain(s))
    {
    for (char c : s) {
        node = node->child[c];
    }
    node->freq =0;
        return true;
    }
    else
        return false;
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


void Trie:: allWords(Node* node, string currentWord, map<string,int> words) {
    if (node->freq>0) {
        words.insert(make_pair(currentWord,node->freq));
    }

    for (auto& pair : node->child) {
        allWords(pair.second, currentWord + pair.first, words);
    }
}


void Trie:: printAllWordsFromNode(Node* node, string prefix) {


    if (node->freq>0) {
        //ابجدىDfs
        mab.push_back({prefix, node->freq}); //equivalent to make_pair(node->freq, prefix)
        // q.push(prefix);
        // m.insert(make_pair(node->path, prefix));


    }
    for (auto& pair : node->child) {
        printAllWordsFromNode(pair.second, prefix + pair.first);
    }


}

void Trie::printSuggestions(const string prefix , int numofsug,bool useBFS,bool useFreq) {
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

    if(!useBFS){
    sort(mab.begin(), mab.end(), [useFreq](const auto& a, const auto& b) {
        return (a.second > b.second && useFreq) || // Higher freq first
               (a.second == b.second && a.first < b.first); // Then alphabetical
    });
    }
    else{

    sort(mab.begin(), mab.end(), [useFreq](const auto& a, const auto& b) {
        if (a.second != b.second && useFreq) return a.second > b.second;
        if (a.first.length() != b.first.length()) // Preserve BFS length order
            return a.first.length() < b.first.length();
        return a.first < b.first; // Then alphabetical
    });
    }
    for (int i = 0; i < min(numofsug, (int)mab.size()); i++) {
          V.push_back(mab[i].first);
    }
}
