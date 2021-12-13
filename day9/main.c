#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TEST 0

#if TEST
#define INPUT_FILE "input.test.txt"
#define GRID_WIDTH 10
#define GRID_HEIGHT 5
#else
#define INPUT_FILE "input.txt"
#define GRID_WIDTH 100
#define GRID_HEIGHT 100
#endif

struct {
    int value;
    bool is_checked;
} typedef Point;

typedef Point Grid[GRID_HEIGHT][GRID_WIDTH];

struct {
    int x;
    int y;
} typedef Position;

enum {
    UP    = 1,
    DOWN  = 2,
    LEFT  = 4,
    RIGHT = 8,
} typedef Direction;

void print_grid(Grid grid) {
    for (int i = 0; i < GRID_HEIGHT; ++i) {
        for (int j = 0; j < GRID_WIDTH; ++j) {
            printf("%d ", grid[i][j].value);
        }

        printf("\n");
    }
}

int get_basin(Grid grid, int y, int x) {
    grid[y][x].is_checked = true;

    if (grid[y][x].value == 9) {
        return 0;
    }

    int result = 1;
    if (x != 0 && !grid[y][x - 1].is_checked) {
        result += get_basin(grid, y, x - 1);
    }

    if (x != GRID_WIDTH - 1 && !grid[y][x + 1].is_checked) {
        result += get_basin(grid, y, x + 1);
    }

    if (y != 0 && !grid[y - 1][x].is_checked) {
        result += get_basin(grid, y - 1, x);
    }

    if (y != GRID_HEIGHT - 1 && !grid[y + 1][x].is_checked) {
        result += get_basin(grid, y + 1, x);
    }

    return result;
}


int find_lowpoints(Grid grid, Position* positions) {
    int size = 0;
    for (int i = 0; i < GRID_HEIGHT; ++i) {
        for (int j = 0; j < GRID_WIDTH; ++j) {
            bool is_lowpoint = true;

            if (i != 0) {
                is_lowpoint = grid[i][j].value < grid[i - 1][j].value;
            }

            if (is_lowpoint && i != GRID_HEIGHT - 1) {
                is_lowpoint = grid[i][j].value < grid[i + 1][j].value;
            }

            if (is_lowpoint && j != 0) {
                is_lowpoint = grid[i][j].value < grid[i][j - 1].value;
            }

            if (is_lowpoint && j != GRID_WIDTH - 1) {
                is_lowpoint = grid[i][j].value < grid[i][j + 1].value;
            }

            if (is_lowpoint) {
                Position pos = {
                    .y = i,
                    .x = j,
                };

                positions[size++] = pos;
            }
        }
    }

    return size;
}

int part1(Grid grid) {
    Position lowpoints[GRID_WIDTH * GRID_HEIGHT] = {0};
    int lowpoints_size = find_lowpoints(grid, lowpoints);

    int result = 0;
    for (int i = 0; i < lowpoints_size; ++i) {
        result += grid[lowpoints[i].y][lowpoints[i].x].value + 1;
    }

    return result;
}

int part2(Grid grid) {
    Position lowpoints[GRID_WIDTH * GRID_HEIGHT] = {0};
    int lowpoints_size = find_lowpoints(grid, lowpoints);

    int heighest[] = {0, 0, 0};

    for (int i = 0; i < lowpoints_size; ++i) {
        int basin = get_basin(grid, lowpoints[i].y, lowpoints[i].x);

        int position = -1;
        if (basin > heighest[2]) {
            position = 2;
        } else if (basin > heighest[1]) {
            position = 1;
        } else if (basin > heighest[0]) {
            position = 0;
        }

        if (position >= 0) {
            for (int j = 1; j <= position; ++j) {
                heighest[j - 1] = heighest[j];
            }

            heighest[position] = basin;
        }
    }

    int result = heighest[0];
    for (int i = 1; i < 3; ++i) {
        result *= heighest[i];
    }

    return result;
}

int main(void) {
    FILE* f = fopen("./"INPUT_FILE, "r");

    if (f == NULL) {
        fprintf(stderr, "Could not find file '%s'", INPUT_FILE);
        exit(1);
    }

    char buffer[GRID_WIDTH + 2] = {0};
    Grid grid = {0};

    int row = 0;
    while(fgets(buffer, sizeof(buffer), f)) {
        char* character = buffer;
        int col = 0;

        while(*character && *character != '\n') {
            grid[row][col++].value = *character - '0';
            character++;
        }

        row++;
    }

    fclose(f);

    printf("part1:\t%d\n", part1(grid));
    printf("part2:\t%d\n", part2(grid));

    return 0;
}
