#include "GameController.h"
#include <parallel/compatibility.h>
#include <conio.h> // For kbhit() and getch()

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
void GameController::play()
{
    cout << "Game started! Use the following keys to control the game:" << endl;
    cout << "W: Move Up | A: Move Left | S: Move Down | D: Move Right | Space: Shoot | Q: Quit" << endl;

    while (!game->game_over)
    {
        // Clear the console for better visualization
        system("cls");

        // Display game status
        cout << "Time: " << game->game_time << " | Lives: " << game->player->lives
             << " | Ammo: " << game->player->current_ammo << "/" << game->player->max_ammo << endl;

        // Check for keyboard input
        if (_kbhit()) // Check if a key is pressed
        {
            char key = _getch(); // Get the pressed key

            // Map key presses to game commands
            if (key == 'a' || key == 'A')
            {
                game->player->move_left();
            }
            else if (key == 'd' || key == 'D')
            {
                game->player->move_right(game->space_grid[0].size()); // Pass the grid width
            }
            else if (key == 'w' || key == 'W')
            {
                game->player->move_up();
            }
            else if (key == 's' || key == 'S')
            {
                game->player->move_down(game->space_grid.size()); // Pass the grid height
            }
            else if (key == ' ') // Space key for shooting
            {
                game->shoot();
            }
            else if (key == 'q' || key == 'Q') // Quit the game
            {
                cout << "Quitting the game..." << endl;
                game->game_over = true;
                break;
            }

            // Clear the input buffer
            while (_kbhit())
            {
                _getch(); // Consume any remaining characters in the buffer
            }
        }

        // Update the game state
        game->update_space_grid();

        // Print the current state of the space grid
        game->print_space_grid();

        // Increment game time
        game->game_time++;

        // Add a small delay to control the game speed
        Sleep(400);
    }
}

// Destructor to delete dynamically allocated member variables here
GameController::~GameController()
{
    delete game; // Deallocate the AsteroidDash instance
}