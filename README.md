Project: CSCI104: Data Structures and Object Oriented Design: Scrabble

Name: James Wolfe

Email: James.wolfe.jr@gmail.com

C++ powered Scrabble game with GUI and multiple AI modes to play against (Either finds the word worth the most points or word with the longest length). Created during the Fall 2018 semester of CSCI104: Data Structures and Object Oriented Design at the University of Southern California.

Instructions:
	make build:
		Object files and scrabble executable will be compiled into the bin directory.
	make all:
		Same as make build except that the scrabble executable and config file will be ran immediately.
	make valgrind:
		Same as make all except that valgrind will also be ran alongside the scrabble executable and config file.

Scrabble AI instructions:
	If a player has a name starting with "CPUS" then an AI player will be created that finds the highest scoring move.
	If a player has a name starting with "CPUL" then an AI player will be created that plays as many tiles as possible.
	Different AI's are distinguished by comparators of AIMove objects rather than classes.
	CPUS is a lot harder to beat than CPUL for obvious reasons.
	AI implementation is mostly inside Game.cpp/Game.h (including AIMove struct and its comparators).
	Various other .cpp and .h files were changed since my part 1 implementation.
	Runtime for a sparse board with 10 tiles, 2 of those being blanks, was tested to be ~35-48 seconds (under the assignment requirement).

Notes from part 1 of project:
	File containing main:
		Scrabble.cpp.
	Move.h, from homework resources, was not used.
	Move.cpp was not created.
	ConsolePrinter.cpp was modified. (Had to change a set variable to a vector variable. 
		I had to either change ConsolePrinter.cpp or Bag.cpp to fix the bug)
	Running with valgrind takes much longer due to the constant memory allocation/deallocation/stack size.
	There might be a memory leak in the given bag class but I'm not going to fix it due to assignment instructions.