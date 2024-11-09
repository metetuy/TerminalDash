#include "CelestialObject.h"

// Constructor to initialize CelestialObject with essential properties
CelestialObject::CelestialObject(const vector<vector<bool>> &shape, ObjectType type, int start_row,
                                 int time_of_appearance)
    : shape(shape), object_type(type), starting_row(start_row), time_of_appearance(time_of_appearance)
{
    // TODO: Your code here
}

// Copy constructor for CelestialObject
CelestialObject::CelestialObject(const CelestialObject *other)
    : shape(other->shape),                          // Copy the 2D vector shape
      object_type(other->object_type),              // Copy the object type
      starting_row(other->starting_row),            // Copy the starting row
      time_of_appearance(other->time_of_appearance) // Copy the time of appearance
{
    // TODO: Your code here
}

std::vector<std::vector<bool>> rotate_shape_90(const std::vector<std::vector<bool>> &shape) {
    int rows = shape.size();
    int cols = shape[0].size();
    std::vector<std::vector<bool>> rotated(cols, std::vector<bool>(rows));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            rotated[j][rows - 1 - i] = shape[i][j];
        }
    }
    return rotated;
}

void CelestialObject::create_rotations(CelestialObject *object) {
    CelestialObject *current = object;

    for (int i = 0; i < 3; ++i) {
        std::vector<std::vector<bool>> rotated_shape = rotate_shape_90(current->shape);
        CelestialObject *rotation = new CelestialObject(rotated_shape, current->object_type, current->starting_row, current->time_of_appearance);

        current->right_rotation = rotation;
        rotation->left_rotation = current;
        current = rotation;
    }

    current->right_rotation = object;
    object->left_rotation = current;
}

void CelestialObject::delete_rotations(CelestialObject *target) {
    CelestialObject *current = target->right_rotation;
    while (current != target) {
        CelestialObject *next = current->right_rotation;
        delete current;
        current = next;
    }

    target->right_rotation = target;
    target->left_rotation = target;
}