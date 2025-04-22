#include "trienode.h"
#include <vector>
#include <string>

class Trie
{
private:
    TrieNode *root;
public:
    Trie();
    void insert(const std::string &word, int frequency = 1);
    std::vector<std::string> getWords(const std::string &prefix, int max_suggestions = 4);
};
