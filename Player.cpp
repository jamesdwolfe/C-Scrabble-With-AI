#include "Player.h"
#include <vector>
#include <string>
#include <iterator>

using namespace std;

Player::Player(string n, size_t m){
	score = 0;
	name = n;
	maxTiles = m;
	for(int i = 0; i < maxTiles; i++){ visited.push_back(false); }
}

Player::~Player(){
	for (vector<Tile*>::iterator ptr = hand.begin(); ptr != hand.end(); ptr++){
		if(*ptr != NULL){
			delete *ptr;
		}
	}
	hand.clear();
}

bool Player::hasTiles(string s){
	vector<Tile*> copy = hand;
	for(unsigned int i=0; i<s.size(); i++){
		if(s[i] == '?' && hasTile(s[i], copy)){
			i++;
			continue;
		} else if(!hasTile(s[i], copy) ) {
			return 0;
		}
	}
	return 1;

}

bool Player::hasTile(char c, vector<Tile*>& copy){
	for (vector<Tile*>::iterator ptr = copy.begin(); ptr != copy.end(); ptr++){
		if((*ptr)->getLetter() == tolower(c)){
			copy.erase(ptr);
			return 1;
		}
	}
	return 0;
}

vector<Tile*> Player::removeTiles(string s){
	vector<Tile*> transferred_tiles;
	for(unsigned int i=0; i<s.size(); i++){
		bool found = 0;
		for (vector<Tile*>::iterator ptr = hand.begin(); ptr != hand.end() && !found; ptr++){
			if(s[i] == '?'){
				if((*ptr)->getLetter() == '?'){
					transferred_tiles.push_back(*ptr);
					hand.erase(ptr);
					found = 1;
				}
				continue;
			}
			if((*ptr)->getLetter() == tolower(s[i]) ){
				transferred_tiles.push_back(*ptr);
				hand.erase(ptr);
				found = 1;
			}
		}
	}
	return transferred_tiles;
}

string Player::fixBlanks(string s){
	for(size_t i = 0; i<s.size(); i++){
		if(s[i] == '?') {
			s = s.erase(i,1);
			for(vector<Tile*>::iterator ptr = hand.begin(); ptr != hand.end(); ptr++){
					if( (*ptr)->getLetter() == '?' && (*ptr)->getUse() == '?'){
					(*ptr)->useAs(s[i]); 
					break;
				}
			}
		}
	}
	return s;
}

void Player::endHand(){
	for (vector<Tile*>::iterator ptr = hand.begin(); ptr != hand.end(); ptr++){
		score -= (*ptr)->getPoints();
	}
}
void Player::incrementScore(int val){
	score += val;
}

int Player::getScore(){
	return score;
}

void Player::addTiles(vector<Tile*> newTiles){
	for (vector<Tile*>::iterator ptr = newTiles.begin(); ptr != newTiles.end(); ptr++){
		hand.push_back(*ptr);
	}
}

std::vector<Tile*> Player::getHandTiles() const {
	return hand;
}

size_t Player::getMaxTiles() const {
	return maxTiles;
}

int Player::getTilePoints(char c) {
	for(vector<Tile*>::iterator ptr = hand.begin(); ptr != hand.end(); ptr++){
		if(tolower((*ptr)->getLetter()) == tolower(c) && tolower((*ptr)->getUse()) == tolower(c)){
			return (*ptr)->getPoints();
		}
	}
	return 0;
}

int Player::getTilePointsMark(char c) {
	int i = 0;
	for(vector<Tile*>::iterator ptr = hand.begin(); ptr != hand.end(); ptr++){
		if(tolower((*ptr)->getLetter()) == tolower(c) && tolower((*ptr)->getUse()) == tolower(c) && !visited[i]){
			visited[i] = true;
			return (*ptr)->getPoints();
		}
		i++;
	}
	return 0;
}

void Player::clearVisitedStatus(){
	for(int i = 0; i < maxTiles; i++){ visited[i] = false; }
}
