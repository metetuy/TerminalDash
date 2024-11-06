#include "AsteroidDash.h"

// Constructor to initialize AsteroidDash with the given parameters
AsteroidDash::AsteroidDash(const string &space_grid_file_name,
                           const string &celestial_objects_file_name,
                           const string &leaderboard_file_name,
                           const string &player_file_name,
                           const string &player_name)

    : leaderboard_file_name(leaderboard_file_name), leaderboard(Leaderboard())
{

    read_player(player_file_name, player_name);          // Initialize player using the player.dat file
    read_space_grid(space_grid_file_name);               // Initialize the grid after the player is loaded
    read_celestial_objects(celestial_objects_file_name); // Load celestial objects
    leaderboard.read_from_file(leaderboard_file_name);
}

// Function to read the space grid from a file
void AsteroidDash::read_space_grid(const string &input_file)
{

    ifstream file(input_file);

    // TODO: Your code here
    string line;
    // read the file line by line
    while (getline(file, line))
    {
        istringstream iss(line);
        vector<int> row;
        int value;
        // fill the rows
        while (iss >> value)
        {
            row.push_back(value);
        }
        // Add the rows to the space_grid vector
        space_grid.push_back(row);
    }
    file.close();
}

// Function to read the player from a file
void AsteroidDash::read_player(const string &player_file_name, const string &player_name)
{

    vector<vector<bool>> shape;
    ifstream file(player_file_name);
    string line;

    int start_row, start_col;
    // TODO: Your code here
    // read the starting row and col pos. for top-left of the vehicle
    file >> start_row >> start_col;

    while (getline(file, line))
    {
        istringstream iss(line);
        vector<bool> row;
        int value;
        while (iss >> value)
        {
            row.push_back(value == 1);
        }
        shape.push_back(row);
    }
    player = new Player(shape, start_row, start_col, player_name);
    file.close();
}

// Function to read celestial objects from a file
void AsteroidDash::read_celestial_objects(const string &input_file)
{
    ifstream file(input_file);
    string line;
    int start_row, time_step;

    while (getline(file, line))
    {

    // we need celestial objects if starts with [ its an asteroid
        if (line[0] == '[')
        {
            ObjectType type = ASTEROID;
        }
    // if starts with { it is a power-up and has an additional e type
        else if (line[0] == '{')
        {
            ObjectType type = LIFE_UP;
        }
        vector<vector<bool>> shape;
        while((line.back()!='{' || line.back() != '[') && getline(file,line)){
            vector<bool> row;
            for(char c : line){
                if(c == 1 || c == 0){
                    row.push_back(c == 1);
                }
            }
        }
    }
    // s is the starting  row of the top left corner of the object
    // t is the tick or time step in the game when the object should begin entering

}

// Print the entire space grid
void AsteroidDash::print_space_grid() const
{

    // TODO: Your code here
    for (const auto &row : space_grid)
    {
        for (int value : row)
        {
            cout << value << " ";
        }
        cout << endl;
    }
}

// Function to update the space grid with player, celestial objects, and any other changes
// It is called in every game tick before moving on to the next tick.
void AsteroidDash::update_space_grid()
{
    // TODO: Your code here
}

// Corresponds to the SHOOT command.
// It should shoot if the player has enough ammo.
// It should decrease the player's ammo
void AsteroidDash::shoot()
{
    // TODO: Your code here
}

// Destructor. Remove dynamically allocated member variables here.
AsteroidDash::~AsteroidDash()
{
    // TODO: Your code here
}