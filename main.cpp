#include "GameController.h"
#include <windows.h>

// Main function that reads the necessary file names and the player name
// from command line arguments
int main(int argc, char **argv)
{
    SetConsoleOutputCP(CP_UTF8);
    system("chcp 65001 > nul");

    string space_grid_file = argv[1];
    string celestial_objects_file = argv[2];
    string player_file = argv[3];
    string leaderboard_file = argv[4];
    string player_name = argv[5];

    GameController *controller = new GameController(
        space_grid_file,
        celestial_objects_file,
        leaderboard_file,
        player_file,
        player_name);

    controller->play();
    delete controller;
}