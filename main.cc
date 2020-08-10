#include <ncurses.h>
#include <vector>
#include <string>
#include <functional>
#include <thread>
#include <chrono>
#include <algorithm>
#include "sudoku.h"
using namespace std;


vector<string> main_menu = {
    "New game", "Select difficulty",
    "About", "Exit",
};

vector<string> diff_menu = {
    "Easy", "Meduim",
    "Hard", "Expert"
};

int difficulty = 0;

const int to_delete[4] = {
    45, 50, 55, 60
};


void performExit(int choice)
{
    endwin();
    exit(0);
}


/*
Shows a new ncurces menu.

Arguments:
    options - vector with strings that are shown in menu.
    functions - vector with void functions taking one int as
    a parameter, this function's position in the menu.
    if_exit - bool that shows if menu must be closed after function call.
    pre - void function that's called before printing the menu
    post - void function that's called after printing the menu
*/
void showMenu(
    vector<string> &options, vector<function<void(int choice)>> &functions, 
    bool if_exit = false, function<void()> pre = NULL, function<void()> post = NULL)
{
    int choice = 0;
    int max_choice = options.size() - 1;
    int exit_menu = false;

    while (true)  // Main loop
    {
        clear();
        if (pre)
            pre();
        for (int i = 0; i < options.size(); i++)
        {
            if (i == choice)
                attron(COLOR_PAIR(3));
            printw(options[i].c_str());
            if (i == choice)
                attroff(COLOR_PAIR(3));
            printw("\n");
        }
        if (post)
            post();

        switch (getch())
        {
        case KEY_DOWN:
            if (choice < max_choice)
                choice++;
            break;

        case KEY_UP:
            if (choice)
                choice--;
            break;

        case '\n':
            if (choice > functions.size() - 1)
                functions.back()(choice);
            else
                functions[choice](choice);
            if (if_exit)
                exit_menu = true;
            break;
        }

        if (exit_menu)
            break;
    }
}


void printSudokuToWindow(Sudoku &sudoku, vector<int> &guessed, int wrong = -1)
{
    clear();
    printw("    1 2 3   4 5 6   7 8 9\n");
    
    for (int i = 0; i < 9; i++)
    {
        if (i % 3 == 0)
        {
            printw("  |-------|-------|-------|\n");
        }

        printw("%d | ", i+1);
        for (int j = 0; j < 9; j++)
        {
            if (sudoku.field[i][j])
            {
                if (find(guessed.begin(), guessed.end(), i * 9 + j) == guessed.end())
                {
                    if (i * 9 + j == wrong)
                    {
                        attron(COLOR_PAIR(2));
                        printw("%d ", sudoku.field[i][j]);
                        attroff(COLOR_PAIR(2));
                    }
                    else
                        printw("%d ", sudoku.field[i][j]);
                }
                else
                {
                    attron(COLOR_PAIR(1));
                    printw("%d ", sudoku.field[i][j]);
                    attroff(COLOR_PAIR(1));
                }

            }
            else
            {
                printw("  ");
            }
            if (j == 2 || j == 5 || j == 8)
            {
                printw("| ");
            }
        }
        printw("\n");
    }
    printw("  |-------|-------|-------|\n");

    if (wrong == -1)
    {
        int* c = sudoku.count();
        mvprintw(2, 30, "Total unfilled: %d", c[0]);
        for (int i = 1; i < 10; i++)
        {
            mvprintw(i+2, 30, "%d: %d", i, c[i]);
        }
        delete[] c;
    }
    move(14, 0);
}


void printAboutInfo(int choice)
{
    clear();
    printw("HOW TO PLAY ------------------------------\n");
    printw("1. Go to main menu > New game             \n");
    printw("2. Hit enter and enter 3 digits: row,     \n");
    printw("column and your guess.                    \n");
    printw("If your guess was successful, number is   \n");
    printw("highlighted in green, otherwise in red.   \n\n");
    printw("AUTHOR -----------------------------------\n");
    printw("Sudoku game made by Maksim Gusev          \n");
    printw("Github: https://github.com/alantheknight/ \n");
    printw("Press any key to exit...\n");
    getch();
}


/* 
Perform a move and try to set field[y][x] to a guessed number.

Returns:
    0 if operation was successful
    1 if row, col or guess have incorrect values
    2 if guess was wrong
    3 if selected cell have been already guessed
*/
int performMove(
    int row, int col, int guess, vector<int> &guessed, Sudoku &sudoku)
{
    if (row >= 0 && row < 10 && col >= 0 && col < 10 && guess >= 0 && guess < 10)
    {
        if (sudoku.field[row][col] == 0)
        {
            if (sudoku.original[row][col] == guess)
            {
                sudoku.field[row][col] = guess;
                guessed.push_back(9 * row + col);
            }
            else
            {
                return 2;
            }
        }
        else
        {
            return 3;
        }
    }
    else
        return 1;
    
    return 0;
}


void gameLoop(int choice)
{
    Sudoku sudoku;
    bool if_exit = false;
    vector<int> guessed;
    sudoku.delete_cells(to_delete[difficulty]);
    int row, col, guess;
    while (sudoku.has_empty_cells())
    {
        printSudokuToWindow(sudoku, guessed);

        switch (getch())
        {
        case '\n':
            printw("\nEnter row, column and suggested number: ");
            scanw("%d %d %d", &row, &col, &guess);
            row--; col--;

            switch (performMove(row, col, guess, guessed, sudoku))
            {
            case 3:
                printw("\nThis cell have been already guessed.");
                refresh();
                this_thread::sleep_for(chrono::seconds(2));
                break;

            case 2:
                sudoku.field[row][col] = guess;
                printSudokuToWindow(sudoku, guessed, row * 9 + col);
                refresh();
                this_thread::sleep_for(chrono::seconds(1));
                sudoku.field[row][col] = 0;
                break;            
            
            default:
                break;
            }

            break;
        
        case 'q':
            if_exit = true;
            break;
        }

        if (if_exit)
            break;
    }
    if (!sudoku.has_empty_cells())
    {
        clear();
        attron(COLOR_PAIR(3));
        mvprintw(getmaxy(stdscr) / 2, (getmaxx(stdscr) - 9) / 2, "You won!");
        attroff(COLOR_PAIR(3));
        refresh();
        this_thread::sleep_for(chrono::seconds(3));
    }
}


void main_m_pre()
{
    mvprintw(0, (getmaxx(stdscr) - 20) / 2, "SUDOKU by ATK v.1.0\n");
}


void main_m_post()
{
    mvprintw(6, 0, "Difficulty: %s", diff_menu[difficulty].c_str());
}


void changeDifficulty(int choice)
{
    difficulty = choice;
}


void diffMenu(int choice)
{
    vector<function<void(int choice)>> diff_m_funcs = {changeDifficulty};
    showMenu(diff_menu, diff_m_funcs, true);
}


int main()
{
    initscr();
    keypad(stdscr, true);
    curs_set(0);

    use_default_colors();
    start_color();
    init_pair(1, COLOR_GREEN, -1);
    init_pair(2, COLOR_RED, -1);
    init_pair(3, COLOR_MAGENTA, -1);


    vector<function<void(int choice)>> main_m_funcs = {
        gameLoop, diffMenu, printAboutInfo, performExit
    };

    showMenu(main_menu, main_m_funcs, false, main_m_pre, main_m_post);

    getch();
    endwin();
    return 0;
}
