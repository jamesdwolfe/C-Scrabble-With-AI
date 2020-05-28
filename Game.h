#ifndef GAME_H_
#define GAME_H_


#include "Board.h"
#include "Bag.h"
#include "Tile.h"
#include "Dictionary.h"
#include "Player.h"
#include "ConsolePrinter.h"
#include <string>
#include <iostream>
#include <vector>
#include <iterator>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <sstream>

struct AIMove {
	int x;
	int y;
	char direction;
	std::string word;
	int score;
	int multiplier;
	int adjScore;
	AIMove(int a, int b, char c, std::string d, int e, int f, int g);
};

std::string removeAfterBlank(std::string s);
struct wordlengthComp { inline bool operator() (const AIMove& struct1, const AIMove& struct2);};
struct scoreComp { inline bool operator() (const AIMove& struct1, const AIMove& struct2);};

class Game{
public:

	Game(std::string dictionaryFileName, std::string boardFileName, std::string bagFileName, int handsize, uint32_t seed, std::string init);
	~Game();

	bool gameOver;
	void getPlayers();
	int playerCount;
	int turn;
	bool turnOver;

	void printScoreboard();
	bool evaluateMove(std::string s);
	std::string stringToUpper(std::string s);
	void turnIO();
	bool executeExchange(std::string s);
	int executeMove(char direction, int col, int row, std::string s);
	int executeMoveAI(std::string type);
	void helperAI(std::string cur, int startx, int starty, int x, int y, char direction, int score, int multiplier, int adjScore, std::vector<Tile*> hand, std::vector<AIMove> &moves, const TrieNode* prefix);
	bool tilesAvailable(std::string s);
	void endGame();

private:
	Bag tileBag;
	std::vector<Player*> playerList;
	Board gameBoard;
	Dictionary dict;
	int passCount;
	bool outOfTiles;
	int maxTiles;
};

#endif