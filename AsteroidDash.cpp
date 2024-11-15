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
/*
bool AsteroidDash::check_collision(int row, int col)
{
    // Check every celestial objects position
    CelestialObject *celestial_object = celestial_objects_list_head;
    while (celestial_object != nullptr)
    {
        int c_start_row = celestial_object->starting_row;
        int c_start_col = space_grid[c_start_row].size() - 1 - (game_time - celestial_object->time_of_appearance);

        // Loop through the celestial objects shape and control if the row and col is occupied or not
        for (int i = 0; i < celestial_object->shape.size(); i++)
        {
            for (int j = 0; j < celestial_object->shape[i].size(); j++)
            {
                if (celestial_object->shape[i][j] && c_start_row + i == row && c_start_col + j == col)
                {
                    return true;
                }
            }
        }
        celestial_object = celestial_object->next_celestial_object;
    }
    return false;
}
*/

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

    int start_row, start_col;
    // TODO: Your code here
    // read the starting row and col pos. for top-left of the vehicle
    file >> start_row >> start_col;
    string line;
    while (getline(file, line))
    {
        istringstream iss(line);
        vector<bool> row;
        int value;
        while (iss >> value)
        {
            row.push_back(value == 1);
        }
        if (!row.empty())
        {

            shape.push_back(row);
        }
    }
    file.close();

    player = new Player(shape, start_row, start_col, player_name);
}

// Function to read celestial objects from a file
void AsteroidDash::read_celestial_objects(const string &input_file)
{
    ifstream file(input_file);
    string line;
    int start_row = 0, time_step = 0;
    CelestialObject *tail = nullptr;
    if (file.peek() == ifstream::traits_type::eof())
    {
        return;
    }
    while (getline(file, line))
    {
        ObjectType type;
        bool is_asteroid = (line[0] == '[');
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
        do
        {
            vector<bool> row;
            for (char c : line)
            {
                if (c == '1' || c == '0')
                {
                    row.push_back(c == '1');
                }
            }
            shape.push_back(row);
        } while (line.back() != '}' && line.back() != ']' && getline(file, line));
        // s is the starting  row of the top left corner of the object
        // t is the tick or time step in the game when the object should begin entering
        // read the additional data provided with the celestial object
        while (getline(file, line) && !line.empty())
        {
            istringstream iss(line);
            char prefix;

            if (line.find(':') != string::npos)
            {
                // Parse the prefix (s, t, or e) and corresponding value
                iss >> prefix;
                iss.ignore(1, ':'); // Ignore the colon

                if (prefix == 's')
                {
                    iss >> start_row; // Read start row as an integer
                }
                else if (prefix == 't')
                {
                    iss >> time_step; // Read time step as an integer
                }
                else if (prefix == 'e')
                {
                    string extra_type;
                    iss >> extra_type;
                    if (extra_type == "ammo")
                    {
                        type = AMMO;
                    }
                    else if (extra_type == "life")
                    {
                        type = LIFE_UP;
                    }
                }
            }
        }
        // create new celestial object
        CelestialObject *new_celestial_object = new CelestialObject(shape, type, start_row, time_step);

        // Append to the end of the linked list
        if (!celestial_objects_list_head)
        {
            celestial_objects_list_head = new_celestial_object; // Initialize head if list is empty
            tail = new_celestial_object;                        // Set tail to new head
        }
        else
        {
            tail->next_celestial_object = new_celestial_object; // Append at the end
            tail = new_celestial_object;                        // Update tail
        }
    }
    file.close();
    tail->next_celestial_object = nullptr;

    CelestialObject *node = celestial_objects_list_head;
    while (node != nullptr)
    {
        if (node->next_celestial_object != nullptr)
        {

            for (int i = 0; i < node->rotations.size(); i++)
            {
                node->rotations[i]->next_celestial_object = node->next_celestial_object;
            }
        }
        node = node->next_celestial_object; // Move to the next node
    }
}

// Print the entire space grid
void AsteroidDash::print_space_grid() const
{

    // TODO: Your code here
    for (const auto &row : space_grid)
    {
        for (int value : row)
        {
            if (value == 1)
            {
                cout << occupiedCellChar;
            }
            else
            {
                cout << unoccupiedCellChar;
            }
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

    for (int i = 0; i < player->spacecraft_shape.size(); i++)
    {
        for (int j = 0; j < player->spacecraft_shape[i].size(); j++)
        {
            space_grid[p_start_row + i][p_start_col + j] = player->spacecraft_shape[i][j];
        }
    }

    /*
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
*/
    // update projectiles and detect collisions
    for (int i = 0; i < projectiles.size(); i++)
    {
        // make projectile move to the right
        space_grid[projectiles[i].row][projectiles[i].col] = 1;
        projectiles[i].col++;
        /*
        // out of bounds or collision check
        if (projectiles[i].col >= space_grid[0].size())
        {
            if (check_collision(projectiles[i].row, projectiles[i].col))
            {
                // handle collisions
                CelestialObject *celestial_object = celestial_objects_list_head;
                while (celestial_object != nullptr)
                {
                    int o_start_row = celestial_object->starting_row;
                    int o_start_col = space_grid[0].size() - 1 - (game_time - celestial_object->time_of_appearance);

                    for (int m = 0; m < celestial_object->shape.size(); m++)
                    {
                        for (int n = 0; n < celestial_object->shape[m].size(); n++)
                        {
                            if (check_collision(m, n))
                            {

                                int o_center_row = o_start_row + celestial_object->shape.size() / 2;
                                if (projectiles[i].row < o_center_row)
                                {
                                    if (celestial_object->right_rotation)
                                    {

                                        celestial_object = celestial_object->right_rotation;
                                    }
                                }
                                else
                                {
                                    if (celestial_object->left_rotation)
                                    {
                                        celestial_object = celestial_object->left_rotation;
                                    }
                                }
                            }
                        }
                    }
                    celestial_object = celestial_object->next_celestial_object;
                }
            }
            // determine which rotation based on above the center row or below the center row
            projectiles[i] = projectiles.back();
            projectiles.pop_back();

            // Decrement `i` to stay at the same index after removing a projectile
            i--;
        */
    }
    // increment game time

    // game_time++;
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
        int front_col = player->position_col + player->spacecraft_shape[0].size();

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
    // free the memory allocated for celestial objects
    CelestialObject *celestial_object = celestial_objects_list_head;

    while (celestial_object != nullptr)
    {
        CelestialObject *next = celestial_object->next_celestial_object;
        delete celestial_object;
        celestial_object = next;
    }

    // delete the player
    delete player;
}
