#ifndef SUDOKU_H
#define SUDOKU_H


class Sudoku {
    public:
        Sudoku();
        int field[9][9];
        int original[9][9];

        int* get_col(int index);
        int* get_row(int index);
        int* get_box(int start_x, int start_y);
        
        int* count();

        bool safe_to_insert(int x, int y, int num);
        bool check_position(int x, int y);
        bool check_field();
        bool solve_sudoku();
        void print_sudoku();
        bool has_empty_cells();
        void generate_field();
        void delete_cells(int amount);

    private:
        bool is_safe(int nums[9], int num);
        bool find_empty_space(int &x, int &y);
        void shuffle_array(int *begin, int *end);
        bool check_if_unique(int *begin, int *end);
        void write_box(int box[9], int start_x, int start_y);
};

#endif