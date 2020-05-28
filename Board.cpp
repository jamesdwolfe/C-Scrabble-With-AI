#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <iomanip>
#include "Board.h"

using namespace std;

Board::Board (string boardFileName)
{
	ifstream boardFile (boardFileName.c_str());
	string line;

	c = r = startingC = startingR = 0;

	if (!boardFile.is_open()){
		throw invalid_argument("Cannot open file: " + boardFileName);
	}

	getline (boardFile, line);
	stringstream s1 (line);
	s1 >> c >> r;

	getline (boardFile, line);
	stringstream s2 (line);
	s2 >> startingC >> startingR;
	startingC--; 
	startingR --;

	BoardSquares = new Square**[r];

	for (int i = 0 ; i < r; i++)
	{
		
		BoardSquares[i] = new Square*[c];

		getline (boardFile, line);
		for (int j = 0; j < c; j++)
		{
			bool start = false;
			if (i == startingR && j == startingC) { 
				start = true;
			}

			switch (line[j]) { 
				case '.' :{
					BoardSquares[i][j] = new Square(1,1,start);
					break;
				}
				case '2' :{
					BoardSquares[i][j] = new Square(2,1,start);
					break;
				}
				case '3' :{
					BoardSquares[i][j] = new Square(3,1,start);
					break;
				}
				case 'd' :{
					BoardSquares[i][j] = new Square(1,2,start);
					break;
				}
				case 't' :{
					BoardSquares[i][j] = new Square(1,3,start);
					break;
				}
				default:{
					string error = "Invalid board file.";
				}
			}
		}
	}
	boardFile.close ();
}


Board::~Board(){
	for (int i = 0; i < r; i++){
		for(int j=0; j < c; j++){
			delete BoardSquares[i][j];
		}
	  delete [] BoardSquares[i];
	}
	delete [] BoardSquares;
}

Board::Board(const Board &source){
	c = source.c;
	r = source.r;
	startingC = source.startingC;
	startingR = source.startingR;

	if(source.BoardSquares){
		BoardSquares = new Square**[r];
		for (int i = 0 ; i < r; i++) { 
			BoardSquares[i] = new Square*[c]; 
			for(int j = 0; j < c; j++){
				BoardSquares[i][j] = new Square(
					source.BoardSquares[i][j]->getLMult(),
					source.BoardSquares[i][j]->getWMult(),
					source.BoardSquares[i][j]->isStart());
				if(source.BoardSquares[i][j]->isOccupied()){
					BoardSquares[i][j]->placeTile(new Tile(
						source.BoardSquares[i][j]->getLetter(),
						source.BoardSquares[i][j]->getScore()));
				}
			}
		}
	} else {
		BoardSquares = nullptr;
	}
}

Board& Board::operator=(const Board &source){
	if (this == &source){ return *this; }

	for (int i = 0; i < r; i++){
		for(int j = 0; j < c; j++){
			delete BoardSquares[i][j];
		}
	  delete [] BoardSquares[i];
	}
	delete [] BoardSquares;

	c = source.c;
	r = source.r;
	startingC = source.startingC;
	startingR = source.startingR;

	if(source.BoardSquares){
		BoardSquares = new Square**[r];
		for (int i = 0 ; i < r; i++) { 
			BoardSquares[i] = new Square*[c]; 
			for(int j = 0; j < c; j++){
				BoardSquares[i][j] = new Square(
					source.BoardSquares[i][j]->getLMult(),
					source.BoardSquares[i][j]->getWMult(),
					source.BoardSquares[i][j]->isStart());
				if(source.BoardSquares[i][j]->isOccupied()){
					BoardSquares[i][j]->placeTile(new Tile(
						source.BoardSquares[i][j]->getLetter(),
						source.BoardSquares[i][j]->getScore()));
				}
			}
		}
	} else {
		BoardSquares = nullptr;
	}

	return *this;

}

void Board::placeTile(int row, int col, Tile* tile_){
	BoardSquares[row][col]->placeTile(tile_);
}

