#include "Game.h"

using namespace std;

string removeAfterBlank(string s){
	for(size_t i=0; i<s.size()-1; i++){
		if(s[i] == '?') {
			s = s.erase(i+1,1);
		}
	}
	return s;
}

AIMove::AIMove(int a, int b, char c, std::string d, int e, int f, int g): 
x(a), y(b), direction(c), word(d), score(e), multiplier(f), adjScore(g){}

inline bool wordlengthComp::operator() (const AIMove& struct1, const AIMove& struct2){
	string s1 = removeAfterBlank(struct1.word);
	string s2 = removeAfterBlank(struct2.word);
    return (s1.size() > s2.size());
}


inline bool scoreComp::operator() (const AIMove& struct1, const AIMove& struct2){
    return ((struct1.score*struct1.multiplier+struct1.adjScore) > (struct2.score*struct2.multiplier+struct2.adjScore));
}

Game::Game(string dictionaryFileName, string boardFileName, string bagFileName,
 int handsize, uint32_t seed, string init) : 
	tileBag(bagFileName,seed), 
	gameBoard(boardFileName),
	dict(dictionaryFileName), 
	maxTiles(handsize){

	playerCount = 0;
	turn = 0;
	passCount = 0;
	turnOver = false;
	gameOver = false;
	outOfTiles = false;
	getPlayers();

	if(init != ""){
		ifstream initFile (init.c_str());
		string line;

		if (initFile.is_open()){
			gameBoard.setCenterTaken();
			int row = 0;
			while (getline (initFile, line))
			{
				for(size_t i = 0; i < line.size()/3; i++){
					if(line[i*3] != '.' && line[i*3+1] != '.' && line[i*3+2] != '.'){
						char letter = line[i*3];
						string sScore = "";
						if(line[i*3+1] != '0') sScore += line[i*3+1];
						sScore += line[i*3+2];
						int score = stoi(sScore);
						gameBoard.placeTile(row,i,new Tile(letter,score));
					}
				}
				row++;
			}
		} else {
			throw invalid_argument("Unable to open the init file: " + init +"\n");
		}
	}

}

Game::~Game(){
	for(size_t i = 0; i < playerList.size(); i++){
		delete playerList[i];
	}
	playerList.clear();
}


void Game::getPlayers(){
	bool numPlayersFound = false;
	cout << "Enter the number of players (1-8):\n";
	while(!numPlayersFound){
		cin >> playerCount;
		if(playerCount > 8 || playerCount < 1 || cin.fail()){
			cout<<"Incorrect format. Try again."<<endl;
			cin.clear();
      		cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
		} else{
			for(int i = 1; i <= playerCount; i++){
				cout<<"Enter name of Player " << (i) << ": ";
				string player_name;
				cin>> player_name;
				Player* p = new Player(player_name, maxTiles);
				vector<Tile*> buffTiles = tileBag.drawTiles(maxTiles);
				p->addTiles(buffTiles);
				playerList.push_back(p);
			}
			numPlayersFound = true;
		}
	}
}

void Game::turnIO(){
	turnOver =0;
	while(!turnOver){
		ConsolePrinter cp;
		printScoreboard();
		cp.printBoard(gameBoard);
		cp.printHand(*playerList[turn]);

		if(playerList[turn]->name.size() >= 4 && 
			(stringToUpper(playerList[turn]->name.substr(0,4)) == "CPUS" ||
			stringToUpper(playerList[turn]->name.substr(0,4)) == "CPUL")) {
			cout << playerList[turn]->name << "'s move:" << endl;
			turnOver = executeMoveAI(stringToUpper(playerList[turn]->name.substr(0,4)));
		} else {
			cout << "It's your move, " << playerList[turn]->name << ":\n";
			string moveCommand;
			cin>> moveCommand;
			moveCommand = stringToUpper(moveCommand);
			if(moveCommand == "PASS" || moveCommand == "EXCHANGE" || moveCommand == "PLACE"){
				turnOver = evaluateMove(moveCommand);
			} else {
				turnOver = 0;
			}
		}
		
		if(turnOver){
			if(outOfTiles || passCount >= playerCount) {
				endGame();
			} else if(turn >= playerCount-1){
				turn = 0;
			}
			else{
				++turn;
			}
		}
		else{
			cout << "Your command was invalid. Try another move." << endl;
		}
	}
}

