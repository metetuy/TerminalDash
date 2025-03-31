#include "GameController.h"
#include <parallel/compatibility.h>
#include <conio.h>
#include <windows.h>

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
    system("cls"); // Clear the console for better visualization

    while (!game->game_over)
    {
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
                std::cout << "Quitting the game..." << std::endl;
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
        Sleep(150);
    }

    // Print the game over message
    system("cls"); // Clear the console for better visualization
    print_game_over();

    // Add the player's score to the leaderboard
    unsigned long final_score = game->current_score; // Example scoring logic
    game->leaderboard.insert(new LeaderboardEntry(final_score, time(nullptr), game->player->player_name));

    // Save the leaderboard to a file
    game->leaderboard.write_to_file(game->leaderboard_file_name);

    // Display the leaderboard
    std::cout << "\nLeaderboard:\n";
    game->leaderboard.print_leaderboard();
}

// Destructor to delete dynamically allocated member variables here
GameController::~GameController()
{
    delete game; // Deallocate the AsteroidDash instance
}

void GameController::print_game_over() const
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD cursorPosition = {0, 0};
    SetConsoleCursorPosition(hConsole, cursorPosition);

    std::string occupied = occupiedCellChar; // Use the occupied cell character
    std::string empty = unoccupiedCellChar;  // Use spaces for empty cells

    // Define the "Game Over!" pattern
    std::vector<std::string> game_over_pattern = {
        "01111000111100011111100111110001111100110001101111101111110011",
        "11000001100110110110110110000011000110110001101100001100110011",
        "11011101111110110110110111110011000110110001101111101111100011",
        "11001101100110110110110110000011000110011011001100001100110000",
        "01111001100110110110110111110001111100001110001111101100110011",
    };

    // Print the "Game Over!" pattern
    for (const auto &row : game_over_pattern)
    {
        for (char c : row)
        {
            if (c == '1')
            {
                std::cout << occupied; // Print occupied cell for '1'
            }
            else
            {
                std::cout << empty; // Print empty space for '0'
            }
        }
        std::cout << std::endl; // Move to the next line
    }
}