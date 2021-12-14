#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define TEST 0

#if TEST
#define INPUT_FILE "input.test.txt"
#else
#define INPUT_FILE "input.txt"
#endif

#define GRID_SIZE 10

typedef int Grid[GRID_SIZE][GRID_SIZE];

void print_grid(Grid grid) {
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            printf("%d ", grid[row][col]);
        }
        printf("\n");
    }
}

void increase_grid(Grid grid) {
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            grid[row][col]++;
        }
    }
}

uint64_t flash(Grid grid, int row, int col, bool visited[GRID_SIZE][GRID_SIZE]) {
    if (visited[row][col]) {
        return 0;
    }

    grid[row][col]++;

    uint64_t result = 0;
    if (grid[row][col] > 9) {
        visited[row][col] = true;

        result = 1;

        bool top_row = row == 0;
        bool bottom_row = row == GRID_SIZE - 1;

        bool start_col = col == 0;
        bool end_col = col == GRID_SIZE - 1;

        if (!top_row) {
            result += flash(grid, row - 1, col, visited);
        }

        if (!bottom_row) {
            result += flash(grid, row + 1, col, visited);
        }

        if (!start_col) {
            result += flash(grid, row, col - 1, visited);
        }

        if (!end_col) {
            result += flash(grid, row, col + 1, visited);
        }

        if (!top_row && !start_col) {
            result += flash(grid, row - 1, col - 1, visited);
        }

        if (!top_row && !end_col) {
            result += flash(grid, row - 1, col + 1, visited);
        }

        if (!bottom_row && !start_col) {
            result += flash(grid, row + 1, col - 1, visited);
        }

        if (!bottom_row && !end_col) {
            result += flash(grid, row + 1, col + 1, visited);
        }

        grid[row][col] = 0;
    }

    return result;
}

uint64_t part1(Grid grid) {
    uint64_t result = 0;
    for (int i = 0; i < 100; ++i) {
        increase_grid(grid);
        bool visited[GRID_SIZE][GRID_SIZE] = {0};

        for (int row = 0; row < GRID_SIZE; ++row) {
            for (int col = 0; col < GRID_SIZE; ++col) {
                if (grid[row][col] > 9) {
                    result += flash(grid, row, col, visited);
                }
            }
        }
    }


    return result;
}

uint64_t part2(Grid grid) {
    uint64_t step = 1;
    while (true) {
        increase_grid(grid);
        bool visited[GRID_SIZE][GRID_SIZE] = {0};

        for (int row = 0; row < GRID_SIZE; ++row) {
            for (int col = 0; col < GRID_SIZE; ++col) {
                if (grid[row][col] > 9) {
                    flash(grid, row, col, visited);
                }
            }
        }

        bool is_equal = true;
        int check_num = grid[0][0];
        for (int row = 0; row < GRID_SIZE && is_equal; ++row) {
            for (int col = 0; col < GRID_SIZE && is_equal; ++col) {
                if (grid[row][col] != check_num) {
                    is_equal = false;
                }
            }
        }

        if (is_equal) {
            return step;
        }

        step++;
    }


    return 0;
}

int main(void) {
    FILE* f = fopen("./"INPUT_FILE, "r");

    if (f == NULL) {
        fprintf(stderr, "Could not open file '%s'\n", INPUT_FILE);
        exit(1);
    }

    char buffer[GRID_SIZE + 2];
    Grid grid = {0};
    int row = 0;

    while(fgets(buffer, sizeof(buffer), f)) {
        char* start = buffer;
        int col = 0;

        while (*start && *start != '\n') {
            grid[row][col++] = *start - '0';
            start++;
        }

        row++;
    }

    fclose(f);

    Grid part2_grid = {0};
    memcpy(part2_grid, grid, sizeof(part2_grid));

    printf("part1:\t%lld\n", part1(grid));
    printf("part2:\t%lld\n", part2(part2_grid));
    return 0;
}