string Game::stringToUpper(string s){
	for (size_t i = 0; i< s.size(); i++){
		s[i] = toupper(s[i]);
	}
	return s;
}

void Game::printScoreboard(){
	cout << '\n';
	if(gameOver) {
		cout<< "----------Final Scoreboard----------\n";
	} else {
		cout<< "---------Current Scoreboard---------\n";
	}
	for(int i=0; i<playerCount; i++){
		cout << playerList[i]->name<< ": " << playerList[i]->getScore() << '\n';
	}
	cout << "------------------------------------\n\n";
}


bool Game::evaluateMove(string c){
	if(c == "PASS"){
		++passCount;
	}
	else if(c == "EXCHANGE"){
		passCount = 0;
		string temp;
		cin>> temp;

		if(cin.fail() ){
			return false;
		}

		temp = stringToUpper(temp);
		if(!executeExchange(temp) ){
			return false;
		} 
	}
	else if(c == "PLACE"){
		passCount = 0;
		char direction;
		int row, col;
		string s;
		cin>> direction >> row >> col >> s;

		if(cin.fail() ){
			return false;
		}
		else{
			s = stringToUpper(s);
			int result = executeMove(direction, row-1 , col-1 , s);
			
			if(result == 0){
				return false;
			}
			else{
				playerList[turn]->incrementScore(result);
				cout<< "Your score for this turn is: " << result << '\n';
				cout<< "Your total score is:         " << playerList[turn]->getScore() << '\n';
			}
		}
	}
	else{
		return false;
	}
	return true;
}

bool Game::executeExchange(string s){
	if(!playerList[turn]->hasTiles(s)) return false;
	vector<Tile*> tiles_out = playerList[turn]->removeTiles(s);
	tileBag.addTiles(tiles_out);
	vector<Tile*> tiles_in;
	if(s.size() >= tileBag.tilesRemaining()){
		tiles_in = tileBag.drawTiles( tileBag.tilesRemaining() );
	} else{
		tiles_in = tileBag.drawTiles(s.size());
	}
	playerList[turn]->addTiles(tiles_in);
	return true;
}

