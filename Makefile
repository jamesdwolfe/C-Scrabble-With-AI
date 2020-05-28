CC       = g++
CPPFLAGS = -g -Wall -std=c++11
BIN_DIR  = bin

build: $(BIN_DIR)/scrabble

all: $(BIN_DIR)/scrabble
	-@echo "---running scrabble executable---"
	$(BIN_DIR)/scrabble config.txt

valgrind: $(BIN_DIR)/scrabble
	valgrind --tool=memcheck --leak-check=yes $(BIN_DIR)/scrabble config.txt

$(BIN_DIR)/scrabble: Scrabble.cpp $(BIN_DIR)/Game.o $(BIN_DIR)/Bag.o $(BIN_DIR)/Player.o $(BIN_DIR)/Dictionary.o $(BIN_DIR)/Board.o $(BIN_DIR)/ConsolePrinter.o
	$(CC) $(CPPFLAGS) $^ -o $@	
	-@echo "---scrabble executable built---"
	
$(BIN_DIR)/Game.o: Game.cpp Game.h Tile.h Bag.h Dictionary.h Player.h Board.h ConsolePrinter.h $(BIN_DIR)/.dirstamp ConsolePrinter.h
	$(CC) $(CPPFLAGS) -c $< -o $@	
	-@echo "---game object file built---"

$(BIN_DIR)/Bag.o: Bag.cpp Exceptions.h Bag.h Tile.h $(BIN_DIR)/.dirstamp
	$(CC) $(CPPFLAGS) -c $< -o $@
	-@echo "---bag object file built---"

$(BIN_DIR)/Player.o: Player.cpp Player.h Tile.h $(BIN_DIR)/.dirstamp
	$(CC) $(CPPFLAGS) -c $< -o $@	
	-@echo "---player object file built---"

$(BIN_DIR)/Dictionary.o: Dictionary.cpp Dictionary.h Trie.cpp Trie.h $(BIN_DIR)/.dirstamp
	$(CC) $(CPPFLAGS) -c $< -o $@	
	-@echo "---dictionary object file built---"

$(BIN_DIR)/Board.o: Board.cpp Board.h Tile.h $(BIN_DIR)/.dirstamp	
	$(CC) $(CPPFLAGS) -c $< -o $@
	-@echo "---board object file built---"

$(BIN_DIR)/ConsolePrinter.o: ConsolePrinter.cpp ConsolePrinter.h rang.h $(BIN_DIR)/.dirstamp	
	$(CC) $(CPPFLAGS) -c $< -o $@
	-@echo "---console printer object file built---"

$(BIN_DIR)/.dirstamp:
	-@mkdir -p $(BIN_DIR)
	-@touch $@

.PHONY: clean tests

clean:
	-@rm -rf $(BIN_DIR)
	-@echo "---Bin Deleted---"
