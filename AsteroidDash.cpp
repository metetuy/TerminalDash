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
// function to erase celestial objects in case of a collision
void AsteroidDash::erase(CelestialObject *object)
{
    if (object == nullptr)
    {
        std::cout << "Debug: The object to be erased is nullptr. Exiting function." << std::endl;
        return;
    }

    // Special case: object is the head of the list
    if (object == celestial_objects_list_head)
    {
        celestial_objects_list_head = object->next_celestial_object;

        for (int i = 0; i < object->rotations.size(); i++)
        {
            if (object->rotations[i] != nullptr && object->rotations[i] != object)
            {
                delete object->rotations[i];
                object->rotations[i] = nullptr;
            }
        }
        object->rotations.clear();
        delete object;
        return;
    }

    CelestialObject *prev = celestial_objects_list_head;

    // Traverse the list to find the object
    while (prev != nullptr && prev->next_celestial_object != object)
    {
        prev = prev->next_celestial_object;
    }

    if (prev == nullptr)
    {
        return; // Object not found
    }

    // Connect prev to object's next
    prev->next_celestial_object = object->next_celestial_object;

    // Delete object's rotations
    for (int i = 0; i < object->rotations.size(); i++)
    {
        if (object->rotations[i] != nullptr && object->rotations[i] != object)
        {
            delete object->rotations[i];
        }
    }
    object->rotations.clear();
    delete object;
}

bool AsteroidDash::check_projectile_collision(int row, int col)
{
    for (int i = 0; i < projectiles.size(); i++)
    {
        if (projectiles[i].row == row && projectiles[i].col == col)
        {
            return true;
        }
    }
    return false;
}