int Game::executeMoveAI(string type){
	vector<AIMove> moves;
	
	if(!gameBoard.centerTaken()){
		int startx = gameBoard.getStartingC();
		int starty = gameBoard.getStartingR();
		int x = startx;
		int y = starty;
		char direction = '-';
		vector<Tile*> hand = playerList[turn]->getHandTiles();
		TrieNode* prefix = dict.returnRoot();
		if(startx > (signed)gameBoard.getColumns() || starty > (signed)gameBoard.getRows()) return false;

		for(size_t i = 0; i < hand.size(); i++){
			vector<Tile*> tempHand = hand;
			if(tempHand[i]->getLetter() == '?'){
				tempHand.erase(tempHand.begin()+i);
				for(int j = 0; j < 26; j++){
					if(prefix->children[j]){
						string str = "?";
						str+= 'a'+j;
						int mult = 1;
						mult *= gameBoard.BoardSquares[y][x]->getWMult();
						helperAI(str,startx,starty,x+1,y,direction,0,mult,0,tempHand,moves,prefix->children[j]);
					}
				}
			} else if (prefix->children[(int)(tolower(tempHand[i]->getLetter())-'a')]) {
				string str = "";
				str+=tempHand[i]->getLetter();
				int index = (int)(tolower(tempHand[i]->getLetter())-'a');
				int mult = 1, score = 0;
				mult *= gameBoard.BoardSquares[y][x]->getWMult();
				score += playerList[turn]->getTilePoints(tempHand[i]->getLetter())*gameBoard.BoardSquares[y][x]->getLMult();
				tempHand.erase(tempHand.begin()+i);
				helperAI(str,startx,starty,x+1,y,direction,score,mult,0,tempHand,moves,prefix->children[index]); 
			}
		}

	} else {
		vector<int> possibleRows = gameBoard.getPossibleRows();
		vector<int> possibleCols = gameBoard.getPossibleCols();


		for(size_t r = 0; r < gameBoard.getRows(); r++){
			for(size_t c = 0; c < gameBoard.getColumns(); c++){
				if(!gameBoard.BoardSquares[r][c]->isOccupied()){
					
					int startx = c;
					int starty = r;
					vector<Tile*> hand = playerList[turn]->getHandTiles();
					TrieNode* prefix;
					if(startx > (signed)gameBoard.getColumns() || starty > (signed)gameBoard.getRows()) return false;

					char direction = '-';
					string preWord = "";
					int preWordScore = 0;
					int adjScore = 0;
					for(int i = c-1; i > -1 && gameBoard.BoardSquares[r][i]->isOccupied(); i--){
						preWord = gameBoard.BoardSquares[r][i]->getLetter() + preWord;
						preWordScore += gameBoard.BoardSquares[r][i]->getScore();
					}

					if(preWord.size() > 0) { prefix = dict.isLegalPrefix(preWord);
					} else { prefix = dict.returnRoot(); }

					for(int i = r-1; i > -1 && gameBoard.BoardSquares[i][c]->isOccupied(); i--){
						adjScore+=gameBoard.BoardSquares[i][c]->getScore();
					}
					for(int i = r+1; i < (signed)gameBoard.getRows() && gameBoard.BoardSquares[i][c]->isOccupied(); i++){
						adjScore+=gameBoard.BoardSquares[i][c]->getScore();
					}
					adjScore*=gameBoard.BoardSquares[r][c]->getWMult();


					for(size_t i = 0; prefix && i < hand.size() && 
						(signed)r >= possibleRows[0] && (signed)r <= possibleRows[possibleRows.size()-1]; i++){
						vector<Tile*> tempHand = hand;
						if(tempHand[i]->getLetter() == '?'){
							tempHand.erase(tempHand.begin()+i);
							for(int j = 0; j < 26; j++){
								if(prefix->children[j]){
									string str = "?";
									str+= 'a'+j;
									int mult = 1;
									mult *= gameBoard.BoardSquares[r][c]->getWMult();
									helperAI(str,startx,starty,c+1,r,direction,preWordScore,mult,adjScore,tempHand,moves,prefix->children[j]);
								}
							}
						} else if (prefix->children[(int)(tolower(tempHand[i]->getLetter())-'a')]) {
							string str = "";
							str+=tempHand[i]->getLetter();
							int index = (int)(tolower(tempHand[i]->getLetter())-'a');
							int charVal = playerList[turn]->getTilePoints(tempHand[i]->getLetter())*gameBoard.BoardSquares[r][c]->getLMult();
							int mult = 1, score = 0;
							mult *= gameBoard.BoardSquares[r][c]->getWMult();
							score += charVal;
							if(adjScore>0) { adjScore += charVal*gameBoard.BoardSquares[r][c]->getWMult(); }
							tempHand.erase(tempHand.begin()+i);
							helperAI(str,startx,starty,c+1,r,direction,score+preWordScore,mult,adjScore,tempHand,moves,prefix->children[index]);
							if(adjScore>0) { adjScore -= charVal*gameBoard.BoardSquares[r][c]->getWMult(); }
						}
					}

					direction = '|';
					preWord = "";
					preWordScore = 0;
					adjScore = 0;
					for(int i = r-1; i > -1 && gameBoard.BoardSquares[i][c]->isOccupied(); i--){
						preWord = gameBoard.BoardSquares[i][c]->getLetter() + preWord;
						preWordScore += gameBoard.BoardSquares[i][c]->getScore();
					}

					if(preWord.size() > 0) { prefix = dict.isLegalPrefix(preWord);
					} else { prefix = dict.returnRoot(); }

					for(int i = c-1; i > -1 && gameBoard.BoardSquares[r][i]->isOccupied(); i--){
						adjScore+=gameBoard.BoardSquares[r][i]->getScore();
					}
					for(int i = c+1; i < (signed)gameBoard.getColumns() && gameBoard.BoardSquares[r][i]->isOccupied(); i++){
						adjScore+=gameBoard.BoardSquares[r][i]->getScore();
					}
					adjScore*=gameBoard.BoardSquares[r][c]->getWMult();

					for(size_t i = 0; prefix && i < hand.size() && 
						(signed)c >= possibleCols[0] && (signed)c <= possibleCols[possibleCols.size()-1]; i++){
						vector<Tile*> tempHand = hand;
						if(tempHand[i]->getLetter() == '?'){
							tempHand.erase(tempHand.begin()+i);
							for(int j = 0; j < 26; j++){
								if(prefix->children[j]){
									string str = "?";
									str+= 'a'+j;
									int mult = 1;
									mult *= gameBoard.BoardSquares[r][c]->getWMult();
									helperAI(str,startx,starty,c,r+1,direction,preWordScore,mult,adjScore,tempHand,moves,prefix->children[j]);
								}
							}
						} else if (prefix->children[(int)(tolower(tempHand[i]->getLetter())-'a')]) {
							string str = "";
							str+=tempHand[i]->getLetter();
							int index = (int)(tolower(tempHand[i]->getLetter())-'a');
							int charVal = playerList[turn]->getTilePoints(tempHand[i]->getLetter())*gameBoard.BoardSquares[r][c]->getLMult();
							int mult = 1, score = 0;
							mult *= gameBoard.BoardSquares[r][c]->getWMult();
							score += charVal;
							if(adjScore>0) { adjScore += charVal*gameBoard.BoardSquares[r][c]->getWMult(); }
							tempHand.erase(tempHand.begin()+i);
							helperAI(str,startx,starty,c,r+1,direction,score+preWordScore,mult,adjScore,tempHand,moves,prefix->children[index]);
							if(adjScore>0) { adjScore -= charVal*gameBoard.BoardSquares[r][c]->getWMult(); }
						}
					}

				}
			}
		}
	}

	int result = 0;
	if(type == "CPUL"){
		sort(moves.begin(),moves.end(),wordlengthComp());
	} else if (type == "CPUS"){
		sort(moves.begin(),moves.end(),scoreComp());
	} 

	for(size_t i = 0; result == 0 && i < moves.size(); i++){
		result = executeMove(moves[i].direction, moves[i].y , moves[i].x , moves[i].word);
	}
	if(result != 0){
		playerList[turn]->incrementScore(result);
		cout<< playerList[turn]->name << "'s score for this turn is: " << result << '\n';
		cout<< playerList[turn]->name << "'s total score is:         " << playerList[turn]->getScore() << '\n';
	} else {
		cout << playerList[turn]->name << " passed.\n";
		passCount++;
		return 1;
	}
	return result;
	
}

