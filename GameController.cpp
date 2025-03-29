#include "GameController.h"
#include <parallel/compatibility.h>

// Simply instantiates the game
GameController::GameController(
    const string &space_grid_file_name,
    const string &celestial_objects_file_name,
    const string &leaderboard_file_name,
    const string &player_file_name,
    const string &player_name

)
{
    try
    {
        // Initialize the game with the provided parameters
        game = new AsteroidDash(space_grid_file_name, celestial_objects_file_name, leaderboard_file_name, player_file_name,
                                player_name);

        // Initialize the game state
        game->game_over = false;
        game->game_time = 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error initializing game: " << e.what() << std::endl;
        throw;
    }
}

// Reads commands from the given input file, executes each command in a game tick
void GameController::play(const string &commands_file)
{
    ifstream file(commands_file);
    if (!file)
    {
        cerr << "Failed to open commands file: " << commands_file << endl;
        return;
    }

    string command;
    while (file >> command && !game->game_over)
    {
        // Clear the console for better visualization

        cout << "Time: " << game->game_time << " | Lives: " << game->player->lives
             << " | Ammo: " << game->player->current_ammo << "/" << game->player->max_ammo << endl;

        if (command == "MOVE_LEFT")
        {
            game->player->move_left();
        }
        else if (command == "MOVE_RIGHT")
        {
            game->player->move_right(game->space_grid[0].size()); // Pass the grid width
        }
        else if (command == "MOVE_UP")
        {
            game->player->move_up();
        }
        else if (command == "MOVE_DOWN")
        {
            game->player->move_down(game->space_grid.size()); // Pass the grid height
        }
        else if (command == "SHOOT")
        {
            game->shoot();
        }

        // Update the game state after each command
        game->update_space_grid();

        // Print the current state of the space grid
        game->print_space_grid();

        // Increment game time
        game->game_time++;
    }

    file.close();
}

// Destructor to delete dynamically allocated member variables here
GameController::~GameController()
{
    delete game; // Deallocate the AsteroidDash instance
}