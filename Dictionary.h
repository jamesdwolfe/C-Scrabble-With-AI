#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include <string>
#include "Trie.h"

class Dictionary {
public:
	Dictionary (std::string dictFileName);
	bool isLegalWord(std::string word);
	TrieNode* isLegalPrefix(std::string word);
	TrieNode* returnRoot();
	TrieSet wordLibrary;
};

#endif
