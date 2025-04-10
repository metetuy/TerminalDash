CC = g++
CFLAGS = -std=c++11 -g
LDFLAGS =

SOURCES = $(filter-out main.cpp Test.cpp, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)
HEADER_GCH = $(wildcard *.gch)


# Default space grid and other files
SPACE_GRID = sample_io/sample_input_space_grid.dat
CELESTIAL_OBJECTS = sample_io/sample_celestial_objects.dat
PLAYER_FILE = sample_io/sample_input_player.dat
LEADERBOARD_FILE = leaderboard.txt
PLAYER_NAME = Player1


# Targets for each test
TESTS = test
GAME = game

all: $(TESTS) $(GAME)

test: Test.cpp $(filter-out main.cpp, $(SOURCES)) $(HEADERS)
	$(CC) $(CFLAGS) Test.cpp $(filter-out main.cpp, $(SOURCES)) $(LDFLAGS) -o $@


game: main.cpp $(filter-out Test.cpp, $(SOURCES)) $(HEADERS)
	$(CC) $(CFLAGS) main.cpp $(filter-out Test.cpp, $(SOURCES)) $(LDFLAGS) -o $@

run: game
	./game $(SPACE_GRID) $(CELESTIAL_OBJECTS) $(PLAYER_FILE) $(LEADERBOARD_FILE) $(PLAYER_NAME)

clean:
	rm -f $(TESTS) $(GAME) $(HEADER_GCH)

