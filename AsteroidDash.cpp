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
    int start_row = 0, time_step = 0;
    bool is_asteroid = (line[0] == '[');

    while (getline(file, line))
    {
        ObjectType type;
        // we need celestial objects if starts with [ its an asteroid
        if (is_asteroid)
        {
            type = ASTEROID;
        }
        // if starts with { it is a power-up and has an additional e type
        else if (line[0] == '{')
        {
            type = LIFE_UP;
        }
        vector<vector<bool>> shape;
        // traverse the celestial object until the end character to get the shape
        while ((line.back() != '}' || line.back() != ']') && getline(file, line))
        {
            vector<bool> row;
            for (char c : line)
            {
                if (c == 1 || c == 0)
                {
                    row.push_back(c == 1);
                }
            }
            shape.push_back(row);
        }
        // s is the starting  row of the top left corner of the object
        // t is the tick or time step in the game when the object should begin entering
        // read the additional data provided with the celestial object
        while (getline(file, line))
        {
            istringstream iss(line);
            char e_type;
            string extra_type;
            // get the additional info
            if (line.find(':') != string::npos)
            {
                if (line[0] == 's')
                {
                    start_row = line[2];
                }
                else if (line[0] == 't')
                {
                    time_step = line[2];
                }
                else if (line[0] == 'e')
                {
                    iss >> e_type >> extra_type;
                    type = (extra_type == "ammo") ? AMMO : LIFE_UP;
                }
            }
        }
        // create new celestial object
        CelestialObject *new_celestial_object = new CelestialObject(shape, type, start_row, time_step);

        // link the created celestial object to the linked list
        new_celestial_object->next_celestial_object = celestial_objects_list_head;
        celestial_objects_list_head = new_celestial_object;
    }
    file.close();
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
    // clear the space grid
    for (int i = 0; i < space_grid.size(); i++)
    {
        for (int j = 0; j < space_grid[i].size(); j++)
        {
            space_grid[i][j] = 0;
        }
    }

    // place the player
    int p_start_row = player->position_row;
    int p_start_col = player->position_col;

    for (int i = 0; i < space_grid.size(); i++)
    {
        for (int j = 0; j < space_grid[i].size(); j++)
        {
            if (space_grid.size() > p_start_row + i && space_grid[i].size() > p_start_col + j)
            {
                space_grid[p_start_row + i][p_start_col + j] = 1;
            }
        }
    }

    // get the celestial objects
    CelestialObject *celestial_object = celestial_objects_list_head;
    while (celestial_object != nullptr)
    {
        // implement the time step
        if (celestial_object->time_of_appearance <= game_time)
        {
            int start_row = celestial_object->starting_row;
            // start_col=last index of the row - the amount it needs to go left
            int start_col = space_grid[0].size() - 1 - (game_time - celestial_object->time_of_appearance);

            // place objects
            for (int i = 0; i < celestial_object->shape.size(); i++)
            {
                for (int j = 0; j < celestial_object->shape[i].size(); j++)
                {
                    if ((start_col + j) < space_grid[i].size() && (start_row + i) < space_grid.size())
                    {
                        space_grid[start_row + i][start_col + j] = 1;
                    }
                }
            }
        }
        // next celestial object
        celestial_object = celestial_object->next_celestial_object;
    }

    // Update projectiles and detect collisions
    for (int i = 0; i < projectiles.size(); i++)
    {
        // make projectile move to the right
        projectiles[i].col++;

        // out of bounds or collision check
        if (projectiles[i].col >= space_grid[0].size() || check_collision(projectiles[i].row, projectiles[i].col))
        {
            projectiles[i] = projectiles.back();
            projectiles.pop_back();

            // Decrement `i` to stay at the same index after removing a projectile
            i--;
        }
    }
    // Handle collisions
    // increment game time
    game_time++;
}

// Corresponds to the SHOOT command.
// It should shoot if the player has enough ammo.
// It should decrease the player's ammo
void AsteroidDash::shoot()
{
    if (player->current_ammo > 0)
    {

        // shooting should be from the center row of the vehicle and starts from the front of the vehicle
        int center_row = player->position_row + player->spacecraft_shape.size() / 2;
        int front_col = player->position_col + player->spacecraft_shape[center_row].size();

        // add the Projectile to the projectiles list
        projectiles.emplace_back(center_row, front_col);

        // decrease the player's ammo
        player->current_ammo--;
    }
    else
    {
        return;
    }
}

// Destructor. Remove dynamically allocated member variables here.
AsteroidDash::~AsteroidDash()
{
    // TODO: Your code here
}