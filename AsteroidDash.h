#ifndef ASTEROIDDASH_H
#define ASTEROIDDASH_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "CelestialObject.h"
#include "LeaderboardEntry.h"
#include "Leaderboard.h"
#include "Player.h"

#define occupiedCellChar "██"
#define unoccupiedCellChar "▒▒"

using namespace std;

struct Projectile
{
    int row;
    int col;

    Projectile(int r, int c) : row(r), col(c) {}
};

// Class that encapsulates the game play internals
class AsteroidDash
{
public:
    // Constructor to initialize the game
    AsteroidDash(const string &space_grid_file_name, const string &celestial_objects_file_name,
                 const string &leaderboard_file_name, const string &player_file_name, const string &player_name);

    // Destructor. Remove dynamically allocated member variables here
    virtual ~AsteroidDash();

    // 2D space_grid
    vector<vector<int>> space_grid;

    // Projectiles
    vector<Projectile> projectiles;

    // Pointer to track the player instance
    Player *player = nullptr;

    // A reference to the head of the celestial objects linked list
    CelestialObject *celestial_objects_list_head = nullptr;

    // Current score of the game
    unsigned long current_score = 0;

    // Current game tick
    unsigned long game_time = 0;

    // Leaderboard file name to store and read the leaderboard from
    string leaderboard_file_name;

    // Leaderboard member variable
    Leaderboard leaderboard;

    // True if the game is over
    bool game_over = false;

    // Helper function to delete celestial object in case of a collision;
    void erase(CelestialObject *object);

    // Function to detect collisions(checks if celestial objects grid_row is equal to the asked row and grid_col is equal to asked col)
    bool check_projectile_collision(int row, int col);

    bool check_player_collision(int row, int col);

    // Function to print the space_grid
    void print_space_grid() const;

    // Function to read the space grid from a file
    void read_space_grid(const string &input_file);

    // Function to read the player from a file
    void read_player(const string &player_file_name, const string &player_name);

    // Function to read celestial objects from a file
    // Reads the input file and calls the read_celestial_object() function for each celestial_object;
    void read_celestial_objects(const string &input_file);

    // Updates the grid based on player and celestial object states
    void update_space_grid();

    // Function to rotate the celestial object
    void rotate_celestial(CelestialObject *object, int hit_row, int hit_col);

    // Corresponds to the SHOOT command.
    void shoot();
};

#endif // ASTEROIDDASH_H
