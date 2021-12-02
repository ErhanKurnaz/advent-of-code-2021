#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFF_CAP 256
#define INSTRUCTION_CAP 1024

struct {
    int horizontal;
    int depth;
    int aim;
} typedef Position;

enum {
    UP = 0,
    DOWN,
    FORWARD,
} typedef Direction;

struct {
    Direction direction;
    int value;
} typedef Instruction;

Direction get_direction_from_str(const char *str, size_t size) {
    if (strncmp(str, "up", size) == 0) {
        return UP;
    }

    if (strncmp(str, "down", size) == 0) {
        return DOWN;
    }

    if (strncmp(str, "forward", size) == 0) {
        return FORWARD;
    }

    fprintf(stderr, "Could not find direction for %s", str);
    exit(1);
}

int part1(Instruction *instructions, size_t size) {
    Position position = {0};

    for (size_t i = 0; i < size; ++i) {
        switch (instructions[i].direction) {
        case UP: position.depth -= instructions[i].value; break;
        case DOWN: position.depth += instructions[i].value; break;
        case FORWARD: position.horizontal += instructions[i].value; break;
        }
    }

    return position.depth * position.horizontal;
}

int part2(Instruction *instructions, size_t size) {
    Position position = {0};

    for (size_t i = 0; i < size; ++i) {
        switch (instructions[i].direction) {
            case UP: position.aim -= instructions[i].value; break;
            case DOWN: position.aim += instructions[i].value; break;
            case FORWARD: {
                position.horizontal += instructions[i].value;
                position.depth += position.aim * instructions[i].value;
            }; break;
        }
    }

    return position.depth * position.horizontal;
}

int main(void) {
    const char *file_name = "./input.txt";
    FILE *f = fopen(file_name, "r");

    if (f == NULL) {
        fprintf(stderr, "Could not find file '%s'", file_name);
        exit(1);
    }

    char buffer[BUFF_CAP];
    Instruction instructions[INSTRUCTION_CAP];
    size_t read = 0;
    while (fgets(buffer, BUFF_CAP, f)) {
        char instruction_str[7] = {0};
        char value_str[10] = {0};
        bool parsing_inst = true;
        int num_start = -1;
        int i = -1;

        while (buffer[++i]) {
            if (buffer[i] == ' ') {
                parsing_inst = false;
                num_start = i + 1;
                continue;
            }

            if (parsing_inst) {
                instruction_str[i] = buffer[i];
            } else {
                value_str[i - num_start] = buffer[i];
            }
        }

        Instruction inst = {
            .direction = get_direction_from_str(instruction_str, num_start - 1),
            .value = (int) strtol(value_str, (char**) NULL, 10),
        };

        instructions[read] = inst;
        read = read + 1;
        memset(instruction_str, 0, sizeof(instruction_str));
        memset(value_str, 0, sizeof(value_str));
    }

    fclose(f);

    printf("part1:\t%d\n", part1(instructions, read));
    printf("part2:\t%d\n", part2(instructions, read));

    return 0;
}
