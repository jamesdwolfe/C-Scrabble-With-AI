#include <cstdlib>
#include <string>
#include "Trie.h"
using namespace std;

TrieSet::TrieSet (){
  root = new TrieNode();
  root->parent = nullptr;
}

TrieSet::~TrieSet (){
  if(root) clear(root);
}

void TrieSet::insert (string input){
  TrieNode* node = root;
  int strSize = input.size();

  for(int i = 0; i < strSize; i++){
    //insertion fail case
    if(tolower(input[i]) < 'a' || tolower(input[i]) > 'z') return;
  }

  for(int i = 0; i < strSize; i++){
    if(!node->children[getCharIndex(input[i])]){
      TrieNode* newNode = new TrieNode();
      newNode->parent = node;
      node->children[getCharIndex(input[i])] = newNode;
    }
    node = node->children[getCharIndex(input[i])];
  }

  node->inSet=true;
}

void TrieSet::remove (string input){
  if(!root) return;
  TrieNode* node = root;
  int strSize = input.size();

  for(int i = 0; i < strSize; i++){
    if((tolower(input[i]) < 'a' || tolower(input[i]) > 'z') || !node->children[getCharIndex(input[i])]) return;
    node = node->children[getCharIndex(input[i])];
  }

  if(isleaf(node) && node->inSet){
    TrieNode* curParent = node->parent;
    node->parent->children[getCharIndex(input[strSize-1])] = nullptr;
    delete node;
    node = nullptr;

    for(int i = 1; isleaf(curParent) && curParent && curParent->parent && !curParent->inSet; i++){
      TrieNode* toDelete = curParent;
      curParent = curParent->parent;
      curParent->children[getCharIndex(input[strSize-1-i])] = nullptr;
      delete toDelete;
      toDelete = nullptr;
    }
    
  } else if(node->inSet){
    node->inSet=false;
  }
}

TrieNode* TrieSet::prefix (string input){
  TrieNode* node = root;
  int strSize = input.size();

  for(int i = 0; i < strSize; i++){
    if((tolower(input[i]) < 'a' || tolower(input[i]) > 'z') || !node->children[getCharIndex(input[i])]) return nullptr;
    node = node->children[getCharIndex(input[i])];
  }

  if(node && node->inSet) return node;
  return nullptr;
}

void TrieSet::clear(TrieNode* node){
  for(TrieNode* x: node->children){
    if(x) clear(x);
  }
  delete node;
}

bool TrieSet::isleaf(TrieNode* node){
  for(TrieNode* x: node->children){
    if(x) return false;
  }
  return true;
}

TrieNode* TrieSet::getRoot(){
	return root;
}

int TrieSet::getCharIndex(char input){
	if(tolower(input) < 'a' || tolower(input) > 'z') return -1;
	return (int)(tolower(input)-'a');
}
