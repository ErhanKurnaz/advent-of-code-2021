#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#define TEST 0

#if TEST
#define FILE_NAME "input.test.txt"
#else
#define FILE_NAME "input.txt"
#endif

#define BUFF_CAP 256
#define INSTRUCTION_CAP 256
#define GRID_CAP 2048

enum {
    VERTICAL = 0,
    HORIZONTAL,
} typedef Direction;

struct {
    Direction direction;
    int value;
} typedef Instruction;

struct {
    bool grid[GRID_CAP][GRID_CAP];
    int width;
    int height;
} typedef Board;

void print_board(Board* board) {
    for (int row = 0; row < board->height; ++row) {
        for (int col = 0; col < board->width; ++col) {
            if (board->grid[row][col]) {
                printf("# ");
            } else {
                printf(". ");
            }
        }

        printf("\n");
    }
}

void print_instructions(Instruction* instructions, int inst_size) {
    const char names[] = {'y', 'x'};
    for (int i = 0; i < inst_size; ++i) {
        printf("%c=%d\n", names[instructions[i].direction], instructions[i].value);
    }
}

void fold(Board* board, Instruction instruction) {
    if (instruction.direction == VERTICAL) {
        for (int row = instruction.value + 1; row < board->height; ++row) {
            for (int col = 0; col < board->width; ++col) {
                if (board->grid[row][col]) {
                    int new_row = board->height - 1 - row;
                    assert(new_row > -1 && "Invalid row\n");
                    board->grid[new_row][col] = board->grid[row][col];
                }
            }
        }
        board->height -= instruction.value + 1;
    } else {
        for (int row = 0; row < board->height; ++row) {
            for (int col = instruction.value + 1; col < board->width; ++col) {
                if (board->grid[row][col]) {
                    int new_col = board->width - 1 - col;
                    assert(col - new_col > -1 && "Invalid col\n");
                    board->grid[row][new_col] = board->grid[row][col];
                }
            }
        }
        board->width -= instruction.value + 1;
    }
}

uint64_t part1(Board* board, Instruction* instructions) {
    fold(board, instructions[0]);
    uint64_t result = 0;

    for (int row = 0; row < board->height; ++row) {
        for (int col = 0; col < board->width; ++col) {
            result += board->grid[row][col];
        }
    }

    return result;
}

void part2(Board* board, Instruction* instructions, int inst_size) {
    for (int i = 0; i < inst_size; ++i) {
        fold(board, instructions[i]);
    }
}

int main(void) {
    FILE* f = fopen("./"FILE_NAME, "r");

    if (f == NULL) {
        fprintf(stderr, "Could not find file '%s'\n", FILE_NAME);
        exit(1);
    }

    char buffer[BUFF_CAP] = {0};
    bool parsing_folds = false;

    Board board = {0};

    Instruction instructions[INSTRUCTION_CAP] = {0};
    int inst_size = 0;

    while(fgets(buffer, sizeof(buffer), f)) {
        if (*buffer == '\n') {
            parsing_folds = true;
            continue;
        }

        if (!parsing_folds) {
            char* endp;
            int x = strtol(buffer, &endp, 10);
            int y = strtol(endp + 1, NULL, 10);

            board.grid[y][x] = true;

            if (x > board.width) {
                board.width = x;
            }

            if (y > board.height) {
                board.height = y;
            }
        } else {
            char dir = buffer[11];
            Direction direction;
            if (dir == 'y') {
                direction = VERTICAL;
            } else if (dir == 'x') {
                direction = HORIZONTAL;
            } else {
                fprintf(stderr, "Invalid direction '%c'\n", dir);
                exit(1);
            }

            int value = strtol(buffer + 13, NULL, 10);

            Instruction inst = {.direction = direction, .value = value};
            instructions[inst_size++] = inst;
        }
    }

    board.width++;
    board.height++;

    fclose(f);
    Board board2 = {0};
    memcpy(&board2, &board, sizeof(board2));

    printf("part1:\t%lld\n", part1(&board, instructions));
    part2(&board2, instructions, inst_size);
    printf("part2:\n");
    print_board(&board2);
    return 0;
}
