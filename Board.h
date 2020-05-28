#ifndef BOARD_H_
#define BOARD_H_

#include <string>
#include <vector>
#include <utility>
#include "Tile.h"
#include "Square.h"
#include "Dictionary.h"

class Board {
public:
	Board (std::string boardFileName);
	~Board();

	Board(const Board &source);
	Board& operator=(const Board &source);

	void placeTile(int row, int col, Tile* tile_);
	int getStartingC();
	int getStartingR();
	bool tileTaken(int row, int col);
	bool centerTaken();
	void setCenterTaken();
	bool validateBoard(Dictionary &dict);
	bool adjacentIsOccupied(int row, int col);
	size_t getRows() const;
	size_t getColumns() const;
	Square * getSquare (size_t x, size_t y) const;
	Square*** BoardSquares;
	std::vector<int> getPossibleRows();
	std::vector<int> getPossibleCols();

private:
	int c, r;
	int startingC, startingR;
	bool centertaken = false;

};

std::vector<std::string> tokenizeString(std::string s, unsigned int minSize);

#endif