vector<string> tokenizeString(string s, unsigned int minSize){
	vector <string> tokens; 
	string line; 
    stringstream ss(s); 
    while(getline(ss, line, ' ')){
    	string ret = "";
    	for(unsigned int i = 0; i < line.size(); i++){
    		if(line[i] == ' '){
    		} else { 
    			ret += line[i];
    		}
    	}
    	if(ret.size() >= minSize){
      		tokens.push_back(line);
      	}
    }
    return tokens;
}

int Board::getStartingC(){
	return startingC;
}
int Board::getStartingR(){
	return startingR;
}

bool Board::tileTaken(int row, int col){
	return (BoardSquares[row][col]->isOccupied());
}

bool Board::centerTaken(){
	return (BoardSquares[startingR][startingC]->isOccupied())||centertaken;
}

void Board::setCenterTaken(){
	centertaken=true;
}

/* Returns the number of rows of the board.*/
size_t Board::getRows() const{
	return r;
}

/* Returns the number of columns of the board.*/
size_t Board::getColumns() const{
	return c;
}

Square * Board::getSquare (size_t x, size_t y) const{
	return BoardSquares[y-1][x-1];
}

bool Board::adjacentIsOccupied(int row, int col){
	if(col >= (signed)getColumns() || row >= (signed)getRows()){ return false; }
	if(col+1 < (signed)getColumns() && col+1 > 0 && tileTaken(row,col+1)){ return true; }
	if(col-1 < (signed)getColumns() && col-1 > 0 && tileTaken(row,col-1)){ return true; }
	if(row+1 < (signed)getRows() && row+1 > 0 && tileTaken(row+1,col)){ return true; }
	if(row-1 < (signed)getRows() && row-1 > 0 && tileTaken(row-1,col)){ return true; }
	return false;
}

bool Board::validateBoard(Dictionary &dict){

	for(unsigned int i = 0; i < getRows(); i++){
		string rowStr = "";
		vector<string> rowStrTokens;
		for(unsigned int j = 0; j < getColumns(); j++){
			rowStr += BoardSquares[i][j]->getLetter();
		}
		rowStrTokens = tokenizeString(rowStr,2);

		for(unsigned int k = 0; k < rowStrTokens.size(); k++){
			if(rowStrTokens[k].size() > 1 && !dict.isLegalWord(rowStrTokens[k])){
				return false;
			}
		}
	}

	
	for(unsigned int i = 0; i < getColumns(); i++){
		string colStr = "";
		vector<string> colStrTokens;
		for(unsigned int j = 0; j < getRows(); j++){
			colStr += BoardSquares[j][i]->getLetter();
		}
		colStrTokens = tokenizeString(colStr,2);

		for(unsigned int k = 0; k < colStrTokens.size(); k++){
			if(colStrTokens[k].size() > 1 && !dict.isLegalWord(colStrTokens[k])){
				return false;
			} 
		}
	}
	return true;
}

std::vector<int> Board::getPossibleRows(){
	vector<int> range;
	for(int i = 0; i < (signed)getRows(); i++){
		bool found = false;
		for(int j = 0; !found && j < (signed)getColumns(); j++){
			if(BoardSquares[i][j]->isOccupied()) found = true;
		}
		for(int j = 0; !found && i+1 < (signed)getRows() && j < (signed)getColumns(); j++){
			if(BoardSquares[i+1][j]->isOccupied()) found = true;
		}
		for(int j = 0; !found && i-1 > -1 && j < (signed)getColumns(); j++){
			if(BoardSquares[i-1][j]->isOccupied()) found = true;
		}
		if(found) range.push_back(i);
	}
	return range;
}

std::vector<int> Board::getPossibleCols(){
	vector<int> range;
	for(int i = 0; i < (signed)getColumns(); i++){
		bool found = false;
		for(int j = 0; !found && j < (signed)getRows(); j++){
			if(BoardSquares[j][i]->isOccupied()) found = true;
		}
		for(int j = 0; !found && i+1 < (signed)getColumns() && j < (signed)getRows(); j++){
			if(BoardSquares[j][i+1]->isOccupied()) found = true;
		}
		for(int j = 0; !found && i-1 > -1 && j < (signed)getRows(); j++){
			if(BoardSquares[j][i-1]->isOccupied()) found = true;
		}
		if(found) range.push_back(i);
	}
	return range;
}