void Game::helperAI(string cur, int startx, int starty, int x, int y, char direction, 
	int score, int multiplier, int adjScore, vector<Tile*> hand, std::vector<AIMove> &moves, const TrieNode* prefix){
	
	
	if(direction == '-'){
		if((x+1 < (signed)gameBoard.getColumns()) && gameBoard.BoardSquares[y][x]->isOccupied()){
			if(prefix->children[(int)(tolower(gameBoard.BoardSquares[y][x]->getLetter())-'a')]){
				score += gameBoard.BoardSquares[y][x]->getScore();
				helperAI(cur,startx,starty,x+1,y,direction,score,multiplier,adjScore,hand,moves,
					prefix->children[(int)(tolower(gameBoard.BoardSquares[y][x]->getLetter())-'a')]);
			} 
			return;
		}
	} else {
		if(y+1 < (signed)gameBoard.getRows() && gameBoard.BoardSquares[y][x]->isOccupied()){
			if(prefix->children[(int)(tolower(gameBoard.BoardSquares[y][x]->getLetter())-'a')]){
				score += gameBoard.BoardSquares[y][x]->getScore();
				helperAI(cur,startx,starty,x,y+1,direction,score,multiplier,adjScore,hand,moves,
					prefix->children[(int)(tolower(gameBoard.BoardSquares[y][x]->getLetter())-'a')]);
			} 
			return;
		}
	}

	if(prefix->inSet){
		string qrem = removeAfterBlank(cur);
		if(qrem.size() == (unsigned)maxTiles){
			moves.push_back(AIMove(startx,starty,direction,cur,score,multiplier,adjScore+50));
		} else {
			moves.push_back(AIMove(startx,starty,direction,cur,score,multiplier,adjScore));
		}
	}

	if(x >= (signed)gameBoard.getColumns() || y >= (signed)gameBoard.getRows()) return;

	int adjScoreTemp = 0;
	if(direction == '-'){
		for(int i = y-1; i > -1 && gameBoard.BoardSquares[i][x]->isOccupied(); i--){
			adjScoreTemp+=gameBoard.BoardSquares[i][x]->getScore();
		}
		for(int i = y+1; i < (signed)gameBoard.getRows() && gameBoard.BoardSquares[i][x]->isOccupied(); i++){
			adjScoreTemp+=gameBoard.BoardSquares[i][x]->getScore();
		}
	} else if (direction == '|'){
		for(int i = x-1; i > -1 && gameBoard.BoardSquares[y][i]->isOccupied(); i--){
			adjScoreTemp+=gameBoard.BoardSquares[y][i]->getScore();
		}
		for(int i = x+1; i < (signed)gameBoard.getColumns() && gameBoard.BoardSquares[y][i]->isOccupied(); i++){
			adjScoreTemp+=gameBoard.BoardSquares[y][i]->getScore();
		}
	}
	adjScoreTemp*=gameBoard.BoardSquares[y][x]->getWMult();

	for(size_t i = 0; i < hand.size(); i++){
		vector<Tile*> tempHand = hand;
		if(tempHand[i]->getLetter() == '?'){
			tempHand.erase(tempHand.begin()+i);
			for(int j = 0; j < 26; j++){
				if(prefix->children[j]){
					string str = cur;
					str += '?';
					str += 'a'+j;
					int tempMult = multiplier;
					tempMult *= gameBoard.BoardSquares[y][x]->getWMult();
					if(direction == '-') { 
						helperAI(str,startx,starty,x+1,y,direction,score,tempMult,adjScore+adjScoreTemp,tempHand,moves,prefix->children[j]);
					} else { 
						helperAI(str,startx,starty,x,y+1,direction,score,tempMult,adjScore+adjScoreTemp,tempHand,moves,prefix->children[j]); 
					}
				}
			}
			
		} else if (prefix->children[(int)(tolower(tempHand[i]->getLetter())-'a')]) {
			string str = cur;
			str += tempHand[i]->getLetter();
			int index = (int)(tolower(tempHand[i]->getLetter())-'a');
			int charVal = playerList[turn]->getTilePoints(tempHand[i]->getLetter())*gameBoard.BoardSquares[y][x]->getLMult();
			int tempMult = multiplier; 
			int tempScore = score;
			tempMult *= gameBoard.BoardSquares[y][x]->getWMult();
			tempScore += charVal;
			if(adjScoreTemp>0) { adjScoreTemp += charVal*gameBoard.BoardSquares[y][x]->getWMult(); }
			tempHand.erase(tempHand.begin()+i);
			if(direction == '-') {
				helperAI(str,startx,starty,x+1,y,direction,tempScore,tempMult,adjScore+adjScoreTemp,tempHand,moves,prefix->children[index]);
			} else {
				helperAI(str,startx,starty,x,y+1,direction,tempScore,tempMult,adjScore+adjScoreTemp,tempHand,moves,prefix->children[index]);
			}
			if(adjScoreTemp>0) { adjScoreTemp -= charVal*gameBoard.BoardSquares[y][x]->getWMult(); }
		}
	}

}

