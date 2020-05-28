#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include "Game.h"

using namespace std;

void readConfigFile (string config_file_name,
					 string & dictionary_file_name,
					 string & board_file_name,
					 string & bag_file_name,
					 unsigned int &hand_size,
					 uint32_t &srandSeed,
					 string & init_file_name)
{
	ifstream configFile (config_file_name.c_str());
	string line;
    int count = 0;

	if (configFile.is_open()){
		while (getline (configFile, line))
		{
			stringstream ss (line);
			string parameter;
			getline(ss, parameter, ':');

			if (parameter == "HANDSIZE"){ 
				ss >> hand_size; count++; 
			} else if (parameter == "BOARD") { 
				ss >> board_file_name; count++; 
			} else if (parameter == "TILES") { 
				ss >> bag_file_name; count++; 
			} else if (parameter == "DICTIONARY") { 
				ss >> dictionary_file_name; count++; 
			} else if (parameter == "SEED") {
				ss >> srandSeed; count++;
			} else if (parameter == "INIT"){
				ss >> init_file_name;
			}

		}
		if (count != 5) throw invalid_argument("Invalid config file.\n");
	} else {
		throw invalid_argument("Unable to open the config file: " + config_file_name +"\n");
	}
}

int main (int argc, char **args)
{
	if (argc > 2) {
		cout << "Too many arguments. Should be: <scrabble executable> <config file name>\n";
		return 0;
	} else if (argc < 2) {
		cout << "Not enough arguments. Should be: <scrabble executable> <config file name>\n";
		return 0;
	}
	try {

		string dictionaryFileName, boardFileName, bagFileName, initFileName = "";
		unsigned int numTiles;
		uint32_t srandSeed;

		readConfigFile (args[1], dictionaryFileName, boardFileName, bagFileName, numTiles, srandSeed, initFileName);
		Game game(dictionaryFileName, boardFileName, bagFileName, numTiles, srandSeed, initFileName);
	
		while(!game.gameOver){
			game.turnIO();
		}
		
		return 0;
	}
	catch (invalid_argument & e){ 
		cout << e.what(); 
	}
	return 1;
}
