#include "CelestialObject.h"

// Constructor to initialize CelestialObject with essential properties
CelestialObject::CelestialObject(const vector<vector<bool>> &shape, ObjectType type, int start_row,
                                 int time_of_appearance)
    : shape(shape), object_type(type), starting_row(start_row), time_of_appearance(time_of_appearance)
{
    create_rotations(this);
}

// Copy constructor for CelestialObject
CelestialObject::CelestialObject(const CelestialObject *other)
    : shape(other->shape),                          // Copy the 2D vector shape
      object_type(other->object_type),              // Copy the object type
      starting_row(other->starting_row),            // Copy the starting row
      time_of_appearance(other->time_of_appearance) // Copy the time of appearance
{
}

vector<std::vector<bool>> CelestialObject::rotate_shape_90(const std::vector<std::vector<bool>> &shape)
{
    if (shape.empty() || shape[0].empty())
    {
        return {};
    }
    int rows = shape.size();
    int cols = shape[0].size();
    std::vector<std::vector<bool>> rotated(cols, std::vector<bool>(rows));

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            rotated[j][rows - 1 - i] = shape[i][j];
        }
    }

    return rotated;
}

void CelestialObject::create_rotations(CelestialObject *object)
{

    CelestialObject *current = object;
    // Store rotations to check for duplicates.
    current->rotations.push_back(current);

    for (int i = 0; i < 3; i++)
    {
        // Rotate the shape 90 degrees
        std::vector<std::vector<bool>> rotated_shape = rotate_shape_90(current->shape);
        CelestialObject *rotation = new CelestialObject(current);
        rotation->shape = rotated_shape;
        // Check for duplicates
        bool is_duplicate = false;
        for (auto &prev_rotation : object->rotations)
        {
            if (rotation->shape == prev_rotation->shape)
            {
                is_duplicate = true;
                break;
            }
        }

        if (is_duplicate)
        {
            delete rotation; // Clean up the duplicate object.
            continue;        // Skip adding this rotation.
        }

        // Add the current rotation to the list
        object->rotations.push_back(rotation);

        // Link the current object to the new rotation
        if (rotation->shape == current->shape)
        {
            current->left_rotation = current;
            current->right_rotation = current;
        }
        else
        {

            current->right_rotation = rotation;
            rotation->left_rotation = current;
        }

        // Move to the next rotation
        current = rotation;
    }

    // Close the rotation loop and link the last rotation back to the original object
    current->right_rotation = object;
    object->left_rotation = current;
}

void CelestialObject::delete_rotations(CelestialObject *target)
{

    CelestialObject *current = target->right_rotation;
    while (current != target)
    {
        CelestialObject *next = current->right_rotation;
        delete current;
        current = next;
    }
    target->right_rotation = target;
    target->left_rotation = target;
}