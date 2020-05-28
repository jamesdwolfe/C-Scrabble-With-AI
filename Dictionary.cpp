#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "Dictionary.h"
#include "Trie.cpp"

using namespace std;

Dictionary::Dictionary (string dictFileName){
	ifstream f (dictFileName.c_str());
	string w;

	if (f.is_open()){
		while (getline (f, w)){	
			wordLibrary.insert(w);	
		}
		f.close ();
	} else {
		throw invalid_argument("Unable to open the dictionary file: " + dictFileName +"\n");
	}
}

bool Dictionary::isLegalWord(string word){
	if(wordLibrary.prefix(word)) return true;
	return false;
}

TrieNode* Dictionary::isLegalPrefix(string word){
	return wordLibrary.prefix(word);

}

TrieNode* Dictionary::returnRoot(){
	return wordLibrary.getRoot();
}
