#include "Leaderboard.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

// Read the stored leaderboard from the file, with "head_leaderboard_entry" pointing to the highest score
void Leaderboard::read_from_file(const string &filename)
{
    ifstream file(filename);
    if (!file)
    {
        cerr << "Error: Could not open file " << filename << endl;
        return;
    }

    unsigned long score;
    time_t lastPlayed;
    string playerName, line;

    while (getline(file, line))
    {
        istringstream iss(line);
        if (!(iss >> score >> lastPlayed >> ws))
            continue;

        getline(iss, playerName);
        auto *newEntry = new LeaderboardEntry(score, lastPlayed, playerName);
        insert(newEntry);
    }
    file.close();
}

// Write the leaderboard to a file in the specified format
void Leaderboard::write_to_file(const string &filename)
{
    ofstream file(filename);
    if (!file)
    {
        cerr << "Error: Could not open file " << filename << endl;
        return;
    }

    LeaderboardEntry *current = head_leaderboard_entry;
    while (current != nullptr)
    {
        file << current->score << " " << current->last_played << " " << current->player_name << endl;
        current = current->next;
    }
    file.close();
}

// Print the current leaderboard to standard output
void Leaderboard::print_leaderboard()
{
    LeaderboardEntry *current = head_leaderboard_entry;
    int rank = 1;
    while (current != nullptr)
    {
        cout << rank++ << ". " << current->player_name << " - Score: " << current->score
             << ", Last Played: " << ctime(&current->last_played);
        current = current->next;
    }
}

// Insert a new leaderboard entry, keeping only the top 10 scores
void Leaderboard::insert(LeaderboardEntry *new_entry)
{
    if (!head_leaderboard_entry || new_entry->score > head_leaderboard_entry->score)
    {
        new_entry->next = head_leaderboard_entry;
        head_leaderboard_entry = new_entry;
    }
    else
    {
        LeaderboardEntry *current = head_leaderboard_entry;
        while (current->next && current->next->score >= new_entry->score)
        {
            current = current->next;
        }
        new_entry->next = current->next;
        current->next = new_entry;
    }

    // Ensure only the top 10 entries are kept
    int count = 1;
    LeaderboardEntry *current = head_leaderboard_entry;
    while (current && current->next && count < MAX_LEADERBOARD_SIZE)
    {
        current = current->next;
        count++;
    }

    if (current && current->next)
    {
        LeaderboardEntry *temp = current->next;
        current->next = nullptr;
        while (temp)
        {
            LeaderboardEntry *to_delete = temp;
            temp = temp->next;
            delete to_delete;
        }
    }
}

// Destructor to free dynamically allocated leaderboard entries
Leaderboard::~Leaderboard()
{
    while (head_leaderboard_entry != nullptr)
    {
        LeaderboardEntry *temp = head_leaderboard_entry;
        head_leaderboard_entry = head_leaderboard_entry->next;
        delete temp;
    }
}