#ifndef PLAYER_H_
#define PLAYER_H_

#include <iostream>
#include <string>
#include <vector>
#include "Tile.h"

class Player{
public:
	Player(std::string n, size_t m);
	~Player();

	int maxTiles;
	std::string name;
	std::vector<Tile*> hand;
	std::vector<bool> visited;

	bool hasTiles(std::string s);
	bool hasTile(char c, std::vector<Tile*>& copy);
	std::vector<Tile*> removeTiles (std::string s);
	std::string fixBlanks(std::string s);
	void addTiles(std::vector<Tile*> newTiles);
	void endHand();
	void incrementScore(int val);
	int getScore();
	std::vector<Tile*> getHandTiles() const;
	size_t getMaxTiles() const;
	int getTilePoints(char c);
	int getTilePointsMark(char c);
	void clearVisitedStatus();

protected:
	int score;
};

#endif