int Game::executeMove(char direction, int row, int col, string s){
	if(!playerList[turn]->hasTiles(s)) return false;
	string tilesToRemove = removeAfterBlank(s);
	s = playerList[turn]->fixBlanks(s);

	bool adj = false;
	int finalScore = 0;
	int mainScore = 0;
	int mainMultiplier = 1;

	if(!gameBoard.centerTaken() ){
		if(s.size() == 1 && !dict.isLegalWord(s)) return false;
		if(direction == '-'){
			if((signed)(col+s.size()) < gameBoard.getStartingC()){
				return false;
			} else if (col > gameBoard.getStartingC()) {
				return false;
			} else if (row != gameBoard.getStartingR()) {
				return false;
			}
		} else {
			if((signed)(row+s.size()) < gameBoard.getStartingR()) {
				return false;
			} else if (row > gameBoard.getStartingR()) {
				return false;
			} else if (col != gameBoard.getStartingC()) {
				return false;
			}
		}
		adj = true;
	}

	if(row >= (signed)gameBoard.getRows() || row < 0 ){
		return false;
	}
	else if(col >= (signed)gameBoard.getColumns() ||  col < 0 ){
		return false;
	}

	if(gameBoard.tileTaken(row, col)) return false;


	Board tempBoard = gameBoard;
	int addedLength = 0;

	if(direction == '-') {

		for(size_t i = 0; i < s.size()+addedLength; i++){
			if((col+i) >= tempBoard.getColumns()){
				playerList[turn]->clearVisitedStatus();
				return false;
			} else {
				if(tempBoard.BoardSquares[row][col+i]->isOccupied()){
					adj = true;
					addedLength++;
					mainScore += tempBoard.BoardSquares[row][col+i]->getScore();
					continue;
				} else {
					if(i == 0){
						for(int bound = 1; col-bound > -1 && tempBoard.BoardSquares[row][col-bound]->isOccupied(); bound++){
							adj = true;
							mainScore += tempBoard.BoardSquares[row][col-bound]->getScore();
						}
					}
				
					if(i == s.size()+addedLength-1){
						for(int bound = 1; col+i+bound < tempBoard.getColumns() && tempBoard.BoardSquares[row][col+i+bound]->isOccupied(); bound++){
							adj = true;
							mainScore += tempBoard.BoardSquares[row][col+i+bound]->getScore();
						}
					}

					//for perpendicular words
					int localMultiplier = 1;
					int localScore = 0;
					bool perpWordFound = false;
					int tilePoints = playerList[turn]->getTilePointsMark(s[i-addedLength]);
					mainScore += tilePoints*tempBoard.BoardSquares[row][col+i]->getLMult();
					localScore += tilePoints*tempBoard.BoardSquares[row][col+i]->getLMult();
					mainMultiplier *= tempBoard.BoardSquares[row][col+i]->getWMult();
					localMultiplier *= tempBoard.BoardSquares[row][col+i]->getWMult();
					tempBoard.placeTile(row,col+i,new Tile(s[i-addedLength],tilePoints));

					for(int bound = 1; row-bound > -1 && tempBoard.BoardSquares[row-bound][col+i]->isOccupied(); bound++){
							adj = true;
							perpWordFound = true;
							localScore += tempBoard.BoardSquares[row-bound][col+i]->getScore();
					}

					for(int bound = 1; row+bound < (signed)tempBoard.getRows() && tempBoard.BoardSquares[row+bound][col+i]->isOccupied(); bound++){
							adj = true;
							perpWordFound = true;
							localScore += tempBoard.BoardSquares[row+bound][col+i]->getScore();
					}

					if(perpWordFound){
						finalScore += (localScore*localMultiplier);
					}

				}
			}
		}
	} else if (direction == '|'){

		for(size_t i = 0; i < s.size()+addedLength; i++){
			if((row+i) >= tempBoard.getRows()){
				playerList[turn]->clearVisitedStatus();
				return false;
			} else {
				if(tempBoard.BoardSquares[row+i][col]->isOccupied()){
					adj = true;
					addedLength++;
					mainScore += tempBoard.BoardSquares[row+i][col]->getScore();
					continue;
				} else {
					if(i == 0){
						for(int bound = 1; row-bound > -1 && tempBoard.BoardSquares[row-bound][col]->isOccupied(); bound++){
							adj = true;
							mainScore += tempBoard.BoardSquares[row-bound][col]->getScore();
						}
					}
				
					if(i == s.size()+addedLength-1){
						for(int bound = 1; row+i+bound < tempBoard.getRows() && tempBoard.BoardSquares[row+i+bound][col]->isOccupied(); bound++){
							adj = true;
							mainScore += tempBoard.BoardSquares[row+i+bound][col]->getScore();
						}
					}

					//for perpendicular words
					int localMultiplier = 1;
					int localScore = 0;
					bool perpWordFound = false;
					int tilePoints = playerList[turn]->getTilePointsMark(s[i-addedLength]);
					mainScore += tilePoints*tempBoard.BoardSquares[row+i][col]->getLMult();
					localScore += tilePoints*tempBoard.BoardSquares[row+i][col]->getLMult();
					mainMultiplier *= tempBoard.BoardSquares[row+i][col]->getWMult();
					localMultiplier *= tempBoard.BoardSquares[row+i][col]->getWMult();
					tempBoard.placeTile(row+i,col,new Tile(s[i-addedLength],tilePoints));

					for(int bound = 1; col-bound > -1 && tempBoard.BoardSquares[row+i][col-bound]->isOccupied(); bound++){
							adj = true;
							perpWordFound = true;
							localScore += tempBoard.BoardSquares[row+i][col-bound]->getScore();
					}

					for(int bound = 1; col+bound < (signed)tempBoard.getColumns() && tempBoard.BoardSquares[row+i][col+bound]->isOccupied(); bound++){
							adj = true;
							perpWordFound = true;
							localScore += tempBoard.BoardSquares[row+i][col+bound]->getScore();
					}

					if(perpWordFound){
						finalScore += (localScore*localMultiplier);
					}

				}
			}
		}

	} else {
		playerList[turn]->clearVisitedStatus();
		return 0;
	}

	bool boardCheck = tempBoard.validateBoard(dict);

	if(boardCheck && adj){

		finalScore += (mainScore*mainMultiplier);

		playerList[turn]->removeTiles(tilesToRemove);
		vector<Tile*> replace_tiles;
		if(s.size() >= tileBag.tilesRemaining()){
			replace_tiles = tileBag.drawTiles( tileBag.tilesRemaining() );
		}
		else{
			replace_tiles = tileBag.drawTiles(s.size());
		}

		if(!replace_tiles.empty() ){
			playerList[turn]->addTiles(replace_tiles);
		}
		
		if((signed)s.size() == maxTiles){
			finalScore += 50;
		}

		gameBoard = tempBoard;
		passCount = 0;
		playerList[turn]->clearVisitedStatus();
		return finalScore;
		
	}
	playerList[turn]->clearVisitedStatus();
	return 0;
}

void Game::endGame(){
	for(int i = 0; i < playerCount; i++){ playerList[i]->endHand(); }

	gameOver = true;
	printScoreboard();
	int winner = 0;
	bool multiWinners = false;
	for(int i=1; i< playerCount; i++){
		if(playerList[i]->getScore() > playerList[winner]->getScore() ){
			winner = i;
		}
		else if(playerList[i]->getScore() == playerList[winner]->getScore()){
			multiWinners = true;
		}
	}

	if(multiWinners){
		cout<< "Winners:";
		for(int i=0; i< playerCount; i++){
			if(playerList[i]->getScore() == playerList[winner]->getScore()){
				cout<< " " << playerList[i]->name;
			}
		}
		cout << '\n';
	}
	else {
		cout << "Winner: ";
		cout << playerList[winner]->name << '\n';
	}

}

