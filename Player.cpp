#include "Player.h"

// Constructor to initialize the player's spacecraft, position, and ammo
Player::Player(const vector<vector<bool>> &shape, int row, int col, const string &player_name, int max_ammo, int lives)
    : spacecraft_shape(shape), position_row(row), position_col(col), player_name(player_name), max_ammo(max_ammo),
      current_ammo(max_ammo), lives(lives)
{
    // TODO: Your code here, if you want to do further initializations
}

// Move player left within the grid boundaries
void Player::move_left()
{
    // Ensure the player does not go out of bounds on the left
    if (position_col > 0)
    {
        position_col--;
    }
}

// Move player right within the grid boundaries
void Player::move_right(int grid_width)
{
    // Ensure the player does not go out of bounds on the right
    int player_width = spacecraft_shape[0].size(); // Width of the spacecraft
    if (position_col + player_width < grid_width)
    {
        position_col++;
    }
}

// Move player up within the grid boundaries
void Player::move_up()
{
    // Ensure the player does not go out of bounds at the top

    if (position_row > 0)
    {
        position_row--;
    }
}

// Move player down within the grid boundaries
void Player::move_down(int grid_height)
{
    // Ensure the player does not go out of bounds at the bottom
    int player_height = spacecraft_shape.size(); // Height of the spacecraft
    if (position_row + player_height < grid_height)
    {
        position_row++;
    }
}