#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define BUFF_CAP 512
#define NUM_CAP 256
#define SHEET_CAP 1024
#define GRID_SIZE 5

struct {
    int value;
    bool checked;
} typedef BingoVal;

enum {
    UP    = 1,
    DOWN  = 2,
    LEFT  = 4,
    RIGHT = 8,
} typedef Direction;

int parse_numbers(char* line, int* numbers) {
    int size = 0;

    char* begin = line;
    char* end;

    while (begin) {
        int num = (int) strtol(begin, &end, 10);
        numbers[size++] = num;
        if (end == NULL || *end == '\0' || *end == '\n') {
            break;
        }

        begin = end + 1;
    }


    return size;
}

void copy_sheets(BingoVal sheets[SHEET_CAP][GRID_SIZE][GRID_SIZE], int size, BingoVal dest[SHEET_CAP][GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            for (int k = 0; k < GRID_SIZE; ++k) {
                BingoVal original = sheets[i][j][k];
                BingoVal copy = { .value = original.value, .checked = original.checked };
                dest[i][j][k] = copy;
            }
        }
    }
}

void print_sheets(BingoVal sheets[SHEET_CAP][GRID_SIZE][GRID_SIZE], int size) {
    for (int i = 0; i < size; ++i) {
        printf("Sheet %d:\n", i + 1);
        for (int j = 0; j < GRID_SIZE; ++j) {
            for (int k = 0; k < GRID_SIZE; ++k) {
                printf("(%d %s) ", sheets[i][j][k].value, sheets[i][j][k].checked ? "true" : "false");
            }

            printf("\n");
        }

        printf("\n");
    }
}

bool check_direction(BingoVal sheet[GRID_SIZE][GRID_SIZE], int row, int col, int directions) {
    while (row < GRID_SIZE && row >= 0 && col < GRID_SIZE && col >= 0) {
        if (!sheet[row][col].checked) {
            return false;
        }

        if ((directions & UP) == UP) {
            row--;
        }

        if ((directions & DOWN) == DOWN) {
            row++;
        }

        if ((directions & LEFT) == LEFT) {
            col--;
        }

        if ((directions & RIGHT) == RIGHT) {
            col++;
        }
    }

    return true;
}

bool has_won(BingoVal sheet[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (i == 0) {
                bool won = check_direction(sheet, i, j, DOWN);
                if (won) {
                    return true;
                }
            }

            if (j == 0) {
                bool won = check_direction(sheet, i, j, RIGHT);
                if (won) {
                    return true;
                }
            }
        }
    }
    return false;
}


int part1(int* pulled_numbers, int pulled_amount, BingoVal sheets[SHEET_CAP][GRID_SIZE][GRID_SIZE], int sheet_amount) {
    for (int i = 0; i < pulled_amount; ++i) {
        int pulled_num = pulled_numbers[i];

        for (int j = 0; j < sheet_amount; ++j) {
            for (int row = 0; row < GRID_SIZE; ++row) {
                for (int col = 0; col < GRID_SIZE; ++col) {
                    if (sheets[j][row][col].value == pulled_num) {
                        sheets[j][row][col].checked = true;
                    }
                }
            }

            if (has_won(sheets[j])) {
                int result = 0;

                for (int row = 0; row < GRID_SIZE; ++row) {
                    for (int col = 0; col < GRID_SIZE; ++col) {
                        if (!sheets[j][row][col].checked) {
                            result += sheets[j][row][col].value;
                        }

                    }
                }

                return result * pulled_num;
            }
        }
    }
    return 0;
}

int part2(int* pulled_numbers, int pulled_amount, BingoVal sheets[SHEET_CAP][GRID_SIZE][GRID_SIZE], int sheet_amount) {
    int last_sheet_to_win = -1;
    int last_winning_result = -1;
    bool sheets_done[SHEET_CAP] = {0};
    for (int i = 0; i < pulled_amount; ++i) {
        int pulled_num = pulled_numbers[i];

        for (int j = 0; j < sheet_amount; ++j) {
            if (sheets_done[j]) {
                continue;
            }

            for (int row = 0; row < GRID_SIZE; ++row) {
                for (int col = 0; col < GRID_SIZE; ++col) {
                    if (sheets[j][row][col].value == pulled_num) {
                        sheets[j][row][col].checked = true;
                    }
                }
            }

            if (has_won(sheets[j])) {
                int result = 0;

                for (int row = 0; row < GRID_SIZE; ++row) {
                    for (int col = 0; col < GRID_SIZE; ++col) {
                        if (!sheets[j][row][col].checked) {
                            result += sheets[j][row][col].value;
                        }

                    }
                }

                last_sheet_to_win = j;
                last_winning_result = result * pulled_num;
                sheets_done[j] = true;
            }
        }
    }

    return last_winning_result;
}

int main(void) {
    const char *file_name = "./input.txt";
    FILE *f = fopen(file_name, "r");

    if (f == NULL) {
        fprintf(stderr, "Could not find file '%s'", file_name);
        exit(1);
    }

    char buffer[BUFF_CAP];

    int nums_pulled[NUM_CAP] = {0};
    int pulled_amount = 0;

    BingoVal sheets[SHEET_CAP][GRID_SIZE][GRID_SIZE] = {0};
    int sheet = 0;
    int row = 0;

    bool parsing_sheets = false;
    while (fgets(buffer, BUFF_CAP, f)) {
        if (*buffer == '\n') {
            if (!parsing_sheets) {
                parsing_sheets = true;
            } else {
                sheet++;
                row = 0;
            }

            continue;
        }

        if (!parsing_sheets) { // we are now parsing the pulled numbers
            pulled_amount = parse_numbers(buffer, nums_pulled);
            continue;
        }

        // from this point forwars we are parsing the bingo sheets
        assert(row < GRID_SIZE && "To many rows for grid");
        int numbers[GRID_SIZE];
        parse_numbers(buffer, numbers);

        for (int i = 0; i < GRID_SIZE; ++i) {
            BingoVal val = { .value = numbers[i], .checked = false };
            sheets[sheet][row][i] = val;
        }
        row++;
    }

    fclose(f);

    BingoVal part1_sheets[SHEET_CAP][GRID_SIZE][GRID_SIZE] = {0};
    BingoVal part2_sheets[SHEET_CAP][GRID_SIZE][GRID_SIZE] = {0};
    copy_sheets(sheets, sheet + 1, part1_sheets);
    copy_sheets(sheets, sheet + 1, part2_sheets);
    printf("part1:\t%d\n", part1(nums_pulled, pulled_amount, part1_sheets, sheet + 1));
    printf("part2:\t%d\n", part2(nums_pulled, pulled_amount, part2_sheets, sheet + 1));

    return 0;
}
