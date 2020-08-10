#include <random>
#include <iostream>
#include <algorithm>
#include "sudoku.h"


Sudoku::Sudoku() : field{0} {
    generate_field();
    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++) {
            original[i][j] = field[i][j];
        }
    }
};


// Print all non-zero contents of sudoku field row by row.
void Sudoku::print_sudoku()
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            // if (field[i][j] != 0)
                std::cout << field[i][j] << ' ';
            // else
            //     std::cout << ' ' << ' ';
        }
        std::cout << '\n';
    }
}


// Randomly shuffle an array with numbers.
void Sudoku::shuffle_array(int *begin, int *end)
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(begin, end, g);
}


/*
Return a pointer to dynamically allocated array containing
selected column of sudoku field.
*/
int* Sudoku::get_col(int index)
{
    int* col = new int[9];
    for (int i = 0; i < 9; i++) {
        col[i] = field[i][index];
    }
    return col;
}


/*
Return a pointer to dynamically allocated array containing
selected row of sudoku field.
*/
int* Sudoku::get_row(int index)
{
    int* row = new int[9];
    for (int i = 0; i < 9; i++)
    {
        row[i] = field[index][i];
    }
    return row;
}


/*
Return a pointer to dynamically allocated array containing
selected 3x3 box of sudoku field.
*/
int* Sudoku::get_box(int start_x, int start_y)
{
    int *box = new int[9];
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            box[3 * i + j] = field[start_y + i][start_x + j];
        }
    }
    return box;
}


bool Sudoku::check_if_unique(int *begin, int *end)
{
    return std::adjacent_find(begin, end + 1) == end + 1;
}


void Sudoku::write_box(int box[9], int start_x, int start_y)
{
    int index = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            field[start_y + i][start_x + j] = box[index];
            index++;
        }
    }
}


/*
Check if row, column and box where cell(x, y) is located
consist of unique elements.
*/
bool Sudoku::check_position(int x, int y)
{
    int *row = get_row(y), *col = get_col(x),
    *box = get_box(x - x % 3, y - y % 3);
    bool result = (
        check_if_unique(row, &row[8]) &&
        check_if_unique(col, &col[8]) &&
        check_if_unique(box, &box[8])
    );
    delete[] row, col, box;
    return result;
}


/* 
Check if sudoku field doesn't contain repeating 
values in it's rows, columns and 3x3 boxes.
*/
bool Sudoku::check_field()
{
    for (int y = 0; y < 9; y++)
    {
        for (int x = 0; x < 9; x++)
        {
            if (check_position(x, y) == 0)
            {
                return false;
            }
        }
    }
    return true;
}


bool Sudoku::find_empty_space(int &x, int &y)
{
    for (int i = 0; i < 9; i++) // i is y of cell
    {
        for (int j = 0; j < 9; j++) // j is x of cell
        {
            if (field[i][j] == 0)
            {
                x = j; y = i;
                return true;
            }
        }
    }
    return false;
}


/* Check that array doesn't contain number num */
bool Sudoku::is_safe(int nums[9], int num)
{
    for (int i = 0; i < 9; i++)
    {
        if (nums[i] == num)
            return false;
    }
    return true;
}


/* Check if row, col and box of cell to be 
changed doesn't contain a new number. */
bool Sudoku::safe_to_insert(int x, int y, int num)
{
    int *row = get_row(y), *col = get_col(x),
    *box = get_box(x - x % 3, y - y % 3);
    bool result = (
        is_safe(row, num) &&
        is_safe(col, num) &&
        is_safe(box, num)
    );
    delete[] row, col, box;
    return result;
}


bool Sudoku::solve_sudoku()
{
    int x, y;
    if (!find_empty_space(x, y)) // Field is already filled
    {
        return true;
    }
    for (int i = 1; i <= 9; i++)
    {
        if (safe_to_insert(x, y, i))
        {
            field[y][x] = i;
            if (solve_sudoku())
            {
                return true;
            }
            field[y][x] = 0;
        }
    }
    return false;
}


bool Sudoku::has_empty_cells()
{
    int x, y;
    return find_empty_space(x, y);
}


void Sudoku::generate_field()
{
    // Filling 3x3 diagonal squares by coords: 0,0; 3,3; 6,6
    // with shuffled numbers from 1 to 9.
    for (int i = 0; i < 7; i += 3)
    {
        int box[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        shuffle_array(box, &box[9]);
        write_box(box, i, i);
    }

    solve_sudoku();
}


void Sudoku::delete_cells(int amount)
{
    int deleted = 0;
    while (deleted < amount)
    {
        int x = (rand() + 1) % 9;
        int y = (rand() + 1) % 9;
        if (field[y][x] != 0)
        {
            field[y][x] = 0;
            deleted++;
        }
    }
}


/*
Return dinamically allocated array with 10 elements showing how 
many cells of each type need to be filled. */
int* Sudoku::count()
{
    int* result = new int[10]();
    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            result[field[i][j]]++;
        }
    }
    for (int i = 1; i < 9; i++) {
        result[i] = 9 - result[i];
    }
    return result;
}