bool AsteroidDash::check_player_collision(int row, int col)
{
    for (int i = 0; i < player->spacecraft_shape.size(); i++)
    {
        for (int j = 0; j < player->spacecraft_shape[i].size(); j++)
        {
            if (player->spacecraft_shape[i][j] == 1 && row == player->position_row + i && col == player->position_col + j)
            {
                return true;
            }
        }
    }
    return false;
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
    if (player->lives == 0)
    {
        game_over = true;
        return;
    }
    // Clear the space grid
    for (int i = 0; i < space_grid.size(); i++)
    {
        for (int j = 0; j < space_grid[i].size(); j++)
        {
            space_grid[i][j] = 0;
        }
    }

    // Place the player
    int p_start_row = player->position_row;
    int p_start_col = player->position_col;

    for (int i = 0; i < player->spacecraft_shape.size(); i++)
    {
        for (int j = 0; j < player->spacecraft_shape[i].size(); j++)
        {
            space_grid[p_start_row + i][p_start_col + j] = player->spacecraft_shape[i][j];
        }
    }

    // Place the projectiles
    for (int i = 0; i < projectiles.size(); i++)
    {
        // Check if the projectile is within bounds before updating the grid
        if (projectiles[i].col < space_grid[0].size() && projectiles[i].row < space_grid.size())
        {
            space_grid[projectiles[i].row][projectiles[i].col] = 1; // Update new position
        }
        if (projectiles[i].col + 1 < space_grid[0].size())
        {
            projectiles[i].col++; // Move the projectile to the right
        }
        else
        {
            // If the projectile goes out of bounds, remove it from the list
            projectiles.erase(projectiles.begin() + i);
            i--; // Adjust index to account for the removed projectile
        }
    }

    // Place the celestial objects
    CelestialObject *celestial_object = celestial_objects_list_head;
    while (celestial_object != nullptr)
    {
        if (celestial_object->time_of_appearance <= game_time)
        {
            int start_row = celestial_object->starting_row;
            int start_col = space_grid[0].size() - 1 - (game_time - celestial_object->time_of_appearance);

            // First determine if there are any collisions before printing
            bool should_erase = false;
            for (int i = 0; i < celestial_object->shape.size(); i++)
            {
                for (int j = 0; j < celestial_object->shape[i].size(); j++)
                {
                    int grid_row = start_row + i;
                    int grid_col = start_col + j;

                    // Ensure grid coordinates are valid
                    if (grid_row >= 0 && grid_row < space_grid.size() &&
                        grid_col >= 0 && grid_col < space_grid[0].size())
                    {
                        // Check for collisions
                        if (check_player_collision(grid_row, grid_col) && celestial_object->shape[i][j] == 1)
                        {
                            // Handle player collision
                            if (celestial_object->object_type == ASTEROID)
                            {
                                player->lives--;
                            }
                            else if (celestial_object->object_type != ASTEROID)
                            {
                                if (celestial_object->object_type == LIFE_UP)
                                    player->lives++;
                                else if (celestial_object->object_type == AMMO && player->current_ammo < player->max_ammo)
                                    player->current_ammo++;
                            }

                            // Erase object and skip placement
                            CelestialObject *temp = celestial_object->next_celestial_object;
                            erase(celestial_object);
                            celestial_object = temp;
                            should_erase = true;
                            break;
                        }

                        // Handle projectile collision with celestial objects
                        else if (check_projectile_collision(grid_row, grid_col) && celestial_object->shape[i][j] == 1 && celestial_object->object_type == ASTEROID)
                        {
                            cout << "Projectile Collision detected at (" << grid_row << ", " << grid_col << ")" << endl;
                            // Find which projectile caused the collision
                            for (int p = 0; p < projectiles.size(); p++)
                            {
                                if (projectiles[p].row == grid_row && projectiles[p].col == grid_col)
                                {
                                    // Remove the projectile
                                    projectiles.erase(projectiles.begin() + p);
                                    p--;
                                    break; // Exit the projectile loop after handling collision
                                }
                            }

                            celestial_object->shape[i][j] = 0; // Mark the hit part as destroyed

                            // Check if the celestial object is completely destroyed
                            bool is_destroyed = true;
                            for (int k = 0; k < celestial_object->shape.size(); k++)
                            {
                                for (int l = 0; l < celestial_object->shape[k].size(); l++)
                                {
                                    if (celestial_object->shape[k][l] == 1)
                                    {
                                        is_destroyed = false;
                                        break;
                                    }
                                }
                                if (!is_destroyed)
                                    break;
                            }

                            if (is_destroyed)
                            {
                                // Erase the celestial object if it is completely destroyed
                                CelestialObject *temp = celestial_object->next_celestial_object;
                                erase(celestial_object);
                                celestial_object = temp;
                                should_erase = true;
                            }
                            else
                            {
                                // Rotate the celestial object
                                rotate_celestial(celestial_object, grid_row, grid_col);
                            }

                            if (should_erase)
                                break; // Exit the shape loop if object was erased
                        }
                    }
                }
                if (should_erase)
                    break;
            }

            if (should_erase)
                continue; // Skip placement for erased objects

            // Then print it on the space_grid
            for (int i = 0; i < celestial_object->shape.size(); i++)
            {
                for (int j = 0; j < celestial_object->shape[i].size(); j++)
                {
                    int grid_row = start_row + i;
                    int grid_col = start_col + j;

                    if (grid_row >= 0 && grid_row < space_grid.size() &&
                        grid_col >= 0 && grid_col < space_grid[0].size())
                    {
                        if (space_grid[grid_row][grid_col] != 1)
                        {
                            space_grid[grid_row][grid_col] = celestial_object->shape[i][j];
                        }
                    }
                }
            }
        }
        celestial_object = celestial_object->next_celestial_object; // Update the iterator
    }
}

void AsteroidDash::rotate_celestial(CelestialObject *object, int hit_row, int hit_col)
{
    // Create rotations for the new design of the object
    CelestialObject delete_rotations(object);
    CelestialObject create_rotations(object);

    // Rotate the celestial object and update its shape in the space grid
    if (object == nullptr)
    {
        return;
    }

    // Check if rotations are available
    if (object->right_rotation == nullptr || object->left_rotation == nullptr)
    {
        return; // Can't rotate if rotations aren't properly defined
    }

    // Calculate the object's dimensions
    int object_height = object->shape.size();
    int start_row = object->starting_row;
    int middle_row = start_row + (object_height / 2);

    if (hit_row < middle_row)
    {
        // Hit above the middle line - rotate right
        object->shape = object->right_rotation->shape;
    }
    else if (hit_row == middle_row)
    {
        // do nothing - hit at the middle line
        return;
    }
    else if (hit_row > middle_row)
    {
        // Hit below or at the middle line - rotate left
        object->shape = object->left_rotation->shape;
    }
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
    // Free the memory allocated for celestial objects
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
